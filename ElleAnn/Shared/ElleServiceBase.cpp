/*******************************************************************************
 * ElleServiceBase.cpp — Windows Service Scaffold Implementation
 * 
 * Handles SCM lifecycle, double-click install, console mode, core init.
 ******************************************************************************/
#include "ElleServiceBase.h"
#include <iostream>
#include <sstream>

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

    s_instance->ReportStatus(SERVICE_START_PENDING, NO_ERROR, 10000);

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
            svc->ReportStatus(SERVICE_STOP_PENDING, NO_ERROR, 10000);
            svc->m_running = false;
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

    /* Set recovery: restart on failure */
    SC_ACTION actions[3] = {
        { SC_ACTION_RESTART, 5000 },
        { SC_ACTION_RESTART, 10000 },
        { SC_ACTION_RESTART, 30000 }
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

    /* Stop if running */
    SERVICE_STATUS status;
    if (ControlService(hService, SERVICE_CONTROL_STOP, &status)) {
        std::cout << "Stopping service...\n";
        Sleep(2000);
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
 * DOUBLE-CLICK INSTALL
 *──────────────────────────────────────────────────────────────────────────────*/
int ElleServiceBase::DoubleClickInstall() {
    std::string msg = m_displayName + " v" + ELLE_VERSION_STRING + "\n\n"
        "Would you like to install this as a Windows service?\n\n"
        "Yes = Install and start service\n"
        "No  = Run in console mode (debug)\n"
        "Cancel = Exit";

    int result = MessageBoxA(nullptr, msg.c_str(), m_displayName.c_str(),
                             MB_YESNOCANCEL | MB_ICONQUESTION);

    switch (result) {
        case IDYES:
            if (InstallService()) {
                MessageBoxA(nullptr, "Service installed and started successfully!",
                            m_displayName.c_str(), MB_OK | MB_ICONINFORMATION);
                return 0;
            } else {
                MessageBoxA(nullptr, "Installation failed. Try running as Administrator.",
                            m_displayName.c_str(), MB_OK | MB_ICONERROR);
                return 1;
            }
        case IDNO:
            return RunConsole();
        default:
            return 0;
    }
}

bool ElleServiceBase::IsRunningFromSCM() {
    /* Heuristic: if parent process is services.exe, we're from SCM */
    DWORD parentPid = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(pe);
        DWORD myPid = GetCurrentProcessId();
        if (Process32First(hSnap, &pe)) {
            do {
                if (pe.th32ProcessID == myPid) {
                    parentPid = pe.th32ParentProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &pe));
        }

        /* Check if parent is services.exe */
        if (parentPid) {
            if (Process32First(hSnap, &pe)) {
                do {
                    if (pe.th32ProcessID == parentPid) {
                        std::string name(pe.szExeFile);
                        for (auto& c : name) c = (char)tolower(c);
                        CloseHandle(hSnap);
                        return name == "services.exe";
                    }
                } while (Process32Next(hSnap, &pe));
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
        Sleep(m_tickIntervalMs);
    }

    OnStop();
    ShutdownCore();
    std::cout << "Service stopped.\n";
    return 0;
}

/*──────────────────────────────────────────────────────────────────────────────
 * SERVICE MAIN LOOP
 *──────────────────────────────────────────────────────────────────────────────*/
void ElleServiceBase::ServiceLoop() {
    m_running = true;
    while (m_running) {
        OnTick();
        Sleep(m_tickIntervalMs);
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * CORE INITIALIZATION
 *──────────────────────────────────────────────────────────────────────────────*/
bool ElleServiceBase::InitializeCore() {
    ELLE_INFO("Initializing %s...", m_displayName.c_str());

    /* 1. Load config */
    std::string configPath = "elle_master_config.json";
    /* Try exe directory first */
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    std::string exeDir(exePath);
    size_t lastSlash = exeDir.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        exeDir = exeDir.substr(0, lastSlash + 1);
        configPath = exeDir + "elle_master_config.json";
    }

    if (!ElleConfig::Instance().Load(configPath)) {
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
    ElleLogger::Instance().SetLogFile(
        ElleConfig::Instance().GetString("logging.file_path", "C:\\ElleAnn\\Logs\\elle.log"),
        (uint32_t)ElleConfig::Instance().GetInt("logging.max_file_size_mb", 100),
        (uint32_t)ElleConfig::Instance().GetInt("logging.max_files", 10)
    );

    /* 3. Initialize SQL connection pool */
    auto& svcCfg = ElleConfig::Instance().GetService();
    if (!ElleSQLPool::Instance().Initialize(svcCfg.sql_connection_string, svcCfg.sql_pool_size)) {
        ELLE_ERROR("Failed to initialize SQL connection pool");
        return false;
    }
    ELLE_INFO("SQL pool initialized (%d connections)", svcCfg.sql_pool_size);

    /* 4. Register this worker in database */
    ElleDB::RegisterWorker(m_serviceId, m_serviceName);

    /* 5. Initialize IPC hub */
    if (!m_ipcHub.Initialize(m_serviceId, svcCfg.iocp_threads)) {
        ELLE_ERROR("Failed to initialize IPC hub");
        return false;
    }
    ELLE_INFO("IPC hub initialized (%d IOCP threads)", svcCfg.iocp_threads);

    /* 6. Set IPC message handler */
    m_ipcHub.SetMessageHandler([this](const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) {
        this->OnMessage(msg, sender);
    });

    /* 7. Connect to dependency services */
    ConnectDependencies();

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
    m_ipcHub.Shutdown();
    ElleLLMEngine::Instance().Shutdown();
    ElleSQLPool::Instance().Shutdown();
    ElleLogger::Instance().Shutdown();
}

void ElleServiceBase::ConnectDependencies() {
    auto deps = GetDependencies();
    for (auto dep : deps) {
        ELLE_INFO("Connecting to %s...", ElleIPC::GetServiceName(dep));
        for (int attempt = 0; attempt < 5; attempt++) {
            if (m_ipcHub.ConnectTo(dep, 3000)) {
                ELLE_INFO("Connected to %s", ElleIPC::GetServiceName(dep));
                break;
            }
            ELLE_WARN("Failed to connect to %s (attempt %d/5)", 
                      ElleIPC::GetServiceName(dep), attempt + 1);
            Sleep(1000 * (attempt + 1));
        }
    }
}
