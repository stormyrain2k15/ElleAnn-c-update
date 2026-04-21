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

/*──────────────────────────────────────────────────────────────────────────────
 * Priority-tier decay model — ported from legacy Python ShortTermMemoryStore
 * (ElleAnnServer/app/services/short_term_memory.py).
 *
 * The C++ ELLE_MEMORY_RECORD has no explicit "priority" field, so we derive
 * one from the importance score. This keeps wire-compat while giving decay
 * behaviour that matches the legacy Python core Elle was trained on.
 *
 * Tiers (lowest index = most durable):
 *   0 critical : importance >= 0.90  — never decays
 *   1 high     : importance >= 0.70  — ~33 min to full decay
 *   2 normal   : importance >= 0.40  — ~8 min
 *   3 low      : importance >= 0.20  — ~2 min
 *   4 fleeting : else                — ~20 sec
 *
 * All rates are expressed per-second. They can be globally scaled by
 * cfg.stm_decay_seconds (default 15) — larger value = slower decay.
 *──────────────────────────────────────────────────────────────────────────────*/
namespace {
    constexpr float kDecayRatePerSec[5] = {
        0.0000f,   /* critical */
        0.0005f,   /* high     */
        0.0020f,   /* normal   */
        0.0080f,   /* low      */
        0.0500f,   /* fleeting */
    };
    constexpr float kAccessBoost[5] = {
        0.00f,     /* critical (no-op) */
        0.30f,     /* high             */
        0.40f,     /* normal           */
        0.50f,     /* low              */
        0.60f,     /* fleeting         */
    };
    /* Below this relevance, entry is auto-promoted to LTM. Matches Python default. */
    constexpr float kPromotionRelevance  = 0.15f;
    /* Below this, entry is force-evicted (still promoted first — no data loss).    */
    constexpr float kMinRelevanceFloor   = 0.05f;

    inline int PriorityTier(float importance) {
        if (importance >= 0.90f) return 0; /* critical */
        if (importance >= 0.70f) return 1; /* high     */
        if (importance >= 0.40f) return 2; /* normal   */
        if (importance >= 0.20f) return 3; /* low      */
        return 4;                          /* fleeting */
    }

    inline const char* PriorityLabel(int t) {
        static const char* L[] = { "critical", "high", "normal", "low", "fleeting" };
        return (t >= 0 && t <= 4) ? L[t] : "unknown";
    }
}

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
    /* Flush ALL remaining STM to LTM — on shutdown we can't afford to lose
     * anything to the promote-criteria filter. Every live entry gets written. */
    std::lock_guard<std::mutex> lock(m_stmMutex);
    if (m_stm.empty()) return;

    uint32_t flushed = 0;
    for (auto& e : m_stm) {
        e.tier = MEM_LTM;
        if (ElleDB::StoreMemory(e)) flushed++;
    }
    ELLE_INFO("Memory shutdown: flushed %u STM entries to LTM", flushed);
    m_stm.clear();
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

    /* Enforce capacity — before falling off the end, promote lowest-relevance,
     * non-critical entries to LTM so nothing valuable is lost. Critical entries
     * (importance >= 0.90) are pinned and never evicted by capacity pressure.  */
    auto cap = ElleConfig::Instance().GetMemory().stm_capacity;
    while (m_stm.size() > cap) {
        auto victim = m_stm.end();
        float lowestRel = 2.0f;
        for (auto it = m_stm.begin(); it != m_stm.end(); ++it) {
            if (PriorityTier(it->importance) == 0) continue; /* skip critical */
            if (it->relevance < lowestRel) {
                lowestRel = it->relevance;
                victim = it;
            }
        }
        if (victim == m_stm.end()) {
            /* Every entry is critical — we must still honour the cap. Evict the
             * oldest critical (FIFO) but still promote it so it survives in LTM. */
            victim = m_stm.begin();
        }
        victim->tier = MEM_LTM;
        ElleDB::StoreMemory(*victim);
        ELLE_DEBUG("STM capacity pressure: promoted [%llu] %.50s... (rel=%.2f)",
                   victim->id, victim->content, victim->relevance);
        m_stm.erase(victim);
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

    /* Boost access counts & relevance (priority-aware). Matches Python
     * ShortTermMemoryStore.get(): accessing a memory should keep it alive. */
    uint64_t now = ELLE_MS_NOW();
    for (auto& mem : results) {
        mem.access_count++;
        mem.last_access_ms = now;
        if (mem.tier == MEM_STM) {
            float boost = kAccessBoost[PriorityTier(mem.importance)];
            mem.relevance = std::min(1.0f, mem.relevance + boost);
            /* Propagate the boost back into the live STM entry. */
            std::lock_guard<std::mutex> lock(m_stmMutex);
            for (auto& live : m_stm) {
                if (live.id == mem.id) {
                    live.access_count = mem.access_count;
                    live.last_access_ms = mem.last_access_ms;
                    live.relevance = mem.relevance;
                    break;
                }
            }
        } else if (mem.tier == MEM_LTM) {
            ElleDB::UpdateMemoryAccess(mem.id);
        }
    }

    return results;
}

