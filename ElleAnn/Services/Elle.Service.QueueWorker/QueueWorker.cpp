/*******************************************************************************
 * QueueWorker.cpp — Intent/Action Queue Bridge
 * Polls SQL queues, routes intents to cognitive, actions to executor.
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"

class ElleQueueWorkerService : public ElleServiceBase {
public:
    ElleQueueWorkerService()
        : ElleServiceBase(SVC_QUEUE_WORKER, "ElleQueueWorker",
                          "Elle-Ann Queue Worker",
                          "SQL queue polling and intent/action routing") {}

protected:
    bool OnStart() override {
        SetTickInterval(500); /* Poll every 500ms */
        ELLE_INFO("Queue worker started (poll interval: 500ms)");
        return true;
    }

    void OnStop() override {
        ELLE_INFO("Queue worker stopped");
    }

    void OnTick() override {
        /* Poll intent queue */
        std::vector<ELLE_INTENT_RECORD> intents;
        ElleDB::GetPendingIntents(intents, 10);
        for (auto& intent : intents) {
            auto msg = ElleIPCMessage::Create(IPC_INTENT_REQUEST, SVC_QUEUE_WORKER, SVC_COGNITIVE);
            msg.SetPayload(intent);
            GetIPCHub().Send(SVC_COGNITIVE, msg);
            m_intentsRouted++;
        }

        /* Poll action queue */
        std::vector<ELLE_ACTION_RECORD> actions;
        ElleDB::GetPendingActions(actions, 10);
        for (auto& action : actions) {
            auto msg = ElleIPCMessage::Create(IPC_ACTION_REQUEST, SVC_QUEUE_WORKER, SVC_ACTION);
            msg.SetPayload(action);
            GetIPCHub().Send(SVC_ACTION, msg);
            m_actionsRouted++;
        }

        /* Periodic stats */
        m_pollCount++;
        if (m_pollCount % 120 == 0) { /* Every ~60 seconds */
            ELLE_INFO("Queue stats: %llu intents, %llu actions routed", 
                      m_intentsRouted, m_actionsRouted);
        }
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        /* Forward responses back to SQL queues */
        if (msg.header.msg_type == IPC_INTENT_RESPONSE) {
            ELLE_INTENT_RECORD intent;
            if (msg.GetPayload(intent)) {
                ElleDB::UpdateIntentStatus(intent.id, (ELLE_INTENT_STATUS)intent.status, intent.response);
            }
        }
        if (msg.header.msg_type == IPC_ACTION_RESPONSE) {
            ELLE_ACTION_RECORD action;
            if (msg.GetPayload(action)) {
                ElleDB::UpdateActionStatus(action.id, (ELLE_ACTION_STATUS)action.status, action.result);
            }
        }
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT, SVC_COGNITIVE, SVC_ACTION };
    }

private:
    uint64_t m_intentsRouted = 0;
    uint64_t m_actionsRouted = 0;
    uint32_t m_pollCount = 0;
};

ELLE_SERVICE_MAIN(ElleQueueWorkerService)
