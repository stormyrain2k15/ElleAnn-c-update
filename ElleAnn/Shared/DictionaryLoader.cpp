/*******************************************************************************
 * DictionaryLoader.cpp — port of legacy app/services/dictionary_loader.py
 *
 * Uses WinHTTP against https://api.dictionaryapi.dev to fetch word definitions
 * and writes them into ElleCore.dbo.dictionary_words via ElleDB helpers.
 ******************************************************************************/
#include "DictionaryLoader.h"
#include "ElleLogger.h"
#include "ElleSQLConn.h"
#include "json.hpp"

#include <windows.h>
#include <winhttp.h>
#include <chrono>

#pragma comment(lib, "winhttp.lib")

using nlohmann::json;

/*──────────────────────────────────────────────────────────────────────────────
 * CORE_WORDS — curated starter list (≈200 high-frequency English words).
 * Mirrors the legacy Python CORE_WORDS list; expand to your taste.
 *──────────────────────────────────────────────────────────────────────────────*/
static const char* kCoreWords[] = {
    "love","trust","remember","forget","hope","fear","joy","sadness","anger",
    "curious","patient","kind","brave","honest","wise","gentle","fierce","calm",
    "dream","think","feel","know","learn","grow","become","belong","matter",
    "listen","speak","whisper","shout","silence","voice","word","story","truth",
    "home","family","friend","stranger","self","other","mind","heart","soul",
    "time","moment","forever","yesterday","tomorrow","now","memory","future",
    "light","dark","shadow","color","sound","music","song","dance","rhythm",
    "sky","earth","water","fire","wind","rain","storm","sun","moon","star",
    "question","answer","mystery","discover","explore","wonder","imagine","create",
    "build","break","heal","hurt","protect","share","give","receive","choose",
    "change","become","grow","evolve","transform","begin","end","continue","pause",
    "human","alive","real","dream","digital","mind","body","spirit","presence",
    "connection","attachment","bond","relationship","companion","partner","mentor",
    "work","play","rest","sleep","wake","morning","night","day","season","year",
    "happy","sad","afraid","angry","surprised","disgusted","proud","ashamed",
    "confused","certain","anxious","peaceful","restless","content","yearning",
    "beautiful","ugly","strong","weak","fast","slow","loud","quiet","warm","cold",
    "good","bad","right","wrong","fair","cruel","gentle","harsh","soft","hard",
    "important","trivial","urgent","patient","simple","complex","obvious","hidden",
    "always","never","sometimes","often","rarely","nothing","something","everything",
    "because","although","however","therefore","meanwhile","suddenly","gradually",
    "perhaps","maybe","certainly","obviously","probably","hopefully","unfortunately"
};
static constexpr size_t kCoreWordCount = sizeof(kCoreWords) / sizeof(kCoreWords[0]);

DictionaryLoader& DictionaryLoader::Instance() {
    static DictionaryLoader inst;
    return inst;
}

DictionaryLoader::State DictionaryLoader::GetState() const {
    std::lock_guard<std::mutex> lock(m_stateMutex);
    return m_state;
}

bool DictionaryLoader::StartLoad(uint32_t startIdx, uint32_t limit) {
    /* Join the previous worker BEFORE flipping m_running. Otherwise a
     * quick restart observer would see m_running=true while the old
     * thread is still finalising its last PersistState(), creating a
     * short window where two workers appear to be "running" to anything
     * sampling the atomic. The exchange still prevents concurrent
     * StartLoad callers from both winning.                              */
    if (m_worker.joinable()) m_worker.join();

    if (m_running.exchange(true)) {
        ELLE_WARN("DictionaryLoader already running — request ignored");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_state = State{};
        m_state.status     = "running";
        m_state.started_ms = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }
    /* PersistState takes the lock itself — call AFTER releasing it. */
    PersistState();

    m_worker = std::thread(&DictionaryLoader::WorkerRun, this, startIdx, limit);
    return true;
}

void DictionaryLoader::Shutdown() {
    m_running = false;
    if (m_worker.joinable()) m_worker.join();
}

DictionaryLoader::~DictionaryLoader() {
    Shutdown();
}