void MemoryEngine::ConsolidateMemories() {
    std::lock_guard<std::mutex> lock(m_stmMutex);

    auto  cfg       = ElleConfig::Instance().GetMemory();
    float impFast   = cfg.promote_threshold;    /* fast-track promotion on importance */
    uint32_t promoted = 0;
    uint32_t dropped  = 0;

    /* Two-phase sweep:
     *  Phase 1 — refresh decay on every entry so we're working with fresh scores.
     *  Phase 2 — walk once; decide: promote-to-LTM, keep-in-STM, or drop.
     *
     * Promotion criteria (any one triggers LTM write + STM erase):
     *   - importance >= cfg.promote_threshold               (semantic fast-track)
     *   - access_count >= 3                                 (frequently recalled)
     *   - |emotional_valence| > 0.5                         (strong feelings → keep)
     *   - relevance <= kPromotionRelevance                  (natural decay auto-promo)
     * Force-drop criterion:
     *   - relevance <= 0 AND importance < 0.5               (trivial, fully faded)
     * Critical (importance >= 0.90) entries are never dropped by decay alone — they
     * stay resident in STM until explicitly consolidated via IPC_MEMORY_CONSOLIDATE
     * or until promoted on access-count / explicit importance fast-track.            */

    /* --- phase 1 : decay refresh (no erase) --- */
    {
        uint64_t now = ELLE_MS_NOW();
        float    scale = (cfg.stm_decay_seconds > 0.0f) ? (15.0f / cfg.stm_decay_seconds) : 1.0f;
        for (auto& e : m_stm) {
            int tier = PriorityTier(e.importance);
            float rate = kDecayRatePerSec[tier] * scale;
            if (rate <= 0.0f) continue; /* critical */
            float age_sec = (float)(now - e.created_ms) / 1000.0f;
            float rel = 1.0f - (rate * age_sec);
            if (e.access_count > 0) {
                float bump = std::min(0.5f, e.access_count * 0.05f);
                rel += bump;
            }
            e.decay     = 1.0f - rel;
            e.relevance = ELLE_CLAMP(rel, 0.0f, 1.0f);
        }
    }

    /* --- phase 2 : promote / keep / drop --- */
    auto it = m_stm.begin();
    while (it != m_stm.end()) {
        int  tier          = PriorityTier(it->importance);
        bool criticalTier  = (tier == 0);
        bool impFastTrack  = (it->importance >= impFast);
        bool accessFreq    = (it->access_count >= 3);
        bool strongEmotion = (it->emotional_valence > 0.5f || it->emotional_valence < -0.5f);
        bool decayedOut    = (it->relevance <= kPromotionRelevance);
        bool trivialGone   = (it->relevance <= 0.0f && it->importance < 0.5f);

        if (impFastTrack || accessFreq || strongEmotion ||
            (!criticalTier && decayedOut)) {
            it->tier = MEM_LTM;
            if (ElleDB::StoreMemory(*it)) {
                promoted++;
                ELLE_INFO("STM→LTM [%llu] pri=%s imp=%.2f rel=%.2f acc=%u | %.60s",
                          it->id, PriorityLabel(tier),
                          it->importance, it->relevance, it->access_count,
                          it->content);
            } else {
                ELLE_WARN("STM→LTM write failed for [%llu] — keeping in STM", it->id);
                ++it;
                continue;
            }
            it = m_stm.erase(it);
        } else if (!criticalTier && trivialGone) {
            ELLE_DEBUG("STM dropped trivial [%llu] imp=%.2f rel=%.2f | %.40s",
                       it->id, it->importance, it->relevance, it->content);
            it = m_stm.erase(it);
            dropped++;
        } else {
            ++it;
        }
    }

    if (promoted > 0 || dropped > 0) {
        ELLE_INFO("Consolidation pass: promoted=%u dropped=%u remaining_stm=%zu",
                  promoted, dropped, m_stm.size());
    }
}

void MemoryEngine::DecaySTM() {
    std::lock_guard<std::mutex> lock(m_stmMutex);

    auto  cfg   = ElleConfig::Instance().GetMemory();
    uint64_t now = ELLE_MS_NOW();
    float scale = (cfg.stm_decay_seconds > 0.0f) ? (15.0f / cfg.stm_decay_seconds) : 1.0f;

    /* Subconscious tick: refresh relevance on every entry using priority-tiered
     * decay rates. Does NOT evict — eviction/promotion is the consolidation pass
     * (periodic, or on-demand via IPC_MEMORY_CONSOLIDATE).                       */
    for (auto& e : m_stm) {
        int   tier = PriorityTier(e.importance);
        float rate = kDecayRatePerSec[tier] * scale;
        if (rate <= 0.0f) continue; /* critical — pinned */

        float age_sec = (float)(now - e.created_ms) / 1000.0f;
        float rel     = 1.0f - (rate * age_sec);
        if (e.access_count > 0) {
            float bump = std::min(0.5f, e.access_count * 0.05f);
            rel += bump;
        }
        e.decay     = 1.0f - rel;
        e.relevance = ELLE_CLAMP(rel, 0.0f, 1.0f);
    }

    /* Emergency floor sweep: anything that has slipped below kMinRelevanceFloor
     * for more than one decay interval gets promoted inline so we don't leak RAM
     * between consolidation ticks. Safety net — matches Python apply_decay(). */
    auto it = m_stm.begin();
    while (it != m_stm.end()) {
        if (PriorityTier(it->importance) != 0 &&
            it->relevance <= kMinRelevanceFloor) {
            it->tier = MEM_LTM;
            if (ElleDB::StoreMemory(*it)) {
                ELLE_DEBUG("DecaySTM floor-promoted [%llu] rel=%.3f", it->id, it->relevance);
            }
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
        case IPC_MEMORY_CONSOLIDATE:
            /* Conscious / on-demand consolidation. The subconscious RecallLoop
             * already ticks this periodically — this path lets the HTTP layer
             * (/api/server/commit-memory) or Cognitive engine force a flush. */
            ELLE_INFO("Conscious consolidation requested by service %u", (unsigned)sender);
            m_engine.ConsolidateMemories();
            break;
        default:
            break;
    }
}

std::vector<ELLE_SERVICE_ID> ElleMemoryService::GetDependencies() {
    return { SVC_HEARTBEAT, SVC_EMOTIONAL };
}

ELLE_SERVICE_MAIN(ElleMemoryService)
