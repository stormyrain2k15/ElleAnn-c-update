/*******************************************************************************
 * MemoryEngine.h — STM/LTM Memory System with 3D Positioning
 ******************************************************************************/
#pragma once
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include <vector>
#include <string>
#include <deque>
#include <mutex>
#include <unordered_map>

class MemoryEngine {
public:
    MemoryEngine();
    ~MemoryEngine();

    bool Initialize();
    void Shutdown();

    /* Store operations */
    uint64_t StoreSTM(const std::string& content, float importance, 
                       const float emotions[ELLE_MAX_EMOTIONS],
                       const std::vector<std::string>& tags = {});
    uint64_t StoreLTM(const ELLE_MEMORY_RECORD& record);

    /* Recall operations */
    std::vector<ELLE_MEMORY_RECORD> Recall(const std::string& query, 
                                            uint32_t maxResults = 10,
                                            float minRelevance = 0.3f);
    std::vector<ELLE_MEMORY_RECORD> RecallByTags(const std::vector<std::string>& tags, 
                                                  uint32_t maxResults = 10);
    std::vector<ELLE_MEMORY_RECORD> RecallByEmotion(ELLE_EMOTION_ID emotion, 
                                                     float minIntensity = 0.3f,
                                                     uint32_t maxResults = 10);
    std::vector<ELLE_MEMORY_RECORD> RecallRecent(uint32_t count = 10);

    /* STM → LTM promotion */
    void ConsolidateMemories();

    /* STM decay */
    void DecaySTM();

    /* Memory linking */
    void LinkMemories(uint64_t id1, uint64_t id2);

    /* Clustering */
    void UpdateClusters();

    /* 3D position computation */
    void ComputePosition(ELLE_MEMORY_RECORD& mem);

    /* Dream processing — reorganize memories during idle */
    void DreamConsolidate(const std::string& narrativePrompt = "");

    /* Statistics */
    uint32_t STMCount() const;
    uint64_t LTMCount() const;
    uint32_t ClusterCount() const;

    /* Get narrative summary of recent memories */
    std::string GenerateNarrativeSummary(uint32_t memoryCount = 10);

private:
    /* STM store (RAM) */
    std::deque<ELLE_MEMORY_RECORD> m_stm;
    std::mutex m_stmMutex;

    /* Memory ID counter */
    uint64_t m_nextId = 1;

    /* Cluster tracking */
    struct MemoryCluster {
        uint32_t id;
        std::string theme;
        float centroid[ELLE_MAX_EMOTIONS];
        std::vector<uint64_t> member_ids;
    };
    std::vector<MemoryCluster> m_clusters;

    /* Relevance scoring */
    float ComputeRelevance(const ELLE_MEMORY_RECORD& mem, const std::string& query);
    float ComputeEmotionSimilarity(const float a[ELLE_MAX_EMOTIONS], 
                                    const float b[ELLE_MAX_EMOTIONS]);
    /* Simple text similarity */
    float TextSimilarity(const std::string& a, const std::string& b);
};

/*──────────────────────────────────────────────────────────────────────────────
 * RECALL LOOP — Periodic background recall and consolidation
 *──────────────────────────────────────────────────────────────────────────────*/
class RecallLoop {
public:
    RecallLoop(MemoryEngine& engine);
    ~RecallLoop();

    void Start();
    void Stop();

private:
    MemoryEngine& m_engine;
    std::thread m_thread;
    std::atomic<bool> m_running{false};

    void Run();
};

/*──────────────────────────────────────────────────────────────────────────────
 * MEMORY SERVICE
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleMemoryService : public ElleServiceBase {
public:
    ElleMemoryService();

protected:
    bool OnStart() override;
    void OnStop() override;
    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override;
    std::vector<ELLE_SERVICE_ID> GetDependencies() override;

private:
    MemoryEngine m_engine;
    RecallLoop   m_recallLoop;
};
