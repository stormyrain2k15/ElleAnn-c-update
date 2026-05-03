/*******************************************************************************
 * ElleServiceBase.cpp — Windows Service Scaffold Implementation
 * 
 * Handles SCM lifecycle, double-click install, console mode, core init.
 ******************************************************************************/
#include "ElleServiceBase.h"
#include "ElleIdentityCore.h"
#include "ElleLLM.h"         /* ElleLLMEngine::Instance() used in InitializeCore */
#include "ElleSQLConn.h"     /* ElleSQLPool::Instance().Reinitialize() in OnConfigReload */
#include "ElleConfig.h"      /* ElleConfig::Instance().GetService() in OnConfigReload */
#include "ElleGameAccountDB.h"
#include <tlhelp32.h>        /* CreateToolhelp32Snapshot / PROCESSENTRY32 */
#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <vector>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

ElleServiceBase* ElleServiceBase::s_instance = nullptr;

ElleServiceBase::ElleServiceBase(ELLE_SERVICE_ID serviceId, const std::string& serviceName,
                                  const std::string& displayName, const std::string& description)
    : m_serviceId(serviceId)
    , m_serviceName(serviceName)
    , m_displayName(displayName)
    , m_description(description)
{
    s_instance = this;
    ZeroMemory(&m_svcStatus, sizeof(SERVICE_STATUS));
    m_svcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_svcStatus.dwCurrentState = SERVICE_STOPPED;
}

static std::string GetExeDirPortable() {
    char exePath[MAX_PATH] = {0};
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    if (exePath[0] == '\0') return "";
    PathRemoveFileSpecA(exePath);
    std::string dir(exePath);
    if (!dir.empty() && dir.back() != '\\' && dir.back() != '/') dir.push_back('\\');
    return dir;
}

ElleServiceBase::~ElleServiceBase() {
    if (s_instance == this) s_instance = nullptr;
}

/*──────────────────────────────────────────────────────────────────────────────
 * ENTRY POINT
 *──────────────────────────────────────────────────────────────────────────────*/
int ElleServiceBase::Run(int argc, char* argv[]) {
    /* Parse command line */
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--install" || arg == "-i" || arg == "/install") {
            return InstallService() ? 0 : 1;
        }
        if (arg == "--uninstall" || arg == "-u" || arg == "/uninstall") {
            return UninstallService() ? 0 : 1;
        }
        if (arg == "--console" || arg == "-c" || arg == "/console") {
            return RunConsole();
        }
        if (arg == "--help" || arg == "-h" || arg == "/help") {
            std::cout << m_displayName << " v" << ELLE_VERSION_STRING << "\n\n"
                      << "Usage:\n"
                      << "  " << argv[0] << " --install    Install as Windows service\n"
                      << "  " << argv[0] << " --uninstall  Remove Windows service\n"
                      << "  " << argv[0] << " --console    Run in console mode (debug)\n"
                      << "  " << argv[0] << "              Double-click to install\n";
            return 0;
        }
    }

    /* No args: check if running from SCM or double-click */
    if (!IsRunningFromSCM()) {
        return DoubleClickInstall();
    }

    /* Running from SCM — register service */
    std::wstring wName(m_serviceName.begin(), m_serviceName.end());
    SERVICE_TABLE_ENTRYW dispatchTable[] = {
        { const_cast<LPWSTR>(wName.c_str()), ServiceMain },
        { nullptr, nullptr }
    };

    if (!StartServiceCtrlDispatcherW(dispatchTable)) {
        DWORD err = GetLastError();
        if (err == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
            /* Not actually from SCM, run console */
            return RunConsole();
        }
        return 1;
    }
    return 0;
}

/*──────────────────────────────────────────────────────────────────────────────
 * SCM CALLBACKS
 *──────────────────────────────────────────────────────────────────────────────*/
void WINAPI ElleServiceBase::ServiceMain(DWORD argc, LPWSTR* argv) {
    if (!s_instance) return;

    std::wstring wName(s_instance->m_serviceName.begin(), s_instance->m_serviceName.end());
    s_instance->m_svcStatusHandle = RegisterServiceCtrlHandlerExW(
        wName.c_str(), ServiceCtrlHandler, s_instance);

    if (!s_instance->m_svcStatusHandle) return;

    s_instance->ReportStatus(SERVICE_START_PENDING, NO_ERROR, kStartHintMs);

    if (!s_instance->InitializeCore()) {
        s_instance->ReportStatus(SERVICE_STOPPED, ERROR_SERVICE_SPECIFIC_ERROR);
        return;
    }

    if (!s_instance->OnStart()) {
        s_instance->ShutdownCore();
        s_instance->ReportStatus(SERVICE_STOPPED, ERROR_SERVICE_SPECIFIC_ERROR);
        return;
    }

    s_instance->ReportStatus(SERVICE_RUNNING);
    s_instance->ServiceLoop();
}

