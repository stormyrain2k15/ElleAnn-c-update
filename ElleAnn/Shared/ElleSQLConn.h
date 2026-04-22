/*******************************************************************************
 * ElleSQLConn.h — SQL Server Connection Pool via Named Pipes
 * 
 * Manages a pool of SQL Server connections using named pipe transport.
 * Thread-safe, connection recycling, prepared statement caching.
 ******************************************************************************/
#pragma once
#ifndef ELLE_SQL_CONN_H
#define ELLE_SQL_CONN_H

#include "ElleTypes.h"
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>
#include <unordered_map>

/* Forward declare ODBC types to avoid header dependency */
typedef void* SQLHENV;
typedef void* SQLHDBC;
typedef void* SQLHSTMT;

/*──────────────────────────────────────────────────────────────────────────────
 * SQL RESULT SET
 *──────────────────────────────────────────────────────────────────────────────*/
struct SQLColumn {
    std::string name;
    int32_t     type;       /* SQL_C_CHAR, SQL_C_LONG, etc. */
    uint32_t    size;
};

struct SQLRow {
    std::vector<std::string> values;
    
    const std::string& operator[](size_t idx) const { return values[idx]; }
    int64_t     GetInt(size_t idx) const;
    double      GetFloat(size_t idx) const;
    bool        IsNull(size_t idx) const;
};

struct SQLResultSet {
    std::vector<SQLColumn>  columns;
    std::vector<SQLRow>     rows;
    int64_t                 rows_affected = 0;
    bool                    success = false;
    std::string             error;

    bool        Empty() const { return rows.empty(); }
    size_t      RowCount() const { return rows.size(); }
    size_t      ColCount() const { return columns.size(); }
    int         ColIndex(const std::string& name) const;
};

/*──────────────────────────────────────────────────────────────────────────────
 * SQL CONNECTION (single connection wrapper)
 *──────────────────────────────────────────────────────────────────────────────*/
class SQLConnection {
public:
    SQLConnection();
    ~SQLConnection();

    bool Connect(const std::string& connectionString);
    void Disconnect();
    bool IsConnected() const;
    bool Ping();

    SQLResultSet Execute(const std::string& sql);
    SQLResultSet ExecuteParams(const std::string& sql, const std::vector<std::string>& params);
    SQLResultSet CallProc(const std::string& proc, const std::vector<std::string>& params);

    int64_t ExecuteScalar(const std::string& sql);
    bool    ExecuteNonQuery(const std::string& sql);

    bool BeginTransaction();
    bool Commit();
    bool Rollback();

    const std::string& LastError() const { return m_lastError; }
    uint64_t LastUsedMs() const { return m_lastUsed; }

private:
    SQLHENV     m_hEnv = nullptr;
    SQLHDBC     m_hDbc = nullptr;
    bool        m_connected = false;
    bool        m_inTransaction = false;
    std::string m_connStr;
    std::string m_lastError;
    uint64_t    m_lastUsed = 0;

    bool AllocHandles();
    void FreeHandles();
    std::string GetDiagnostics(int16_t handleType, void* handle);
};

/*──────────────────────────────────────────────────────────────────────────────
 * SQL CONNECTION POOL
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleSQLPool {
public:
    static ElleSQLPool& Instance();

    bool Initialize(const std::string& connectionString, uint32_t poolSize = 8);
    void Shutdown();

    /* Acquire/Release pattern */
    std::shared_ptr<SQLConnection> Acquire(uint32_t timeoutMs = 5000);
    void Release(std::shared_ptr<SQLConnection> conn);

    /* Convenience: auto-acquire, execute, release */
    SQLResultSet Query(const std::string& sql);
    SQLResultSet QueryParams(const std::string& sql, const std::vector<std::string>& params);
    SQLResultSet CallProc(const std::string& proc, const std::vector<std::string>& params);
    bool         Exec(const std::string& sql);
    int64_t      Scalar(const std::string& sql);

    /* Pool stats */
    uint32_t AvailableConnections() const;
    uint32_t TotalConnections() const { return m_poolSize; }
    uint64_t TotalQueries() const { return m_totalQueries; }

private:
    ElleSQLPool() = default;
    ~ElleSQLPool() = default;

    std::string     m_connStr;
    uint32_t        m_poolSize = 0;
    bool            m_initialized = false;

    std::queue<std::shared_ptr<SQLConnection>>  m_available;
    std::mutex                                   m_mutex;
    std::condition_variable                      m_cv;
    uint64_t                                     m_totalQueries = 0;

    bool CreateConnection(std::shared_ptr<SQLConnection>& conn);
};

