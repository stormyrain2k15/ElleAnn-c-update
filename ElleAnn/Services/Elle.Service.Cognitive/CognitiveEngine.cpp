/*******************************************************************************
 * CognitiveEngine.cpp — Attention, Intent Parsing, Chain-of-Thought
 * 
 * The thinking brain. Manages cognitive threads, attention, intent parsing,
 * and routes to appropriate services.
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLLM.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"
#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>

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

    std::string llmResult = ElleLLMEngine::Instance().ParseIntent(text, context);
    /* Parse LLM JSON response to extract intent type and confidence */
    /* In production, this parses the JSON properly */

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
            default:
                break;
        }
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT, SVC_EMOTIONAL, SVC_MEMORY, SVC_ACTION };
    }

private:
    CognitiveEngine m_engine;

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