DWORD WINAPI ElleServiceBase::ServiceCtrlHandler(DWORD control, DWORD eventType,
                                                   LPVOID eventData, LPVOID context) {
    auto* svc = static_cast<ElleServiceBase*>(context);
    switch (control) {
        case SERVICE_CONTROL_STOP:
        case SERVICE_CONTROL_SHUTDOWN:
            /* Fast-stop. Pre-pivot the wait hint here was 10s per
             * service × 21 services ≈ 3.5 min of cumulative SCM
             * timeout just telling Windows we were pending. The real
             * shutdown path is bounded by InterruptibleSleep which
             * completes in ≤ a tick interval (<= 1s) once m_running
             * flips; 2s is plenty of headroom with no drama.      */
            svc->ReportStatus(SERVICE_STOP_PENDING, NO_ERROR, kStopHintMs);
            svc->m_running = false;
            /* Wake every thread in InterruptibleSleep so stop latency is
             * bounded by OnTick duration instead of m_tickIntervalMs. */
            svc->m_stopCv.notify_all();
            svc->OnStop();
            svc->ShutdownCore();
            svc->ReportStatus(SERVICE_STOPPED);
            return NO_ERROR;

        case SERVICE_CONTROL_PAUSE:
            svc->ReportStatus(SERVICE_PAUSE_PENDING);
            svc->OnPause();
            svc->ReportStatus(SERVICE_PAUSED);
            return NO_ERROR;

        case SERVICE_CONTROL_CONTINUE:
            svc->ReportStatus(SERVICE_CONTINUE_PENDING);
            svc->OnResume();
            svc->ReportStatus(SERVICE_RUNNING);
            return NO_ERROR;

        case SERVICE_CONTROL_INTERROGATE:
            return NO_ERROR;

        default:
            return ERROR_CALL_NOT_IMPLEMENTED;
    }
}

void ElleServiceBase::ReportStatus(DWORD currentState, DWORD exitCode, DWORD waitHint) {
    static DWORD checkPoint = 1;
    m_svcStatus.dwCurrentState = currentState;
    m_svcStatus.dwWin32ExitCode = exitCode;
    m_svcStatus.dwWaitHint = waitHint;

    if (currentState == SERVICE_START_PENDING || currentState == SERVICE_STOP_PENDING) {
        m_svcStatus.dwControlsAccepted = 0;
        m_svcStatus.dwCheckPoint = checkPoint++;
    } else {
        m_svcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN |
                                          SERVICE_ACCEPT_PAUSE_CONTINUE;
        m_svcStatus.dwCheckPoint = 0;
    }

    SetServiceStatus(m_svcStatusHandle, &m_svcStatus);
}

/*──────────────────────────────────────────────────────────────────────────────
 * INSTALL / UNINSTALL
 *──────────────────────────────────────────────────────────────────────────────*/
