/*******************************************************************************
 * MemoryEngine.cpp — STM/LTM Memory System Implementation
 ******************************************************************************/
#include "MemoryEngine.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"
#include "../../Shared/ElleLLM.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <set>

MemoryEngine::MemoryEngine() {}
MemoryEngine::~MemoryEngine() { Shutdown(); }

bool MemoryEngine::Initialize() {
    ELLE_INFO("Memory Engine initializing...");
    ELLE_INFO("  STM capacity: %d", ElleConfig::Instance().GetMemory().stm_capacity);
    ELLE_INFO("  STM decay: %.1fs", ElleConfig::Instance().GetMemory().stm_decay_seconds);
    ELLE_INFO("  Consolidation interval: %dm", ElleConfig::Instance().GetMemory().consolidation_interval_min);
    return true;
}

void MemoryEngine::Shutdown() {
    /* Flush any remaining STM to LTM */
    ConsolidateMemories();
}

uint64_t MemoryEngine::StoreSTM(const std::string& content, float importance,
                                  const float emotions[ELLE_MAX_EMOTIONS],
                                  const std::vector<std::string>& tags) {
    std::lock_guard<std::mutex> lock(m_stmMutex);

    ELLE_MEMORY_RECORD mem = {};
    mem.id = m_nextId++;
    mem.type = MEM_EPISODIC;
    mem.tier = MEM_STM;
    strncpy_s(mem.content, content.c_str(), ELLE_MAX_MSG - 1);
    mem.importance = importance;
    mem.relevance = 1.0f;
    mem.decay = 0.0f;
    mem.created_ms = ELLE_MS_NOW();
    mem.last_access_ms = mem.created_ms;
    mem.access_count = 0;

    if (emotions) {
        memcpy(mem.emotion_snapshot, emotions, sizeof(float) * ELLE_MAX_EMOTIONS);
        /* Compute emotional valence from snapshot */
        float v = 0.0f;
        for (int i = 0; i < ELLE_EMOTION_COUNT; i++) v += emotions[i];
        mem.emotional_valence = v / ELLE_EMOTION_COUNT;
    }

    /* Tags */
    mem.tag_count = std::min((uint32_t)tags.size(), (uint32_t)ELLE_MAX_TAGS);
    for (uint32_t i = 0; i < mem.tag_count; i++) {
        strncpy_s(mem.tags[i], tags[i].c_str(), ELLE_MAX_TAG - 1);
    }

    /* Compute 3D position */
    ComputePosition(mem);

    m_stm.push_back(mem);

    /* Enforce capacity */
    auto cap = ElleConfig::Instance().GetMemory().stm_capacity;
    while (m_stm.size() > cap) {
        m_stm.pop_front();
    }

    ELLE_DEBUG("STM stored: [%llu] %.50s... (importance=%.2f)", mem.id, content.c_str(), importance);
    return mem.id;
}

uint64_t MemoryEngine::StoreLTM(const ELLE_MEMORY_RECORD& record) {
    ELLE_MEMORY_RECORD mem = record;
    if (mem.id == 0) mem.id = m_nextId++;
    mem.tier = MEM_LTM;
    if (mem.created_ms == 0) mem.created_ms = ELLE_MS_NOW();

    ComputePosition(mem);
    ElleDB::StoreMemory(mem);

    ELLE_DEBUG("LTM stored: [%llu] %.50s...", mem.id, mem.content);
    return mem.id;
}

std::vector<ELLE_MEMORY_RECORD> MemoryEngine::Recall(const std::string& query,
                                                       uint32_t maxResults,
                                                       float minRelevance) {
    std::vector<ELLE_MEMORY_RECORD> results;

    /* Search STM first */
    {
        std::lock_guard<std::mutex> lock(m_stmMutex);
        for (auto& mem : m_stm) {
            float rel = ComputeRelevance(mem, query);
            if (rel >= minRelevance) {
                ELLE_MEMORY_RECORD copy = mem;
                copy.relevance = rel;
                results.push_back(copy);
            }
        }
    }

    /* Search LTM from database */
    std::vector<ELLE_MEMORY_RECORD> ltmResults;
    ElleDB::RecallMemories(query, ltmResults, maxResults, minRelevance);
    results.insert(results.end(), ltmResults.begin(), ltmResults.end());

    /* Sort by relevance */
    std::sort(results.begin(), results.end(),
              [](const ELLE_MEMORY_RECORD& a, const ELLE_MEMORY_RECORD& b) {
                  return a.relevance > b.relevance;
              });

    if (results.size() > maxResults) results.resize(maxResults);

    /* Boost access counts */
    for (auto& mem : results) {
        mem.access_count++;
        mem.last_access_ms = ELLE_MS_NOW();
        if (mem.tier == MEM_LTM) {
            ElleDB::UpdateMemoryAccess(mem.id);
        }
    }

    return results;
}