/*──────────────────────────────────────────────────────────────────────────────
 * DATABASE HELPER — High-level operations for Elle-specific tables
 *──────────────────────────────────────────────────────────────────────────────*/
namespace ElleDB {

    /* Intent Queue */
    bool SubmitIntent(const ELLE_INTENT_RECORD& intent);
    bool GetPendingIntents(std::vector<ELLE_INTENT_RECORD>& out, uint32_t maxCount = 10);
    bool UpdateIntentStatus(uint64_t intentId, ELLE_INTENT_STATUS status, const std::string& response = "");
    /* Timeout reaper — re-queue intents stuck in PROCESSING past their
     * TimeoutMs. Returns how many rows were reaped. Rows whose retry_count
     * exceeds max_retries are marked FAILED instead.                       */
    uint32_t ReapStaleIntents(uint32_t defaultTimeoutMs = 120000, uint32_t maxRetries = 3);

    /* Action Queue */
    bool SubmitAction(const ELLE_ACTION_RECORD& action);
    bool GetPendingActions(std::vector<ELLE_ACTION_RECORD>& out, uint32_t maxCount = 10);
    bool UpdateActionStatus(uint64_t actionId, ELLE_ACTION_STATUS status, const std::string& result = "");
    /* Timeout reaper — re-queue actions stuck in LOCKED/EXECUTING past
     * their timeout_ms. Rows past max attempts are marked ACTION_TIMEOUT.  */
    uint32_t ReapStaleActions(uint32_t defaultTimeoutMs = 60000, uint32_t maxAttempts = 3);

    /* Queue diagnostics — cheap COUNT-style reads for /api/diag/queues.    */
    struct QueueSnapshot {
        uint32_t intent_pending        = 0;
        uint32_t intent_processing     = 0;
        uint32_t intent_completed_1h   = 0;
        uint32_t intent_failed_1h      = 0;
        uint32_t intent_stale_processing = 0;
        uint32_t action_queued         = 0;
        uint32_t action_locked         = 0;
        uint32_t action_executing      = 0;
        uint32_t action_success_1h     = 0;
        uint32_t action_failure_1h     = 0;
        uint32_t action_timeout_1h     = 0;
        uint32_t action_stale_locked   = 0;
        uint32_t hardware_pending      = 0;
        uint32_t hardware_dispatched   = 0;
    };
    bool GetQueueSnapshot(QueueSnapshot& out);

    /* Messages */
    bool StoreMessage(uint64_t convoId, uint32_t role, const std::string& content, 
                      const ELLE_EMOTION_STATE& emotions, float sentiment);
    bool GetConversationHistory(uint64_t convoId, std::vector<ELLE_CONVERSATION_MSG>& out, uint32_t limit = 50);

    /* Memories */
    bool StoreMemory(const ELLE_MEMORY_RECORD& mem);
    bool RecallMemories(const std::string& query, std::vector<ELLE_MEMORY_RECORD>& out, 
                        uint32_t maxCount = 10, float minRelevance = 0.3f);
    bool UpdateMemoryAccess(uint64_t memId);
    bool PromoteToLTM(uint64_t memId);
    bool ArchiveMemory(uint64_t memId);

    /* X-Chromosome subjective layer — reads the wife's lived-experience
     * answers written by Lua/scripts/x_subjective.lua into
     * ElleHeart.dbo.x_subjective. Returns the empty string if the key
     * has never been filled. See that Lua file for the question list.    */
    std::string GetSubjective(const std::string& key);

    /* Emotions */
    bool StoreEmotionSnapshot(const ELLE_EMOTION_STATE& state);
    bool GetLatestEmotionState(ELLE_EMOTION_STATE& out);

    /* Trust */
    bool UpdateTrust(int32_t delta, const std::string& reason);
    bool GetTrustState(ELLE_TRUST_STATE& out);

    /* Goals */
    bool StoreGoal(const ELLE_GOAL_RECORD& goal);
    bool UpdateGoalProgress(uint64_t goalId, float progress);
    bool GetActiveGoals(std::vector<ELLE_GOAL_RECORD>& out);

    /* World Model */
    bool StoreEntity(const ELLE_WORLD_ENTITY& entity);
    bool GetEntity(const std::string& name, ELLE_WORLD_ENTITY& out);
    bool UpdateEntityInteraction(uint64_t entityId);