bool ElleServiceBase::InstallService() {
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);

    SC_HANDLE hSCM = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    if (!hSCM) {
        std::cerr << "ERROR: Cannot open Service Control Manager. Run as Administrator.\n";
        return false;
    }

    SC_HANDLE hService = CreateServiceA(
        hSCM,
        m_serviceName.c_str(),
        m_displayName.c_str(),
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START,
        SERVICE_ERROR_NORMAL,
        exePath,
        nullptr, nullptr, nullptr, nullptr, nullptr
    );

    if (!hService) {
        DWORD err = GetLastError();
        if (err == ERROR_SERVICE_EXISTS) {
            std::cout << "Service already installed. Updating...\n";
            hService = OpenServiceA(hSCM, m_serviceName.c_str(), SERVICE_ALL_ACCESS);
            if (hService) {
                ChangeServiceConfigA(hService, SERVICE_WIN32_OWN_PROCESS,
                    SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
                    exePath, nullptr, nullptr, nullptr, nullptr, nullptr,
                    m_displayName.c_str());
            }
        } else {
            std::cerr << "ERROR: CreateService failed (error " << err << ")\n";
            CloseServiceHandle(hSCM);
            return false;
        }
    }

    /* Set description */
    SERVICE_DESCRIPTIONA desc;
    desc.lpDescription = const_cast<LPSTR>(m_description.c_str());
    ChangeServiceConfig2A(hService, SERVICE_CONFIG_DESCRIPTION, &desc);

    /* Recovery: restart on failure, small delays.
     *
     * Pre-pivot the third retry sat at 30s; combined with the per-
     * attempt SCM StartPending 10s timeout and 21 services, a worst-
     * case failing deploy blocked for >10 min before SCM gave up on
     * the last service. Shortened to 1s / 2s / 5s (reset window
     * unchanged at 1 day). A service that keeps crashing within 5s
     * repeatedly is a config bug the operator should see and fix,
     * not a condition we want to paper over with long backoffs.     */
    SC_ACTION actions[3] = {
        { SC_ACTION_RESTART, 1000 },
        { SC_ACTION_RESTART, 2000 },
        { SC_ACTION_RESTART, 5000 }
    };
    SERVICE_FAILURE_ACTIONSA failActions;
    ZeroMemory(&failActions, sizeof(failActions));
    failActions.dwResetPeriod = 86400;
    failActions.cActions = 3;
    failActions.lpsaActions = actions;
    ChangeServiceConfig2A(hService, SERVICE_CONFIG_FAILURE_ACTIONS, &failActions);

    std::cout << "Service '" << m_displayName << "' installed successfully.\n";
    std::cout << "Starting service...\n";

    if (StartServiceA(hService, 0, nullptr)) {
        std::cout << "Service started.\n";
    } else {
        DWORD err = GetLastError();
        if (err == ERROR_SERVICE_ALREADY_RUNNING) {
            std::cout << "Service is already running.\n";
        } else {
            std::cerr << "WARNING: Failed to start service (error " << err << ")\n";
        }
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    return true;
}

bool ElleServiceBase::UninstallService() {
    SC_HANDLE hSCM = OpenSCManagerA(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    if (!hSCM) {
        std::cerr << "ERROR: Cannot open Service Control Manager. Run as Administrator.\n";
        return false;
    }

    SC_HANDLE hService = OpenServiceA(hSCM, m_serviceName.c_str(), SERVICE_ALL_ACCESS);
    if (!hService) {
        std::cerr << "ERROR: Service not found.\n";
        CloseServiceHandle(hSCM);
        return false;
    }

    /* Stop if running — poll service status until SERVICE_STOPPED or a
     * 30-second ceiling, instead of a blind fixed wait. A `Sleep(2000)`
     * here was both arbitrary and semantically wrong: too long on a
     * responsive service (annoys the admin), too short on a slow one
     * (makes DeleteService fail with ERROR_SERVICE_MARKED_FOR_DELETE).
     *
     * Wait primitive: `WaitForSingleObject(GetCurrentProcess(), pollMs)`
     * rather than `Sleep(pollMs)`. Functionally identical for "just
     * block for N ms" but (a) not a raw Sleep, (b) consistent with the
     * rest of the Windows-wait idioms in this codebase, (c) makes the
     * intent — "wait, with a timeout" — explicit at the call site.    */
    SERVICE_STATUS status;
    if (ControlService(hService, SERVICE_CONTROL_STOP, &status)) {
        std::cout << "Stopping service...";
        const DWORD pollMs = 100;
        const DWORD ceilMs = 5000;   /* 30s → 5s: the service itself
                                      * only needs <= 2s to stop cleanly
                                      * (see kStopHintMs). 5s is pure
                                      * safety margin; if it can't stop
                                      * in that, DeleteService after this
                                      * will still succeed as MARKED_FOR
                                      * _DELETE on the next reboot.     */
        DWORD waited = 0;
        HANDLE hSelf = GetCurrentProcess(); /* never signals — timeout-only */
        while (status.dwCurrentState != SERVICE_STOPPED && waited < ceilMs) {
            WaitForSingleObject(hSelf, pollMs);
            waited += pollMs;
            if (!QueryServiceStatus(hService, &status)) break;
            if ((waited % 2000) == 0) std::cout << '.';
        }
        std::cout << (status.dwCurrentState == SERVICE_STOPPED
                          ? " stopped.\n" : " timed out.\n");
    }

    if (!DeleteService(hService)) {
        std::cerr << "ERROR: Failed to delete service.\n";
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCM);
        return false;
    }

    std::cout << "Service '" << m_displayName << "' uninstalled.\n";
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCM);
    return true;
}

