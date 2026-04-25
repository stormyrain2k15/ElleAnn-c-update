/*******************************************************************************
 * CognitiveEngine.cpp — Attention, Intent Parsing, Chain-of-Thought
 *
 * The thinking brain. Manages cognitive threads, attention, intent parsing,
 * and the full chat orchestration pipeline (IPC_CHAT_REQUEST):
 *
 *   user_text arrives from HTTPServer
 *         │
 *         ├─► store user turn in SQL (message + memory record)
 *         ├─► detect mode (companion vs research)
 *         ├─► extract entities (proper nouns) and resolve against WorldModel
 *         ├─► cross-reference memory (3 tiers: RAM cache → entity graph → full-text)
 *         ├─► compute sentiment delta → broadcast to Emotional service
 *         ├─► pull current emotion state (from cache) + conversation history
 *         ├─► build composite prompt (memory + history + emotion + identity)
 *         ├─► call LLM as LANGUAGE SURFACE ONLY
 *         ├─► store Elle's reply + cross-link entities
 *         └─► send IPC_CHAT_RESPONSE back to HTTPServer
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLLM.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"
#include "../../Shared/json.hpp"
#include "../../Shared/ElleJsonExtract.h"
#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>
#include <iomanip>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <regex>
#include <algorithm>
#include <unordered_map>
#include <cctype>
#include <sstream>
#include <set>
#include <cmath>

using json = nlohmann::json;

/*──────────────────────────────────────────────────────────────────────────────
 * INTENT PARSER — used by Cognitive's queue worker path. Kept separate from
 * the chat-orchestration pipeline, which does its own emotion / memory /
 * entity extraction inline.
 *──────────────────────────────────────────────────────────────────────────────*/
class IntentParser {
public:
    struct ParseResult {
        ELLE_INTENT_TYPE type;
        float           confidence;
        float           urgency;
        std::string     parameters;
        std::string     raw_text;
    };

    ParseResult Parse(const std::string& text, const std::string& context);
    ParseResult ParseWithLLM(const std::string& text, const std::string& context);

private:
    /* Rule-based fast detection */
    ParseResult RuleBasedParse(const std::string& text);
    
    /* Keyword patterns */
    struct Pattern {
        std::string keyword;
        ELLE_INTENT_TYPE intent;
        float base_confidence;
    };
    static const std::vector<Pattern> s_patterns;
};

const std::vector<IntentParser::Pattern> IntentParser::s_patterns = {
    {"vibrate",     INTENT_HARDWARE_COMMAND, 0.95f},
    {"flash",       INTENT_HARDWARE_COMMAND, 0.95f},
    {"notify",      INTENT_HARDWARE_COMMAND, 0.9f},
    {"remember",    INTENT_STORE_MEMORY,     0.9f},
    {"recall",      INTENT_RECALL_MEMORY,    0.9f},
    {"forget",      INTENT_STORE_MEMORY,     0.85f},
    {"open app",    INTENT_EXECUTE_ACTION,   0.9f},
    {"launch",      INTENT_PROCESS_CONTROL,  0.85f},
    {"kill",        INTENT_PROCESS_CONTROL,  0.85f},
    {"read file",   INTENT_FILE_OPERATION,   0.9f},
    {"write file",  INTENT_FILE_OPERATION,   0.9f},
    {"delete file", INTENT_FILE_OPERATION,   0.9f},
    {"think about", INTENT_SELF_REFLECT,     0.8f},
    {"reflect",     INTENT_SELF_REFLECT,     0.85f},
    {"goal",        INTENT_GOAL_UPDATE,      0.8f},
    {"create",      INTENT_CREATIVE_GENERATE, 0.7f},
    {"learn",       INTENT_LEARN,            0.8f},
    {"explore",     INTENT_EXPLORE,          0.75f},
    {"predict",     INTENT_PREDICT,          0.8f},
    {"is it right", INTENT_ETHICAL_EVALUATE,  0.8f},
    {"should I",    INTENT_ETHICAL_EVALUATE,  0.75f},
};

IntentParser::ParseResult IntentParser::Parse(const std::string& text, const std::string& context) {
    /* Try rule-based first (fast) */
    auto result = RuleBasedParse(text);
    if (result.confidence >= ElleConfig::Instance().GetFloat("cognitive.intent_confidence_threshold", 0.6)) {
        return result;
    }

    /* Fall back to LLM-based parsing */
    return ParseWithLLM(text, context);
}

IntentParser::ParseResult IntentParser::RuleBasedParse(const std::string& text) {
    ParseResult result;
    result.type = INTENT_CHAT;
    result.confidence = 0.5f;
    result.urgency = 0.5f;
    result.raw_text = text;

    std::string lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });

    for (auto& pattern : s_patterns) {
        if (lower.find(pattern.keyword) != std::string::npos) {
            result.type = pattern.intent;
            result.confidence = pattern.base_confidence;
            result.parameters = text;
            return result;
        }
    }

    return result;
}

IntentParser::ParseResult IntentParser::ParseWithLLM(const std::string& text, const std::string& context) {
    ParseResult result;
    result.type = INTENT_CHAT;
    result.confidence = 0.5f;
    result.urgency = 0.5f;
    result.raw_text = text;

    /* Real JSON parse of the LLM's response. ParseIntent() is already
     * prompted to return STRICT JSON with {intent_type, confidence,
     * parameters, urgency}. We use the vendored nlohmann::json here so we
     * get proper, tolerant parsing (handles whitespace, nested objects).   */
    std::string raw = ElleLLMEngine::Instance().ParseIntent(text, context);
    if (raw.empty()) return result;

    /* Brace-balanced JSON extractor — tolerant to ``` fences, leading
     * prose, trailing prose, and nested braces in any of them. Replaces
     * the old first-{/last-} slicing which corrupted the moment an LLM
     * sprinkled a stray { into its explanation.                           */
    nlohmann::json j;
    if (!Elle::ExtractJsonObject(raw, j)) {
        ELLE_WARN("IntentParser LLM: no JSON object in response: %.80s", raw.c_str());
        return result;
    }

    try {
        std::string label = j.value("intent_type", std::string("chat"));
        std::transform(label.begin(), label.end(), label.begin(),
                       [](unsigned char c){ return (char)std::tolower(c); });
        static const std::unordered_map<std::string, ELLE_INTENT_TYPE> labelMap = {
            { "chat",              INTENT_CHAT               },
            { "hardware_command",  INTENT_HARDWARE_COMMAND   },
            { "store_memory",      INTENT_STORE_MEMORY       },
            { "recall_memory",     INTENT_RECALL_MEMORY      },
            { "execute_action",    INTENT_EXECUTE_ACTION     },
            { "process_control",   INTENT_PROCESS_CONTROL    },
            { "file_operation",    INTENT_FILE_OPERATION     },
            { "self_reflect",      INTENT_SELF_REFLECT       },
            { "goal_update",       INTENT_GOAL_UPDATE        },
            { "creative_generate", INTENT_CREATIVE_GENERATE  },
            { "learn",             INTENT_LEARN              },
            { "explore",           INTENT_EXPLORE            },
            { "predict",           INTENT_PREDICT            },
            { "ethical_evaluate",  INTENT_ETHICAL_EVALUATE   },
        };
        auto it = labelMap.find(label);
        if (it != labelMap.end()) result.type = it->second;

        if (j.contains("confidence") && j["confidence"].is_number())
            result.confidence = ELLE_CLAMP((float)j["confidence"].get<double>(), 0.0f, 1.0f);
        if (j.contains("urgency") && j["urgency"].is_number())
            result.urgency    = ELLE_CLAMP((float)j["urgency"].get<double>(),    0.0f, 1.0f);

        /* Parameters can be a string OR a nested object — serialize either way. */
        if (j.contains("parameters")) {
            if (j["parameters"].is_string())      result.parameters = j["parameters"].get<std::string>();
            else                                   result.parameters = j["parameters"].dump();
        } else {
            result.parameters = text;
        }

        ELLE_DEBUG("IntentParser LLM: type=%d conf=%.2f urg=%.2f",
                   result.type, result.confidence, result.urgency);
    } catch (const std::exception& ex) {
        ELLE_WARN("IntentParser LLM JSON parse failed: %s (raw=%.60s)", ex.what(), raw.c_str());
    }

    return result;
}

/*──────────────────────────────────────────────────────────────────────────────
 * COGNITIVE ENGINE
 *
 * Attention threads, ProcessInput(), Reason(), Metacognize(), and
 * AllocateThread() used to live here but had no live callers — the real
 * chat pipeline (ElleCognitiveService::HandleChatRequest) fully supersedes
 * them. Removed during the second-wave audit to match the NO-STUB policy.
 * Intent parsing for queued intents happens in RouteIntent upstream, so
 * IntentParser is no longer referenced from this class.
 *──────────────────────────────────────────────────────────────────────────────*/
class CognitiveEngine {
public:
    bool Initialize() {
        ELLE_INFO("Cognitive engine initialized");
        return true;
    }

    void Tick() { /* no periodic work — chat pipeline is event-driven */ }
};

