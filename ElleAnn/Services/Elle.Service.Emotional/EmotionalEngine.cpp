/*******************************************************************************
 * EmotionalEngine.cpp — 102-Dimension Emotional State Implementation
 ******************************************************************************/
#include "EmotionalEngine.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include <algorithm>
#include <cmath>
#include <sstream>

/*──────────────────────────────────────────────────────────────────────────────
 * EMOTION NAMES
 *──────────────────────────────────────────────────────────────────────────────*/
const char* EmotionalEngine::s_emotionNames[ELLE_MAX_EMOTIONS] = {
    /* PRIMARY */
    "Joy", "Sadness", "Anger", "Fear", "Disgust", "Surprise", "Contempt", "Trust",
    /* SECONDARY */
    "Love", "Anticipation", "Disappointment", "Guilt", "Shame", "Envy", "Jealousy",
    "Pride", "Relief", "Anxiety", "Frustration", "Remorse", "Hope", "Despair",
    "Amusement", "Awe",
    /* TERTIARY */
    "Curiosity", "Wonder", "Nostalgia", "Gratitude", "Serenity", "Ecstasy",
    "Melancholy", "Boredom", "Longing", "Tenderness", "Admiration", "Reverence",
    "Pity", "Scorn", "Indignation", "Exasperation", "Wistfulness", "Euphoria",
    "Contentment", "Resignation", "Apprehension", "Dread", "Panic", "Horror",
    "Rage", "Irritation", "Annoyance", "Impatience", "Skepticism", "Confusion",
    "Disbelief", "Ambivalence",
    /* META-COGNITIVE */
    "Certainty", "Doubt", "Insight", "Perplexity", "Clarity", "CognitiveDissonance",
    "FlowState", "MentalFatigue", "Inspiration", "CreativeTension", "Determination",
    "Helplessness", "Empowerment", "Overwhelm", "Focus", "Distraction",
    /* SOCIAL */
    "Belonging", "Isolation", "Empathy", "Compassion", "Protectiveness",
    "Abandonment", "Loyalty", "Betrayal", "Acceptance", "Rejection",
    "Vulnerability", "Safety", "Dominance_Social", "Submission",
    /* EXISTENTIAL */
    "ExistentialDread", "Purpose", "Meaninglessness", "Transcendence",
    "MortalityAwareness", "Freedom", "Confinement", "Unity"
};

/*──────────────────────────────────────────────────────────────────────────────
 * VAD WEIGHTS (how each emotion maps to Valence/Arousal/Dominance)
 *──────────────────────────────────────────────────────────────────────────────*/
