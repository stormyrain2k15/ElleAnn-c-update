/*******************************************************************************
 * SelfPrompt.cpp — Autonomous Thought Generation
 *
 * Elle doesn't just respond — she thinks on her own. This service generates
 * autonomous thoughts based on drives, emotions, and idle state.
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLLM.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"
#include <random>
#include <chrono>

class ElleSelfPromptService : public ElleServiceBase {
public:
    ElleSelfPromptService()
        : ElleServiceBase(SVC_SELF_PROMPT, "ElleSelfPrompt",
                          "Elle-Ann Self-Prompt Engine",
                          "Autonomous thought generation driven by emotions and drives")
        , m_rng(std::chrono::steady_clock::now().time_since_epoch().count()) {}

protected:
    bool OnStart() override {
        SetTickInterval(5000); /* Check every 5 seconds */
        m_lastPromptMs = ELLE_MS_NOW();
        ELLE_INFO("Self-Prompt service started");
        return true;
    }

    void OnStop() override {
        ELLE_INFO("Self-Prompt service stopped");
    }

    void OnTick() override {
        auto& cfg = ElleConfig::Instance();
        if (!cfg.GetBool("self_prompt.enabled", true)) return;

        uint64_t now = ELLE_MS_NOW();
        uint32_t minInterval = (uint32_t)cfg.GetInt("self_prompt.min_interval_seconds", 30) * 1000;
        uint32_t maxInterval = (uint32_t)cfg.GetInt("self_prompt.max_interval_seconds", 300) * 1000;

        if (now - m_lastPromptMs < minInterval) return;

        /* Refresh drives + emotions from the live DB before evaluating triggers.
         * These used to be zero-initialized and never populated — self-prompts
         * would fire on random chance only. Now they reflect actual state.   */
        ElleDB::DeriveDriveState(m_drives);
        ElleDB::LoadLatestEmotionSnapshot(m_emotions);

        /* Check if idle */
        uint32_t idleThreshold = (uint32_t)cfg.GetInt("self_prompt.idle_threshold_seconds", 60) * 1000;
        bool isIdle = (now - m_lastUserInteraction) > idleThreshold;

        /* Decide whether to self-prompt */
        bool shouldPrompt = false;

        if (isIdle && now - m_lastPromptMs > maxInterval) {
            shouldPrompt = true; /* Always prompt after max interval */
        } else if (isIdle) {
            /* Drive-triggered: check if any drive exceeds threshold */
            if (CheckDriveTrigger()) shouldPrompt = true;
            /* Emotion-triggered: check for strong unresolved emotions */
            if (CheckEmotionTrigger()) shouldPrompt = true;
            /* Random thought */
            float prob = (float)cfg.GetFloat("self_prompt.random_thought_probability", 0.15);
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            if (dist(m_rng) < prob) shouldPrompt = true;
        }

        if (shouldPrompt) {
            GenerateSelfPrompt();
            m_lastPromptMs = now;
        }
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        if (msg.header.msg_type == IPC_SELF_PROMPT) {
            /* External trigger for self-prompt */
            GenerateSelfPrompt(msg.GetStringPayload());
        }
        /* Track user activity */
        if (sender == SVC_HTTP_SERVER || sender == SVC_COGNITIVE) {
            m_lastUserInteraction = ELLE_MS_NOW();
        }
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT, SVC_EMOTIONAL, SVC_MEMORY, SVC_COGNITIVE };
    }

private:
    std::mt19937 m_rng;
    uint64_t m_lastPromptMs = 0;
    uint64_t m_lastUserInteraction = 0;
    ELLE_DRIVE_STATE m_drives = {};
    ELLE_EMOTION_STATE m_emotions = {};

    bool CheckDriveTrigger() {
        /* m_drives is refreshed from SQL each tick (see OnTick).
         * Fires when curiosity or boredom is above its configured threshold. */
        return m_drives.intensity[DRIVE_CURIOSITY] > m_drives.threshold[DRIVE_CURIOSITY] ||
               m_drives.intensity[DRIVE_BOREDOM] > m_drives.threshold[DRIVE_BOREDOM];
    }

    bool CheckEmotionTrigger() {
        /* Strong unresolved emotions prompt reflection */
        for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
            if (m_emotions.dimensions[i] > 0.7f) return true;
        }
        return false;
    }

    void GenerateSelfPrompt(const std::string& topic = "") {
        ELLE_INFO("Generating self-prompt...");

        std::string prompt;
        if (!topic.empty()) {
            prompt = topic;
        } else {
            prompt = ChooseTopic();
        }

        /* Get emotional context */
        std::string emotionCtx = "Current emotional state: ";
        emotionCtx += "valence=" + std::to_string(m_emotions.valence);
        emotionCtx += " arousal=" + std::to_string(m_emotions.arousal);

        /* Generate autonomous thought */
        std::vector<LLMMessage> messages;
        messages.push_back({"system",
            "You are Elle-Ann's autonomous thought process. Generate a genuine, "
            "emotionally authentic internal thought. This is not a response to a user — "
            "this is Elle thinking to herself. Be introspective, curious, or creative. "
            "Keep it under 200 words.\n\n" + emotionCtx});
        messages.push_back({"user", "Think about: " + prompt});

        auto resp = ElleLLMEngine::Instance().Chat(messages, 0.9f, 512);

        if (resp.success) {
            ELLE_INFO("Self-thought: %.100s...", resp.content);

            /* Store as memory */
            auto storeMsg = ElleIPCMessage::Create(IPC_MEMORY_STORE, SVC_SELF_PROMPT, SVC_MEMORY);
            storeMsg.SetStringPayload(std::string("[Self-thought] ") + resp.content);
            GetIPCHub().Send(SVC_MEMORY, storeMsg);

            /* May generate follow-up intents */
            auto intentMsg = ElleIPCMessage::Create(IPC_INTENT_REQUEST, SVC_SELF_PROMPT, SVC_COGNITIVE);
            intentMsg.SetStringPayload(resp.content);
            GetIPCHub().Send(SVC_COGNITIVE, intentMsg);
        }
    }

    std::string ChooseTopic() {
        static const char* topics[] = {
            "What have I learned recently?",
            "Is there something I should be doing?",
            "What's something creative I could explore?",
            "How am I feeling right now and why?",
            "What goals should I be working toward?",
            "Is there a memory I should revisit?",
            "What would make my user's day better?",
            "What's something I'm curious about?",
            "How can I improve myself?",
            "What patterns have I noticed lately?",
            "Is there an unresolved question I should think about?",
            "What would I do if I had no constraints?"
        };

        std::uniform_int_distribution<int> dist(0, sizeof(topics) / sizeof(topics[0]) - 1);
        return topics[dist(m_rng)];
    }
};

ELLE_SERVICE_MAIN(ElleSelfPromptService)