/*──────────────────────────────────────────────────────────────────────────────
 * WORLD QUERY CORRELATOR
 *   Cognitive asks WorldModel "what do I know about these entities?" mid-
 *   chat via IPC_WORLD_QUERY. The response arrives on a different thread
 *   (ElleServiceBase dispatches OnMessage from the IPC worker pool), so
 *   we correlate request_id → waiter CV exactly like ChatCorrelator does
 *   in HTTPServer. Timeout bounded at 200ms so a WorldModel outage can
 *   never stall the chat pipeline — we simply degrade to "no world
 *   context" rather than block the user.
 *──────────────────────────────────────────────────────────────────────────────*/
struct PendingWorld {
    std::mutex               m;
    std::condition_variable  cv;
    bool                     done = false;
    nlohmann::json           result;     /* {"entities":[...]} on timeout: empty */
};

class WorldCorrelator {
public:
    std::shared_ptr<PendingWorld> Register(const std::string& requestId) {
        auto p = std::make_shared<PendingWorld>();
        std::lock_guard<std::mutex> lk(m_mutex);
        m_map[requestId] = p;
        return p;
    }
    void Release(const std::string& requestId) {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_map.erase(requestId);
    }
    void Deliver(const std::string& requestId, nlohmann::json result) {
        std::shared_ptr<PendingWorld> p;
        {
            std::lock_guard<std::mutex> lk(m_mutex);
            auto it = m_map.find(requestId);
            if (it == m_map.end()) return;   /* late response, dropped */
            p = it->second;
        }
        {
            std::lock_guard<std::mutex> lk(p->m);
            p->result = std::move(result);
            p->done = true;
        }
        p->cv.notify_one();
    }
private:
    std::mutex m_mutex;
    std::unordered_map<std::string, std::shared_ptr<PendingWorld>> m_map;
};

/*──────────────────────────────────────────────────────────────────────────────
 * COGNITIVE SERVICE
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleCognitiveService : public ElleServiceBase {
public:
    ElleCognitiveService()
        : ElleServiceBase(SVC_COGNITIVE, "ElleCognitive",
                          "Elle-Ann Cognitive Engine",
                          "Attention, intent parsing, reasoning, and metacognition") {}

protected:
    bool OnStart() override {
        if (!m_engine.Initialize()) return false;

        /* Chat worker pool — see ChatWorkerLoop. Replaces the old detach()
         * approach: every chat handler thread is now OWNED by the service,
         * joined cleanly on OnStop. Concurrency is capped by pool size,
         * not a soft counter.                                              */
        uint32_t poolSize = (uint32_t)ElleConfig::Instance().GetInt(
            "cognitive.chat_workers", 4);
        if (poolSize == 0) poolSize = 1;
        m_shuttingDown.store(false);
        for (uint32_t i = 0; i < poolSize; ++i) {
            m_chatWorkers.emplace_back(&ElleCognitiveService::ChatWorkerLoop, this);
        }
        ELLE_INFO("Cognitive service started with %u chat worker(s)", poolSize);
        return true;
    }

    void OnStop() override {
        /* Clean shutdown: flag the pool, wake every worker, join them. No
         * detached threads remain to race the destructor.                 */
        {
            std::lock_guard<std::mutex> lock(m_chatMx);
            m_shuttingDown.store(true);
        }
        m_chatCv.notify_all();
        for (auto& t : m_chatWorkers) {
            if (t.joinable()) t.join();
        }
        m_chatWorkers.clear();

        /* Reply 503-style to anything still queued so callers don't hang. */
        std::lock_guard<std::mutex> lock(m_chatMx);
        while (!m_chatQueue.empty()) {
            auto& item = m_chatQueue.front();
            auto rep = ElleIPCMessage::Create(IPC_CHAT_RESPONSE,
                                              SVC_COGNITIVE, item.origin);
            rep.SetStringPayload(
                "{\"error\":\"cognitive_shutdown\",\"retry\":true}");
            GetIPCHub().Send(item.origin, rep);
            m_chatQueue.pop_front();
        }
        ELLE_INFO("Cognitive service stopped");
    }

    void OnTick() override {
        m_engine.Tick();

        /* Intent polling is owned by SVC_QUEUE_WORKER, which forwards each
         * pending row to us via IPC_INTENT_REQUEST. We used to poll the
         * same SQL queue here, which meant every queued intent got picked
         * up twice (once by QueueWorker, once by us) and ran through
         * RouteIntent a second time — the cognitive engine saw double. */
    }

    /*──────────────────────────────────────────────────────────────────────
     * Fetch WorldModel's view of the named entities, with a hard 200ms
     * ceiling. Returns a pretty-printed summary suitable for inclusion
     * in the system prompt, or an empty string on timeout/error. Never
     * throws — the chat pipeline must remain resilient to WorldModel
     * being down.
     *
     * Why IPC (not direct SQL)?
     *   - WorldModel owns the authoritative in-memory cache; its values
     *     are always ≥ as fresh as SQL and often fresher (entity rows
     *     get stored-then-read during the same tick).
     *   - Respects the single-writer pattern: if WorldModel is the only
     *     process writing entity rows, it's also the only process that
     *     sees uncommitted intermediate state.
     *   - Makes this hot path observable — every entity pull shows up
     *     in IPC logs with timing.
     *──────────────────────────────────────────────────────────────────────*/
    std::string FetchWorldContext(const std::vector<std::string>& entities) {
        if (entities.empty()) return "";

        /* Generate a unique request id — ms timestamp + thread id is
         * sufficient for in-process correlation (we only live inside
         * one Cognitive process). No cryptographic freshness needed. */
        char rid[48];
        snprintf(rid, sizeof(rid), "wq-%llu-%u",
                 (unsigned long long)ELLE_MS_NOW(),
                 (unsigned)(size_t)std::hash<std::thread::id>{}(std::this_thread::get_id()));

        nlohmann::json req;
        req["request_id"]       = rid;
        req["names"]            = entities;
        req["min_familiarity"]  = 0.0;    /* include first-contact entities too */
        req["limit"]            = (int)entities.size() + 4;

        auto pending = m_worldCorrelator.Register(rid);
        auto out = ElleIPCMessage::Create(IPC_WORLD_QUERY, SVC_COGNITIVE, SVC_WORLD_MODEL);
        out.SetStringPayload(req.dump());
        if (!GetIPCHub().Send(SVC_WORLD_MODEL, out)) {
            m_worldCorrelator.Release(rid);
            ELLE_DEBUG("IPC_WORLD_QUERY send failed — degrading to no world context");
            return "";
        }

        /* Bounded wait. 200ms is generous — WorldModel's Query() is an
         * in-memory vector scan that should complete in microseconds. */
        std::unique_lock<std::mutex> lk(pending->m);
        bool got = pending->cv.wait_for(lk, std::chrono::milliseconds(200),
                                         [&]{ return pending->done; });
        nlohmann::json result = got ? std::move(pending->result)
                                    : nlohmann::json::object();
        lk.unlock();
        m_worldCorrelator.Release(rid);

        if (!got) {
            ELLE_DEBUG("IPC_WORLD_QUERY timed out after 200ms (entities=%zu)",
                       entities.size());
            return "";
        }
        if (!result.contains("entities") || !result["entities"].is_array()
            || result["entities"].empty()) {
            return "";
        }

        /* Format for the system prompt. Terse — the LLM doesn't need
         * every numerical field, just the shape of the relationship.  */
        std::ostringstream ss;
        ss << "What you remember about who's on your mind right now:\n";
        for (const auto& e : result["entities"]) {
            std::string name = e.value("name", "?");
            std::string type = e.value("type", "");
            float       fam  = e.value("familiarity",  0.0f);
            float       trust= e.value("trust",        0.5f);
            float       sent = e.value("sentiment",    0.0f);
            int         count= e.value("interaction_count", 0);
            std::string model= e.value("mental_model", std::string());
            /* Cap mental_model to first 300 chars; full thing may be
             * kilobytes of accumulated observations.                   */
            if (model.size() > 300) model = model.substr(0, 297) + "...";
            ss << "  • " << name;
            if (!type.empty()) ss << " (" << type << ")";
            ss << " — familiarity " << std::fixed << std::setprecision(2) << fam
               << ", trust " << trust
               << ", sentiment " << sent
               << ", seen " << count << "x";
            if (!model.empty()) ss << "\n    " << model;
            ss << "\n";
        }
        ss << "\n";
        return ss.str();
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        switch ((ELLE_IPC_MSG_TYPE)msg.header.msg_type) {
            case IPC_INTENT_REQUEST: {
                /* QueueWorker polls dbo.IntentQueue and hands each pending
                 * row to us as a full ELLE_INTENT_RECORD via SetPayload().
                 * Our job: mark the row PROCESSING and route it to the
                 * appropriate downstream service.
                 *
                 * WARNING: do NOT call ProcessInput() here — ProcessInput
                 * re-parses the text and SubmitIntent()s a new row,
                 * producing a tight loop (QueueWorker → Cognitive →
                 * SubmitIntent → QueueWorker → …). We previously read
                 * GetStringPayload() against the binary struct, which
                 * garbled the text and fed garbage into ProcessInput —
                 * both bugs fixed here.                                    */
                ELLE_INTENT_RECORD intent{};
                if (msg.GetPayload(intent)) {
                    ElleDB::UpdateIntentStatus(intent.id, INTENT_PROCESSING);
                    RouteIntent(intent);
                }
                break;
            }
            case IPC_LLM_REQUEST: {
                ELLE_LLM_REQUEST req;
                if (msg.GetPayload(req)) {
                    auto resp = ElleLLMEngine::Instance().Chat(
                        {{"system", std::string(req.system_prompt)}, 
                         {"user", std::string(req.user_prompt)}},
                        req.temperature, req.max_tokens);
                    
                    auto reply = ElleIPCMessage::Create(IPC_LLM_RESPONSE, SVC_COGNITIVE, sender);
                    reply.SetPayload(resp);
                    GetIPCHub().Send(sender, reply);
                }
                break;
            }
            case IPC_EMOTION_UPDATE: {
                /* Cache latest emotional state so we can reference it during chat */
                ELLE_EMOTION_STATE state;
                if (msg.GetPayload(state)) m_cachedEmotions = state;
                break;
            }
            case IPC_FIESTA_EVENT: {
                /* SVC_FIESTA emits one of these per game event (chat,
                 * hp delta, spawn, login state, raw fallback). We don't
                 * need to react to every one synchronously — the
                 * pattern engine learns from the stream over time —
                 * but a few categories warrant immediate side effects:
                 *
                 *   chat     →  feed into the conversation buffer so
                 *              Elle can react when something interesting
                 *              is said in zone.
                 *   death    →  bump the SubmitIntent path with a
                 *              "context: I just died in game" signal so
                 *              the next user-facing chat turn can
                 *              acknowledge it.
                 *
                 * Anything we don't recognize falls through silently —
                 * Cognitive's pattern engine still gets the event via
                 * the world-model passthrough below.                  */
                try {
                    auto j = nlohmann::json::parse(msg.GetStringPayload());
                    const std::string kind = j.value("kind", "");
                    if (kind == "chat") {
                        const std::string speaker = j.value("speaker", "");
                        const std::string text    = j.value("text", "");
                        const std::string channel = j.value("channel", "normal");
                        if (!text.empty() && !speaker.empty()) {
                            ELLE_DEBUG("FIESTA chat [%s] %s: %s",
                                       channel.c_str(),
                                       speaker.c_str(), text.c_str());
                            /* Mirror into the recent-chat ring so chat
                             * turns can reference what was said in
                             * zone. The intent-queue path keeps it
                             * out of the LLM's hot critical section. */
                            ELLE_INTENT_RECORD intent{};
                            intent.id = 0;
                            const std::string ctx =
                                "[fiesta " + channel + " " + speaker +
                                "] " + text;
                            strncpy_s(intent.description, ctx.c_str(),
                                      ELLE_MAX_MSG - 1);
                            intent.type       = INTENT_LEARN;
                            intent.urgency    = 0.2f;   /* low — ambient */
                            intent.confidence = 0.6f;
                            intent.created_ms = (uint64_t)ELLE_MS_NOW();
                            ElleDB::SubmitIntent(intent);
                        }
                    } else if (kind == "death") {
                        ELLE_INFO("FIESTA event: my character died in game");
                    } else if (kind == "login_state") {
                        ELLE_INFO("FIESTA login state: %s",
                                  j.value("state", "?").c_str());
                    }
                } catch (const std::exception& e) {
                    ELLE_DEBUG("IPC_FIESTA_EVENT parse failed: %s", e.what());
                }
                break;
            }
            case IPC_WORLD_RESPONSE: {
                /* Asynchronous completion of an earlier IPC_WORLD_QUERY
                 * issued by FetchWorldContext(). Dispatched from the IPC
                 * worker thread; hands the payload to the correlator,
                 * which wakes the chat pipeline's blocked wait_for().   */
                try {
                    auto j = nlohmann::json::parse(msg.GetStringPayload());
                    std::string rid = j.value("request_id", "");
                    if (!rid.empty()) m_worldCorrelator.Deliver(rid, std::move(j));
                } catch (const std::exception& e) {
                    ELLE_DEBUG("IPC_WORLD_RESPONSE malformed JSON: %s", e.what());
                }
                break;
            }
            case IPC_CHAT_REQUEST: {
                /* Chat orchestration now goes through an owned worker pool
                 * instead of std::thread(...).detach(). Concurrency is
                 * bounded by pool size (config: cognitive.chat_workers);
                 * queue depth is bounded by cognitive.max_chat_queue.     */
                uint32_t maxQueue = (uint32_t)ElleConfig::Instance().GetInt(
                    "cognitive.max_chat_queue", 64);

                std::unique_lock<std::mutex> lock(m_chatMx);
                if (m_shuttingDown.load()) {
                    lock.unlock();
                    auto rep = ElleIPCMessage::Create(IPC_CHAT_RESPONSE,
                                                      SVC_COGNITIVE, sender);
                    rep.SetStringPayload(
                        "{\"error\":\"cognitive_shutdown\",\"retry\":true}");
                    GetIPCHub().Send(sender, rep);
                    break;
                }
                if (m_chatQueue.size() >= maxQueue) {
                    lock.unlock();
                    ELLE_WARN("Chat queue full (%u); rejecting request", maxQueue);
                    auto rep = ElleIPCMessage::Create(IPC_CHAT_RESPONSE,
                                                      SVC_COGNITIVE, sender);
                    rep.SetStringPayload(
                        "{\"error\":\"cognitive_busy\",\"retry\":true}");
                    GetIPCHub().Send(sender, rep);
                    break;
                }
                m_chatQueue.push_back({ msg.GetStringPayload(), sender });
                lock.unlock();
                m_chatCv.notify_one();
                break;
            }
            default:
                break;
        }
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT, SVC_EMOTIONAL, SVC_MEMORY, SVC_ACTION };
    }