    /* Memory listing / CRUD (real backing in dbo.memory) */
    struct MemoryRow {
        int64_t id; int type; int tier;
        std::string content; std::string summary;
        float emotional_valence; float importance; float relevance;
        int access_count; uint64_t created_ms; uint64_t last_access_ms;
    };
    bool ListMemories(std::vector<MemoryRow>& out, int memory_type /* -1 = all */,
                      uint32_t limit, uint32_t offset);
    bool GetMemory(int64_t memId, MemoryRow& out);
    bool DeleteMemory(int64_t memId);
    bool UpdateMemoryContent(int64_t memId, const std::string& content,
                              const std::string& summary, float importance);

    /* Conversations CRUD */
    struct ConversationRow {
        int32_t id; int32_t user_id; std::string title;
        std::string started_at; std::string last_message_at;
        int32_t total_messages; bool is_active;
    };
    bool CreateConversation(int32_t user_id, const std::string& title, int32_t& newId);
    bool ListConversations(std::vector<ConversationRow>& out, uint32_t limit = 50);
    bool GetConversation(int32_t convId, ConversationRow& out);

    /* Voice calls */
    bool StartVoiceCall(int32_t user_id, int32_t conv_id, std::string& callId);
    bool EndVoiceCall(const std::string& callId);

    /* Generic counts */
    int64_t CountTable(const std::string& table);

    /* Intimacy layer — reads from prior-system tables */
    struct CrystalProfile {
        bool        found = false;
        std::string traits;                /* JSON blob */
        std::string vulnerability_patterns;
        std::string comfort_patterns;
        std::string trigger_patterns;
        std::string preferred_tone;
        float       trust_level    = 0.0f;
        float       intimacy_level = 0.0f;
    };
    bool GetCrystalProfile(int32_t user_id, CrystalProfile& out);

    struct ElleThread {
        int32_t     id = 0;
        std::string topic;
        std::string status;
        float       emotional_weight = 0.0f;
        float       intensity = 0.0f;
        std::string summary;
        std::string unresolved_questions;
    };
    /* Return up to `limit` unresolved threads ordered by weight desc */
    bool GetOpenThreads(std::vector<ElleThread>& out, uint32_t limit = 5);

    struct UserPresence {
        bool        found = false;
        int32_t     silence_minutes = 0;
        int32_t     threshold_minutes = 0;
        std::string silence_interpretation;
        int32_t     abnormal_silence_count = 0;
    };
    bool GetUserPresence(int32_t user_id, UserPresence& out);
    bool UpdateUserPresenceOnInteraction(int32_t user_id);

    /* Workers/Services */
    bool RegisterWorker(ELLE_SERVICE_ID svc, const std::string& name);
    bool UpdateWorkerHeartbeat(ELLE_SERVICE_ID svc);
    bool GetWorkerStatuses(std::vector<ELLE_SERVICE_STATUS>& out);

    /* Logging */
    bool WriteLog(ELLE_LOG_LEVEL level, ELLE_SERVICE_ID svc, const std::string& msg);
    bool GetRecentLogs(std::vector<ELLE_LOG_ENTRY>& out, uint32_t count = 100, 
                       ELLE_LOG_LEVEL minLevel = LOG_INFO);

    /* Analytics */
    bool RecordMetric(const std::string& name, double value);

    /*──────────────────────────────────────────────────────────────────────
     * Education — learned_subjects / education_references / learning_milestones / skills
     * Ported from legacy Python `app/routers/education.py`.
     *──────────────────────────────────────────────────────────────────────*/
    struct LearnedSubject {
        int32_t     id = 0;
        std::string subject;
        std::string category;
        int32_t     proficiency_level = 0;
        std::string who_taught;
        std::string where_learned;
        float       time_to_learn_hours = 0.0f;
        std::string notes;
        std::string date_started;
        std::string date_completed;
    };
    struct EducationReference {
        int32_t     id = 0;
        int32_t     subject_id = 0;
        std::string reference_type;
        std::string reference_title;
        std::string reference_content;
        std::string file_path;
        float       relevance_score = 0.5f;
        std::string notes;
    };
    struct LearningMilestone {
        int32_t     id = 0;
        int32_t     subject_id = 0;
        std::string milestone;
        std::string description;
        std::string achieved_at;
    };
    struct Skill {
        int32_t     id = 0;
        std::string skill_name;
        std::string category;
        int32_t     proficiency = 0;
        int32_t     learned_from_subject_id = 0;  /* 0 = null */
        int32_t     times_used = 0;
        std::string last_used;
        std::string notes;
    };
    bool ListSubjects(std::vector<LearnedSubject>& out,
                      const std::string& category /* empty = all */, uint32_t limit = 50);
    bool GetSubject(int32_t subject_id, LearnedSubject& out);
    bool CreateSubject(const LearnedSubject& in, int32_t& newId);
    bool UpdateSubject(int32_t subject_id, const LearnedSubject& patch,
                       const std::vector<std::string>& fieldsToUpdate);
    bool ListSubjectReferences(int32_t subject_id, std::vector<EducationReference>& out);
    bool AddSubjectReference(const EducationReference& in);
    bool ListSubjectMilestones(int32_t subject_id, std::vector<LearningMilestone>& out);
    bool AddSubjectMilestone(const LearningMilestone& in);
    bool ListSkills(std::vector<Skill>& out, const std::string& category /* empty = all */);
    bool CreateSkill(const Skill& in, int32_t& newId);
    bool RecordSkillUse(const std::string& skill_name);