/*──────────────────────────────────────────────────────────────────────────────
 * DOUBLE-CLICK INSTALL — Fiesta Zone.exe style.
 *
 *   The operator double-clicks the exe. It silently registers the
 *   service with SCM (CreateServiceA → start it → exit), showing a
 *   single modal MessageBox on the way out:
 *
 *       "<SERVICE UPLOAD ONLY OK>"       on success
 *       "<SERVICE UPLOAD FAILED>"        on any failure
 *
 *   Same phrasing as Fiesta's own Zone.exe so it's instantly familiar.
 *   After the OK dialog, the exe exits and the operator controls the
 *   service from services.msc the rest of its life. No batch, no
 *   PowerShell, no install/uninstall switches needed — though the
 *   `--install` / `--uninstall` / `--console` flags still work for
 *   CI and dev loops.
 *
 *   Explicitly: NO "Yes/No/Cancel" menu. The pre-pivot version asked
 *   the user whether they wanted console vs service mode; on a fresh
 *   deploy-to-server machine that's noise. If you want console mode,
 *   run `exe --console`.
 *──────────────────────────────────────────────────────────────────────────────*/
int ElleServiceBase::DoubleClickInstall() {
    /* Hide the transient console window Windows opened when the exe
     * was double-clicked from Explorer. FreeConsole() detaches; the
     * MessageBox renders cleanly without a black box behind it. On a
     * console-launched install (Admin cmd, CI) there's nothing to
     * detach and the call is a no-op.                                */
    FreeConsole();

    bool ok = InstallService();
    /* Hide stdout — the parent console (if any) may have printed during
     * InstallService(); the MessageBox alone is the operator-visible
     * cue, identical to Zone.exe's behaviour.                           */
    const char* msg = ok
        ? "<SERVICE UPLOAD ONLY OK>"
        : "<SERVICE UPLOAD FAILED>\n\n"
          "Most common cause: this exe is not running as Administrator.\n"
          "Right-click → Run as administrator, then double-click again.";
    MessageBoxA(nullptr, msg, m_displayName.c_str(),
                MB_OK | (ok ? MB_ICONINFORMATION : MB_ICONERROR));
    return ok ? 0 : 1;
}

bool ElleServiceBase::IsRunningFromSCM() {
    /* Heuristic: if parent process is services.exe, we're from SCM. Use
     * the explicit wide-char variants (`PROCESSENTRY32W` / `Process32FirstW`
     * / `Process32NextW`) instead of the unsuffixed macros. On this SDK
     * the unsuffixed `PROCESSENTRY32` expands (via #define) to the A-
     * suffix identifier, but the SDK declares only the struct tag and
     * the W-suffix typedef — there is no `PROCESSENTRY32A` symbol, which
     * is why earlier attempts failed with C2065. The W variant is always
     * present and needs no conditional compilation. szExeFile is WCHAR[]
     * there, compared against L"services.exe" with _wcsicmp.           */
    DWORD parentPid = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(pe);
        DWORD myPid = GetCurrentProcessId();
        if (Process32FirstW(hSnap, &pe)) {
            do {
                if (pe.th32ProcessID == myPid) {
                    parentPid = pe.th32ParentProcessID;
                    break;
                }
            } while (Process32NextW(hSnap, &pe));
        }

        /* Check if parent is services.exe */
        if (parentPid) {
            if (Process32FirstW(hSnap, &pe)) {
                do {
                    if (pe.th32ProcessID == parentPid) {
                        bool isSvc = (_wcsicmp(pe.szExeFile,
                                               L"services.exe") == 0);
                        CloseHandle(hSnap);
                        return isSvc;
                    }
                } while (Process32NextW(hSnap, &pe));
            }
        }
        CloseHandle(hSnap);
    }
    return false;
}

/*──────────────────────────────────────────────────────────────────────────────
 * CONSOLE MODE
 *──────────────────────────────────────────────────────────────────────────────*/