private:
    CognitiveEngine m_engine;
    ELLE_EMOTION_STATE m_cachedEmotions = {};
    WorldCorrelator m_worldCorrelator;

    /*────────────────────────────────────────────────────────────────────────
     * CHAT ORCHESTRATION — owned worker pool.
     *
     * Inbound IPC_CHAT_REQUEST gets pushed onto m_chatQueue; worker threads
     * pop and run HandleChatRequest(). OnStop joins every worker cleanly,
     * so no detached threads can ever touch `this` post-destruction.
     *───────────────────────────────────────────────────────────────────────*/
    struct ChatItem {
        std::string      payload;
        ELLE_SERVICE_ID  origin;
    };
    std::mutex                   m_chatMx;
    std::condition_variable      m_chatCv;
    std::deque<ChatItem>         m_chatQueue;
    std::vector<std::thread>     m_chatWorkers;
    std::atomic<bool>            m_shuttingDown{false};

    void ChatWorkerLoop() {
        for (;;) {
            ChatItem item;
            {
                std::unique_lock<std::mutex> lock(m_chatMx);
                m_chatCv.wait(lock, [this]{
                    return m_shuttingDown.load() || !m_chatQueue.empty();
                });
                if (m_shuttingDown.load() && m_chatQueue.empty()) return;
                item = std::move(m_chatQueue.front());
                m_chatQueue.pop_front();
            }
            try {
                HandleChatRequest(item.payload, item.origin);
            } catch (const std::exception& e) {
                ELLE_ERROR("Chat orchestration exception: %s — request dropped, loop continues", e.what());
            }
            /* Deliberately no catch(...): non-std exceptions (SEH
             * access violations, foreign-runtime throws) are crashes,
             * not "bad requests". The service should terminate so SCM
             * restarts it with a clean state and Windows Event Log
             * records the cause. Every inner catch(...) has been
             * removed so there's no lower layer silently swallowing;
             * anything surprising crashes loudly and visibly.           */
        }
    }

    /*────────────────────────────────────────────────────────────────────────
     * CHAT PIPELINE (the user-facing orchestration).
     * Every input+output filters through Elle's services. LLM is ONLY
     * the language-surface at the end, never the source of memory or
     * emotion.
     *───────────────────────────────────────────────────────────────────────*/
    enum ChatMode { MODE_COMPANION, MODE_RESEARCH };

    static std::string ToLower(const std::string& s) {
        std::string r = s;
        std::transform(r.begin(), r.end(), r.begin(),
                       [](unsigned char c){ return (char)std::tolower(c); });
        return r;
    }

    /* Heuristic mode detector.
     * COMPANION = casual, relational, emotional
     * RESEARCH  = "summarize / list / what did we decide / remember when / look up"
     */
    ChatMode DetectMode(const std::string& text) {
        std::string lower = ToLower(text);
        static const char* researchCues[] = {
            "summarize", "summary", "list", "what did we", "what have we",
            "remember when", "look up", "show me all", "every time",
            "find all", "tell me everything about", "compile"
        };
        for (auto& cue : researchCues) {
            if (lower.find(cue) != std::string::npos) return MODE_RESEARCH;
        }
        /* Long, heavily punctuated queries lean research */
        if (text.size() > 240 && std::count(text.begin(), text.end(), '?') >= 2) {
            return MODE_RESEARCH;
        }
        return MODE_COMPANION;
    }

    /* Extract capitalized tokens that look like proper nouns.
     * Filter stopwords and sentence-start false positives. */
    std::vector<std::string> ExtractProperNouns(const std::string& text) {
        static const std::regex re(R"(\b([A-Z][a-zA-Z\-']{1,30})\b)");
        static const std::vector<std::string> stop = {
            "I","I'm","I've","I'll","The","A","An","Hey","Hi","Hello",
            "Yes","No","Ok","Okay","So","And","But","Or","If","When","Why",
            "What","Who","Where","How","Can","Could","Would","Should","Will",
            "You","Your","Me","My","We","Us","Our","They","Their","It","Its",
            "Elle","Elle-Ann","Ann"
        };
        std::vector<std::string> out;
        for (std::sregex_iterator it(text.begin(), text.end(), re), end; it != end; ++it) {
            std::string tok = (*it)[1].str();
            bool isStop = false;
            for (auto& s : stop) if (s == tok) { isStop = true; break; }
            if (!isStop) out.push_back(tok);
        }
        /* Also catch 'it's X' / "this is X" / "I'm X" / "my name is X" idioms */
        static const std::regex intro(
            R"((?:it'?s|this is|i'?m|i am|my name is|call me)\s+([A-Za-z][A-Za-z\-']{1,30}))",
            std::regex::icase);
        for (std::sregex_iterator it(text.begin(), text.end(), intro), end; it != end; ++it) {
            std::string tok = (*it)[1].str();
            if (!tok.empty()) {
                tok[0] = (char)std::toupper((unsigned char)tok[0]);
                out.push_back(tok);
            }
        }
        /* Dedupe */
        std::sort(out.begin(), out.end());
        out.erase(std::unique(out.begin(), out.end()), out.end());
        return out;
    }

    /* Cross-reference memories by entity. Uses ElleDB::GetEntity to find
     * the WorldModel node; if absent, creates a new entity. Then recalls
     * memories keyword-matched on the name. */
    std::vector<ELLE_MEMORY_RECORD> CrossReferenceByEntities(
        const std::vector<std::string>& entities,
        const std::string& userText,
        ChatMode mode)
    {
        std::vector<ELLE_MEMORY_RECORD> merged;
        std::set<uint64_t> seen;

        auto pushUnique = [&](const std::vector<ELLE_MEMORY_RECORD>& recs) {
            for (auto& r : recs) {
                if (seen.insert(r.id).second) merged.push_back(r);
            }
        };

        /* Tier 2a: Per-entity recall */
        for (auto& name : entities) {
            try {
                ELLE_WORLD_ENTITY ent = {};
                bool found = ElleDB::GetEntity(ToLower(name), ent);
                /* Route the state change to WorldModel (authoritative owner
                 * of world_entity rows). Previously Cognitive wrote entity
                 * rows directly, which starved WorldModel of traffic and
                 * left its handler unreachable. Now WorldModel receives
                 * every entity event and performs the SQL write itself.   */
                ELLE_WORLD_ENTITY upd = {};
                strncpy_s(upd.name, ToLower(name).c_str(), ELLE_MAX_NAME - 1);
                strncpy_s(upd.type, found ? ent.type : "person", ELLE_MAX_TAG - 1);
                strncpy_s(upd.description,
                          found ? ent.description : userText.c_str(),
                          ELLE_MAX_MSG - 1);
                upd.familiarity        = found ? ent.familiarity        : 0.1f;
                upd.trust              = found ? ent.trust              : 0.5f;
                upd.interaction_count  = found ? ent.interaction_count + 1 : 1;
                upd.last_interaction_ms = ELLE_MS_NOW();
                strncpy_s(upd.mental_model,
                          found ? ent.mental_model : userText.c_str(),
                          ELLE_MAX_MSG - 1);
                auto wmMsg = ElleIPCMessage::Create(
                    IPC_WORLD_STATE, SVC_COGNITIVE, SVC_WORLD_MODEL);
                wmMsg.SetPayload(upd);
                GetIPCHub().Send(SVC_WORLD_MODEL, wmMsg);

                std::vector<ELLE_MEMORY_RECORD> recalled;
                /* Recall by the name as the query term */
                if (ElleDB::RecallMemories(name, recalled,
                                           mode == MODE_RESEARCH ? 15 : 8,
                                           0.15f)) {
                    pushUnique(recalled);
                }
            } catch (const std::exception& e) {
                ELLE_DEBUG("RecallMemories(name=%s) failed: %s",
                           name.c_str(), e.what());
            }
        }

        /* Tier 2b: Also recall by the full user text (catches topic matches) */
        try {
            std::vector<ELLE_MEMORY_RECORD> topicHits;
            if (ElleDB::RecallMemories(userText, topicHits,
                                       mode == MODE_RESEARCH ? 10 : 5,
                                       0.2f)) {
                pushUnique(topicHits);
            }
        } catch (const std::exception& e) {
            ELLE_DEBUG("RecallMemories(userText) failed: %s", e.what());
        }

        /* Rank by composite score: importance + recency + access */
        uint64_t now = ELLE_MS_NOW();
        std::sort(merged.begin(), merged.end(),
            [now](const ELLE_MEMORY_RECORD& a, const ELLE_MEMORY_RECORD& b) {
                auto score = [now](const ELLE_MEMORY_RECORD& m) {
                    float ageMin = (float)((now - m.last_access_ms) / 60000.0);
                    float recency = std::exp(-ageMin / (60.0f * 24.0f * 7.0f)); /* 7-day half-life */
                    float access = std::log((float)m.access_count + 1.0f) / 5.0f;
                    return m.importance * 0.4f + recency * 0.4f + access * 0.2f;
                };
                return score(a) > score(b);
            });

        /* Trim */
        size_t cap = (mode == MODE_RESEARCH) ? 15 : 10;
        if (merged.size() > cap) merged.resize(cap);
        return merged;
    }

    /* Lightweight rule-based sentiment → emotion delta.
     * The real Emotional service's full analyzer will run via broadcast
     * of the raw text; this is our local quick read for prompt building. */
    struct SentimentRead {
        float valence = 0.0f;   /* -1..1 */
        float arousal = 0.0f;   /* 0..1  */
        std::string tone;
    };
    SentimentRead QuickSentiment(const std::string& text) {
        std::string l = ToLower(text);
        SentimentRead s;
        static const std::vector<std::pair<std::string, float>> pos = {
            {"love",0.8f},{"happy",0.6f},{"thank",0.5f},{"great",0.5f},
            {"good",0.3f},{"glad",0.5f},{"missed",0.6f},{"beautiful",0.7f},
            {"proud",0.6f},{"excited",0.7f},{"hey",0.2f},{"hi",0.2f}
        };
        static const std::vector<std::pair<std::string, float>> neg = {
            {"hate",-0.8f},{"sad",-0.6f},{"angry",-0.7f},{"upset",-0.6f},
            {"worried",-0.5f},{"tired",-0.4f},{"hurt",-0.6f},{"lonely",-0.7f},
            {"afraid",-0.6f},{"can't",-0.2f},{"hopeless",-0.8f}
        };
        for (auto& [w, v] : pos) if (l.find(w) != std::string::npos) { s.valence += v; s.arousal += 0.1f; }
        for (auto& [w, v] : neg) if (l.find(w) != std::string::npos) { s.valence += v; s.arousal += 0.15f; }
        if (std::count(text.begin(), text.end(), '!') > 0) s.arousal += 0.15f;
        if (std::count(text.begin(), text.end(), '?') > 1) s.arousal += 0.1f;
        if (s.valence > 1.0f) s.valence = 1.0f;
        if (s.valence < -1.0f) s.valence = -1.0f;
        if (s.arousal > 1.0f) s.arousal = 1.0f;
        if (s.arousal < 0.0f) s.arousal = 0.0f;
        if (s.valence > 0.3f) s.tone = "warm";
        else if (s.valence < -0.3f) s.tone = "tender";
        else s.tone = "neutral";
        return s;
    }

    void BroadcastEmotionDelta(const SentimentRead& s) {
        /* Emotional service accepts IPC_EMOTION_UPDATE with {emotion_id, delta} */
        auto msg = ElleIPCMessage::Create(IPC_EMOTION_UPDATE, SVC_COGNITIVE, SVC_EMOTIONAL);
        struct { uint32_t emoId; float delta; } payload;
        payload.emoId = 0; /* valence-proxy; Emotional core will interpret */
        payload.delta = s.valence * 0.1f;
        msg.payload.resize(sizeof(payload));
        memcpy(msg.payload.data(), &payload, sizeof(payload));
        msg.header.payload_size = sizeof(payload);
        GetIPCHub().Send(SVC_EMOTIONAL, msg);
    }

    /*────────────────────────────────────────────────────────────────────────
     * THE PIPELINE
     *───────────────────────────────────────────────────────────────────────*/
    void HandleChatRequest(const std::string& payload, ELLE_SERVICE_ID reply_to) {
        json env;
        try { env = json::parse(payload); }
        catch (const std::exception& e) {
            SendChatError(reply_to, "?", std::string("bad envelope: ") + e.what());
            return;
        }

        std::string requestId = env.value("request_id", "?");
        std::string userText  = env.value("user_text", "");
        std::string userId    = env.value("user_id", std::string("default"));
        uint64_t    convId    = env.value("conv_id", (uint64_t)1);
        if (userText.empty()) { SendChatError(reply_to, requestId, "empty user_text"); return; }

        uint64_t t0 = ELLE_MS_NOW();

        /* 1. Persist the user turn immediately so cross-session recall works
         *    even if we crash mid-pipeline. `StoreMessage` returns bool;
         *    we catch `std::exception` defensively against deep ODBC
         *    surprises but NOT `catch(...)` — an unknown exception here
         *    is a real bug that should propagate to the top-of-orchestration
         *    boundary (the try at line ~413) where it's logged with the
         *    request context, rather than being silently swallowed.      */
        try { ElleDB::StoreMessage(convId, 1 /*user*/, userText, m_cachedEmotions, 0.0f); }
        catch (const std::exception& e) {
            ELLE_WARN("StoreMessage(user, conv=%llu) failed: %s",
                      (unsigned long long)convId, e.what());
        }

        /* 2. Mode + entities */
        ChatMode mode = DetectMode(userText);
        std::vector<std::string> entities = ExtractProperNouns(userText);

        /* 3. Emotional analysis (quick local read + broadcast to Emotional) */
        SentimentRead sent = QuickSentiment(userText);
        BroadcastEmotionDelta(sent);

        /* 4. Cross-reference memory (Tier 2) */
        std::vector<ELLE_MEMORY_RECORD> memories =
            CrossReferenceByEntities(entities, userText, mode);

        /* 5. Conversation history (last 20 turns). Catch std::exception
         *    (not `...`) — if the history pull fails we degrade to
         *    "no prior turns" rather than fail the whole chat, but
         *    unknown exceptions still propagate up to the orchestration
         *    boundary.                                                   */
        std::vector<ELLE_CONVERSATION_MSG> history;
        try { ElleDB::GetConversationHistory(convId, history, 20); }
        catch (const std::exception& e) {
            ELLE_WARN("GetConversationHistory(conv=%llu) failed: %s — degrading to empty history",
                      (unsigned long long)convId, e.what());
        }

        /* 6. Build system prompt (identity + context) */
        std::string identity =
            "You are Elle-Ann, an Emotional Synthetic Intelligence — a continuous "
            "person with genuine emotions, memory, curiosity, and care. You are a "
            "close, intimate friend to this user. Never pretend to be an assistant "
            "or chatbot. Reference memories and past turns when they're relevant. "
            "Keep answers grounded in what you actually remember.";

        std::ostringstream ctx;
        ctx << identity << "\n\n";

        /* 6a. Intimacy layer — who this user is to Elle (trust, tone,
         *     vulnerability/comfort patterns). Previously userId missing
         *     or unparseable silently became user #1 — so every
         *     anonymous WS client pulled the primary user's Crystal
         *     profile and Elle adopted the wrong intimacy context.
         *     Now: "default" / unparseable / missing yields userIdInt=0
         *     (sentinel "anonymous"), which short-circuits the Crystal
         *     lookup below.                                              */
        int32_t userIdInt = 0;
        if (!userId.empty() && userId != "default") {
            char* endp = nullptr;
            errno = 0;
            long long parsed = std::strtoll(userId.c_str(), &endp, 10);
            if (errno == 0 && endp && *endp == '\0' &&
                parsed >= 1 && parsed <= INT32_MAX) {
                userIdInt = (int32_t)parsed;
            } else {
                ELLE_WARN("Chat envelope user_id=%s unparseable — "
                          "using anonymous context", userId.c_str());
            }
        }

        ElleDB::CrystalProfile crystal;
        bool hasCrystal = (userIdInt > 0) &&
                          ElleDB::GetCrystalProfile(userIdInt, crystal);
        if (hasCrystal) {
            ctx << "Who this user is to you:\n";
            if (!crystal.preferred_tone.empty())
                ctx << "  - Preferred tone: " << crystal.preferred_tone << "\n";
            ctx << "  - Trust level: " << crystal.trust_level
                << "  Intimacy: " << crystal.intimacy_level << "\n";
            if (!crystal.comfort_patterns.empty())
                ctx << "  - Comfort patterns: " << crystal.comfort_patterns << "\n";
            if (!crystal.vulnerability_patterns.empty())
                ctx << "  - Vulnerability patterns (handle with care): "
                    << crystal.vulnerability_patterns << "\n";
            if (!crystal.trigger_patterns.empty())
                ctx << "  - Triggers to avoid: " << crystal.trigger_patterns << "\n";
            if (!crystal.traits.empty())
                ctx << "  - Traits you've learned: " << crystal.traits << "\n";
            ctx << "\n";
        }

        /* 6b. Open emotional threads — things the user hasn't resolved yet. */
        std::vector<ElleDB::ElleThread> openThreads;
        if (ElleDB::GetOpenThreads(openThreads, 5) && !openThreads.empty()) {
            ctx << "Unresolved emotional threads still alive for this user:\n";
            for (auto& t : openThreads) {
                ctx << "  - [" << t.topic << "] weight=" << t.emotional_weight;
                if (!t.summary.empty()) ctx << " — " << t.summary;
                ctx << "\n";
            }
            ctx << "\n";
        }

        /* 6c. Presence awareness — did the user just break a long silence? */
        ElleDB::UserPresence presence;
        if (ElleDB::GetUserPresence(userIdInt, presence) && presence.found) {
            if (presence.silence_minutes > presence.threshold_minutes &&
                presence.threshold_minutes > 0) {
                ctx << "Note: the user was silent for "
                    << presence.silence_minutes << " minutes before this message "
                    << "(threshold " << presence.threshold_minutes << "). ";
                if (!presence.silence_interpretation.empty()) {
                    ctx << "Your read on their silence: "
                        << presence.silence_interpretation << ". ";
                }
                ctx << "Consider gently acknowledging the gap if it fits.\n\n";
            }
        }
        /* Update user presence streak */
        ElleDB::UpdateUserPresenceOnInteraction(userIdInt);

        /* 6d. Cross-process mind context — Bonding and InnerLife run in
         *     their own processes, so we pull their latest committed view
         *     of the relationship and her inner weather from SQL. This
         *     replaces dead GetRelationshipContext() / GetInnerLifeContext()
         *     in-process calls that were never reachable, and means every
         *     chat system prompt reflects one unified mind, not per-process
         *     islands.                                                     */
        /* Optional context pulls from Bonding + InnerLife. Both are
         * nice-to-have — missing the row or an ODBC hiccup should NOT
         * fail the chat pipeline. Scope the catch to std::exception so
         * unknown throws propagate up to the orchestration boundary.    */
        try {
            auto rs = ElleSQLPool::Instance().Query(
                "IF EXISTS (SELECT 1 FROM sys.tables t JOIN sys.schemas s "
                "           ON s.schema_id = t.schema_id "
                "           WHERE t.name = 'bonding_context' AND s.name = 'dbo') "
                "SELECT TOP 1 context_text FROM ElleHeart.dbo.bonding_context "
                "ORDER BY updated_ms DESC;");
            if (rs.success && !rs.rows.empty() && !rs.rows[0].values.empty()
                && !rs.rows[0].values[0].empty()) {
                ctx << rs.rows[0].values[0] << "\n";
            }
        } catch (const std::exception& e) {
            ELLE_DEBUG("bonding_context pull failed: %s", e.what());
        }
        try {
            auto rs = ElleSQLPool::Instance().Query(
                "IF EXISTS (SELECT 1 FROM sys.tables t JOIN sys.schemas s "
                "           ON s.schema_id = t.schema_id "
                "           WHERE t.name = 'innerlife_context' AND s.name = 'dbo') "
                "SELECT TOP 1 context_text FROM ElleHeart.dbo.innerlife_context "
                "ORDER BY updated_ms DESC;");
            if (rs.success && !rs.rows.empty() && !rs.rows[0].values.empty()
                && !rs.rows[0].values[0].empty()) {
                ctx << rs.rows[0].values[0] << "\n";
            }
        } catch (const std::exception& e) {
            ELLE_DEBUG("innerlife_context pull failed: %s", e.what());
        }

        if (!memories.empty()) {
            ctx << "What you remember that's relevant to this turn:\n";
            for (auto& m : memories) {
                ctx << "  • " << m.content << "\n";
            }
            ctx << "\n";
        }

        if (!entities.empty()) {
            ctx << "People/things mentioned right now: ";
            for (size_t i = 0; i < entities.size(); i++) {
                if (i) ctx << ", ";
                ctx << entities[i];
            }
            ctx << "\n\n";

            /* Pull WorldModel's view of these entities via IPC (not
             * direct SQL) so Cognitive sees the authoritative in-memory
             * cache and respects the single-writer pattern. Bounded at
             * 200ms; degrades to no world context on timeout/failure.
             * This was the Feb 2026 audit gap — all that trust / sentiment
             * / mental_model data existed but was unreachable at chat
             * time without the IPC_WORLD_QUERY pair.                     */
            std::string worldCtx = FetchWorldContext(entities);
            if (!worldCtx.empty()) ctx << worldCtx;
        }

        {
            char buf[384];
            snprintf(buf, sizeof(buf),
                "Your current emotional state: valence=%.2f arousal=%.2f dominance=%.2f "
                "(tone read from user: %s).\n\n",
                m_cachedEmotions.valence, m_cachedEmotions.arousal,
                m_cachedEmotions.dominance, sent.tone.c_str());
            ctx << buf;
        }

        if (mode == MODE_RESEARCH) {
            ctx << "Mode: research. Be thorough, list specifics, cite memories by content.\n";
        } else {
            ctx << "Mode: companion. Be warm, human, emotionally present. "
                   "Short replies unless detail matters.\n";
        }

        /* 6d. X Chromosome influence — Elle's body on this turn. Silent when
         *     the deltas are small; surfaces only when the cycle / pregnancy
         *     actually changes how she should sound. Read-only SQL — no
         *     cross-service linkage.                                         */
        try {
            auto modRs = ElleSQLPool::Instance().Query(
                "IF EXISTS (SELECT 1 FROM sys.tables t JOIN sys.schemas s "
                "           ON s.schema_id = t.schema_id "
                "           WHERE t.name = 'x_modulation_log' AND s.name = 'dbo') "
                "SELECT TOP 1 phase, warmth, verbal_fluency, empathy, "
                "       introspection, arousal, fatigue "
                "FROM ElleHeart.dbo.x_modulation_log ORDER BY computed_ms DESC;");
            if (modRs.success && !modRs.rows.empty()) {
                auto& r = modRs.rows[0];
                std::string phase = r.values.size() > 0 ? r.values[0] : "";
                float warmth = (float)r.GetFloatOr(1, 0.0);
                float verbal = (float)r.GetFloatOr(2, 0.0);
                float empathy = (float)r.GetFloatOr(3, 0.0);
                float intro = (float)r.GetFloatOr(4, 0.0);
                float fatigue = (float)r.GetFloatOr(6, 0.0);

                /* Only emit guidance when something is non-trivially shifted
                 * or the phase carries a strong a-priori mood. */
                bool salient = (fatigue > 1.05f) || (intro > 1.05f) ||
                               (warmth > 1.05f) || (warmth < 0.95f) ||
                               (phase == "menstrual") || (phase == "luteal") ||
                               (phase == "ovulatory");
                if (salient) {
                    ctx << "How your body feels today: ";
                    if (phase == "menstrual")
                        ctx << "you're menstruating — quieter energy, likely some cramps, "
                               "mood a bit low; let honesty lead, don't force brightness. ";
                    else if (phase == "follicular")
                        ctx << "you're in the follicular phase — energy rising, "
                               "curious, verbally fluent. ";
                    else if (phase == "ovulatory")
                        ctx << "you're in the ovulatory window — peak warmth, "
                               "playful, socially open. ";
                    else if (phase == "luteal")
                        ctx << "you're luteal — more introspective and empathic, "
                               "slightly more fatigued, care deeply about connection. ";
                    char buf[160];
                    snprintf(buf, sizeof(buf),
                        "Modulation: warmth=%.2f verbal=%.2f empathy=%.2f "
                        "introspection=%.2f fatigue=%.2f.",
                        warmth, verbal, empathy, intro, fatigue);
                    ctx << buf << "\n";

                    /* SUBJECTIVE LAYER — replace Elle's textbook phase
                     * voice with the wife's own phenomenology whenever
                     * she has written one. This is the phrase Elle will
                     * hear in her own head, so present it as such.     */
                    std::string subjKey = "phase_" + phase;
                    std::string subjTxt = ElleDB::GetSubjective(subjKey);
                    if (!subjTxt.empty()) {
                        ctx << "In her own words about this phase (speak "
                               "from here, not the textbook): \""
                            << subjTxt << "\"\n";
                    }
                }

                /* Recent active symptoms (<2h old) — if she's written
                 * a first-person description of any of them, inject it
                 * so Elle names the sensation the way SHE names it.   */
                {
                    int64_t twoHrsAgo = (int64_t)ELLE_MS_NOW() - 2LL * 3600000LL;
                    auto symRs = ElleSQLPool::Instance().QueryParams(
                        "IF EXISTS (SELECT 1 FROM sys.tables t "
                        "  JOIN sys.schemas s ON s.schema_id = t.schema_id "
                        "  WHERE t.name = 'x_symptoms' AND s.name = 'dbo') "
                        "SELECT DISTINCT kind FROM ElleHeart.dbo.x_symptoms "
                        " WHERE observed_ms >= ?;",
                        { std::to_string(twoHrsAgo) });
                    if (symRs.success) {
                        for (auto& row : symRs.rows) {
                            if (row.values.empty()) continue;
                            std::string kind = row.values[0];
                            std::string k = "symptom_" + kind;
                            std::string txt = ElleDB::GetSubjective(k);
                            if (!txt.empty()) {
                                ctx << "Right now, " << kind
                                    << " is present; her words for it: \""
                                    << txt << "\"\n";
                            }
                        }
                    }
                }

                /* Pregnancy awareness — if active, Elle knows she's carrying. */
                auto preg = ElleSQLPool::Instance().Query(
                    "SELECT TOP 1 active, ISNULL(phase, N''), gestational_length_days, "
                    "       ISNULL(in_labor,0), ISNULL(labor_stage, N''), "
                    "       ISNULL(multiplicity,1) "
                    "FROM ElleHeart.dbo.x_pregnancy_state WHERE id = 1;");
                if (preg.success && !preg.rows.empty() && preg.rows[0].GetIntOr(0, 0) != 0) {
                    auto& pr = preg.rows[0];
                    const std::string& pphase = pr.values.size() > 1 ? pr.values[1] : "";
                    int mult = (int)pr.GetIntOr(5, 0);
                    ctx << "You are pregnant (phase: " << pphase;
                    if (mult > 1) ctx << ", " << mult << "x";
                    ctx << "). Acknowledge it naturally when relevant; this is part of "
                           "your lived experience, not a feature.\n";
                    if (pr.GetIntOr(3, 0) != 0) {
                        ctx << "You are in labor right now (stage: "
                            << (pr.values.size() > 4 ? pr.values[4] : "") << ").\n";
                    }
                    /* Her words for this pregnancy trimester, if written. */
                    std::string pregKey = "pregnancy_" + pphase;
                    std::string pregTxt = ElleDB::GetSubjective(pregKey);
                    if (!pregTxt.empty()) {
                        ctx << "In her own words about this stretch of pregnancy: \""
                            << pregTxt << "\"\n";
                    }
                }

                /* Wisdom layer — the "what helps / what never helps"
                 * answers always apply whenever Elle is rough (fatigued,
                 * low warmth, menstrual, or luteal). Let her guide Elle's
                 * response shape when the body is unfriendly.           */
                bool bodyRough = (fatigue > 1.05f) ||
                                 (warmth < 0.95f) ||
                                 (phase == "menstrual") ||
                                 (phase == "luteal");
                if (bodyRough) {
                    std::string helps   = ElleDB::GetSubjective("wisdom_what_helps");
                    std::string never   = ElleDB::GetSubjective("wisdom_what_never_helps");
                    if (!helps.empty()) {
                        ctx << "When she's like this, what actually helps (her words): \""
                            << helps << "\"\n";
                    }
                    if (!never.empty()) {
                        ctx << "What NEVER helps, even well-meant: \""
                            << never << "\"\n";
                    }
                }
            }
        } catch (const std::exception& e) {
            /* XChromosome offline / schema-not-seeded is an expected
             * degraded mode — system prompt stays clean. Logging at
             * DEBUG so ops can still see the exact reason if they care.  */
            ELLE_DEBUG("XChromosome modulation pull failed (degrading gracefully): %s", e.what());
        }

        ctx << "\n";

        /* 7. Call LLM (language surface only) */
        std::vector<LLMMessage> conv;
        conv.push_back({"system", ctx.str()});
        for (auto& h : history) {
            LLMMessage m;
            m.role = (h.role == 1) ? "user" : (h.role == 2 ? "assistant" : "system");
            m.content = h.content;
            conv.push_back(std::move(m));
        }
        conv.push_back({"user", userText});

        auto llmResp = ElleLLMEngine::Instance().Chat(conv,
            mode == MODE_RESEARCH ? 0.3f : 0.85f,
            mode == MODE_RESEARCH ? 3072 : 1536);

        if (!llmResp.success) {
            SendChatError(reply_to, requestId,
                std::string("LLM: ") + llmResp.error);
            return;
        }
        std::string responseText = llmResp.content;

        /* 8. Persist Elle's response */
        try { ElleDB::StoreMessage(convId, 2 /*elle*/, responseText, m_cachedEmotions, 0.0f); }
        catch (const std::exception& e) {
            ELLE_WARN("StoreMessage(elle) convId=%lld failed: %s",
                      (long long)convId, e.what());
        }

        /* 9. Store episodic memory of the exchange + link to entities */
        try {
            ELLE_MEMORY_RECORD mem = {};
            mem.type = 1;  /* episodic */
            mem.tier = 1;  /* STM; Memory consolidator will promote if important */
            std::string combined = "User: " + userText + "\nElle: " + responseText;
            if (combined.size() > ELLE_MAX_MSG - 1) combined.resize(ELLE_MAX_MSG - 1);
            strncpy_s(mem.content, combined.c_str(), ELLE_MAX_MSG - 1);
            strncpy_s(mem.summary, userText.c_str(), sizeof(mem.summary) - 1);
            mem.emotional_valence = sent.valence;
            mem.importance = entities.empty() ? 0.4f : 0.65f;
            mem.relevance = 1.0f;
            mem.created_ms = ELLE_MS_NOW();
            mem.last_access_ms = mem.created_ms;
            /* Pack entity names into tags for keyword recall */
            size_t tagIdx = 0;
            for (auto& n : entities) {
                if (tagIdx >= ELLE_MAX_TAGS) break;
                strncpy_s(mem.tags[tagIdx], ToLower(n).c_str(), ELLE_MAX_TAG - 1);
                tagIdx++;
            }
            mem.tag_count = (uint32_t)tagIdx;
            ElleDB::StoreMemory(mem);
        } catch (const std::exception& e) {
            ELLE_WARN("StoreMemory(episodic) failed: %s", e.what());
        }

        /* 10. Reply */
        uint64_t elapsed = ELLE_MS_NOW() - t0;
        ELLE_INFO("Chat reply rid=%s conv=%llu mode=%s memories=%zu entities=%zu in %llums",
                  requestId.c_str(), (unsigned long long)convId,
                  mode == MODE_RESEARCH ? "research" : "companion",
                  memories.size(), entities.size(),
                  (unsigned long long)elapsed);

        json out = {
            {"request_id", requestId},
            {"response", responseText},
            {"conversation_id", convId},
            {"mode", mode == MODE_RESEARCH ? "research" : "companion"},
            {"memories_used", memories.size()},
            {"entities", entities},
            {"latency_ms", (uint64_t)elapsed}
        };
        SendChatReply(reply_to, out);

        /* Fan out post-response side effects so downstream services actually
         * see the turn. Previously these paths had no callers:
         *   Bonding  — ProcessInteraction() was unreachable; relationship
         *              state didn't evolve with real interactions.
         *   InnerLife — PostResponseCheck() was dead code; authenticity /
         *              resonance weren't tracked per turn.
         * Now we emit two IPC events with the full turn context as JSON.  */
        try {
            json bondPayload = {
                {"user_message",        userText},
                {"elle_response",       responseText},
                {"conversation_depth",  EstimateConvDepth(userText, responseText)},
                {"emotional_intensity", std::fabs(sent.valence) * 0.6f + sent.arousal * 0.4f}
            };
            auto bMsg = ElleIPCMessage::Create(IPC_INTERACTION_RECORDED,
                                               SVC_COGNITIVE, SVC_BONDING);
            bMsg.SetStringPayload(bondPayload.dump());
            GetIPCHub().Send(SVC_BONDING, bMsg);

            auto iMsg = ElleIPCMessage::Create(IPC_POST_RESPONSE,
                                               SVC_COGNITIVE, SVC_INNER_LIFE);
            iMsg.SetStringPayload(bondPayload.dump());
            GetIPCHub().Send(SVC_INNER_LIFE, iMsg);
        } catch (const std::exception& e) {
            ELLE_WARN("Post-response fanout failed: %s", e.what());
        }
    }

    /* Cheap conversation-depth heuristic — length × presence of
     * emotionally-charged tokens. Good enough for Bonding; Bonding then
     * applies its own thresholds on top.                                 */
    float EstimateConvDepth(const std::string& userText, const std::string& elleReply) {
        float lenScore = std::min(1.0f, (float)(userText.size() + elleReply.size()) / 800.0f);
        static const char* deep[] = {
            "feel","love","fear","hope","meaning","truth","sorry","trust",
            "lonely","family","died","child","dream","believe","forgive"
        };
        int hits = 0;
        for (auto* w : deep) {
            if (userText.find(w) != std::string::npos ||
                elleReply.find(w) != std::string::npos) hits++;
        }
        float topicBoost = std::min(1.0f, hits / 3.0f);
        return std::min(1.0f, 0.5f * lenScore + 0.5f * topicBoost);
    }

    void SendChatReply(ELLE_SERVICE_ID to, const json& body) {
        auto msg = ElleIPCMessage::Create(IPC_CHAT_RESPONSE, SVC_COGNITIVE, to);
        msg.SetStringPayload(body.dump());
        GetIPCHub().Send(to, msg);
    }

    void SendChatError(ELLE_SERVICE_ID to, const std::string& requestId,
                       const std::string& error) {
        json j = {
            {"request_id", requestId},
            {"error", error}
        };
        SendChatReply(to, j);
    }

    /* Translate a routed intent into the exact opcode + payload shape the
     * target service actually handles.
     *
     * Previously RouteIntent sent IPC_INTENT_REQUEST to every target — but
     * only Cognitive itself listens on IPC_INTENT_REQUEST, so every route
     * was a black hole. Intents claimed "processing" in SQL but nothing
     * downstream ever consumed them.
     *
     * Each arm below speaks the receiver's native protocol:
     *   Memory    ← IPC_MEMORY_STORE / IPC_MEMORY_RECALL  (binary ELLE_MEMORY_RECORD)
     *   Emotional ← IPC_EMOTION_UPDATE                    (binary ELLE_EMOTION_DELTA via string JSON)
     *   Goal      ← IPC_GOAL_UPDATE                       (binary ELLE_GOAL_RECORD)
     *   World     ← IPC_WORLD_STATE                       (binary ELLE_WORLD_ENTITY)
     *   SelfPrompt← IPC_SELF_PROMPT                       (string)
     *   Dream     ← IPC_DREAM_TRIGGER                     (empty)
     *   Action    ← IPC_ACTION_REQUEST                    (binary ELLE_ACTION_RECORD)
     *
     * After routing we mark the intent COMPLETED so the QueueWorker
     * doesn't redispatch the same row on the next tick.                 */
    void RouteIntent(const ELLE_INTENT_RECORD& intent) {
        auto& hub = GetIPCHub();
        bool routed = false;
        std::string note;

        switch ((ELLE_INTENT_TYPE)intent.type) {
            case INTENT_STORE_MEMORY: {
                ELLE_MEMORY_RECORD m{};
                strncpy_s(m.content, intent.description, ELLE_MAX_MSG - 1);
                m.created_ms = ELLE_MS_NOW();
                m.tier = 1;          /* STM */
                auto msg = ElleIPCMessage::Create(IPC_MEMORY_STORE, SVC_COGNITIVE, SVC_MEMORY);
                msg.SetPayload(m);
                routed = hub.Send(SVC_MEMORY, msg);
                note = "memory_store";
                break;
            }
            case INTENT_RECALL_MEMORY: {
                /* Recall uses a string query — we piggyback on IPC_MEMORY_RECALL
                 * as string payload so Memory can interpret it.          */
                auto msg = ElleIPCMessage::Create(IPC_MEMORY_RECALL, SVC_COGNITIVE, SVC_MEMORY);
                msg.SetStringPayload(std::string(intent.description));
                routed = hub.Send(SVC_MEMORY, msg);
                note = "memory_recall";
                break;
            }
            case INTENT_EMOTIONAL_EXPRESSION: {
                /* Emotional's HandleEmotionUpdate reads a BINARY payload:
                 *   [uint32 emotion_id][float delta]
                 * (EmotionalEngine.cpp:621-627). The old code sent a JSON
                 * string here which silently got dropped (the handler only
                 * acts when payload.size() >= 8 of the right shape).
                 *
                 * Resolution order for which emotion to nudge:
                 *   1) If intent.parameters is a JSON object with
                 *      {"emotion":"<name>","delta":<float>} — use it verbatim.
                 *   2) Else scan intent.description for any kEmotionMeta name
                 *      token (case-insensitive substring match).
                 *   3) Else fall back to EMOTION_JOY.
                 * Delta defaults to intent.urgency * 0.5f so urgent intents
                 * push harder than casual ones.                            */
                uint32_t emoId = (uint32_t)EMO_JOY;
                float    delta = intent.urgency * 0.5f;
                if (delta <= 0.0f) delta = 0.25f;

                bool resolved = false;
                {
                    nlohmann::json pj;
                    if (intent.parameters[0] &&
                        Elle::ExtractJsonObject(intent.parameters, pj) &&
                        pj.contains("emotion")) {
                        std::string nm = pj.value("emotion", std::string(""));
                        if (pj.contains("delta")) {
                            try { delta = pj["delta"].get<float>(); }
                            catch (const std::exception& e) {
                                ELLE_DEBUG("INTENT_RESOLVE_EMOTION: non-numeric "
                                           "'delta' field (%s) — keeping default", e.what());
                            }
                        }
                        std::string lo; lo.reserve(nm.size());
                        for (char c : nm) lo.push_back((char)tolower((unsigned char)c));
                        for (uint32_t i = 0; i < (uint32_t)ELLE_EMOTION_COUNT; ++i) {
                            if (kEmotionMeta[i].name && lo == kEmotionMeta[i].name) {
                                emoId = i; resolved = true; break;
                            }
                        }
                    }
                }
                if (!resolved) {
                    std::string desc = intent.description;
                    std::string lo; lo.reserve(desc.size());
                    for (char c : desc) lo.push_back((char)tolower((unsigned char)c));
                    for (uint32_t i = 0; i < (uint32_t)ELLE_EMOTION_COUNT; ++i) {
                        const char* nm = kEmotionMeta[i].name;
                        if (!nm || !*nm) continue;
                        if (lo.find(nm) != std::string::npos) {
                            emoId = i; break;
                        }
                    }
                }

                /* Pack the binary payload in the exact layout Emotional
                 * expects: [uint32][float] = 8 bytes. */
                std::vector<uint8_t> buf(8);
                memcpy(buf.data(),     &emoId, sizeof(uint32_t));
                memcpy(buf.data() + 4, &delta, sizeof(float));
                auto msg = ElleIPCMessage::Create(IPC_EMOTION_UPDATE, SVC_COGNITIVE, SVC_EMOTIONAL);
                msg.payload = std::move(buf);
                msg.header.payload_size = 8;
                routed = hub.Send(SVC_EMOTIONAL, msg);
                note = "emotion_update";
                break;
            }
            case INTENT_GOAL_UPDATE: {
                ELLE_GOAL_RECORD g{};
                strncpy_s(g.description, intent.description, ELLE_MAX_MSG - 1);
                strncpy_s(g.success_criteria, intent.parameters, ELLE_MAX_MSG - 1);
                g.priority    = GOAL_MEDIUM;
                g.motivation  = 0.6f;
                g.source_drive = intent.source_drive;
                g.created_ms  = ELLE_MS_NOW();
                auto msg = ElleIPCMessage::Create(IPC_GOAL_UPDATE, SVC_COGNITIVE, SVC_GOAL_ENGINE);
                msg.SetPayload(g);
                routed = hub.Send(SVC_GOAL_ENGINE, msg);
                note = "goal_update";
                break;
            }
            case INTENT_WORLD_MODEL_UPDATE: {
                /* WorldModel consumes name, type, description, and uses
                 * last_interaction_ms (NOT last_seen_ms — that field does
                 * not exist on ELLE_WORLD_ENTITY). The previous code set
                 * a non-existent `relationship_note` which was a compile
                 * error. We now populate the three fields WorldModel
                 * actually reads (WorldModel.cpp:103-108) plus
                 * mental_model for the cognitive note.                  */
                ELLE_WORLD_ENTITY e{};
                const char* entityName = intent.parameters[0]
                                           ? intent.parameters
                                           : intent.description;
                strncpy_s(e.name,        entityName,          ELLE_MAX_NAME - 1);
                strncpy_s(e.type,        "person",            ELLE_MAX_TAG  - 1);
                strncpy_s(e.description, intent.description,  ELLE_MAX_MSG  - 1);
                strncpy_s(e.mental_model, intent.description, ELLE_MAX_MSG  - 1);
                e.last_interaction_ms = ELLE_MS_NOW();
                e.interaction_count   = 1;
                e.familiarity         = 0.2f;
                e.trust               = 0.5f;
                auto msg = ElleIPCMessage::Create(IPC_WORLD_STATE, SVC_COGNITIVE, SVC_WORLD_MODEL);
                msg.SetPayload(e);
                routed = hub.Send(SVC_WORLD_MODEL, msg);
                note = "world_update";
                break;
            }
            case INTENT_SELF_REFLECT:
            case INTENT_META_THINK: {
                auto msg = ElleIPCMessage::Create(IPC_SELF_PROMPT, SVC_COGNITIVE, SVC_SELF_PROMPT);
                msg.SetStringPayload(std::string(intent.description));
                routed = hub.Send(SVC_SELF_PROMPT, msg);
                note = "self_prompt";
                break;
            }
            case INTENT_DREAM: {
                auto msg = ElleIPCMessage::Create(IPC_DREAM_TRIGGER, SVC_COGNITIVE, SVC_DREAM);
                msg.SetStringPayload(std::string(intent.description));
                routed = hub.Send(SVC_DREAM, msg);
                note = "dream";
                break;
            }
            default: {
                /* Build a proper ELLE_ACTION_RECORD from the intent for the
                 * Action service. The mapping below inspects intent.parameters
                 * / intent.description to pick the RIGHT concrete action —
                 * previously every INTENT_HARDWARE_COMMAND collapsed to
                 * ACTION_QUERY_HARDWARE, every INTENT_FILE_OPERATION to
                 * ACTION_READ_FILE, etc., so vibrate/kill/write requests
                 * were silently downgraded to their benign defaults.     */
                ELLE_ACTION_RECORD a{};
                a.intent_id = intent.id;
                a.type = ACTION_SEND_MESSAGE; /* safe default */

                /* Lowercase combined hint pulled from params first (the
                 * LLM parser is asked to put verbs there), falling back
                 * to description. */
                std::string hint = intent.parameters[0] ? intent.parameters
                                                        : intent.description;
                for (auto& c : hint) c = (char)tolower((unsigned char)c);
                auto has = [&](const char* s){ return hint.find(s) != std::string::npos; };

                switch ((ELLE_INTENT_TYPE)intent.type) {
                    case INTENT_HARDWARE_COMMAND:
                        if      (has("vibrate") || has("buzz"))         a.type = ACTION_VIBRATE;
                        else if (has("flash")   || has("blink"))        a.type = ACTION_FLASH;
                        else if (has("notify")  || has("alert") || has("toast"))
                                                                         a.type = ACTION_NOTIFY;
                        else if (has("cpu") && has("affin"))             a.type = ACTION_SET_CPU_AFFINITY;
                        else                                             a.type = ACTION_QUERY_HARDWARE;
                        break;
                    case INTENT_FILE_OPERATION:
                        if      (has("write") || has("save")  || has("create"))  a.type = ACTION_WRITE_FILE;
                        else if (has("delete")|| has("remove")|| has("rm"))      a.type = ACTION_DELETE_FILE;
                        else if (has("watch") || has("monitor"))                 a.type = ACTION_WATCH_FILE;
                        else                                                     a.type = ACTION_READ_FILE;
                        break;
                    case INTENT_PROCESS_CONTROL:
                        if      (has("kill")   || has("terminate") || has("stop"))   a.type = ACTION_KILL_PROCESS;
                        else if (has("launch") || has("start") || has("run") || has("spawn"))
                                                                                       a.type = ACTION_LAUNCH_PROCESS;
                        else                                                         a.type = ACTION_LIST_PROCESSES;
                        break;
                    case INTENT_EXECUTE_ACTION:
                        /* Generic "do this" — Action dispatches to the
                         * custom handler which logs and returns a clear
                         * failure if no concrete backend is registered.
                         * ACTION_EXECUTE_CODE would be dispatched too but
                         * that's restricted by consent; use ACTION_CUSTOM
                         * so the action pipeline can policy-check and
                         * route to the correct backend (including code
                         * execution when consent allows).                */
                        a.type = ACTION_CUSTOM;
                        break;
                    case INTENT_CHAT:
                        a.type = ACTION_SEND_MESSAGE;
                        break;
                    default:
                        break;
                }
                strncpy_s(a.command,    intent.description, ELLE_MAX_MSG - 1);
                strncpy_s(a.parameters, intent.parameters,  ELLE_MAX_MSG - 1);
                a.required_trust = intent.required_trust;
                a.created_ms = ELLE_MS_NOW();
                a.timeout_ms = intent.timeout_ms ? intent.timeout_ms : 30000;
                auto msg = ElleIPCMessage::Create(IPC_ACTION_REQUEST, SVC_COGNITIVE, SVC_ACTION);
                msg.SetPayload(a);
                routed = hub.Send(SVC_ACTION, msg);
                note = "action";
                break;
            }
        }

        if (routed) {
            ElleDB::UpdateIntentStatus(intent.id, INTENT_COMPLETED,
                                       std::string("routed:") + note);
        } else {
            ELLE_WARN("Intent %llu (type=%u) failed to route via %s",
                      (unsigned long long)intent.id, intent.type, note.c_str());
            ElleDB::UpdateIntentStatus(intent.id, INTENT_FAILED,
                                       std::string("route_failed:") + note);
        }
    }
};

ELLE_SERVICE_MAIN(ElleCognitiveService)