    /*──────────────────────────────────────────────────────────────────────
     * Video — job queue + avatar registry
     * Ported from legacy Python `app/services/video_generator.py`.
     *──────────────────────────────────────────────────────────────────────*/
    struct VideoJob {
        int64_t     id = 0;
        std::string job_uuid;
        std::string text;
        std::string avatar_path;
        int64_t     call_id = 0;
        std::string status;         /* queued|running|done|failed */
        int32_t     progress = 0;
        std::string output_path;
        std::string error;
        int64_t     created_ms = 0;
        int64_t     started_ms = 0;
        int64_t     finished_ms = 0;
    };
    bool CreateVideoJob(const std::string& text, const std::string& avatar_path,
                        int64_t call_id, VideoJob& out);
    bool GetVideoJob(const std::string& job_uuid, VideoJob& out);
    bool ClaimNextVideoJob(VideoJob& out);  /* atomic queued→running */
    bool UpdateVideoJobProgress(const std::string& job_uuid, int32_t progress);
    bool CompleteVideoJob(const std::string& job_uuid, const std::string& output_path);
    bool FailVideoJob(const std::string& job_uuid, const std::string& error);

    struct UserAvatar {
        int32_t     id = 0;
        int32_t     user_id = 1;
        std::string label;
        std::string file_path;
        std::string mime_type;
        bool        is_default = false;
    };
    bool RegisterAvatar(const UserAvatar& in, int32_t& newId);
    bool GetDefaultAvatar(int32_t user_id, UserAvatar& out);
    bool ListAvatars(int32_t user_id, std::vector<UserAvatar>& out);

    /*──────────────────────────────────────────────────────────────────────
     * Dictionary loader state (companion to dictionary_words)
     * Ported from legacy `dictionary_loader.py` progress tracking.
     *──────────────────────────────────────────────────────────────────────*/
    struct DictionaryLoaderState {
        std::string status;
        int32_t     loaded = 0;
        int32_t     failed = 0;
        int32_t     skipped = 0;
        std::string last_word;
        std::string error;
        int64_t     started_ms = 0;
        int64_t     updated_ms = 0;
    };
    bool GetDictionaryLoaderState(DictionaryLoaderState& out);
    bool UpsertDictionaryLoaderState(const DictionaryLoaderState& in);
    bool InsertDictionaryWord(const std::string& word,
                              const std::string& part_of_speech,
                              const std::string& definition,
                              const std::string& example);
    int64_t CountDictionaryWords();

    /*──────────────────────────────────────────────────────────────────────
     * Drives — derived from current emotional snapshot + goal backlog.
     * Consumed by GoalEngine and SelfPrompt which both previously carried
     * an always-zero m_drives stub. Single source of truth = the live
     * EmotionalEngine state (via SQL) + a count of active goals.
     *──────────────────────────────────────────────────────────────────────*/
    bool DeriveDriveState(ELLE_DRIVE_STATE& out);

    /* Persist the emotional engine's 102-dim state on shutdown so it's
     * restored on next boot instead of resetting to baseline.             */
    bool PersistEmotionSnapshot(const ELLE_EMOTION_STATE& state);
    bool LoadLatestEmotionSnapshot(ELLE_EMOTION_STATE& out);

    struct EmotionHistoryPoint {
        int64_t     taken_ms = 0;
        float       valence  = 0.0f;
        float       arousal  = 0.0f;
        float       dominance= 0.0f;
    };
    /* Reads snapshots newer than (now - hours). Chronological order. */
    bool GetEmotionHistory(uint32_t hours,
                           std::vector<EmotionHistoryPoint>& out,
                           uint32_t maxPoints = 500);
}

#endif /* ELLE_SQL_CONN_H */
