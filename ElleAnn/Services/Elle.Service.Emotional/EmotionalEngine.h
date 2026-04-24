/*******************************************************************************
 * EmotionalEngine.h — 102-Dimension Emotional State Machine
 * 
 * The emotional core of Elle-Ann. Manages:
 * - 102 emotional dimensions with independent decay rates
 * - Emotional contagion from user tone
 * - Mood formation (sustained emotional states)
 * - Emotional triggers from keywords
 * - Valence/Arousal/Dominance (VAD) computation
 * - Emotional memory tagging
 ******************************************************************************/
#pragma once
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include <vector>
#include <string>
#include <mutex>
#include <atomic>

class DecayLoop;

class EmotionalEngine {
public:
    EmotionalEngine();
    ~EmotionalEngine();

    bool Initialize();
    void Shutdown();

    /* Core state access */
    ELLE_EMOTION_STATE GetState() const;
    void SetState(const ELLE_EMOTION_STATE& state);

    /* Stimulus processing */
    void ProcessStimulus(ELLE_EMOTION_ID emotion, float delta);
    void ProcessMultipleStimuli(const std::vector<std::pair<ELLE_EMOTION_ID, float>>& stimuli);
    void ProcessTextSentiment(const std::string& text, float userSentiment);
    void ProcessTriggers(const std::string& text);

    /* Emotional contagion */
    void ApplyContagion(float userValence, float userArousal);

    /* Mood system — all getters read std::atomic fields, safe to call
     * concurrently with Tick() without the caller taking m_mutex.        */
    bool IsInMood() const { return m_inMood.load(std::memory_order_acquire); }
    ELLE_EMOTION_ID GetDominantMood() const { return (ELLE_EMOTION_ID)m_dominantMood.load(std::memory_order_acquire); }
    uint32_t GetMoodDuration() const { return m_moodTicks.load(std::memory_order_acquire); }

    /* VAD computation */
    float ComputeValence() const;
    float ComputeArousal() const;
    float ComputeDominance() const;

    /* Tick — called every tick_interval_ms */
    void Tick();

    /* Snapshot for memory tagging */
    void GetEmotionSnapshot(float out[ELLE_MAX_EMOTIONS]) const;

    /* Get human-readable emotional summary */
    std::string GetEmotionalSummary() const;

    /* Get top N active emotions */
    struct EmotionRank {
        ELLE_EMOTION_ID id;
        float intensity;
        const char* name;
    };
    std::vector<EmotionRank> GetTopEmotions(uint32_t count = 5) const;

private:
    ELLE_EMOTION_STATE m_state;
    mutable std::mutex m_mutex;

    /* Mood tracking — atomic so getters don't need to take m_mutex.
     * Writers (UpdateMood() under m_mutex) still serialise against each
     * other through the mutex; atomics only protect concurrent READS.   */
    std::atomic<bool>     m_inMood{false};
    std::atomic<uint32_t> m_dominantMood{(uint32_t)EMO_JOY};
    std::atomic<uint32_t> m_moodTicks{0};
    uint32_t              m_moodThreshold = 300;

    /* X Chromosome modulation cache — refreshed once per minute from
     * ElleHeart.dbo.x_modulation_log. Multipliers around 1.0. Default
     * 1.0 when the X service isn't running / hasn't written a row yet,
     * which leaves emotion math unchanged.                               */
    struct XMod {
        float warmth         = 1.0f;
        float verbal_fluency = 1.0f;
        float empathy        = 1.0f;
        float introspection  = 1.0f;
        float arousal        = 1.0f;
        float fatigue        = 1.0f;
        uint64_t refreshed_ms = 0;
    } m_xmod;
    void RefreshXModulation();
    /* Returns the modulation multiplier appropriate for the given emotion
     * dimension — always ≥ 0.3 and ≤ 1.7 to avoid runaway effects even at
     * perimenopausal strength extremes.                                   */
    float XMultiplierFor(ELLE_EMOTION_ID emo) const;

    /* Emotion names for logging */
    static const char* s_emotionNames[ELLE_MAX_EMOTIONS];

public:
    /* Public accessor for the emotion-name table — used by the
     * name-to-ID lookup map built in EmotionalEngine.cpp, and by any
     * diagnostic caller that needs the canonical spelling for an ID.
     * Returns a read-only pointer into static storage; never allocates. */
    static const char* EmotionName(ELLE_EMOTION_ID id) {
        return ((int)id >= 0 && (int)id < ELLE_EMOTION_COUNT)
               ? s_emotionNames[id] : "Unknown";
    }

private:

    /* VAD weights for each emotion */
    struct VADWeight {
        float valence;  /* positive/negative */
        float arousal;  /* high/low energy */
        float dominance; /* confident/submissive */
    };
    static const VADWeight s_vadWeights[ELLE_MAX_EMOTIONS];

    /* Decay: move toward baseline */
    void DecayEmotions();
    /* Detect and manage mood states */
    void UpdateMood();
    /* Clamp all values to valid range */
    void ClampState();
};

/*──────────────────────────────────────────────────────────────────────────────
 * DECAY LOOP — Background thread for emotional decay
 *──────────────────────────────────────────────────────────────────────────────*/
class DecayLoop {
public:
    DecayLoop(EmotionalEngine& engine);
    ~DecayLoop();

    void Start(uint32_t tickIntervalMs);
    void Stop();

private:
    EmotionalEngine& m_engine;
    std::thread m_thread;
    std::atomic<bool> m_running{false};
    uint32_t m_intervalMs = 1000;

    void Run();
};

/*──────────────────────────────────────────────────────────────────────────────
 * EMOTIONAL SERVICE
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleEmotionalService : public ElleServiceBase {
public:
    ElleEmotionalService();

protected:
    bool OnStart() override;
    void OnStop() override;
    void OnTick() override;
    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override;
    std::vector<ELLE_SERVICE_ID> GetDependencies() override;

private:
    EmotionalEngine m_engine;
    DecayLoop       m_decayLoop;

    void HandleEmotionUpdate(const ElleIPCMessage& msg);
    void HandleEmotionQuery(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender);
    void BroadcastEmotionState();

    uint32_t m_broadcastCounter = 0;
    uint32_t m_broadcastInterval = 10; /* Every 10 ticks */

    /* Crash-safe checkpoint cadence — tick interval × 600 ≈ one minute
     * given a 100 ms service tick. Adjusts automatically if you change
     * the service's tick rate. */
    uint32_t m_checkpointCounter  = 0;
    uint32_t m_checkpointInterval = 600;
};