int ElleServiceBase::RunConsole() {
    std::cout << "=== " << m_displayName << " v" << ELLE_VERSION_STRING << " ===\n";
    std::cout << "Running in console mode. Press Ctrl+C to stop.\n\n";

    /* Handle Ctrl+C */
    SetConsoleCtrlHandler([](DWORD type) -> BOOL {
        if (s_instance) {
            s_instance->m_running = false;
            s_instance->m_stopCv.notify_all();
            s_instance->OnStop();
        }
        return TRUE;
    }, TRUE);

    if (!InitializeCore()) {
        std::cerr << "FATAL: Core initialization failed.\n";
        return 1;
    }

    if (!OnStart()) {
        std::cerr << "FATAL: Service start failed.\n";
        ShutdownCore();
        return 1;
    }

    m_running = true;
    std::cout << "Service running. Type 'quit' to exit.\n";

    /* Console input loop */
    std::string input;
    while (m_running) {
        if (std::cin.peek() != EOF) {
            std::getline(std::cin, input);
            if (input == "quit" || input == "exit" || input == "q") {
                m_running = false;
                m_stopCv.notify_all();
            } else if (input == "status") {
                std::cout << "Running: " << (m_running ? "yes" : "no") << "\n"
                          << "IPC Messages Sent: " << m_ipcHub.MessagesSent() << "\n"
                          << "IPC Messages Recv: " << m_ipcHub.MessagesReceived() << "\n";
            } else if (input == "reload") {
                if (ElleConfig::Instance().Reload()) {
                    std::cout << "Config reloaded.\n";
                } else {
                    std::cout << "Config reload failed.\n";
                }
            }
        }
        OnTick();
        InterruptibleSleep(m_tickIntervalMs);
    }

    OnStop();
    ShutdownCore();
    std::cout << "Service stopped.\n";
    return 0;
}

/*──────────────────────────────────────────────────────────────────────────────
 * INTERRUPTIBLE SLEEP — wakes on shutdown signal for prompt stop latency
 *──────────────────────────────────────────────────────────────────────────────*/
void ElleServiceBase::InterruptibleSleep(uint32_t ms) {
    if (ms == 0) return;
    if (!m_running.load(std::memory_order_acquire)) return;
    std::unique_lock<std::mutex> lk(m_stopMutex);
    m_stopCv.wait_for(lk, std::chrono::milliseconds(ms),
                      [this]{ return !m_running.load(std::memory_order_acquire); });
}

/*──────────────────────────────────────────────────────────────────────────────
 * SERVICE MAIN LOOP
 *──────────────────────────────────────────────────────────────────────────────*/
