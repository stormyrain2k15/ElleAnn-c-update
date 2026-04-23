/*******************************************************************************
 * DictionaryLoader.h — port of legacy dictionary_loader.py
 *
 * Downloads word definitions from api.dictionaryapi.dev and persists them to
 * ElleCore.dbo.dictionary_words. Runs in a background thread so the HTTP
 * caller gets an immediate "started" response. Progress is mirrored into
 * ElleCore.dbo.dictionary_loader_state so the GET endpoint can poll.
 ******************************************************************************/
#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

class DictionaryLoader {
public:
    static DictionaryLoader& Instance();

    /* Kick off a background load of the built-in CORE_WORDS list. If a load
     * is already running, returns false with a note. Caller polls GetState()
     * for progress.                                                          */
    bool StartLoad(uint32_t startIdx = 0, uint32_t limit = 0);

    /* Fetch a single word on demand (synchronous — used by lookup endpoint
     * when the local DB has no entry for a word). Returns true on success.  */
    bool FetchOneAndStore(const std::string& word);

    struct State {
        std::string status;   /* idle|running|done|failed */
        uint32_t    loaded = 0;
        uint32_t    failed = 0;
        uint32_t    skipped = 0;
        std::string last_word;
        std::string error;
        uint64_t    started_ms = 0;
        uint64_t    updated_ms = 0;
    };
    State GetState() const;

    /* Request a cooperative shutdown — flips the running flag so the
     * worker exits at the next word boundary, then joins the thread so
     * no in-flight SQL/HTTP I/O is left outstanding when the process
     * tears down. Safe to call multiple times.                           */
    void Shutdown();

    /* Destructor guarantees the worker thread is joined. Previously the
     * thread was launched .detach()'d (now m_worker.joinable() is used)
     * so process teardown raced with an in-flight curl/SQL call.       */
    ~DictionaryLoader();

private:
    DictionaryLoader() = default;

    std::atomic<bool>     m_running{false};
    mutable std::mutex    m_stateMutex;
    State                 m_state;
    std::thread           m_worker;

    void WorkerRun(uint32_t startIdx, uint32_t limit);
    bool FetchDefinition(const std::string& word, std::string& outJson);
    bool ParseAndInsert(const std::string& word, const std::string& apiJson,
                        uint32_t& entriesAdded);
    void PersistState();
};