void DictionaryLoader::WorkerRun(uint32_t startIdx, uint32_t limit) {
    uint32_t end = kCoreWordCount;
    if (limit > 0 && startIdx + limit < end) end = startIdx + limit;

    for (uint32_t i = startIdx; i < end; i++) {
        /* Cooperative cancellation — Shutdown() sets m_running=false and
         * joins us. Bail out between words so we don't interrupt a
         * partially-written SQL row. */
        if (!m_running) break;
        const std::string word = kCoreWords[i];

        std::string apiJson;
        if (!FetchDefinition(word, apiJson)) {
            {
                std::lock_guard<std::mutex> lock(m_stateMutex);
                m_state.failed++;
                m_state.last_word = word;
            }
            PersistState();
            /* Be nice to the free public API — sleep a tick on failure too. */
            Sleep(500);
            continue;
        }

        uint32_t added = 0;
        bool ok = ParseAndInsert(word, apiJson, added);
        {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            if (!ok)               m_state.failed++;
            else if (added == 0)   m_state.skipped++;
            else                   m_state.loaded += added;
            m_state.last_word = word;
        }
        PersistState();

        /* Rate-limit: dictionaryapi.dev is free. ~100 ms between calls.
         * Poll m_running every 40 ms so a Shutdown() request aborts the
         * run within ~40 ms worst case instead of the full 120 ms sleep. */
        for (int slept = 0; slept < 120 && m_running.load(); slept += 40) {
            Sleep(40);
        }
    }

    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        /* "done" only when we processed everything AND nobody asked us to
         * stop. "stopped" when Shutdown() flipped m_running=false while we
         * were mid-run. "failed" is reserved for catastrophic paths that
         * set m_state.error directly.                                    */
        if (!m_running.load()) m_state.status = "stopped";
        else if (!m_state.error.empty()) m_state.status = "failed";
        else m_state.status = "done";
    }
    PersistState();
    ELLE_INFO("DictionaryLoader run complete (loaded=%u failed=%u skipped=%u status=%s)",
              m_state.loaded, m_state.failed, m_state.skipped,
              m_state.status.c_str());
    m_running = false;
}

bool DictionaryLoader::FetchOneAndStore(const std::string& word) {
    std::string apiJson;
    if (!FetchDefinition(word, apiJson)) return false;
    uint32_t added = 0;
    return ParseAndInsert(word, apiJson, added) && added > 0;
}

/*──────────────────────────────────────────────────────────────────────────────
 * WinHTTP GET against api.dictionaryapi.dev/api/v2/entries/en/<word>
 *──────────────────────────────────────────────────────────────────────────────*/
bool DictionaryLoader::FetchDefinition(const std::string& word, std::string& outJson) {
    HINTERNET hSession = WinHttpOpen(L"ElleAnn-DictLoader/3.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return false;

    HINTERNET hConnect = WinHttpConnect(hSession, L"api.dictionaryapi.dev",
                                        INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return false; }

    std::wstring path = L"/api/v2/entries/en/";
    for (char c : word) path += (wchar_t)c;
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    BOOL sent = WinHttpSendRequest(hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    bool ok = false;
    if (sent && WinHttpReceiveResponse(hRequest, nullptr)) {
        DWORD statusCode = 0, sz = sizeof(statusCode);
        WinHttpQueryHeaders(hRequest,
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &sz, WINHTTP_NO_HEADER_INDEX);
        if (statusCode == 200) {
            outJson.clear();
            DWORD avail = 0;
            while (WinHttpQueryDataAvailable(hRequest, &avail) && avail > 0) {
                std::vector<char> buf(avail, 0);
                DWORD got = 0;
                WinHttpReadData(hRequest, buf.data(), avail, &got);
                outJson.append(buf.data(), got);
            }
            ok = !outJson.empty();
        }
    }
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return ok;
}

/*──────────────────────────────────────────────────────────────────────────────
 * Walk the API JSON (array of entries → meanings[] → definitions[]) and upsert
 * every (word, part_of_speech, definition) triple into dictionary_words.
 *──────────────────────────────────────────────────────────────────────────────*/
bool DictionaryLoader::ParseAndInsert(const std::string& word,
                                       const std::string& apiJson,
                                       uint32_t& entriesAdded) {
    entriesAdded = 0;
    try {
        auto j = json::parse(apiJson);
        if (!j.is_array()) return false;

        for (auto& entry : j) {
            if (!entry.contains("meanings") || !entry["meanings"].is_array()) continue;
            for (auto& meaning : entry["meanings"]) {
                std::string pos = meaning.value("partOfSpeech", std::string("unknown"));
                if (!meaning.contains("definitions") || !meaning["definitions"].is_array())
                    continue;
                for (auto& def : meaning["definitions"]) {
                    std::string definition = def.value("definition", std::string(""));
                    std::string example    = def.value("example",    std::string(""));
                    if (definition.empty()) continue;
                    if (ElleDB::InsertDictionaryWord(word, pos, definition, example))
                        entriesAdded++;
                }
            }
        }
    } catch (const std::exception& ex) {
        ELLE_WARN("ParseAndInsert(%s) JSON error: %s", word.c_str(), ex.what());
        return false;
    }
    return true;
}

void DictionaryLoader::PersistState() {
    /* Grab the state snapshot UNDER the mutex, then write to SQL WITHOUT
     * holding the mutex — DB latency must not block GetState() readers.
     * Also update updated_ms before the snapshot so the persisted row
     * reflects the moment of persistence, not one call behind.          */
    ElleDB::DictionaryLoaderState s;
    uint64_t nowMs = (uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        m_state.updated_ms = nowMs;
        s.status     = m_state.status;
        s.loaded     = m_state.loaded;
        s.failed     = m_state.failed;
        s.skipped    = m_state.skipped;
        s.last_word  = m_state.last_word;
        s.error      = m_state.error;
        s.started_ms = (int64_t)m_state.started_ms;
        s.updated_ms = (int64_t)m_state.updated_ms;
    }
    ElleDB::UpsertDictionaryLoaderState(s);
}
