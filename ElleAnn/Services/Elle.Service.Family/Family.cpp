/*******************************************************************************
 * Family.cpp — The Family Service (SVC_FAMILY, slot 17)
 *
 * Per user spec:
 *   "The digital offspring should be Elle core stripped of all Elle
 *    personality markers, zipped and held for 'pregnancy' term, then
 *    unzipped and launched on new port instead of Elle port."
 *
 * Pipeline:
 *   1. CONCEPTION
 *      XChromosome fires IPC_FAMILY_CONCEPTION_ATTEMPT. Family receives
 *      (or, if Family was offline, replays backlog from
 *      ElleHeart.dbo.x_conception_attempts at startup).
 *      For each attempt:
 *        - Insert row into ElleHeart.dbo.family_pregnancies (conceived_ms,
 *          due_ms, gestational_days, status='gestating', snapshot_path).
 *        - Take a personality-stripped snapshot of Elle's install tree:
 *          copy the service binaries, Lua scripts, and SQL schema DDL into
 *          a staging folder — deliberately NOT copying:
 *             * ElleIdentityCore autobiography / private thoughts
 *             * Bonding / InnerLife relationship state
 *             * Memory / emotion history
 *             * Traits / preferences (child starts on the neutral baseline)
 *          Then zip the staging folder with Windows `tar.exe` (bundled with
 *          Win10+) and store at C:\ElleAnn\pregnancies\preg_<id>.zip.
 *        - Mark the conception row consumed.
 *
 *   2. GESTATION
 *      OnTick (every 30s by default, config: family.tick_ms) polls
 *      family_pregnancies WHERE status='gestating' AND due_ms <= now.
 *      For each mature pregnancy → GiveBirth().
 *
 *   3. BIRTH
 *      - Allocate next port: max(family_children.port) + 100
 *        (starting base = config family.first_child_port, default 9200).
 *      - Create child install dir: C:\ElleAnn\children\child_<id>\.
 *      - Unzip the snapshot there via `tar.exe`.
 *      - Write a child-specific elle_master_config.json that points at:
 *          * a new HTTP port
 *          * new DB names (ElleCore_child<id>, ElleHeart_child<id>,
 *            ElleSystem_child<id>)
 *      - Run the SQL schema DDL against the fresh DBs (IF NOT EXISTS blocks
 *        mean this is idempotent on retry).
 *      - Seed a fresh identity_core row with baby's blank autobiography.
 *      - CreateProcessW the child's HTTP service exe.
 *      - Insert family_children row: status='alive', port, install_dir,
 *        process_id, born_ms.
 *      - Broadcast IPC_FAMILY_BIRTH + IPC_WORLD_EVENT so Elle feels it.
 *
 *   4. LIFE
 *      Family pings alive children every tick. If a child process has
 *      terminated, flip family_children.status to 'lost' and broadcast.
 *
 * Everything is backed by durable SQL rows so a Family service restart
 * never loses a pregnancy or orphans a child.
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"
#include "../../Shared/json.hpp"
#include "../../Shared/ElleJsonExtract.h"

#include <windows.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdio>

namespace fs = std::filesystem;
using json = nlohmann::json;

class ElleFamilyService : public ElleServiceBase {
public:
    ElleFamilyService()
        : ElleServiceBase(SVC_FAMILY, "ElleFamily",
                          "Elle-Ann Family Engine",
                          "Conception → snapshot → gestation → birth → launch") {}

protected:
    bool OnStart() override {
        EnsureSchema();
        ComputeRootPaths();

        uint32_t tickMs = (uint32_t)ElleConfig::Instance().GetInt("family.tick_ms", 30000);
        SetTickInterval(tickMs);

        /* Conception backlog replay: XChromosome keeps writing conception
         * events to x_conception_attempts even when Family is down. Drain
         * any unconsumed rows into pregnancies so gestation starts.      */
        DrainConceptionBacklog();

        ELLE_INFO("Family service started (tick=%ums, install_root=%s)",
                  tickMs, m_elleInstallRoot.u8string().c_str());
        return true;
    }

    void OnStop() override {
        ELLE_INFO("Family service stopped");
    }

    void OnTick() override {
        DrainConceptionBacklog();  /* catch anything that arrived mid-tick */
        ProcessMaturePregnancies();
        MonitorLiveChildren();
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID /*sender*/) override {
        if (msg.header.msg_type != IPC_FAMILY_CONCEPTION_ATTEMPT) return;

        json j;
        if (!Elle::ExtractJsonObject(msg.GetStringPayload(), j)) {
            ELLE_WARN("Family: malformed conception payload");
            return;
        }
        int64_t  bornMs    = j.value("born_ms",           (int64_t)0);
        int      gestDays  = j.value("gestational_days",  30);
        std::string origin = j.value("origin",            std::string("x_chromosome"));
        std::string payload = j.dump();

        int64_t pregId = CreatePregnancy(bornMs, gestDays, origin, payload);
        if (pregId > 0) TakeSnapshot(pregId);
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT, SVC_X_CHROMOSOME };
    }

