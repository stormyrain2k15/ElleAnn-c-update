/*******************************************************************************
 * ElleLogger.cpp — Centralized Logging Implementation
 ******************************************************************************/
#include "ElleLogger.h"
#include "ElleSQLConn.h"
#include <cstdarg>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <filesystem>

ElleLogger& ElleLogger::Instance() {
    static ElleLogger inst;
    return inst;
}

ElleLogger::~ElleLogger() { Shutdown(); }

bool ElleLogger::Initialize(ELLE_SERVICE_ID sourceService, uint32_t targets, 
                             ELLE_LOG_LEVEL minLevel) {
    std::lock_guard<std::mutex> lock(m_logMutex);
    m_sourceService = sourceService;
    m_targets = targets;
    m_minLevel = minLevel;

    if (targets & ELLE_LOG_TARGET_CONSOLE) {
        m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    if (targets & ELLE_LOG_TARGET_DATABASE) {
        m_dbRunning = true;
        m_dbThread = std::thread(&ElleLogger::DatabaseWriterThread, this);
    }

    m_initialized = true;
    return true;
}

void ElleLogger::Shutdown() {
    m_initialized = false;
    m_dbRunning = false;
    if (m_dbThread.joinable()) m_dbThread.join();
    if (m_logFile.is_open()) m_logFile.close();
}

void ElleLogger::SetLogFile(const std::string& path, uint32_t maxSizeMB, uint32_t maxFiles) {
    std::lock_guard<std::mutex> lock(m_fileMutex);
    m_logPath = path;
    m_maxFileSizeMB = maxSizeMB;
    m_maxFiles = maxFiles;

    /* Create directory if needed */
    std::filesystem::path p(path);
    std::filesystem::create_directories(p.parent_path());

    m_logFile.open(path, std::ios::app);
    if (m_logFile.is_open()) {
        m_currentFileSize = (uint64_t)std::filesystem::file_size(path);
    }
}

void ElleLogger::SetWebSocketBroadcaster(std::function<void(const std::string&)> broadcaster) {
    m_wsBroadcaster = broadcaster;
}

/*──────────────────────────────────────────────────────────────────────────────
 * CORE LOGGING
 *──────────────────────────────────────────────────────────────────────────────*/
void ElleLogger::Log(ELLE_LOG_LEVEL level, const char* fmt, ...) {
    if (!m_initialized || level < m_minLevel) return;

    char buffer[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    std::string formatted = FormatEntry(level, buffer);

    if (m_targets & ELLE_LOG_TARGET_CONSOLE) WriteConsole(level, formatted);
    if (m_targets & ELLE_LOG_TARGET_FILE)    WriteFile(formatted);
    if (m_targets & ELLE_LOG_TARGET_WEBSOCKET && m_wsBroadcaster) m_wsBroadcaster(formatted);

    if (m_targets & ELLE_LOG_TARGET_DATABASE) {
        ELLE_LOG_ENTRY entry;
        entry.level = level;
        entry.source_svc = m_sourceService;
        entry.timestamp_ms = ELLE_MS_NOW();
        strncpy_s(entry.message, buffer, ELLE_MAX_MSG - 1);
        
        std::lock_guard<std::mutex> lock(m_dbMutex);
        m_dbQueue.push(entry);
    }

    m_totalEntries++;
    if (level >= LOG_ERROR) m_errorCount++;
}

void ElleLogger::Trace(const char* fmt, ...) {
    char buffer[4096];
    va_list args; va_start(args, fmt); vsnprintf(buffer, sizeof(buffer), fmt, args); va_end(args);
    Log(LOG_TRACE, "%s", buffer);
}

void ElleLogger::Debug(const char* fmt, ...) {
    char buffer[4096];
    va_list args; va_start(args, fmt); vsnprintf(buffer, sizeof(buffer), fmt, args); va_end(args);
    Log(LOG_DEBUG, "%s", buffer);
}

void ElleLogger::Info(const char* fmt, ...) {
    char buffer[4096];
    va_list args; va_start(args, fmt); vsnprintf(buffer, sizeof(buffer), fmt, args); va_end(args);
    Log(LOG_INFO, "%s", buffer);
}

void ElleLogger::Warn(const char* fmt, ...) {
    char buffer[4096];
    va_list args; va_start(args, fmt); vsnprintf(buffer, sizeof(buffer), fmt, args); va_end(args);
    Log(LOG_WARN, "%s", buffer);
}

void ElleLogger::Error(const char* fmt, ...) {
    char buffer[4096];
    va_list args; va_start(args, fmt); vsnprintf(buffer, sizeof(buffer), fmt, args); va_end(args);
    Log(LOG_ERROR, "%s", buffer);
}

void ElleLogger::Fatal(const char* fmt, ...) {
    char buffer[4096];
    va_list args; va_start(args, fmt); vsnprintf(buffer, sizeof(buffer), fmt, args); va_end(args);
    Log(LOG_FATAL, "%s", buffer);
}

/*──────────────────────────────────────────────────────────────────────────────
 * SPECIALIZED LOGGING
 *──────────────────────────────────────────────────────────────────────────────*/
void ElleLogger::LogEmotion(const ELLE_EMOTION_STATE& state) {
    /* Find top 5 emotions */
    struct EmoPair { int idx; float val; };
    std::vector<EmoPair> sorted;
    for (int i = 0; i < ELLE_EMOTION_COUNT; i++) {
        if (state.dimensions[i] > 0.01f) {
            sorted.push_back({i, state.dimensions[i]});
        }
    }
    std::sort(sorted.begin(), sorted.end(), 
              [](const EmoPair& a, const EmoPair& b) { return a.val > b.val; });

    std::ostringstream ss;
    ss << "Emotion[V:" << std::fixed << std::setprecision(2) << state.valence
       << " A:" << state.arousal << " D:" << state.dominance << "] Top: ";
    
    int shown = 0;
    for (auto& e : sorted) {
        if (shown >= 5) break;
        if (shown > 0) ss << ", ";
        ss << e.idx << "=" << std::setprecision(2) << e.val;
        shown++;
    }
    
    Info("%s", ss.str().c_str());
}

void ElleLogger::LogTrustChange(int32_t oldScore, int32_t newScore, const std::string& reason) {
    Info("Trust: %d -> %d (%+d) [%s]", oldScore, newScore, newScore - oldScore, reason.c_str());
}

void ElleLogger::LogLLM(const ELLE_LLM_REQUEST& req, const ELLE_LLM_RESPONSE& resp) {
    if (resp.success) {
        Info("LLM[%s] %d+%d=%d tokens, %.0fms", 
             req.model_name, resp.tokens_prompt, resp.tokens_completion, 
             resp.tokens_total, resp.latency_ms);
    } else {
        Error("LLM[%s] FAILED: %s", req.model_name, resp.error);
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * FORMATTING
 *──────────────────────────────────────────────────────────────────────────────*/
std::string ElleLogger::FormatEntry(ELLE_LOG_LEVEL level, const std::string& message,
                                     const std::string& context) {
    SYSTEMTIME st;
    GetLocalTime(&st);

    std::ostringstream ss;
    ss << "[" << std::setfill('0') 
       << std::setw(4) << st.wYear << "-"
       << std::setw(2) << st.wMonth << "-"
       << std::setw(2) << st.wDay << " "
       << std::setw(2) << st.wHour << ":"
       << std::setw(2) << st.wMinute << ":"
       << std::setw(2) << st.wSecond << "."
       << std::setw(3) << st.wMilliseconds << "]"
       << " [" << LevelString(level) << "]"
       << " [" << ElleIPC::GetServiceName(m_sourceService) << "]";
    
    if (!context.empty()) ss << " [" << context << "]";
    ss << " " << message;

    return ss.str();
}

const char* ElleLogger::LevelString(ELLE_LOG_LEVEL level) {
    switch (level) {
        case LOG_TRACE: return "TRACE";
        case LOG_DEBUG: return "DEBUG";
        case LOG_INFO:  return "INFO ";
        case LOG_WARN:  return "WARN ";
        case LOG_ERROR: return "ERROR";
        case LOG_FATAL: return "FATAL";
        default:        return "?????";
    }
}

uint16_t ElleLogger::LevelColor(ELLE_LOG_LEVEL level) {
    switch (level) {
        case LOG_TRACE: return 8;   /* Dark gray */
        case LOG_DEBUG: return 7;   /* Gray */
        case LOG_INFO:  return 10;  /* Green */
        case LOG_WARN:  return 14;  /* Yellow */
        case LOG_ERROR: return 12;  /* Red */
        case LOG_FATAL: return 79;  /* White on red */
        default:        return 7;
    }
}

void ElleLogger::WriteConsole(ELLE_LOG_LEVEL level, const std::string& formatted) {
    if (m_hConsole) {
        SetConsoleTextAttribute(m_hConsole, LevelColor(level));
    }
    std::cout << formatted << std::endl;
    if (m_hConsole) {
        SetConsoleTextAttribute(m_hConsole, 7); /* Reset */
    }
}

void ElleLogger::WriteFile(const std::string& formatted) {
    std::lock_guard<std::mutex> lock(m_fileMutex);
    if (!m_logFile.is_open()) return;

    m_logFile << formatted << "\n";
    m_logFile.flush();
    m_currentFileSize += formatted.size() + 1;

    if (m_currentFileSize > (uint64_t)m_maxFileSizeMB * 1024 * 1024) {
        RotateFile();
    }
}

void ElleLogger::RotateFile() {
    m_logFile.close();

    /* Rotate existing files — wrap every std::filesystem::rename because
     * a missing file or a locked file would otherwise throw out of the
     * log-write path and take the caller down with it. */
    std::error_code ec;
    for (int i = (int)m_maxFiles - 1; i >= 1; i--) {
        std::string oldName = m_logPath + "." + std::to_string(i);
        std::string newName = m_logPath + "." + std::to_string(i + 1);
        if (std::filesystem::exists(oldName, ec)) {
            std::filesystem::rename(oldName, newName, ec);
        }
    }
    if (std::filesystem::exists(m_logPath, ec)) {
        std::filesystem::rename(m_logPath, m_logPath + ".1", ec);
    }
    m_logFile.open(m_logPath, std::ios::out);
    m_currentFileSize = 0;
}

void ElleLogger::DatabaseWriterThread() {
    while (m_dbRunning) {
        Sleep(1000); /* Batch writes every second */

        std::vector<ELLE_LOG_ENTRY> batch;
        {
            std::lock_guard<std::mutex> lock(m_dbMutex);
            while (!m_dbQueue.empty() && batch.size() < 100) {
                batch.push_back(m_dbQueue.front());
                m_dbQueue.pop();
            }
        }

        for (auto& entry : batch) {
            ElleDB::WriteLog((ELLE_LOG_LEVEL)entry.level,
                            (ELLE_SERVICE_ID)entry.source_svc,
                            entry.message);
        }
    }

    /* Final flush on shutdown — drain the remaining queued entries so tail
     * logs don't disappear. Previously we exited the loop the moment
     * m_dbRunning flipped false and the last second's worth of logs got
     * dropped silently.                                                   */
    std::vector<ELLE_LOG_ENTRY> tail;
    {
        std::lock_guard<std::mutex> lock(m_dbMutex);
        while (!m_dbQueue.empty()) {
            tail.push_back(m_dbQueue.front());
            m_dbQueue.pop();
        }
    }
    for (auto& entry : tail) {
        ElleDB::WriteLog((ELLE_LOG_LEVEL)entry.level,
                        (ELLE_SERVICE_ID)entry.source_svc,
                        entry.message);
    }
}
