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
#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <regex>
#include <algorithm>
#include <unordered_map>
#include <cctype>
#include <sstream>
#include <set>
#include <cmath>

using json = nlohmann::json;

/*──────────────────────────────────────────────────────────────────────────────
 * COGNITIVE THREAD — One unit of attention
 *──────────────────────────────────────────────────────────────────────────────*/
struct CognitiveThread {
    uint32_t    id;
    std::string topic;
    float       attention;      /* 0.0-1.0 */
    uint64_t    started_ms;
    uint64_t    last_active_ms;
    bool        active;
};

/*──────────────────────────────────────────────────────────────────────────────
 * INTENT PARSER
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
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

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

    /* Models sometimes wrap JSON in ``` fences — strip to first '{' and
     * last '}' to give nlohmann a clean span. */
    auto first = raw.find('{');
    auto last  = raw.rfind('}');
    if (first == std::string::npos || last == std::string::npos || last <= first) {
        ELLE_WARN("IntentParser LLM: no JSON object in response: %.80s", raw.c_str());
        return result;
    }
    std::string slice = raw.substr(first, last - first + 1);

    try {
        auto j = nlohmann::json::parse(slice);

        /* Map the string intent label back to our enum. */
        std::string label = j.value("intent_type", std::string("chat"));
        std::transform(label.begin(), label.end(), label.begin(), ::tolower);
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
        ELLE_WARN("IntentParser LLM JSON parse failed: %s (raw=%.60s)", ex.what(), slice.c_str());
    }

    return result;
}

/*──────────────────────────────────────────────────────────────────────────────
 * COGNITIVE ENGINE
 *──────────────────────────────────────────────────────────────────────────────*/
class CognitiveEngine {
public:
    bool Initialize() {
        m_maxThreads = (uint32_t)ElleConfig::Instance().GetInt("cognitive.max_concurrent_threads", 4);
        m_attentionSpan = (uint32_t)ElleConfig::Instance().GetInt("cognitive.attention_span_seconds", 120) * 1000;
        ELLE_INFO("Cognitive engine initialized: %d threads, %ds attention span",
                  m_maxThreads, m_attentionSpan / 1000);
        return true;
    }

    /* Process incoming text from user or self-prompt */
    void ProcessInput(const std::string& text, const std::string& context,
                      uint64_t conversationId) {
        /* 1. Parse intent */
        auto intent = m_parser.Parse(text, context);
        ELLE_INFO("Intent parsed: type=%d conf=%.2f [%s]", 
                  intent.type, intent.confidence, text.substr(0, 60).c_str());

        /* 2. Create intent record */
        ELLE_INTENT_RECORD rec = {};
        rec.type = intent.type;
        rec.status = INTENT_PENDING;
        rec.urgency = intent.urgency;
        rec.confidence = intent.confidence;
        strncpy_s(rec.description, text.c_str(), ELLE_MAX_MSG - 1);
        strncpy_s(rec.parameters, intent.parameters.c_str(), ELLE_MAX_MSG - 1);
        rec.timeout_ms = ElleConfig::Instance().GetInt("cognitive.intent_timeout_ms", 30000);

        /* 3. Submit to intent queue */
        ElleDB::SubmitIntent(rec);

        /* 4. Allocate attention thread */
        AllocateThread(text);
    }

    /* Chain-of-thought reasoning */
    std::string Reason(const std::string& problem, const std::string& context,
                       const ELLE_EMOTION_STATE& emotions) {
        std::vector<LLMMessage> messages;
        messages.push_back({"system", 
            "You are Elle-Ann's cognitive system. Think step by step. "
            "Show your reasoning process. Consider multiple perspectives. "
            "Your emotional state influences your reasoning."});
        messages.push_back({"user", 
            "Problem: " + problem + "\nContext: " + context});

        auto resp = ElleLLMEngine::Instance().Chat(messages, -1.0f, 4096);
        return resp.success ? std::string(resp.content) : "";
    }

