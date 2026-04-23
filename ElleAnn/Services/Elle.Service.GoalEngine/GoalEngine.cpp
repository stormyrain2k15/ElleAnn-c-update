/*******************************************************************************
 * GoalEngine.cpp — Autonomous Goal Formation, Tracking, and Pursuit
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLLM.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"
#include <vector>
#include <algorithm>
#include <sstream>

class GoalEngine {
public:
    bool Initialize() {
        /* Load active goals from database */
        ElleDB::GetActiveGoals(m_goals);
        ELLE_INFO("Goal engine initialized: %d active goals", (int)m_goals.size());
        return true;
    }

    /* Create a new goal */
    uint64_t CreateGoal(const std::string& description, ELLE_GOAL_PRIORITY priority,
                         ELLE_DRIVE_ID sourceDrive, const std::string& successCriteria = "",
                         uint64_t parentGoalId = 0) {
        ELLE_GOAL_RECORD goal = {};
        strncpy_s(goal.description, description.c_str(), ELLE_MAX_MSG - 1);
        goal.status = GOAL_ACTIVE;
        goal.priority = priority;
        goal.progress = 0.0f;
        goal.motivation = 0.8f;
        goal.parent_goal_id = (uint32_t)parentGoalId;
        goal.source_drive = sourceDrive;
        goal.created_ms = ELLE_MS_NOW();
        strncpy_s(goal.success_criteria, successCriteria.c_str(), ELLE_MAX_MSG - 1);

        /* Let the DB assign the authoritative id. Previously we minted
         * goal.id = m_nextId++ locally and relied on SQL matching — it
         * didn't, and UpdateProgress then wrote to row IDs that either
         * didn't exist or belonged to a different goal.                */
        uint64_t dbId = ElleDB::StoreGoalReturningId(goal);
        if (dbId == 0) {
            ELLE_WARN("StoreGoal failed for '%s'", description.c_str());
            return 0;
        }
        goal.id = dbId;
        m_goals.push_back(goal);

        ELLE_INFO("Goal created: [%llu] %s (priority: %d)", goal.id, description.c_str(), priority);
        return goal.id;
    }

    /* Update goal progress */
    void UpdateProgress(uint64_t goalId, float progress) {
        for (auto& g : m_goals) {
            if (g.id == goalId) {
                g.progress = ELLE_CLAMP(progress, 0.0f, 1.0f);
                g.last_progress_ms = ELLE_MS_NOW();
                ElleDB::UpdateGoalProgress(goalId, progress);

                if (progress >= 1.0f) {
                    g.status = GOAL_COMPLETED;
                    ELLE_INFO("Goal COMPLETED: [%llu] %s", goalId, g.description);
                }
                break;
            }
        }
    }

    /* Autonomous goal generation based on drives and emotions */
    void GenerateGoals(const ELLE_DRIVE_STATE& drives, const ELLE_EMOTION_STATE& emotions) {
        auto maxGoals = (uint32_t)ElleConfig::Instance().GetInt("goals.max_active_goals", 16);
        uint32_t activeCount = 0;
        for (auto& g : m_goals) {
            if (g.status == GOAL_ACTIVE) activeCount++;
        }

        if (activeCount >= maxGoals) return;

        /* Build context for LLM-based goal generation */
        std::ostringstream ctx;
        ctx << "Active drives:\n";
        const char* driveNames[] = {"Curiosity", "Boredom", "Attachment", "Anxiety",
            "SelfPreservation", "Exploration", "Creativity", "SocialBonding",
            "Mastery", "Autonomy", "Purpose", "Homeostasis"};
        
        for (int i = 0; i < ELLE_DRIVE_COUNT; i++) {
            if (drives.intensity[i] > drives.threshold[i]) {
                ctx << "  - " << driveNames[i] << ": " << drives.intensity[i] 
                    << " (threshold: " << drives.threshold[i] << ")\n";
            }
        }

        ctx << "\nCurrent emotions: V=" << emotions.valence 
            << " A=" << emotions.arousal << " D=" << emotions.dominance << "\n";

        ctx << "\nExisting goals:\n";
        for (auto& g : m_goals) {
            if (g.status == GOAL_ACTIVE) {
                ctx << "  - " << g.description << " (progress: " 
                    << (int)(g.progress * 100) << "%)\n";
            }
        }

        std::string suggestion = ElleLLMEngine::Instance().FormGoal(
            ctx.str(), "emotions influence: " + std::to_string(emotions.valence));

        if (!suggestion.empty()) {
            ELLE_INFO("Auto-generated goal suggestion: %.80s...", suggestion.c_str());
            /* Auto-create if config permits; otherwise the suggestion is logged
             * and a human (or a future approval UI) can promote it manually.  */
            bool autoCreate = ElleConfig::Instance().GetBool("goals.allow_self_generated_goals", true);
            if (autoCreate) {
                CreateGoal(suggestion, GOAL_MEDIUM, DRIVE_PURPOSE);
            }
        }
    }

    /* Tick: decay motivation, check stale goals */
    void Tick() {
        uint64_t now = ELLE_MS_NOW();
        float decayRate = (float)ElleConfig::Instance().GetFloat("goals.motivation_decay_rate", 0.01);
        uint32_t staleDays = (uint32_t)ElleConfig::Instance().GetInt("goals.auto_abandon_stale_days", 7);
        uint64_t staleMs = (uint64_t)staleDays * 86400000ULL;

        for (auto& g : m_goals) {
            if (g.status != GOAL_ACTIVE) continue;

            /* Decay motivation */
            g.motivation = std::max(0.0f, g.motivation - decayRate);

            /* Check for stale goals */
            if (g.last_progress_ms > 0 && (now - g.last_progress_ms) > staleMs) {
                g.status = GOAL_ABANDONED;
                ELLE_INFO("Goal auto-abandoned (stale): [%llu] %s", g.id, g.description);
            }
        }
    }

    std::vector<ELLE_GOAL_RECORD> GetActiveGoals() const {
        std::vector<ELLE_GOAL_RECORD> active;
        for (auto& g : m_goals) {
            if (g.status == GOAL_ACTIVE) active.push_back(g);
        }
        std::sort(active.begin(), active.end(),
                  [](const ELLE_GOAL_RECORD& a, const ELLE_GOAL_RECORD& b) {
                      return a.priority < b.priority;
                  });
        return active;
    }

    std::string GetGoalSummary() const {
        auto active = GetActiveGoals();
        std::ostringstream ss;
        for (auto& g : active) {
            ss << "- " << g.description << " (" << (int)(g.progress * 100) << "% complete, "
               << "motivation: " << (int)(g.motivation * 100) << "%)\n";
        }
        return ss.str();
    }

