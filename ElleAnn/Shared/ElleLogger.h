/*******************************************************************************
 * ElleLogger.h — Centralized Logging System
 * 
 * Multi-target logging: console, file, database, WebSocket broadcast.
 * Thread-safe, async file writes, log rotation.
 ******************************************************************************/
#pragma once
#ifndef ELLE_LOGGER_H
#define ELLE_LOGGER_H

#include "ElleTypes.h"
#include <string>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <fstream>

/*──────────────────────────────────────────────────────────────────────────────
 * LOG TARGETS
 *──────────────────────────────────────────────────────────────────────────────*/
#define ELLE_LOG_TARGET_CONSOLE     0x01
#define ELLE_LOG_TARGET_FILE        0x02
#define ELLE_LOG_TARGET_DATABASE    0x04
#define ELLE_LOG_TARGET_WEBSOCKET   0x08
#define ELLE_LOG_TARGET_ALL         0x0F

/*──────────────────────────────────────────────────────────────────────────────
 * LOGGER CLASS
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleLogger {
public:
    static ElleLogger& Instance();

    bool Initialize(ELLE_SERVICE_ID sourceService, 
                    uint32_t targets = ELLE_LOG_TARGET_ALL,
                    ELLE_LOG_LEVEL minLevel = LOG_INFO);
    void Shutdown();

    void SetMinLevel(ELLE_LOG_LEVEL level);
    void SetLogFile(const std::string& path, uint32_t maxSizeMB = 100, uint32_t maxFiles = 10);
    void SetWebSocketBroadcaster(std::function<void(const std::string&)> broadcaster);

    /* Core logging */
    void Log(ELLE_LOG_LEVEL level, const char* fmt, ...);
    void LogWithContext(ELLE_LOG_LEVEL level, const char* context, const char* fmt, ...);

    /* Convenience macros get routed here */
    void Trace(const char* fmt, ...);
    void Debug(const char* fmt, ...);
    void Info(const char* fmt, ...);
    void Warn(const char* fmt, ...);
    void Error(const char* fmt, ...);
    void Fatal(const char* fmt, ...);

    /* Specialized logging */
    void LogEmotion(const ELLE_EMOTION_STATE& state);
    void LogIntent(const ELLE_INTENT_RECORD& intent);
    void LogAction(const ELLE_ACTION_RECORD& action);
    void LogTrustChange(int32_t oldScore, int32_t newScore, const std::string& reason);
    void LogIPC(const ELLE_IPC_HEADER& header, bool incoming);
    void LogLLM(const ELLE_LLM_REQUEST& req, const ELLE_LLM_RESPONSE& resp);

    /* Metrics */
    uint64_t TotalEntries() const { return m_totalEntries; }
    uint64_t ErrorCount() const { return m_errorCount; }

private:
    ElleLogger() = default;
    ~ElleLogger();
    ElleLogger(const ElleLogger&) = delete;
    ElleLogger& operator=(const ElleLogger&) = delete;

    ELLE_SERVICE_ID m_sourceService = SVC_HEARTBEAT;
    uint32_t        m_targets = 0;
    std::atomic<ELLE_LOG_LEVEL>  m_minLevel{LOG_INFO};
    bool            m_initialized = false;

    /* Console output */
    HANDLE m_hConsole = nullptr;
    void WriteConsole(ELLE_LOG_LEVEL level, const std::string& formatted);

    /* File output */
    std::string     m_logPath;
    std::ofstream   m_logFile;
    uint32_t        m_maxFileSizeMB = 100;
    uint32_t        m_maxFiles = 10;
    uint64_t        m_currentFileSize = 0;
    std::mutex      m_fileMutex;
    void WriteFile(const std::string& formatted);
    void RotateFile();

    /* Database output (async, condition-variable driven) */
    std::queue<ELLE_LOG_ENTRY> m_dbQueue;
    std::mutex                  m_dbMutex;
    std::condition_variable     m_dbCv;
    std::thread                 m_dbThread;
    std::atomic<bool>           m_dbRunning{false};
    /* Shutdown-drain barrier — when true, Log() refuses new producers so
     * the tail drain inside DatabaseWriterThread() cannot race with a
     * late call that would enqueue after the final drain has started.  */
    std::atomic<bool>           m_dbClosing{false};
    void DatabaseWriterThread();

    /* WebSocket broadcast — mutex-protected because SetWebSocketBroadcaster
     * can be called from a different thread than Log(). */
    std::function<void(const std::string&)> m_wsBroadcaster;
    std::mutex                              m_wsMutex;

    /* Formatting */
    std::string FormatEntry(ELLE_LOG_LEVEL level, const std::string& message, 
                            const std::string& context = "");
    const char* LevelString(ELLE_LOG_LEVEL level);
    uint16_t LevelColor(ELLE_LOG_LEVEL level);

    /* Stats */
    std::atomic<uint64_t> m_totalEntries{0};
    std::atomic<uint64_t> m_errorCount{0};

    std::mutex m_logMutex;
};

/*──────────────────────────────────────────────────────────────────────────────
 * CONVENIENCE MACROS
 *──────────────────────────────────────────────────────────────────────────────*/
#define ELLE_TRACE(fmt, ...)  ElleLogger::Instance().Trace(fmt, ##__VA_ARGS__)
#define ELLE_DEBUG(fmt, ...)  ElleLogger::Instance().Debug(fmt, ##__VA_ARGS__)
#define ELLE_INFO(fmt, ...)   ElleLogger::Instance().Info(fmt, ##__VA_ARGS__)
#define ELLE_WARN(fmt, ...)   ElleLogger::Instance().Warn(fmt, ##__VA_ARGS__)
#define ELLE_ERROR(fmt, ...)  ElleLogger::Instance().Error(fmt, ##__VA_ARGS__)
#define ELLE_FATAL(fmt, ...)  ElleLogger::Instance().Fatal(fmt, ##__VA_ARGS__)

#endif /* ELLE_LOGGER_H */
