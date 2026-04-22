/*******************************************************************************
 * Heartbeat.cpp — Keepalive, Dead Man Switch, Watchdog
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"

class ElleHeartbeatService : public ElleServiceBase {
public:
    ElleHeartbeatService()
        : ElleServiceBase(SVC_HEARTBEAT, "ElleHeartbeat",
                          "Elle-Ann Heartbeat Monitor",
                          "Service health monitoring, dead man switch, and watchdog") {}

protected:
    bool OnStart() override {
        auto& cfg = ElleConfig::Instance().GetService();
        SetTickInterval(cfg.heartbeat_ms);
        m_timeoutMs = cfg.heartbeat_timeout_ms;
        m_deadManMs = cfg.dead_man_timeout_ms;
        m_watchdogMs = cfg.watchdog_ms;

        /* Initialize tracking for all services */
        for (int i = 0; i < ELLE_SERVICE_COUNT; i++) {
            m_lastHeartbeat[i] = ELLE_MS_NOW();
            m_healthy[i] = false;
        }
        m_healthy[SVC_HEARTBEAT] = true;

        ELLE_INFO("Heartbeat service started (interval=%dms, timeout=%dms, deadman=%dms)",
                  cfg.heartbeat_ms, m_timeoutMs, m_deadManMs);
        return true;
    }

    void OnStop() override {
        ELLE_INFO("Heartbeat service stopped");
    }

    void OnTick() override {
        uint64_t now = ELLE_MS_NOW();

        /* Send heartbeat to database */
        ElleDB::UpdateWorkerHeartbeat(SVC_HEARTBEAT);

        /* Broadcast heartbeat to all services */
        auto msg = ElleIPCMessage::Create(IPC_HEARTBEAT, SVC_HEARTBEAT, (ELLE_SERVICE_ID)0);
        msg.header.flags |= ELLE_IPC_FLAG_BROADCAST;
        GetIPCHub().Broadcast(msg);

        /* Check service health */
        for (int i = 0; i < ELLE_SERVICE_COUNT; i++) {
            if (i == SVC_HEARTBEAT) continue;

            uint64_t elapsed = now - m_lastHeartbeat[i];

            if (elapsed > m_deadManMs && m_healthy[i]) {
                ELLE_FATAL("DEAD MAN SWITCH: %s not responding for %llums!",
                           ElleIPC::GetServiceName((ELLE_SERVICE_ID)i), elapsed);
                m_healthy[i] = false;
                /* Dead-man switch tripped — initiate real SCM restart. */
                AttemptRestart((ELLE_SERVICE_ID)i);
            } else if (elapsed > m_timeoutMs && m_healthy[i]) {
                ELLE_WARN("Service %s heartbeat timeout (%llums)",
                          ElleIPC::GetServiceName((ELLE_SERVICE_ID)i), elapsed);
                m_healthy[i] = false;
            }
        }

        /* Periodic health summary */
        m_tickCount++;
        if (m_tickCount % 60 == 0) { /* Every ~5 minutes at 5s interval */
            LogHealthSummary();
        }
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        /* Bounds-check sender against the fixed-size tracking arrays. A
         * malformed frame or a yet-to-be-added service ID could index past
         * the end otherwise.                                               */
        if ((int)sender < 0 || (int)sender >= ELLE_SERVICE_COUNT) {
            ELLE_WARN("Heartbeat OnMessage: sender %d out of range [0, %d)",
                      (int)sender, ELLE_SERVICE_COUNT);
            return;
        }
        if (msg.header.msg_type == IPC_HEARTBEAT) {
            m_lastHeartbeat[sender] = ELLE_MS_NOW();
            m_healthy[sender] = true;
        }
        if (msg.header.msg_type == IPC_SERVICE_STATUS) {
            ELLE_SERVICE_STATUS status;
            if (msg.GetPayload(status)) {
                if ((int)status.service_id < 0 ||
                    (int)status.service_id >= ELLE_SERVICE_COUNT) return;
                m_lastHeartbeat[status.service_id] = ELLE_MS_NOW();
                m_healthy[status.service_id] = status.healthy;
            }
        }
    }

private:
    uint64_t m_lastHeartbeat[ELLE_SERVICE_COUNT] = {};
    bool     m_healthy[ELLE_SERVICE_COUNT] = {};
    uint32_t m_timeoutMs = 15000;
    uint32_t m_deadManMs = 60000;
    uint32_t m_watchdogMs = 10000;
    uint32_t m_tickCount = 0;
    uint32_t m_restartAttempts[ELLE_SERVICE_COUNT] = {};

    void AttemptRestart(ELLE_SERVICE_ID svc) {
        if ((int)svc < 0 || (int)svc >= ELLE_SERVICE_COUNT) {
            ELLE_WARN("AttemptRestart: service id %d out of range", (int)svc);
            return;
        }
        auto maxRestarts = ElleConfig::Instance().GetService().max_restarts;
        if (m_restartAttempts[svc] >= maxRestarts) {
            ELLE_FATAL("Max restart attempts reached for %s — manual intervention required",
                       ElleIPC::GetServiceName(svc));
            return;
        }

        m_restartAttempts[svc]++;
        ELLE_INFO("Attempting restart of %s (attempt %d/%d)",
                  ElleIPC::GetServiceName(svc), m_restartAttempts[svc], maxRestarts);

        /* Via SCM: stop and start the service */
        SC_HANDLE hSCM = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
        if (hSCM) {
            std::string svcName = std::string("Elle") + ElleIPC::GetServiceName(svc);
            SC_HANDLE hService = OpenServiceA(hSCM, svcName.c_str(), SERVICE_ALL_ACCESS);
            if (hService) {
                SERVICE_STATUS status;
                ControlService(hService, SERVICE_CONTROL_STOP, &status);
                Sleep(2000);
                StartServiceA(hService, 0, nullptr);
                CloseServiceHandle(hService);
            }
            CloseServiceHandle(hSCM);
        }
    }

    void LogHealthSummary() {
        ELLE_INFO("=== SERVICE HEALTH SUMMARY ===");
        for (int i = 0; i < ELLE_SERVICE_COUNT; i++) {
            uint64_t age = ELLE_MS_NOW() - m_lastHeartbeat[i];
            ELLE_INFO("  %s: %s (last seen %llums ago)",
                      ElleIPC::GetServiceName((ELLE_SERVICE_ID)i),
                      m_healthy[i] ? "HEALTHY" : "UNHEALTHY", age);
        }
    }
};

ELLE_SERVICE_MAIN(ElleHeartbeatService)