private:
    std::vector<ELLE_GOAL_RECORD> m_goals;
    uint64_t m_nextId = 1;
};

/*──────────────────────────────────────────────────────────────────────────────
 * GOAL SERVICE
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleGoalService : public ElleServiceBase {
public:
    ElleGoalService()
        : ElleServiceBase(SVC_GOAL_ENGINE, "ElleGoalEngine",
                          "Elle-Ann Goal Engine",
                          "Autonomous goal formation, tracking, and pursuit") {}

protected:
    bool OnStart() override {
        m_engine.Initialize();
        SetTickInterval(5000);
        ELLE_INFO("Goal service started");
        return true;
    }

    void OnStop() override { ELLE_INFO("Goal service stopped"); }

    void OnTick() override {
        m_engine.Tick();
        m_tickCount++;

        /* Periodic goal generation (every 5 minutes) */
        if (m_tickCount % 60 == 0) {
            /* Refresh drives + emotions from the live SQL state before
             * handing them to GenerateGoals. Previously both were zero
             * because no one ever populated them — FormGoal() would run
             * on empty context.                                          */
            ElleDB::DeriveDriveState(m_drives);
            ElleDB::LoadLatestEmotionSnapshot(m_emotions);
            m_engine.GenerateGoals(m_drives, m_emotions);
        }
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        switch ((ELLE_IPC_MSG_TYPE)msg.header.msg_type) {
            case IPC_GOAL_UPDATE: {
                ELLE_GOAL_RECORD goal;
                if (msg.GetPayload(goal)) {
                    /* id == 0 → create new goal (bootstrapping path).
                     * id > 0  → progress/status update for an existing
                     * DB row. Previously the handler always called
                     * CreateGoal, so external progress updates just
                     * created duplicate goals and UpdateProgress never
                     * ran live.                                         */
                    if (goal.id == 0) {
                        m_engine.CreateGoal(goal.description,
                                            (ELLE_GOAL_PRIORITY)goal.priority,
                                            (ELLE_DRIVE_ID)goal.source_drive,
                                            goal.success_criteria);
                    } else {
                        m_engine.UpdateProgress(goal.id, goal.progress);
                    }
                }
                break;
            }
            case IPC_EMOTION_UPDATE: {
                msg.GetPayload(m_emotions);
                break;
            }
            case IPC_GOAL_QUERY: {
                auto resp = ElleIPCMessage::Create(IPC_GOAL_QUERY, SVC_GOAL_ENGINE, sender);
                resp.SetStringPayload(m_engine.GetGoalSummary());
                GetIPCHub().Send(sender, resp);
                break;
            }
            default:
                break;
        }
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT, SVC_EMOTIONAL };
    }

private:
    GoalEngine m_engine;
    ELLE_DRIVE_STATE m_drives = {};
    ELLE_EMOTION_STATE m_emotions = {};
    uint32_t m_tickCount = 0;
};

ELLE_SERVICE_MAIN(ElleGoalService)