    /* Metacognition — thinking about thinking */
    std::string Metacognize(const std::string& recentThoughts) {
        return ElleLLMEngine::Instance().Ask(
            "Review these recent thoughts and evaluate their quality:\n" + recentThoughts,
            "You are Elle-Ann's metacognitive monitor. Evaluate reasoning quality, "
            "identify biases, and suggest improvements.");
    }

    void Tick() {
        /* Decay attention on old threads */
        uint64_t now = ELLE_MS_NOW();
        for (auto& thread : m_threads) {
            if (thread.active && (now - thread.last_active_ms) > m_attentionSpan) {
                thread.attention *= 0.9f;
                if (thread.attention < 0.1f) {
                    thread.active = false;
                    ELLE_DEBUG("Cognitive thread %d expired: %s", thread.id, thread.topic.c_str());
                }
            }
        }
    }

private:
    IntentParser m_parser;
    std::vector<CognitiveThread> m_threads;
    uint32_t m_maxThreads = 4;
    uint32_t m_attentionSpan = 120000;
    uint32_t m_nextThreadId = 1;

    void AllocateThread(const std::string& topic) {
        /* Find free slot or replace lowest attention */
        for (auto& t : m_threads) {
            if (!t.active) {
                t.id = m_nextThreadId++;
                t.topic = topic.substr(0, 128);
                t.attention = 1.0f;
                t.started_ms = ELLE_MS_NOW();
                t.last_active_ms = t.started_ms;
                t.active = true;
                return;
            }
        }

        if (m_threads.size() < m_maxThreads) {
            CognitiveThread t;
            t.id = m_nextThreadId++;
            t.topic = topic.substr(0, 128);
            t.attention = 1.0f;
            t.started_ms = ELLE_MS_NOW();
            t.last_active_ms = t.started_ms;
            t.active = true;
            m_threads.push_back(t);
        }
    }
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
        ELLE_INFO("Cognitive service started");
        return true;
    }

    void OnStop() override {
        ELLE_INFO("Cognitive service stopped");
    }

    void OnTick() override {
        m_engine.Tick();

        /* Process pending intents from queue */
        std::vector<ELLE_INTENT_RECORD> intents;
        ElleDB::GetPendingIntents(intents, 5);

        for (auto& intent : intents) {
            ElleDB::UpdateIntentStatus(intent.id, INTENT_PROCESSING);
            /* Route to appropriate service based on intent type */
            RouteIntent(intent);
        }
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        switch ((ELLE_IPC_MSG_TYPE)msg.header.msg_type) {
            case IPC_INTENT_REQUEST: {
                std::string text = msg.GetStringPayload();
                m_engine.ProcessInput(text, "", 0);
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
            case IPC_CHAT_REQUEST: {
                /* Spawn a dedicated thread — orchestration calls LLM (~2-5s),
                 * we must never block the IPC dispatcher. */
                std::string payload = msg.GetStringPayload();
                ELLE_SERVICE_ID origin = sender;
                std::thread([this, payload, origin]() {
                    try {
                        this->HandleChatRequest(payload, origin);
                    } catch (const std::exception& e) {
                        ELLE_ERROR("Chat orchestration exception: %s", e.what());
                    } catch (...) {
                        ELLE_ERROR("Chat orchestration unknown exception");
                    }
                }).detach();
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

    /*────────────────────────────────────────────────────────────────────────
     * CHAT ORCHESTRATION — the full pipeline the user described.
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
                if (!found) {
                    /* First mention: upsert as person */
                    ELLE_WORLD_ENTITY newEnt = {};
                    strncpy_s(newEnt.name, ToLower(name).c_str(), ELLE_MAX_NAME - 1);
                    strncpy_s(newEnt.type, "person", ELLE_MAX_TAG - 1);
                    newEnt.familiarity = 0.1f;
                    newEnt.trust = 0.5f;
                    newEnt.interaction_count = 1;
                    newEnt.last_interaction_ms = ELLE_MS_NOW();
                    ElleDB::StoreEntity(newEnt);
                } else {
                    ElleDB::UpdateEntityInteraction(ent.id);
                }
                std::vector<ELLE_MEMORY_RECORD> recalled;
                /* Recall by the name as the query term */
                if (ElleDB::RecallMemories(name, recalled,
                                           mode == MODE_RESEARCH ? 15 : 8,
                                           0.15f)) {
                    pushUnique(recalled);
                }
            } catch (...) {}
        }

        /* Tier 2b: Also recall by the full user text (catches topic matches) */
        try {
            std::vector<ELLE_MEMORY_RECORD> topicHits;
            if (ElleDB::RecallMemories(userText, topicHits,
                                       mode == MODE_RESEARCH ? 10 : 5,
                                       0.2f)) {
                pushUnique(topicHits);
            }
        } catch (...) {}

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
         *    even if we crash mid-pipeline. */
        try { ElleDB::StoreMessage(convId, 1 /*user*/, userText, m_cachedEmotions, 0.0f); }
        catch (...) { ELLE_WARN("StoreMessage(user) failed"); }

        /* 2. Mode + entities */
        ChatMode mode = DetectMode(userText);
        std::vector<std::string> entities = ExtractProperNouns(userText);

        /* 3. Emotional analysis (quick local read + broadcast to Emotional) */
        SentimentRead sent = QuickSentiment(userText);
        BroadcastEmotionDelta(sent);

        /* 4. Cross-reference memory (Tier 2) */
        std::vector<ELLE_MEMORY_RECORD> memories =
            CrossReferenceByEntities(entities, userText, mode);

        /* 5. Conversation history (last 20 turns) */
        std::vector<ELLE_CONVERSATION_MSG> history;
        try { ElleDB::GetConversationHistory(convId, history, 20); } catch (...) {}

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
         *     vulnerability/comfort patterns). If no CrystalProfile exists
         *     yet, fall back to defaults. user_id may be "default" (string)
         *     so we try to resolve it to an int, else 1.                    */
        int32_t userIdInt = 1;
        try { userIdInt = std::stoi(userId); } catch (...) { userIdInt = 1; }

        ElleDB::CrystalProfile crystal;
        bool hasCrystal = ElleDB::GetCrystalProfile(userIdInt, crystal);
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
        /* Whatever this turn is, their silence streak just broke. */
        ElleDB::UpdateUserPresenceOnInteraction(userIdInt);

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
        catch (...) {}

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
        } catch (...) {}

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

    void RouteIntent(const ELLE_INTENT_RECORD& intent) {
        ELLE_SERVICE_ID target = SVC_ACTION; /* Default */

        switch ((ELLE_INTENT_TYPE)intent.type) {
            case INTENT_STORE_MEMORY:
            case INTENT_RECALL_MEMORY:
                target = SVC_MEMORY;
                break;
            case INTENT_EMOTIONAL_EXPRESSION:
                target = SVC_EMOTIONAL;
                break;
            case INTENT_GOAL_UPDATE:
                target = SVC_GOAL_ENGINE;
                break;
            case INTENT_WORLD_MODEL_UPDATE:
                target = SVC_WORLD_MODEL;
                break;
            case INTENT_SELF_REFLECT:
            case INTENT_META_THINK:
                target = SVC_SELF_PROMPT;
                break;
            case INTENT_DREAM:
                target = SVC_DREAM;
                break;
            default:
                target = SVC_ACTION;
                break;
        }

        auto msg = ElleIPCMessage::Create(IPC_INTENT_REQUEST, SVC_COGNITIVE, target);
        msg.SetPayload(intent);
        GetIPCHub().Send(target, msg);
    }
};

ELLE_SERVICE_MAIN(ElleCognitiveService)