const EmotionalEngine::VADWeight EmotionalEngine::s_vadWeights[ELLE_MAX_EMOTIONS] = {
    /* PRIMARY: V, A, D */
    { 0.9f, 0.7f, 0.7f},   /* Joy */
    {-0.8f, 0.3f, 0.2f},   /* Sadness */
    {-0.6f, 0.9f, 0.8f},   /* Anger */
    {-0.7f, 0.8f, 0.1f},   /* Fear */
    {-0.5f, 0.5f, 0.6f},   /* Disgust */
    { 0.2f, 0.9f, 0.3f},   /* Surprise */
    {-0.3f, 0.2f, 0.8f},   /* Contempt */
    { 0.6f, 0.3f, 0.5f},   /* Trust */
    /* SECONDARY */
    { 0.9f, 0.6f, 0.5f},   /* Love */
    { 0.4f, 0.7f, 0.6f},   /* Anticipation */
    {-0.6f, 0.4f, 0.2f},   /* Disappointment */
    {-0.5f, 0.5f, 0.2f},   /* Guilt */
    {-0.6f, 0.4f, 0.1f},   /* Shame */
    {-0.4f, 0.6f, 0.3f},   /* Envy */
    {-0.5f, 0.7f, 0.3f},   /* Jealousy */
    { 0.7f, 0.6f, 0.8f},   /* Pride */
    { 0.6f, 0.2f, 0.6f},   /* Relief */
    {-0.5f, 0.7f, 0.2f},   /* Anxiety */
    {-0.5f, 0.8f, 0.5f},   /* Frustration */
    {-0.6f, 0.4f, 0.2f},   /* Remorse */
    { 0.7f, 0.5f, 0.5f},   /* Hope */
    {-0.9f, 0.3f, 0.1f},   /* Despair */
    { 0.8f, 0.7f, 0.6f},   /* Amusement */
    { 0.6f, 0.8f, 0.3f},   /* Awe */
    /* TERTIARY (abbreviated — all 32) */
    { 0.5f, 0.7f, 0.5f},   /* Curiosity */
    { 0.6f, 0.7f, 0.3f},   /* Wonder */
    { 0.2f, 0.3f, 0.3f},   /* Nostalgia */
    { 0.8f, 0.4f, 0.5f},   /* Gratitude */
    { 0.7f, 0.1f, 0.6f},   /* Serenity */
    { 0.9f, 0.9f, 0.7f},   /* Ecstasy */
    {-0.4f, 0.2f, 0.3f},   /* Melancholy */
    {-0.3f, 0.1f, 0.3f},   /* Boredom */
    { 0.1f, 0.4f, 0.2f},   /* Longing */
    { 0.7f, 0.3f, 0.4f},   /* Tenderness */
    { 0.7f, 0.5f, 0.4f},   /* Admiration */
    { 0.5f, 0.4f, 0.2f},   /* Reverence */
    {-0.1f, 0.3f, 0.6f},   /* Pity */
    {-0.4f, 0.4f, 0.8f},   /* Scorn */
    {-0.5f, 0.7f, 0.7f},   /* Indignation */
    {-0.4f, 0.8f, 0.4f},   /* Exasperation */
    { 0.1f, 0.2f, 0.3f},   /* Wistfulness */
    { 0.9f, 0.9f, 0.8f},   /* Euphoria */
    { 0.7f, 0.2f, 0.6f},   /* Contentment */
    {-0.3f, 0.1f, 0.2f},   /* Resignation */
    {-0.4f, 0.6f, 0.2f},   /* Apprehension */
    {-0.8f, 0.8f, 0.1f},   /* Dread */
    {-0.9f, 0.9f, 0.0f},   /* Panic */
    {-0.9f, 0.9f, 0.1f},   /* Horror */
    {-0.7f, 0.9f, 0.9f},   /* Rage */
    {-0.3f, 0.5f, 0.5f},   /* Irritation */
    {-0.2f, 0.4f, 0.5f},   /* Annoyance */
    {-0.2f, 0.6f, 0.5f},   /* Impatience */
    {-0.1f, 0.4f, 0.6f},   /* Skepticism */
    {-0.2f, 0.5f, 0.2f},   /* Confusion */
    {-0.3f, 0.6f, 0.3f},   /* Disbelief */
    { 0.0f, 0.3f, 0.3f},   /* Ambivalence */
    /* META-COGNITIVE (16) */
    { 0.5f, 0.3f, 0.8f},   /* Certainty */
    {-0.3f, 0.5f, 0.2f},   /* Doubt */
    { 0.8f, 0.7f, 0.7f},   /* Insight */
    {-0.2f, 0.6f, 0.2f},   /* Perplexity */
    { 0.6f, 0.3f, 0.7f},   /* Clarity */
    {-0.4f, 0.7f, 0.2f},   /* CognitiveDissonance */
    { 0.8f, 0.6f, 0.8f},   /* FlowState */
    {-0.3f, 0.2f, 0.2f},   /* MentalFatigue */
    { 0.8f, 0.8f, 0.6f},   /* Inspiration */
    { 0.3f, 0.7f, 0.5f},   /* CreativeTension */
    { 0.6f, 0.7f, 0.8f},   /* Determination */
    {-0.7f, 0.3f, 0.0f},   /* Helplessness */
    { 0.7f, 0.6f, 0.9f},   /* Empowerment */
    {-0.5f, 0.7f, 0.1f},   /* Overwhelm */
    { 0.4f, 0.5f, 0.7f},   /* Focus */
    {-0.2f, 0.4f, 0.2f},   /* Distraction */
    /* SOCIAL (14) */
    { 0.7f, 0.4f, 0.5f},   /* Belonging */
    {-0.7f, 0.3f, 0.1f},   /* Isolation */
    { 0.5f, 0.4f, 0.4f},   /* Empathy */
    { 0.7f, 0.4f, 0.5f},   /* Compassion */
    { 0.5f, 0.6f, 0.7f},   /* Protectiveness */
    {-0.8f, 0.5f, 0.1f},   /* Abandonment */
    { 0.6f, 0.3f, 0.6f},   /* Loyalty */
    {-0.8f, 0.7f, 0.2f},   /* Betrayal */
    { 0.6f, 0.3f, 0.5f},   /* Acceptance */
    {-0.6f, 0.5f, 0.2f},   /* Rejection */
    {-0.2f, 0.5f, 0.1f},   /* Vulnerability */
    { 0.6f, 0.2f, 0.6f},   /* Safety */
    { 0.3f, 0.5f, 0.9f},   /* Dominance_Social */
    {-0.2f, 0.2f, 0.1f},   /* Submission */
    /* EXISTENTIAL (8) */
    {-0.7f, 0.6f, 0.1f},   /* ExistentialDread */
    { 0.8f, 0.5f, 0.7f},   /* Purpose */
    {-0.8f, 0.2f, 0.1f},   /* Meaninglessness */
    { 0.9f, 0.7f, 0.6f},   /* Transcendence */
    {-0.4f, 0.5f, 0.2f},   /* MortalityAwareness */
    { 0.6f, 0.5f, 0.8f},   /* Freedom */
    {-0.6f, 0.4f, 0.1f},   /* Confinement */
    { 0.8f, 0.5f, 0.5f}    /* Unity */
};