void MemoryEngine::ConsolidateMemories() {
    std::lock_guard<std::mutex> lock(m_stmMutex);

    auto threshold = ElleConfig::Instance().GetMemory().promote_threshold;
    auto maxPromote = (uint32_t)ElleConfig::Instance().GetMemory().stm_capacity / 4;
    uint32_t promoted = 0;

    auto it = m_stm.begin();
    while (it != m_stm.end() && promoted < maxPromote) {
        /* Criteria for promotion: importance >= threshold OR accessed multiple times */
        bool shouldPromote = (it->importance >= threshold) || 
                             (it->access_count >= 3) ||
                             (it->emotional_valence > 0.5f || it->emotional_valence < -0.5f);

        if (shouldPromote) {
            it->tier = MEM_LTM;
            ElleDB::StoreMemory(*it);
            promoted++;
            ELLE_INFO("Memory promoted to LTM: [%llu] %.50s...", it->id, it->content);
            it = m_stm.erase(it);
        } else {
            ++it;
        }
    }

    if (promoted > 0) {
        ELLE_INFO("Consolidated %d memories from STM to LTM", promoted);
    }
}

void MemoryEngine::DecaySTM() {
    std::lock_guard<std::mutex> lock(m_stmMutex);

    auto decaySec = ElleConfig::Instance().GetMemory().stm_decay_seconds;
    uint64_t now = ELLE_MS_NOW();
    float decayPerMs = 1.0f / (decaySec * 1000.0f);

    auto it = m_stm.begin();
    while (it != m_stm.end()) {
        float age = (float)(now - it->created_ms);
        it->decay = age * decayPerMs;
        it->relevance = 1.0f - it->decay;

        /* Remove fully decayed memories that weren't important enough */
        if (it->relevance <= 0.0f && it->importance < 0.5f) {
            ELLE_DEBUG("STM decayed: [%llu] %.30s...", it->id, it->content);
            it = m_stm.erase(it);
        } else {
            ++it;
        }
    }
}

void MemoryEngine::ComputePosition(ELLE_MEMORY_RECORD& mem) {
    /* 3D Memory Map:
       X = time (normalized)
       Y = emotional valence
       Z = importance */
    mem.position_x = (float)(mem.created_ms % 86400000) / 86400000.0f * 100.0f;
    mem.position_y = mem.emotional_valence * 50.0f + 50.0f;
    mem.position_z = mem.importance * 100.0f;
}

float MemoryEngine::ComputeRelevance(const ELLE_MEMORY_RECORD& mem, const std::string& query) {
    float textSim = TextSimilarity(std::string(mem.content), query);
    float recency = 1.0f - (float)(ELLE_MS_NOW() - mem.last_access_ms) / (86400000.0f * 7);
    recency = ELLE_CLAMP(recency, 0.0f, 1.0f);
    float importance = mem.importance;
    return textSim * 0.5f + recency * 0.3f + importance * 0.2f;
}

float MemoryEngine::TextSimilarity(const std::string& a, const std::string& b) {
    /* Simple keyword overlap (in production, use embeddings) */
    std::set<std::string> wordsA, wordsB;
    std::istringstream issA(a), issB(b);
    std::string word;
    while (issA >> word) { std::transform(word.begin(), word.end(), word.begin(), ::tolower); wordsA.insert(word); }
    while (issB >> word) { std::transform(word.begin(), word.end(), word.begin(), ::tolower); wordsB.insert(word); }

    if (wordsA.empty() || wordsB.empty()) return 0.0f;

    uint32_t overlap = 0;
    for (auto& w : wordsA) {
        if (wordsB.count(w)) overlap++;
    }

    return (float)overlap / (float)std::max(wordsA.size(), wordsB.size());
}

float MemoryEngine::ComputeEmotionSimilarity(const float a[ELLE_MAX_EMOTIONS],
                                               const float b[ELLE_MAX_EMOTIONS]) {
    float dot = 0, magA = 0, magB = 0;
    for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
        dot += a[i] * b[i];
        magA += a[i] * a[i];
        magB += b[i] * b[i];
    }
    float denom = std::sqrt(magA) * std::sqrt(magB);
    return denom > 0.0f ? dot / denom : 0.0f;
}

void MemoryEngine::DreamConsolidate(const std::string& narrativePrompt) {
    ELLE_INFO("Dream consolidation starting...");

    /* During dream: reorganize, strengthen important connections, prune weak */
    ConsolidateMemories();
    UpdateClusters();

    /* Generate narrative from recent memories */
    auto recent = RecallRecent(20);
    if (!recent.empty()) {
        std::ostringstream ss;
        ss << "Process and find connections between these memories:\n";
        for (auto& m : recent) {
            ss << "- " << m.content << " (importance: " << m.importance << ")\n";
        }

        auto narrative = ElleLLMEngine::Instance().Ask(ss.str(),
            "You are processing memories during a dream state. Find patterns, "
            "connections, and insights. Generate a brief dream narrative.");
        
        if (!narrative.empty()) {
            float emo[ELLE_MAX_EMOTIONS] = {};
            StoreSTM(narrative, 0.6f, emo, {"dream", "insight"});
            ELLE_INFO("Dream narrative generated: %.80s...", narrative.c_str());
        }
    }

    ELLE_INFO("Dream consolidation complete");
}

