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
#include <memory>
#include <unordered_map>

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

    /*──────────────────────────────────────────────────────────────
     * Channel logging — one line to a channel file next to the exe.
     *
     * Contract: fire-and-forget, thread-safe, rotated to
     * `<channel>_YYYY-MM-DD.log` with a fresh file every 10,000
     * lines so no single file grows past sanity. First write auto-
     * creates the logs directory if missing. Used by:
     *
     *   Channel  | Written by     | Format
     *   ---------+----------------+--------------------------------
     *   asserts  | every service  | ELLE_ASSERT trips + crash notes
     *   socket   | every service  | accept/connect/close, tx/rx sz
     *   http     | HTTP service   | %h %r %>s %b %t + auth id
     *   sql      | every service  | query class, latency, rowcount
     *
     * Files land in `<exe_dir>\` (channel logs) or the date-rotated
     * `<exe_dir>\debug\<channel>_YYYY-MM-DD.log` for the debug
     * stream. SQL plan-B queue (`sqllogs/`) is a separate system —
     * see ElleSQLFallback in ElleSQLConn.h.                        */
    void LogChannel(const char* channel, const char* fmt, ...);

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
    uint32_t        m_maxFiles      = 10;
    uint64_t        m_currentFileSize = 0;
    /* Line-count cap for the date-rotated debug file. Once crossed a
     * sequence suffix (-01, -02…) is appended so a single run can't
     * produce a 500MB wall of text.                                  */
    uint32_t        m_maxLinesPerFile = 10000;
    uint32_t        m_currentLineCount = 0;
    int             m_currentDateYmd   = 0;   /* yyyymmdd */
    uint32_t        m_currentDateSuffix = 0;
    std::mutex      m_fileMutex;
    void WriteFile(const std::string& formatted);
    void RotateFile();
    /* Compute today's yyyymmdd; compare against m_currentDateYmd to
     * decide if we need to close the current file and open a new one
     * under the new date (day boundary).                             */
    int  TodayYmd() const;
    /* Open the current file path for today. Bumps line count/date
     * counters. Called from Initialize() and RotateFile().           */
    void OpenDatedDebugFile();

    /* ──────────────── CHANNEL LOGS ──────────────────────────────
     * One LogChannel caller ends up here. Each channel gets its own
     * mutex + ofstream + line counter so `asserts` traffic never
     * blocks on `socket` traffic. Files live next to the exe (same
     * convention as Fiesta's own Zone.log / Socket.log).            */
    struct ChannelState {
        std::ofstream   file;
        uint32_t        lineCount = 0;
        int             dateYmd   = 0;
        uint32_t        dateSuffix = 0;
        std::mutex      mtx;
    };
    std::mutex                                                  m_channelsMutex;
    std::unordered_map<std::string, std::unique_ptr<ChannelState>> m_channels;
    /* Resolve <exe_dir>, cached once per process. Used by channel
     * logs and by the debug file. */
    std::string ExeDirectory() const;
    ChannelState& GetOrOpenChannel(const std::string& name);

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

/* Channel shortcut macros — one call site, one line of log. The
 * channel name becomes `<exe_dir>\<name>_YYYY-MM-DD.log`.            */
#define ELLE_LOG_ASSERT(fmt, ...)  ElleLogger::Instance().LogChannel("asserts", fmt, ##__VA_ARGS__)
#define ELLE_LOG_SOCKET(fmt, ...)  ElleLogger::Instance().LogChannel("socket",  fmt, ##__VA_ARGS__)
#define ELLE_LOG_HTTP(fmt, ...)    ElleLogger::Instance().LogChannel("http",    fmt, ##__VA_ARGS__)
#define ELLE_LOG_SQL(fmt, ...)     ElleLogger::Instance().LogChannel("sql",     fmt, ##__VA_ARGS__)

/* Assert helper — logs via ELLE_LOG_ASSERT AND an ERROR via main logger.
 * Kept as an expression so `if (ELLE_ASSERT(...))` style still parses.  */
#define ELLE_ASSERT(cond, fmt, ...)                                          \
    do {                                                                     \
        if (!(cond)) {                                                       \
            ELLE_LOG_ASSERT("ASSERT FAILED (%s:%d) [%s]: " fmt,              \
                            __FILE__, __LINE__, #cond, ##__VA_ARGS__);       \
            ELLE_ERROR("ASSERT FAILED [%s]: " fmt, #cond, ##__VA_ARGS__);    \
        }                                                                    \
    } while (0)

#endif /* ELLE_LOGGER_H */