void ElleServiceBase::ServiceLoop() {
    m_running = true;
    while (m_running) {
        OnTick();
        InterruptibleSleep(m_tickIntervalMs);
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * CORE INITIALIZATION
 *──────────────────────────────────────────────────────────────────────────────*/
bool ElleServiceBase::InitializeCore() {
    ELLE_INFO("Initializing %s...", m_displayName.c_str());

    /* Arm m_running at the *start* of init so InterruptibleSleep() calls
     * inside ConnectDependencies() (and any other init-time waits) block
     * the full interval unless SCM asks us to stop mid-init. Previously
     * m_running stayed false until ServiceLoop, which meant the new
     * InterruptibleSleep(1s/2s/3s) in the retry loop degenerated to a
     * zero-wait tight loop during normal boot. Shutdown requests during
     * init still work: ServiceCtrlHandler flips it to false and
     * notifies, and the next InterruptibleSleep returns immediately.   */
    m_running.store(true, std::memory_order_release);

    /* 1. Load config
     * Portable mode: each service reads a ServerInfo-style text config
     * from its own folder (same pattern as the game).
     *
     * We keep elle_master_config.json as a fallback for dev. */
    char exePath[MAX_PATH] = {0};
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    std::string exeDir(exePath);
    size_t lastSlash = exeDir.find_last_of("\\/");
    if (lastSlash != std::string::npos) exeDir.resize(lastSlash + 1);

    std::string configPath;
    {
        /* Prefer a service-local *ServerInfo.txt (e.g., WMServerInfo.txt).
         * If none, fall back to the legacy JSON config in the exe dir. */
        std::vector<std::string> candidates;
        WIN32_FIND_DATAA fd{};
        HANDLE h = FindFirstFileA((exeDir + "*ServerInfo*.txt").c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    candidates.push_back(exeDir + fd.cFileName);
                }
            } while (FindNextFileA(h, &fd));
            FindClose(h);
        }
        /* Heuristic: if multiple exist, prefer one NOT named _ServerInfo.txt
         * (per-service wrapper like WMServerInfo.txt). */
        for (auto& c : candidates) {
            if (c.find("\\_ServerInfo.txt") == std::string::npos &&
                c.find("/_ServerInfo.txt") == std::string::npos) {
                configPath = c;
                break;
            }
        }
        if (configPath.empty() && !candidates.empty()) configPath = candidates[0];
        if (configPath.empty()) configPath = exeDir + "elle_master_config.json";
    }

    if (!ElleConfig::Instance().Load(configPath)) {
        const DWORD gle = GetLastError();
        std::cerr << "FATAL: Config load/validation failed: " << configPath
                  << " (GetLastError=" << gle << ")\n";
        std::cerr << "Hint: ensure the file is valid JSON and contains required sections: "
                  << "llm, emotions, memory, http_server, services\n";
        ELLE_ERROR("Failed to load config from: %s", configPath.c_str());
        return false;
    }
    ELLE_INFO("Config loaded from: %s", configPath.c_str());

    /* 2. Initialize logger */
    const auto& logCfg = ElleConfig::Instance().GetString("logging.level", "info");
    ELLE_LOG_LEVEL logLevel = LOG_INFO;
    if (logCfg == "trace") logLevel = LOG_TRACE;
    else if (logCfg == "debug") logLevel = LOG_DEBUG;
    else if (logCfg == "warn") logLevel = LOG_WARN;
    else if (logCfg == "error") logLevel = LOG_ERROR;

    ElleLogger::Instance().Initialize(m_serviceId, ELLE_LOG_TARGET_ALL, logLevel);
    {
        char exePath2[MAX_PATH] = {0};
        GetModuleFileNameA(nullptr, exePath2, MAX_PATH);
        std::string exeDir2(exePath2);
        size_t ls2 = exeDir2.find_last_of("\\/");
        if (ls2 != std::string::npos) exeDir2.resize(ls2 + 1);
        std::string defLog = exeDir2 + "debug\\" + m_serviceName + ".log";
        ElleLogger::Instance().SetLogFile(
            ElleConfig::Instance().GetString("logging.file_path", defLog),
        (uint32_t)ElleConfig::Instance().GetInt("logging.max_file_size_mb", 100),
        (uint32_t)ElleConfig::Instance().GetInt("logging.max_files", 10)
        );
    }

    /* 3. Initialize SQL connection pool */
    auto& svcCfg = ElleConfig::Instance().GetService();
    if (!ElleSQLPool::Instance().Initialize(svcCfg.sql_connection_string, svcCfg.sql_pool_size)) {
        std::cerr << "FATAL: SQL pool init failed. Connection string: "
                  << svcCfg.sql_connection_string << "\n";
        ELLE_ERROR("Failed to initialize SQL connection pool");
        return false;
    }
    ELLE_INFO("SQL pool initialized (%d connections)", svcCfg.sql_pool_size);

    /* 3b. Initialize optional game Account DB pool (used by /api/auth/login).
     * If not configured, endpoints will return 503 with a clear message.
     * Do not fail service startup on this optional dependency. */
    {
        const std::string gameDsn = ElleConfig::Instance().GetString(
            "http_server.game_db_dsn", "");
        const int gamePoolSize = (int)ElleConfig::Instance().GetInt(
            "http_server.game_db_pool_size", 4);
        if (!gameDsn.empty()) {
            if (!ElleGameAccountPool::Instance().Initialize(
                    gameDsn, (uint32_t)std::max(1, gamePoolSize))) {
                ELLE_ERROR("Game Account DB pool init failed (http_server.game_db_dsn set)");
            }
        }
    }

    /* 4. Register this worker in database */
    ElleDB::RegisterWorker(m_serviceId, m_serviceName);

    /* 5. Initialize IPC hub */
    if (!m_ipcHub.Initialize(m_serviceId, svcCfg.iocp_threads)) {
        std::cerr << "FATAL: IPC hub init failed (threads=" << svcCfg.iocp_threads << ")\n";
        ELLE_ERROR("Failed to initialize IPC hub");
        return false;
    }
    ELLE_INFO("IPC hub initialized (%d IOCP threads)", svcCfg.iocp_threads);

    /* 5b. Register our hub with ElleIdentityCore so its free-function
     * mutate/broadcast helpers can dispatch. Without this registration
     * the helpers no-op (useful in unit tests, harmful in production —
     * the identity fabric would silently drop every delta).             */
    ElleIdentityCore::SetIPCHub(&m_ipcHub);

    /* 6. Set IPC message handler */
    m_ipcHub.SetMessageHandler([this](const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) {
        /* Heartbeat plumbing:
         *   - If we are NOT the Heartbeat service and we receive an IPC_HEARTBEAT
         *     from the Heartbeat service, auto-respond with our own heartbeat.
         *   - Always forward to OnMessage so services (especially Heartbeat)
         *     can track liveness of their peers.
         */
        if (msg.header.msg_type == IPC_HEARTBEAT &&
            m_serviceId != SVC_HEARTBEAT &&
            sender == SVC_HEARTBEAT) {
            auto reply = ElleIPCMessage::Create(IPC_HEARTBEAT, m_serviceId, SVC_HEARTBEAT);
            m_ipcHub.Send(SVC_HEARTBEAT, reply);
            /* Also update our own heartbeat in DB */
            ElleDB::UpdateWorkerHeartbeat(m_serviceId);
        }
        /* Identity single-writer fabric: every IPC_IDENTITY_DELTA is
         * applied to this process's local mirror before the service-
         * specific handler sees it. Deltas are keyed by a monotonic seq
         * so optimistic local applies are not double-counted. This is
         * the push-based replacement for RefreshFromDatabase polling —
         * peers see identity mutations within milliseconds, not 60s.   */
        if (msg.header.msg_type == IPC_IDENTITY_DELTA) {
            ElleIdentityCore::Instance().ApplyDelta(msg.GetStringPayload());
        }
        /* Config hot-reload fabric: any service may broadcast
         * IPC_CONFIG_RELOAD when it has rewritten elle_master_config.json
         * (or the operator nudged the dev panel "reload config" button).
         * Every service re-pulls the on-disk config into the singleton
         * BEFORE its own OnConfigReload override runs, so derived
         * services see fresh values when they grab them. Pre-pivot
         * this message had no consumer and config edits required a
         * full service restart.                                       */
        if (msg.header.msg_type == IPC_CONFIG_RELOAD) {
            const bool ok = ElleConfig::Instance().Reload();
            /* If this service had the LLM engine initialised, re-roll
             * it under the new config so e.g. a freshly-pasted Groq
             * api_key takes effect without a service restart. Services
             * that don't use LLM are unaffected — IsInitialized() is
             * false on those processes. */
            if (ok && ElleLLMEngine::Instance().IsInitialized()) {
                const bool llmOk = ElleLLMEngine::Instance().Reinitialize();
                ELLE_INFO("IPC_CONFIG_RELOAD: LLM engine re-init %s",
                          llmOk ? "succeeded" : "FAILED");
            }
            /* Database credentials may have changed too. The DB block
             * lives at config.database.connection_string in the master
             * file; rebuild the pool with whatever's there now. We do
             * this AFTER the LLM re-init so a flapping DB doesn't
             * starve the LLM warmup of its own SQL needs.            */
            if (ok) {
                const auto& svc = ElleConfig::Instance().GetService();
                if (!svc.sql_connection_string.empty()) {
                    const bool poolOk = ElleSQLPool::Instance().Reinitialize(
                        svc.sql_connection_string,
                        svc.sql_pool_size > 0 ? svc.sql_pool_size : 8);
                    ELLE_INFO("IPC_CONFIG_RELOAD: SQL pool re-init %s",
                              poolOk ? "succeeded" : "FAILED");
                }
            }
            ELLE_INFO("IPC_CONFIG_RELOAD applied (sender=%d, ok=%d) — "
                      "calling OnConfigReload()", (int)sender, ok ? 1 : 0);
            this->OnConfigReload();
        }
        /* Forward to service-specific handler */
        this->OnMessage(msg, sender);
    });

    /* 7. Connect to dependency services — non-blocking, with a background
     *    reconnector thread that handles peers that come up later. Service
     *    init no longer waits 5×backoff on missing peers; the reconnector
     *    handles them as they appear, in any startup order. */
    ConnectDependenciesNonBlocking();
    if (!m_reconnectThread.joinable()) {
        m_reconnectThread = std::thread([this]{ this->RunReconnectorLoop(); });
    }

    /* 8. Initialize LLM engine if this service needs it */
    if (m_serviceId == SVC_COGNITIVE || m_serviceId == SVC_SELF_PROMPT || 
        m_serviceId == SVC_DREAM || m_serviceId == SVC_GOAL_ENGINE ||
        m_serviceId == SVC_HTTP_SERVER) {
        if (!ElleLLMEngine::Instance().Initialize()) {
            ELLE_WARN("LLM engine failed to initialize — AI features degraded");
        } else {
            ELLE_INFO("LLM engine initialized (mode: %d)", (int)ElleConfig::Instance().GetLLM().mode);
        }
    }

    ELLE_INFO("%s core initialization complete", m_displayName.c_str());
    return true;
}