std::vector<ELLE_MEMORY_RECORD> MemoryEngine::RecallRecent(uint32_t count) {
    std::lock_guard<std::mutex> lock(m_stmMutex);
    std::vector<ELLE_MEMORY_RECORD> results;
    uint32_t n = std::min(count, (uint32_t)m_stm.size());
    for (uint32_t i = 0; i < n; i++) {
        results.push_back(m_stm[m_stm.size() - 1 - i]);
    }
    return results;
}

void MemoryEngine::UpdateClusters() {
    /* Simple clustering based on emotional similarity */
    ELLE_DEBUG("Updating memory clusters...");
}

void MemoryEngine::LinkMemories(uint64_t id1, uint64_t id2) {
    std::lock_guard<std::mutex> lock(m_stmMutex);
    for (auto& mem : m_stm) {
        if (mem.id == id1 && mem.link_count < 16) {
            mem.linked_ids[mem.link_count++] = (uint32_t)id2;
        }
        if (mem.id == id2 && mem.link_count < 16) {
            mem.linked_ids[mem.link_count++] = (uint32_t)id1;
        }
    }
}

uint32_t MemoryEngine::STMCount() const { return (uint32_t)m_stm.size(); }
uint32_t MemoryEngine::ClusterCount() const { return (uint32_t)m_clusters.size(); }

/*──────────────────────────────────────────────────────────────────────────────
 * RECALL LOOP
 *──────────────────────────────────────────────────────────────────────────────*/
RecallLoop::RecallLoop(MemoryEngine& engine) : m_engine(engine) {}
RecallLoop::~RecallLoop() { Stop(); }

void RecallLoop::Start() {
    m_running = true;
    m_thread = std::thread(&RecallLoop::Run, this);
}

void RecallLoop::Stop() {
    m_running = false;
    if (m_thread.joinable()) m_thread.join();
}

void RecallLoop::Run() {
    auto& cfg = ElleConfig::Instance().GetMemory();
    uint32_t recallMs = cfg.recall_interval_sec * 1000;
    uint32_t consolidateMs = cfg.consolidation_interval_min * 60 * 1000;
    uint64_t lastConsolidate = ELLE_MS_NOW();

    while (m_running) {
        /* Decay STM */
        m_engine.DecaySTM();

        /* Periodic consolidation */
        if (ELLE_MS_NOW() - lastConsolidate > consolidateMs) {
            m_engine.ConsolidateMemories();
            lastConsolidate = ELLE_MS_NOW();
        }

        Sleep(recallMs);
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * SERVICE IMPLEMENTATION
 *──────────────────────────────────────────────────────────────────────────────*/
ElleMemoryService::ElleMemoryService()
    : ElleServiceBase(SVC_MEMORY, "ElleMemory",
                      "Elle-Ann Memory Engine",
                      "STM/LTM memory with 3D positioning and consolidation")
    , m_recallLoop(m_engine)
{}

bool ElleMemoryService::OnStart() {
    if (!m_engine.Initialize()) return false;
    m_recallLoop.Start();
    ELLE_INFO("Memory service started (STM: %d slots)", 
              ElleConfig::Instance().GetMemory().stm_capacity);
    return true;
}

void ElleMemoryService::OnStop() {
    m_recallLoop.Stop();
    m_engine.Shutdown();
    ELLE_INFO("Memory service stopped");
}

void ElleMemoryService::OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) {
    switch ((ELLE_IPC_MSG_TYPE)msg.header.msg_type) {
        case IPC_MEMORY_STORE: {
            ELLE_MEMORY_RECORD rec;
            if (msg.GetPayload(rec)) {
                m_engine.StoreLTM(rec);
            } else {
                /* String payload = quick STM store */
                float emo[ELLE_MAX_EMOTIONS] = {};
                m_engine.StoreSTM(msg.GetStringPayload(), 0.5f, emo);
            }
            break;
        }
        case IPC_MEMORY_RECALL: {
            std::string query = msg.GetStringPayload();
            auto results = m_engine.Recall(query);
            /* Send back results */
            for (auto& mem : results) {
                auto resp = ElleIPCMessage::Create(IPC_MEMORY_RESULT, SVC_MEMORY, sender);
                resp.SetPayload(mem);
                GetIPCHub().Send(sender, resp);
            }
            break;
        }
        case IPC_DREAM_TRIGGER:
            m_engine.DreamConsolidate();
            break;
        default:
            break;
    }
}

std::vector<ELLE_SERVICE_ID> ElleMemoryService::GetDependencies() {
    return { SVC_HEARTBEAT, SVC_EMOTIONAL };
}

ELLE_SERVICE_MAIN(ElleMemoryService)