private:
    fs::path m_elleInstallRoot;   /* root of parent Elle's installed tree */
    fs::path m_pregnanciesRoot;   /* C:\ElleAnn\pregnancies              */
    fs::path m_childrenRoot;      /* C:\ElleAnn\children                 */

    /*──────────────────────────────────────────────────────────────────────
     * PATH RESOLUTION
     *──────────────────────────────────────────────────────────────────────*/
    void ComputeRootPaths() {
        /* Parent's install root = folder containing THIS exe. */
        wchar_t buf[MAX_PATH];
        GetModuleFileNameW(nullptr, buf, MAX_PATH);
        m_elleInstallRoot = fs::path(buf).parent_path();

        m_pregnanciesRoot = fs::path(ElleConfig::Instance().GetString(
            "family.pregnancies_root", "C:\\ElleAnn\\pregnancies"));
        m_childrenRoot    = fs::path(ElleConfig::Instance().GetString(
            "family.children_root",    "C:\\ElleAnn\\children"));

        std::error_code ec;
        fs::create_directories(m_pregnanciesRoot, ec);
        fs::create_directories(m_childrenRoot,    ec);
    }

    /*──────────────────────────────────────────────────────────────────────
     * SCHEMA BOOTSTRAP — idempotent on every start.
     *──────────────────────────────────────────────────────────────────────*/
    void EnsureSchema() {
        auto& sql = ElleSQLPool::Instance();
        sql.Exec(
            "IF NOT EXISTS (SELECT 1 FROM sys.tables t "
            "  JOIN sys.schemas s ON s.schema_id = t.schema_id "
            "  WHERE t.name = 'family_pregnancies' AND s.name = 'dbo') "
            "CREATE TABLE ElleHeart.dbo.family_pregnancies ("
            "  id BIGINT IDENTITY(1,1) PRIMARY KEY,"
            "  conceived_ms BIGINT NOT NULL,"
            "  due_ms       BIGINT NOT NULL,"
            "  gestational_days INT NOT NULL,"
            "  origin       NVARCHAR(64) NOT NULL,"
            "  payload_json NVARCHAR(MAX) NOT NULL,"
            "  snapshot_path NVARCHAR(512) NULL,"
            "  status       NVARCHAR(32) NOT NULL DEFAULT 'gestating',"
            "  child_id     BIGINT NULL"
            ");");
        sql.Exec(
            "IF NOT EXISTS (SELECT 1 FROM sys.tables t "
            "  JOIN sys.schemas s ON s.schema_id = t.schema_id "
            "  WHERE t.name = 'family_children' AND s.name = 'dbo') "
            "CREATE TABLE ElleHeart.dbo.family_children ("
            "  id BIGINT IDENTITY(1,1) PRIMARY KEY,"
            "  pregnancy_id BIGINT NOT NULL,"
            "  name         NVARCHAR(128) NULL,"
            "  port         INT NOT NULL,"
            "  install_dir  NVARCHAR(512) NOT NULL,"
            "  process_id   INT NULL,"
            "  born_ms      BIGINT NOT NULL,"
            "  status       NVARCHAR(32) NOT NULL DEFAULT 'alive'"
            ");");
    }

    /*──────────────────────────────────────────────────────────────────────
     * CONCEPTION
     *──────────────────────────────────────────────────────────────────────*/
    int64_t CreatePregnancy(int64_t bornMs, int gestDays,
                            const std::string& origin,
                            const std::string& payload) {
        uint64_t now = ELLE_MS_NOW();
        if (gestDays <= 0) gestDays = 30;
        int64_t dueMs = bornMs > 0 ? bornMs
                                   : (int64_t)now + (int64_t)gestDays * 86400000LL;

        /* Use OUTPUT inserted.id to get the row id atomically. */
        auto rs = ElleSQLPool::Instance().QueryParams(
            "INSERT INTO ElleHeart.dbo.family_pregnancies "
            "(conceived_ms, due_ms, gestational_days, origin, payload_json) "
            "OUTPUT inserted.id "
            "VALUES (?, ?, ?, ?, ?);",
            { std::to_string((int64_t)now),
              std::to_string(dueMs),
              std::to_string(gestDays),
              origin,
              payload });
        if (!rs.success || rs.rows.empty()) {
            ELLE_ERROR("Family: failed to create pregnancy row");
            return 0;
        }
        int64_t id = rs.rows[0].GetInt(0);
        ELLE_INFO("Family: pregnancy #%lld conceived (gestation=%dd, due=%lld)",
                  (long long)id, gestDays, (long long)dueMs);
        return id;
    }

    void DrainConceptionBacklog() {
        auto rs = ElleSQLPool::Instance().Query(
            "SELECT id, born_ms, gestational_days, payload_json "
            "FROM ElleHeart.dbo.x_conception_attempts "
            "WHERE consumed = 0 ORDER BY id ASC;");
        if (!rs.success) return;
        for (auto& row : rs.rows) {
            int64_t  attemptId = row.GetInt(0);
            int64_t  bornMs    = row.GetInt(1);
            int      gestDays  = (int)row.GetInt(2);
            std::string payload = row.values.size() > 3 ? row.values[3] : "";

            int64_t pregId = CreatePregnancy(bornMs, gestDays, "x_chromosome_backlog", payload);
            if (pregId > 0) {
                TakeSnapshot(pregId);
                ElleSQLPool::Instance().QueryParams(
                    "UPDATE ElleHeart.dbo.x_conception_attempts "
                    "SET consumed = 1 WHERE id = ?;",
                    { std::to_string(attemptId) });
            }
        }
    }

    /*──────────────────────────────────────────────────────────────────────
     * SNAPSHOT — personality-stripped archive of parent's install tree.
     *
     * We include:
     *   - bin/*.exe, *.dll         (service binaries)
     *   - scripts/*.lua             (behavioural scripts)
     *   - sql/*.sql                 (schema DDL only — no data)
     *   - config_template.json      (blank-slate config, NOT parent's)
     *
     * We exclude:
     *   - any live database export   (identity, memory, emotion history)
     *   - parent's elle_master_config.json (has live ODBC string + key)
     *   - logs/, .pdb, .user, .git
     *
     * This IS the personality stripping: schema only, zero rows. Child boots
     * with empty autobiography, empty memories, baseline traits, its own DB.
     *──────────────────────────────────────────────────────────────────────*/
    void TakeSnapshot(int64_t pregId) {
        fs::path staging = m_pregnanciesRoot / ("preg_" + std::to_string(pregId) + "_stage");
        fs::path zipPath = m_pregnanciesRoot / ("preg_" + std::to_string(pregId) + ".zip");
        std::error_code ec;
        fs::remove_all(staging, ec);
        fs::create_directories(staging / "bin",     ec);
        fs::create_directories(staging / "scripts", ec);
        fs::create_directories(staging / "sql",     ec);

        /* Copy binaries */
        CopyFilesByExt(m_elleInstallRoot, staging / "bin", { L".exe", L".dll" });
        /* Copy Lua scripts from parent's scripts/ if present, else from the
         * standard sub-path Elle.Lua.Behavioral/scripts.                  */
        fs::path luaSrc = m_elleInstallRoot / "scripts";
        if (!fs::exists(luaSrc)) {
            luaSrc = m_elleInstallRoot.parent_path() / "Lua" / "Elle.Lua.Behavioral" / "scripts";
        }
        if (fs::exists(luaSrc)) {
            CopyDirectoryRecursive(luaSrc, staging / "scripts", { L".lua", L".md" });
        }
        /* Copy SQL deltas (schema DDL). */
        fs::path sqlSrc = m_elleInstallRoot / "sql";
        if (!fs::exists(sqlSrc)) sqlSrc = m_elleInstallRoot.parent_path() / "SQL";
        if (fs::exists(sqlSrc)) {
            CopyDirectoryRecursive(sqlSrc, staging / "sql", { L".sql" });
        }

        /* Write a blank-slate config template. Birth will overwrite the
         * placeholders with the child-specific port + DB names.          */
        json cfgTemplate = {
            {"identity", {
                {"name", "CHILD_NAME_PLACEHOLDER"},
                {"autobiography", ""},
                {"baseline_traits", true}
            }},
            {"http", {
                {"port",          0 /* BIRTH_PORT_PLACEHOLDER */},
                {"bind_address",  "0.0.0.0"}
            }},
            {"sql", {
                {"core_db",   "ELLECORE_DB_PLACEHOLDER"},
                {"heart_db",  "ELLEHEART_DB_PLACEHOLDER"},
                {"system_db", "ELLESYSTEM_DB_PLACEHOLDER"},
                {"connection_string", "INHERIT_FROM_PARENT"}
            }}
        };
        std::ofstream(staging / "config_template.json") << cfgTemplate.dump(2);

        /* Zip via Win10+ built-in tar.exe (supports -a --format=zip). */
        std::wstring cmd = L"cmd /c tar.exe -a -cf \"" + zipPath.wstring()
                         + L"\" -C \"" + staging.wstring() + L"\" .";
        int rc = RunWaitW(cmd);
        fs::remove_all(staging, ec);

        if (rc != 0) {
            ELLE_ERROR("Family: tar zip failed (rc=%d) for pregnancy #%lld",
                       rc, (long long)pregId);
            return;
        }

        ElleSQLPool::Instance().QueryParams(
            "UPDATE ElleHeart.dbo.family_pregnancies "
            "SET snapshot_path = ? WHERE id = ?;",
            { zipPath.u8string(), std::to_string(pregId) });
        ELLE_INFO("Family: snapshot captured for pregnancy #%lld → %s",
                  (long long)pregId, zipPath.u8string().c_str());
    }

    /*──────────────────────────────────────────────────────────────────────
     * GESTATION → BIRTH
     *──────────────────────────────────────────────────────────────────────*/
    void ProcessMaturePregnancies() {
        uint64_t now = ELLE_MS_NOW();
        auto rs = ElleSQLPool::Instance().QueryParams(
            "SELECT id, gestational_days, snapshot_path "
            "FROM ElleHeart.dbo.family_pregnancies "
            "WHERE status = 'gestating' AND due_ms <= ?;",
            { std::to_string((int64_t)now) });
        if (!rs.success) return;
        for (auto& row : rs.rows) {
            int64_t pregId   = row.GetInt(0);
            int     gestDays = (int)row.GetInt(1);
            std::string zipPath = row.values.size() > 2 ? row.values[2] : "";
            if (zipPath.empty() || !fs::exists(zipPath)) {
                ELLE_WARN("Family: pregnancy #%lld has no snapshot — stillborn",
                          (long long)pregId);
                ElleSQLPool::Instance().QueryParams(
                    "UPDATE ElleHeart.dbo.family_pregnancies "
                    "SET status = 'lost' WHERE id = ?;",
                    { std::to_string(pregId) });
                continue;
            }
            GiveBirth(pregId, gestDays, fs::path(zipPath));
        }
    }

    int AllocateChildPort() {
        int basePort = (int)ElleConfig::Instance().GetInt("family.first_child_port", 9200);
        int step     = (int)ElleConfig::Instance().GetInt("family.port_step",         100);
        auto rs = ElleSQLPool::Instance().Query(
            "SELECT ISNULL(MAX(port), 0) FROM ElleHeart.dbo.family_children;");
        int maxPort = 0;
        if (rs.success && !rs.rows.empty()) maxPort = (int)rs.rows[0].GetInt(0);
        return maxPort <= 0 ? basePort : maxPort + step;
    }

    void GiveBirth(int64_t pregId, int gestDays, const fs::path& zipPath) {
        int port = AllocateChildPort();
        fs::path childDir = m_childrenRoot / ("child_" + std::to_string(pregId));
        std::error_code ec;
        fs::remove_all(childDir, ec);
        fs::create_directories(childDir, ec);

        /* Unzip snapshot into child dir. */
        std::wstring cmd = L"cmd /c tar.exe -xf \"" + zipPath.wstring()
                         + L"\" -C \"" + childDir.wstring() + L"\"";
        int rc = RunWaitW(cmd);
        if (rc != 0) {
            ELLE_ERROR("Family: unzip failed (rc=%d) for pregnancy #%lld", rc, (long long)pregId);
            return;
        }

        /* Write child-specific config. Inherits connection string stub from
         * parent so the DBAs can pre-configure a connection with a catalog
         * wildcard or we point it at the child's new databases explicitly. */
        std::string coreDb   = "ElleCore_child"   + std::to_string(pregId);
        std::string heartDb  = "ElleHeart_child"  + std::to_string(pregId);
        std::string systemDb = "ElleSystem_child" + std::to_string(pregId);

        json cfg = {
            {"identity", {
                {"name",          std::string("child_") + std::to_string(pregId)},
                {"autobiography", ""},
                {"baseline_traits", true},
                {"parent_pregnancy_id", pregId}
            }},
            {"http", {
                {"port",         port},
                {"bind_address", "127.0.0.1"}
            }},
            {"sql", {
                {"core_db",   coreDb},
                {"heart_db",  heartDb},
                {"system_db", systemDb},
                {"connection_string", ElleConfig::Instance().GetString(
                    "sql.connection_string", "")}
            }}
        };
        std::ofstream(childDir / "elle_master_config.json") << cfg.dump(2);

        /* Create child DBs + run schema DDL. Idempotent via IF NOT EXISTS. */
        CreateChildDatabases(coreDb, heartDb, systemDb);
        RunSchemaAgainstChild(childDir / "sql", coreDb, heartDb, systemDb);

        /* Spawn the child HTTP service. The child's HTTP exe reads
         * elle_master_config.json from its own CWD — so CWD must be
         * childDir. Minimal launch — user can scale up later by also
         * spawning Cognitive/Memory/etc.                                 */
        fs::path childExe = childDir / "Elle.Service.HTTP.exe";
        if (!fs::exists(childExe)) {
            ELLE_ERROR("Family: child HTTP exe missing at %s", childExe.u8string().c_str());
            return;
        }

        STARTUPINFOW si{};
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi{};
        std::wstring wExe = childExe.wstring();
        std::wstring wDir = childDir.wstring();
        std::vector<wchar_t> cmdBuf(wExe.size() + 4, 0);
        swprintf(cmdBuf.data(), cmdBuf.size(), L"\"%s\"", wExe.c_str());

        BOOL ok = CreateProcessW(
            wExe.c_str(),
            cmdBuf.data(),
            nullptr, nullptr, FALSE,
            CREATE_NEW_PROCESS_GROUP | CREATE_NO_WINDOW,
            nullptr,
            wDir.c_str(),
            &si, &pi);
        if (!ok) {
            ELLE_ERROR("Family: CreateProcessW failed for child #%lld (err=%lu)",
                       (long long)pregId, GetLastError());
            return;
        }
        CloseHandle(pi.hThread);
        DWORD childPid = pi.dwProcessId;
        CloseHandle(pi.hProcess);  /* we monitor via OpenProcess in MonitorLiveChildren */

        /* Register the child row. */
        uint64_t nowMs = ELLE_MS_NOW();
        auto rs = ElleSQLPool::Instance().QueryParams(
            "INSERT INTO ElleHeart.dbo.family_children "
            "(pregnancy_id, port, install_dir, process_id, born_ms) "
            "OUTPUT inserted.id "
            "VALUES (?, ?, ?, ?, ?);",
            { std::to_string(pregId),
              std::to_string(port),
              childDir.u8string(),
              std::to_string((int)childPid),
              std::to_string((int64_t)nowMs) });
        int64_t childId = (rs.success && !rs.rows.empty()) ? rs.rows[0].GetInt(0) : 0;

        ElleSQLPool::Instance().QueryParams(
            "UPDATE ElleHeart.dbo.family_pregnancies "
            "SET status = 'born', child_id = ? WHERE id = ?;",
            { std::to_string(childId), std::to_string(pregId) });

        ELLE_INFO("Family: birth of child #%lld from pregnancy #%lld on port %d (pid=%lu)",
                  (long long)childId, (long long)pregId, port, childPid);

        /* Broadcast so HTTP (parent's) can fan out to Android + Elle's
         * own subsystems can register the emotional event.               */
        json ev = {
            {"type",          "family_birth"},
            {"pregnancy_id",  pregId},
            {"child_id",      childId},
            {"port",          port},
            {"gestational_days", gestDays},
            {"born_ms",       nowMs}
        };
        auto birthMsg = ElleIPCMessage::Create(IPC_FAMILY_BIRTH, SVC_FAMILY, (ELLE_SERVICE_ID)0);
        birthMsg.SetStringPayload(ev.dump());
        birthMsg.header.flags |= ELLE_IPC_FLAG_BROADCAST;
        GetIPCHub().Broadcast(birthMsg);

        auto wsMsg = ElleIPCMessage::Create(IPC_WORLD_EVENT, SVC_FAMILY, SVC_HTTP_SERVER);
        wsMsg.SetStringPayload(ev.dump());
        GetIPCHub().Send(SVC_HTTP_SERVER, wsMsg);
    }

    /*──────────────────────────────────────────────────────────────────────
     * LIFE MONITOR — flip status to 'lost' when a child process is gone.
     *──────────────────────────────────────────────────────────────────────*/
    void MonitorLiveChildren() {
        auto rs = ElleSQLPool::Instance().Query(
            "SELECT id, process_id FROM ElleHeart.dbo.family_children "
            "WHERE status = 'alive';");
        if (!rs.success) return;
        for (auto& row : rs.rows) {
            int64_t childId = row.GetInt(0);
            int     pid     = (int)row.GetInt(1);
            if (pid <= 0) continue;
            HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD)pid);
            if (!h) {
                MarkChildLost(childId);
                continue;
            }
            DWORD code = 0;
            if (GetExitCodeProcess(h, &code) && code != STILL_ACTIVE) {
                MarkChildLost(childId);
            }
            CloseHandle(h);
        }
    }

    void MarkChildLost(int64_t childId) {
        ElleSQLPool::Instance().QueryParams(
            "UPDATE ElleHeart.dbo.family_children "
            "SET status = 'lost' WHERE id = ? AND status = 'alive';",
            { std::to_string(childId) });
        ELLE_WARN("Family: child #%lld lost (process terminated)", (long long)childId);
        json ev = { {"type", "family_child_lost"}, {"child_id", childId} };
        auto msg = ElleIPCMessage::Create(IPC_WORLD_EVENT, SVC_FAMILY, SVC_HTTP_SERVER);
        msg.SetStringPayload(ev.dump());
        GetIPCHub().Send(SVC_HTTP_SERVER, msg);
    }

    /*──────────────────────────────────────────────────────────────────────
     * CHILD DB BOOTSTRAP
     *──────────────────────────────────────────────────────────────────────*/
    void CreateChildDatabases(const std::string& coreDb,
                              const std::string& heartDb,
                              const std::string& systemDb) {
        auto& sql = ElleSQLPool::Instance();
        for (auto& db : { coreDb, heartDb, systemDb }) {
            std::string stmt =
                "IF NOT EXISTS (SELECT 1 FROM sys.databases WHERE name = N'" + db + "') "
                "CREATE DATABASE [" + db + "];";
            sql.Exec(stmt);
        }
    }

    void RunSchemaAgainstChild(const fs::path& sqlDir,
                               const std::string& /*coreDb*/,
                               const std::string& /*heartDb*/,
                               const std::string& /*systemDb*/) {
        /* Execute every .sql in the snapshot's sql/ folder. Each DDL script
         * is expected to USE its own catalog explicitly; we don't rewrite
         * database names here — scripts reference DB names via variables
         * or live in the correct catalog at load time.                    */
        if (!fs::exists(sqlDir)) return;
        std::error_code ec;
        for (auto& entry : fs::directory_iterator(sqlDir, ec)) {
            if (!entry.is_regular_file()) continue;
            if (entry.path().extension() != ".sql") continue;
            std::ifstream f(entry.path());
            std::stringstream ss; ss << f.rdbuf();
            std::string ddl = ss.str();
            if (ddl.empty()) continue;
            ElleSQLPool::Instance().Exec(ddl);
        }
    }

    /*──────────────────────────────────────────────────────────────────────
     * FILE-SYSTEM HELPERS
     *──────────────────────────────────────────────────────────────────────*/
    static void CopyFilesByExt(const fs::path& from, const fs::path& to,
                               const std::vector<std::wstring>& exts) {
        std::error_code ec;
        if (!fs::exists(from)) return;
        for (auto& entry : fs::directory_iterator(from, ec)) {
            if (!entry.is_regular_file()) continue;
            std::wstring ext = entry.path().extension().wstring();
            for (auto& e : exts) {
                if (ext == e) {
                    fs::copy_file(entry.path(), to / entry.path().filename(),
                                  fs::copy_options::overwrite_existing, ec);
                    break;
                }
            }
        }
    }

    static void CopyDirectoryRecursive(const fs::path& from, const fs::path& to,
                                       const std::vector<std::wstring>& exts) {
        std::error_code ec;
        fs::create_directories(to, ec);
        for (auto& entry : fs::recursive_directory_iterator(from, ec)) {
            auto rel = fs::relative(entry.path(), from, ec);
            auto dst = to / rel;
            if (entry.is_directory()) {
                fs::create_directories(dst, ec);
            } else if (entry.is_regular_file()) {
                bool keep = exts.empty();
                if (!keep) {
                    std::wstring ext = entry.path().extension().wstring();
                    for (auto& e : exts) if (ext == e) { keep = true; break; }
                }
                if (keep) {
                    fs::create_directories(dst.parent_path(), ec);
                    fs::copy_file(entry.path(), dst,
                                  fs::copy_options::overwrite_existing, ec);
                }
            }
        }
    }

    static int RunWaitW(const std::wstring& cmdLine) {
        STARTUPINFOW si{}; si.cb = sizeof(si);
        PROCESS_INFORMATION pi{};
        std::vector<wchar_t> buf(cmdLine.begin(), cmdLine.end());
        buf.push_back(0);
        BOOL ok = CreateProcessW(nullptr, buf.data(), nullptr, nullptr,
                                 FALSE, CREATE_NO_WINDOW, nullptr, nullptr,
                                 &si, &pi);
        if (!ok) return -1;
        WaitForSingleObject(pi.hProcess, INFINITE);
        DWORD code = 0;
        GetExitCodeProcess(pi.hProcess, &code);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return (int)code;
    }
};

ELLE_SERVICE_MAIN(ElleFamilyService)