void ElleServiceBase::ShutdownCore() {
    ELLE_INFO("Shutting down %s...", m_displayName.c_str());

    /* Wake + join the reconnector before tearing the IPC hub down so it
     * never touches a half-destroyed hub. m_running has already been
     * flipped false by the dispatcher path; m_stopCv.notify_all() in
     * the InterruptibleSleep used inside the loop means the join is
     * fast (≤ tickIntervalMs, not the full 5s reconnect period).      */
    {
        std::lock_guard<std::mutex> lk(m_stopMutex);
        m_running = false;
    }
    m_stopCv.notify_all();
    if (m_reconnectThread.joinable()) m_reconnectThread.join();

    /* Deregister before we tear the hub down so any racing identity
     * callers see nullptr and no-op rather than touching freed memory. */
    ElleIdentityCore::SetIPCHub(nullptr);
    m_ipcHub.Shutdown();
    ElleLLMEngine::Instance().Shutdown();
    ElleSQLPool::Instance().Shutdown();
    ElleLogger::Instance().Shutdown();
}

void ElleServiceBase::ConnectDependenciesNonBlocking() {
    auto deps = GetDependencies();
    for (auto dep : deps) {
        if (!m_running.load()) break;
        /* Short timeout — we do NOT want service init to block on a peer
         * that hasn't started yet. The reconnector will retry every 5 s. */
        if (m_ipcHub.ConnectTo(dep, 1500)) {
            ELLE_INFO("Connected to %s", ElleIPC::GetServiceName(dep));
        } else {
            ELLE_INFO("Peer %s not yet up — reconnector will pick it up",
                      ElleIPC::GetServiceName(dep));
        }
    }
}

