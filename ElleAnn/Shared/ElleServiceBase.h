/*******************************************************************************
 * ElleServiceBase.h — Windows Service Scaffold
 * 
 * All Elle services inherit from this base class. Provides:
 * - SCM (Service Control Manager) registration
 * - Install/Uninstall via command line
 * - Start/Stop/Pause lifecycle
 * - IOCP hub initialization
 * - Config loading
 * - Logging initialization
 * - Double-click install behavior
 ******************************************************************************/
#pragma once
#ifndef ELLE_SERVICE_BASE_H
#define ELLE_SERVICE_BASE_H

#include "ElleTypes.h"
#include "ElleConfig.h"
#include "ElleLogger.h"
#include "ElleQueueIPC.h"
#include "ElleSQLConn.h"
#include <string>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

/*──────────────────────────────────────────────────────────────────────────────
 * SERVICE BASE CLASS
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleServiceBase {
public:
    ElleServiceBase(ELLE_SERVICE_ID serviceId, const std::string& serviceName,
                    const std::string& displayName, const std::string& description);
    virtual ~ElleServiceBase();

    /*──────────────────────────────────────────────────────────────────────────
     * ENTRY POINT — call from main()
     * Handles: --install, --uninstall, --console, or SCM dispatch
     *──────────────────────────────────────────────────────────────────────────*/
    int Run(int argc, char* argv[]);

    /* Service identity */
    ELLE_SERVICE_ID GetServiceId() const { return m_serviceId; }
    const std::string& GetName() const { return m_serviceName; }
    bool IsRunning() const { return m_running; }

    /* IPC Hub access for subclasses */
    ElleIPCHub& GetIPCHub() { return m_ipcHub; }

    /* Access to the currently-running service singleton. Helper classes
     * (ActionExecutor, HTTPHandler, etc.) use this to publish IPC messages
     * without being members of the service class themselves. Public so a
     * helper that doesn't inherit from ElleServiceBase can still reach it. */
    static ElleServiceBase* Current() { return s_instance; }

protected:
    /*──────────────────────────────────────────────────────────────────────────
     * OVERRIDE THESE IN YOUR SERVICE
     *──────────────────────────────────────────────────────────────────────────*/
    
    /* Called once after config/IPC/SQL are initialized. Start your threads here. */
    virtual bool OnStart() = 0;
    
    /* Called when service is stopping. Clean up threads. */
    virtual void OnStop() = 0;
    
    /* Called when service is paused (optional) */
    virtual void OnPause() {}
    
    /* Called when service resumes from pause (optional) */
    virtual void OnResume() {}

    /* Called when an IPC message arrives for this service */
    virtual void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) {}

    /* Called periodically by the service main loop (default: 100ms) */
    virtual void OnTick() {}

    /* Override to specify which services to connect to */
    virtual std::vector<ELLE_SERVICE_ID> GetDependencies() { return {}; }

    /* Tick interval in milliseconds */
    void SetTickInterval(uint32_t ms) { m_tickIntervalMs = ms; }

    /* Access the running flag */
    std::atomic<bool>& Running() { return m_running; }

    /* Sleep that yields immediately when shutdown is requested.
     * Prefer this in tick loops and background threads over raw Sleep()/
     * sleep_for — a plain Sleep(500) means a 500ms stop latency per
     * sleep and forces SCM timeouts during shutdown. InterruptibleSleep
     * wakes on either the timeout OR m_running flipping to false, so
     * a stop request observed mid-sleep aborts promptly.                */
    void InterruptibleSleep(uint32_t ms);

private:
    ELLE_SERVICE_ID m_serviceId;
    std::string     m_serviceName;
    std::string     m_displayName;
    std::string     m_description;
    std::atomic<bool> m_running{false};
    uint32_t        m_tickIntervalMs = 100;

    ElleIPCHub      m_ipcHub;

    /* Signalling primitives for InterruptibleSleep — flipped during the
     * transition m_running true → false. */
    std::mutex                 m_stopMutex;
    std::condition_variable    m_stopCv;

    /*──────────────────────────────────────────────────────────────────────────
     * SCM INTEGRATION
     *──────────────────────────────────────────────────────────────────────────*/
    static ElleServiceBase* s_instance;
    SERVICE_STATUS          m_svcStatus;
    SERVICE_STATUS_HANDLE   m_svcStatusHandle = nullptr;

    static void WINAPI ServiceMain(DWORD argc, LPWSTR* argv);
    static DWORD WINAPI ServiceCtrlHandler(DWORD control, DWORD eventType,
                                            LPVOID eventData, LPVOID context);

    void ReportStatus(DWORD currentState, DWORD exitCode = NO_ERROR, DWORD waitHint = 0);

    /*──────────────────────────────────────────────────────────────────────────
     * INSTALL / UNINSTALL
     *──────────────────────────────────────────────────────────────────────────*/
    bool InstallService();
    bool UninstallService();

    /*──────────────────────────────────────────────────────────────────────────
     * CONSOLE MODE (for debugging)
     *──────────────────────────────────────────────────────────────────────────*/
    int RunConsole();

    /*──────────────────────────────────────────────────────────────────────────
     * DOUBLE-CLICK INSTALL
     * When exe is run without args and not from SCM, shows install dialog
     *──────────────────────────────────────────────────────────────────────────*/
    int DoubleClickInstall();
    bool IsRunningFromSCM();

    /*──────────────────────────────────────────────────────────────────────────
     * SERVICE MAIN LOOP
     *──────────────────────────────────────────────────────────────────────────*/
    void ServiceLoop();
    bool InitializeCore();
    void ShutdownCore();

    /* Connect to dependency services with retry */
    void ConnectDependencies();
};

/*──────────────────────────────────────────────────────────────────────────────
 * SERVICE ENTRY MACRO — put in each Service.cpp
 * Usage: ELLE_SERVICE_MAIN(MyService)
 *──────────────────────────────────────────────────────────────────────────────*/
#define ELLE_SERVICE_MAIN(ServiceClass)                          \
    int main(int argc, char* argv[]) {                           \
        ServiceClass svc;                                         \
        return svc.Run(argc, argv);                               \
    }

#endif /* ELLE_SERVICE_BASE_H */