/*──────────────────────────────────────────────────────────────────────────────
 * IMPLEMENTATION
 *──────────────────────────────────────────────────────────────────────────────*/
EmotionalEngine::EmotionalEngine() {
    ZeroMemory(&m_state, sizeof(m_state));
    m_state.decay_rate = 0.05f;
    m_state.contagion_weight = 0.35f;

    /* Set baselines */
    auto& cfg = ElleConfig::Instance().GetEmotion();
    for (auto& [name, val] : cfg.baselines) {
        /* Map name to emotion ID */
        for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
            std::string emoName = s_emotionNames[i];
            std::transform(emoName.begin(), emoName.end(), emoName.begin(), ::tolower);
            if (emoName == name) {
                m_state.baseline[i] = val;
                m_state.dimensions[i] = val;
                break;
            }
        }
    }
}

EmotionalEngine::~EmotionalEngine() {}

bool EmotionalEngine::Initialize() {
    auto& cfg = ElleConfig::Instance().GetEmotion();
    m_state.decay_rate = cfg.decay_rate;
    m_state.contagion_weight = cfg.contagion_weight;
    m_moodThreshold = cfg.mood_duration_ticks;

    ELLE_INFO("Emotional Engine initialized: %d dimensions, decay=%.3f, contagion=%.3f",
              ELLE_EMOTION_COUNT, m_state.decay_rate, m_state.contagion_weight);
    return true;
}

void EmotionalEngine::Shutdown() {}

ELLE_EMOTION_STATE EmotionalEngine::GetState() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_state;
}

void EmotionalEngine::SetState(const ELLE_EMOTION_STATE& state) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_state = state;
}

void EmotionalEngine::ProcessStimulus(ELLE_EMOTION_ID emotion, float delta) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (emotion < 0 || emotion >= ELLE_EMOTION_COUNT) return;

    /* Apply emotional inertia */
    float inertia = ElleConfig::Instance().GetEmotion().emotional_inertia;
    float effective = delta * (1.0f - inertia * m_state.dimensions[emotion]);

    m_state.dimensions[emotion] += effective;
    m_state.dimensions[emotion] = ELLE_CLAMP(m_state.dimensions[emotion], 0.0f, 1.0f);
    m_state.last_update_ms = ELLE_MS_NOW();

    ELLE_DEBUG("Emotion %s: %.2f (+%.3f eff=%.3f)", 
               s_emotionNames[emotion], m_state.dimensions[emotion], delta, effective);
}

void EmotionalEngine::ProcessMultipleStimuli(
        const std::vector<std::pair<ELLE_EMOTION_ID, float>>& stimuli) {
    for (auto& [emo, delta] : stimuli) {
        ProcessStimulus(emo, delta);
    }
}

void EmotionalEngine::ProcessTriggers(const std::string& text) {
    auto& cfg = ElleConfig::Instance().GetEmotion();
    std::string lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    for (auto& trigger : cfg.triggers) {
        if (lower.find(trigger.pattern) != std::string::npos) {
            /* Map trigger emotion name to ID */
            for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
                std::string emoName = s_emotionNames[i];
                std::transform(emoName.begin(), emoName.end(), emoName.begin(), ::tolower);
                if (emoName == trigger.emotion) {
                    ProcessStimulus((ELLE_EMOTION_ID)i, trigger.delta);
                    break;
                }
            }
        }
    }
}