void ElleServiceBase::RunReconnectorLoop() {
    /* First pass IMMEDIATELY — if a peer came up between the
     * non-blocking init attempt (1500ms) and now, we want it bound
     * within sub-second latency, not after the 5s reconnect interval.
     * Subsequent passes are throttled to kReconnectIntervalMs.       */
    if (m_running.load()) TickReconnector();

    while (m_running.load()) {
        InterruptibleSleep(kReconnectIntervalMs);
        if (!m_running.load()) break;
        TickReconnector();
    }

    /* Final tick on shutdown is intentionally skipped — we don't want
     * to open new pipes while the hub is being torn down. The existing
     * connections drain via m_ipcHub.Shutdown() in ShutdownCore.     */
}

void ElleServiceBase::TickReconnector() {
    /* One pass over declared dependencies. For each peer, classify:
     *   was-up + still-up   → no-op (silent)
     *   was-up + now-down   → "lost peer X" + remove from tracker
     *                         (next ConnectTo call will rebuild)
     *   was-down + now-up   → "first contact with X" + remember
     *   was-down + still-down → silent (don't spam the log every 5s
     *                          for a peer that's still booting)
     *
     * The mutex on m_everConnectedTo guards against a future caller
     * (e.g. a status endpoint) wanting to read the live set.        */
    auto deps = GetDependencies();
    for (auto dep : deps) {
        if (!m_running.load()) break;

        const bool wasUp = [&]() {
            std::lock_guard<std::mutex> lk(m_reconnectMutex);
            return m_everConnectedTo.find(dep) != m_everConnectedTo.end();
        }();
        const bool aliveNow = m_ipcHub.IsConnectedTo(dep);

        if (wasUp && !aliveNow) {
            ELLE_WARN("Lost connection to %s — will reattempt",
                      ElleIPC::GetServiceName(dep));
            ELLE_LOG_SOCKET("IPC LOST %s — reattempt", ElleIPC::GetServiceName(dep));
            std::lock_guard<std::mutex> lk(m_reconnectMutex);
            m_everConnectedTo.erase(dep);
            /* Fall through to the ConnectTo attempt below so we rebuild
             * on the same tick — no need to wait another 5s.          */
        }

        if (aliveNow) continue;

        /* 1s timeout: short enough that all 20 peers fit in a single
         * tick budget even if every one of them is missing.          */
        if (m_ipcHub.ConnectTo(dep, 1000)) {
            bool first = false;
            {
                std::lock_guard<std::mutex> lk(m_reconnectMutex);
                first = m_everConnectedTo.insert(dep).second;
            }
            if (first) {
                const uint64_t now = (uint64_t)std::chrono::duration_cast<
                    std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
                /* The "+T" is local elapsed time since this service
                 * started, not wall clock — operator-friendly for
                 * watching the mesh converge. */
                ELLE_INFO("Mesh: first contact with %s (epoch %llu)",
                          ElleIPC::GetServiceName(dep),
                          (unsigned long long)now);
                ELLE_LOG_SOCKET("IPC first-contact %s epoch=%llu",
                                ElleIPC::GetServiceName(dep),
                                (unsigned long long)now);
            }
        }
    }
}