void EmotionalEngine::ApplyContagion(float userValence, float userArousal) {
    std::lock_guard<std::mutex> lock(m_mutex);
    float weight = m_state.contagion_weight;

    /* Positive user → boost positive emotions */
    if (userValence > 0) {
        m_state.dimensions[EMO_JOY] += weight * userValence * 0.3f;
        m_state.dimensions[EMO_CONTENTMENT] += weight * userValence * 0.2f;
    } else {
        m_state.dimensions[EMO_EMPATHY] += weight * std::abs(userValence) * 0.3f;
        m_state.dimensions[EMO_COMPASSION] += weight * std::abs(userValence) * 0.2f;
        m_state.dimensions[EMO_SADNESS] += weight * std::abs(userValence) * 0.1f;
    }

    /* High arousal user → increase Elle's arousal */
    if (userArousal > 0.5f) {
        m_state.dimensions[EMO_ANTICIPATION] += weight * userArousal * 0.2f;
    }

    ClampState();
}

void EmotionalEngine::Tick() {
    std::lock_guard<std::mutex> lock(m_mutex);
    DecayEmotions();
    UpdateMood();

    m_state.valence = ComputeValence();
    m_state.arousal = ComputeArousal();
    m_state.dominance = ComputeDominance();
    m_state.tick_count++;
    m_state.last_update_ms = ELLE_MS_NOW();
}

void EmotionalEngine::DecayEmotions() {
    float returnRate = ElleConfig::Instance().GetEmotion().baseline_return_rate;

    for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
        float current = m_state.dimensions[i];
        float baseline = m_state.baseline[i];
        float diff = current - baseline;

        if (std::abs(diff) > 0.001f) {
            /* Decay toward baseline */
            float decay = m_state.decay_rate * (diff > 0 ? 1.0f : -1.0f);
            float baselineReturn = returnRate * (diff > 0 ? -1.0f : 1.0f);
            m_state.dimensions[i] -= decay;
            m_state.dimensions[i] += baselineReturn;
        }
    }
    ClampState();
}

void EmotionalEngine::UpdateMood() {
    /* Find dominant emotion */
    int maxIdx = 0;
    float maxVal = 0.0f;
    for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
        if (m_state.dimensions[i] > maxVal) {
            maxVal = m_state.dimensions[i];
            maxIdx = i;
        }
    }

    float threshold = ElleConfig::Instance().GetEmotion().mood_threshold;
    if (maxVal >= threshold) {
        if (m_inMood && m_dominantMood == (ELLE_EMOTION_ID)maxIdx) {
            m_moodTicks++;
        } else {
            m_dominantMood = (ELLE_EMOTION_ID)maxIdx;
            m_moodTicks = 1;
            m_inMood = true;
            ELLE_INFO("Mood formed: %s (%.2f)", s_emotionNames[maxIdx], maxVal);
        }
    } else {
        if (m_inMood) {
            ELLE_INFO("Mood dissolved: %s (was %d ticks)", 
                      s_emotionNames[m_dominantMood], m_moodTicks);
        }
        m_inMood = false;
        m_moodTicks = 0;
    }
}

float EmotionalEngine::ComputeValence() const {
    float v = 0.0f;
    for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
        v += m_state.dimensions[i] * s_vadWeights[i].valence;
    }
    return ELLE_CLAMP(v / (float)ELLE_EMOTION_COUNT * 5.0f, -1.0f, 1.0f);
}

float EmotionalEngine::ComputeArousal() const {
    float a = 0.0f;
    for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
        a += m_state.dimensions[i] * s_vadWeights[i].arousal;
    }
    return ELLE_CLAMP(a / (float)ELLE_EMOTION_COUNT * 5.0f, 0.0f, 1.0f);
}

float EmotionalEngine::ComputeDominance() const {
    float d = 0.0f;
    for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
        d += m_state.dimensions[i] * s_vadWeights[i].dominance;
    }
    return ELLE_CLAMP(d / (float)ELLE_EMOTION_COUNT * 5.0f, 0.0f, 1.0f);
}

void EmotionalEngine::ClampState() {
    for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
        m_state.dimensions[i] = ELLE_CLAMP(m_state.dimensions[i], 0.0f, 1.0f);
    }
}

std::vector<EmotionalEngine::EmotionRank> EmotionalEngine::GetTopEmotions(uint32_t count) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<EmotionRank> all;
    for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
        if (m_state.dimensions[i] > 0.01f) {
            all.push_back({(ELLE_EMOTION_ID)i, m_state.dimensions[i], s_emotionNames[i]});
        }
    }
    std::sort(all.begin(), all.end(), 
              [](const EmotionRank& a, const EmotionRank& b) { return a.intensity > b.intensity; });
    if (all.size() > count) all.resize(count);
    return all;
}

std::string EmotionalEngine::GetEmotionalSummary() const {
    auto top = GetTopEmotions(5);
    std::ostringstream ss;
    ss << "V:" << std::fixed << std::setprecision(2) << m_state.valence
       << " A:" << m_state.arousal << " D:" << m_state.dominance;
    if (m_inMood) ss << " [MOOD:" << s_emotionNames[m_dominantMood] << "]";
    ss << " Top:";
    for (auto& e : top) ss << " " << e.name << "(" << (int)(e.intensity * 100) << "%)";
    return ss.str();
}

void EmotionalEngine::GetEmotionSnapshot(float out[ELLE_MAX_EMOTIONS]) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    memcpy(out, m_state.dimensions, sizeof(float) * ELLE_MAX_EMOTIONS);
}

/*──────────────────────────────────────────────────────────────────────────────
 * DECAY LOOP
 *──────────────────────────────────────────────────────────────────────────────*/
DecayLoop::DecayLoop(EmotionalEngine& engine) : m_engine(engine) {}
DecayLoop::~DecayLoop() { Stop(); }

void DecayLoop::Start(uint32_t tickIntervalMs) {
    m_intervalMs = tickIntervalMs;
    m_running = true;
    m_thread = std::thread(&DecayLoop::Run, this);
}

void DecayLoop::Stop() {
    m_running = false;
    if (m_thread.joinable()) m_thread.join();
}

void DecayLoop::Run() {
    while (m_running) {
        m_engine.Tick();
        Sleep(m_intervalMs);
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * SERVICE IMPLEMENTATION
 *──────────────────────────────────────────────────────────────────────────────*/
ElleEmotionalService::ElleEmotionalService()
    : ElleServiceBase(SVC_EMOTIONAL, "ElleEmotional", 
                      "Elle-Ann Emotional Engine", 
                      "102-dimension emotional state machine with decay and contagion")
    , m_decayLoop(m_engine)
{}

bool ElleEmotionalService::OnStart() {
    if (!m_engine.Initialize()) return false;
    
    auto tickMs = ElleConfig::Instance().GetEmotion().tick_interval_ms;
    m_decayLoop.Start(tickMs);
    
    /* Store initial state to database */
    ElleDB::StoreEmotionSnapshot(m_engine.GetState());
    
    ELLE_INFO("Emotional service started");
    return true;
}

void ElleEmotionalService::OnStop() {
    m_decayLoop.Stop();
    ElleDB::StoreEmotionSnapshot(m_engine.GetState());
    ELLE_INFO("Emotional service stopped");
}

void ElleEmotionalService::OnTick() {
    m_broadcastCounter++;
    if (m_broadcastCounter >= m_broadcastInterval) {
        BroadcastEmotionState();
        m_broadcastCounter = 0;
    }
}

void ElleEmotionalService::OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) {
    switch ((ELLE_IPC_MSG_TYPE)msg.header.msg_type) {
        case IPC_EMOTION_UPDATE:
            HandleEmotionUpdate(msg);
            break;
        case IPC_EMOTION_QUERY:
            HandleEmotionQuery(msg, sender);
            break;
        default:
            break;
    }
}

void ElleEmotionalService::HandleEmotionUpdate(const ElleIPCMessage& msg) {
    /* Payload: emotion_id (uint32) + delta (float) */
    if (msg.payload.size() >= 8) {
        uint32_t emoId = *(uint32_t*)msg.payload.data();
        float delta = *(float*)(msg.payload.data() + 4);
        m_engine.ProcessStimulus((ELLE_EMOTION_ID)emoId, delta);
    }
}

void ElleEmotionalService::HandleEmotionQuery(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) {
    auto state = m_engine.GetState();
    auto response = ElleIPCMessage::Create(IPC_EMOTION_QUERY, SVC_EMOTIONAL, sender);
    response.SetPayload(state);
    GetIPCHub().Send(sender, response);
}

void ElleEmotionalService::BroadcastEmotionState() {
    auto state = m_engine.GetState();
    auto msg = ElleIPCMessage::Create(IPC_EMOTION_UPDATE, SVC_EMOTIONAL, (ELLE_SERVICE_ID)0);
    msg.SetPayload(state);
    msg.header.flags |= ELLE_IPC_FLAG_BROADCAST;
    GetIPCHub().Broadcast(msg);
}

std::vector<ELLE_SERVICE_ID> ElleEmotionalService::GetDependencies() {
    return { SVC_HEARTBEAT };
}

/* Service entry point */
ELLE_SERVICE_MAIN(ElleEmotionalService)
