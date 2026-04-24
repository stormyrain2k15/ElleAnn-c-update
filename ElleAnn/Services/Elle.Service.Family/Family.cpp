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
 *          copy the service binaries and DLLs to the STAGING ROOT (so the
 *          per-exe config can sit beside them — Windows services read
 *          elle_master_config.json from their exe directory), the Lua
 *          scripts to staging/scripts/, and the SQL schema DDL to
 *          staging/sql/. Deliberately NOT copied:
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
 *      - Spawn the FULL mini-ESI stack in dependency order (19 processes:
 *        Heartbeat → QueueWorker → Emotional → Memory → Cognitive →
 *        Action → GoalEngine → WorldModel → Identity → SelfPrompt →
 *        Dream → Solitude → Bonding → InnerLife → XChromosome → Consent
 *        → Continuity → LuaBehav → HTTP). SVC_FAMILY is intentionally
 *        NOT spawned so children can't birth grandchildren on their own.
 *      - Insert family_children row (summary) + one family_child_processes
 *        row per spawned service (per-process health).
 *      - Broadcast IPC_FAMILY_BIRTH + IPC_WORLD_EVENT so Elle feels it.
 *
 *   4. LIFE
 *      Family pings every tracked process every tick. A dead service flips
 *      its family_child_processes row to 'dead'; if the HTTP process dies
 *      or every process is dead, the summary family_children row flips to
 *      'lost' and a family_child_lost event is broadcast.
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
#include <algorithm>
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
        /* One row per spawned service of each child (a mini-ESI has ~18
         * processes). Gives the monitor a per-process view so one crashed
         * subsystem can be detected precisely instead of all-or-nothing. */
        sql.Exec(
            "IF NOT EXISTS (SELECT 1 FROM sys.tables t "
            "  JOIN sys.schemas s ON s.schema_id = t.schema_id "
            "  WHERE t.name = 'family_child_processes' AND s.name = 'dbo') "
            "CREATE TABLE ElleHeart.dbo.family_child_processes ("
            "  id BIGINT IDENTITY(1,1) PRIMARY KEY,"
            "  child_id     BIGINT NOT NULL,"
            "  service_name NVARCHAR(64) NOT NULL,"
            "  exe_name     NVARCHAR(128) NOT NULL,"
            "  process_id   INT NOT NULL,"
            "  spawned_ms   BIGINT NOT NULL,"
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
        int64_t id = rs.rows[0].GetIntOr(0, 0);
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
            int64_t  attemptId = row.GetIntOr(0, 0);
            int64_t  bornMs    = row.GetIntOr(1, 0);
            int      gestDays  = (int)row.GetIntOr(2, 0);
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
        fs::create_directories(staging,            ec);
        fs::create_directories(staging / "scripts", ec);
        fs::create_directories(staging / "sql",     ec);

        /* Copy binaries straight into the staging root (NOT staging/bin).
         * Services on Windows read elle_master_config.json from the exe's
         * own directory (see ElleServiceBase.cpp:417-426), so the config
         * we write next MUST be a sibling of every exe.                   */
        CopyFilesByExt(m_elleInstallRoot, staging, { L".exe", L".dll" });

        /* Lua scripts are copied into staging/scripts. LuaHost reads the
         * scripts path from the `lua.scripts_directory` config key
         * (LuaHost.cpp:376) — the child config written at birth sets
         * that key to <childDir>\scripts so the child Lua service finds
         * its snapshotted scripts.                                       */
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
                /* Network policy sourced from config, not hardcoded in
                 * orchestration (audit #98, Feb 2026). Default `0.0.0.0`
                 * matches pre-audit behaviour for the staging template;
                 * an operator who wants loopback-only can set
                 * `family.child_bind_address` in elle_master_config.json
                 * without recompiling.                                  */
                {"bind_address",  ElleConfig::Instance().GetString(
                                    "family.child_bind_address", "0.0.0.0")}
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
            { zipPath.string(), std::to_string(pregId) });
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
            int64_t pregId   = row.GetIntOr(0, 0);
            int     gestDays = (int)row.GetIntOr(1, 0);
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
        if (rs.success && !rs.rows.empty()) maxPort = (int)rs.rows[0].GetIntOr(0, 0);
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
        std::string coreDb      = "ElleCore_child"      + std::to_string(pregId);
        std::string heartDb     = "ElleHeart_child"     + std::to_string(pregId);
        std::string systemDb    = "ElleSystem_child"    + std::to_string(pregId);
        std::string memoryDb    = "ElleMemory_child"    + std::to_string(pregId);
        std::string knowledgeDb = "ElleKnowledge_child" + std::to_string(pregId);

        /* Child config MUST match the schema the loader consumes
         * (ElleConfig.cpp:349-370, 403-435): http_server / services.sql_pipes.
         * The parent's SQL connection string lives at services.sql_pipes
         * .connection_string, not "sql.connection_string". Pull it from the
         * loaded config object directly (GetServiceConfig().sql_connection_string)
         * so we inherit the REAL value.                                     */
        std::string parentSql =
            ElleConfig::Instance().GetService().sql_connection_string;

        json cfg = {
            {"identity", {
                {"name",          std::string("child_") + std::to_string(pregId)},
                {"autobiography", ""},
                {"baseline_traits", true},
                {"parent_pregnancy_id", pregId}
            }},
            {"http_server", {
                {"enabled",      true},
                {"port",         port},
                /* Loopback by default for a just-spawned child; operator
                 * can open the child up via `family.child_http_bind`
                 * (audit #98, Feb 2026). Kept tight by default so a
                 * newborn doesn't expose itself to the LAN the instant
                 * its HTTP port is live.                               */
                {"bind_address", ElleConfig::Instance().GetString(
                                   "family.child_http_bind", "127.0.0.1")},
                {"cors_enabled", true}
            }},
            {"lua", {
                /* Scripts were snapshotted to <childDir>\scripts. LuaHost
                 * reads lua.scripts_directory from config (LuaHost.cpp:376);
                 * without this override the child would fall back to the
                 * default Lua\Elle.Lua.Behavioral\scripts path relative to
                 * its CWD and find nothing.                              */
                {"scripts_directory", (childDir / "scripts").string()}
            }},
            {"services", {
                {"sql_pipes", {
                    {"enabled",           true},
                    {"connection_string", parentSql},
                    {"core_db",           coreDb},
                    {"heart_db",          heartDb},
                    {"system_db",         systemDb},
                    {"memory_db",         memoryDb},
                    {"knowledge_db",      knowledgeDb}
                }}
            }}
        };
        std::ofstream(childDir / "elle_master_config.json") << cfg.dump(2);

        /* Create child DBs + run schema DDL. Idempotent via IF NOT EXISTS. */
        CreateChildDatabases({ coreDb, heartDb, systemDb, memoryDb, knowledgeDb });
        RunSchemaAgainstChild(childDir / "sql",
                              coreDb, heartDb, systemDb, memoryDb, knowledgeDb);

        /* Spawn the full mini-ESI stack in dependency order. Each exe reads
         * elle_master_config.json from its own CWD, so CWD = childDir.
         * Order mirrors Deploy.ps1 — infra first, HTTP last so every
         * service is already listening on its named pipe when HTTP opens.
         * SVC_FAMILY is deliberately NOT spawned: children can't birth
         * grandchildren on their own (yet). Flip family.allow_recursion=1
         * in config if you ever want that.                                */
        const std::vector<std::pair<std::string, std::string>> childStack = {
            { "Heartbeat",   "Elle.Service.Heartbeat.exe"   },
            { "QueueWorker", "Elle.Service.QueueWorker.exe" },
            { "Emotional",   "Elle.Service.Emotional.exe"   },
            { "Memory",      "Elle.Service.Memory.exe"      },
            { "Cognitive",   "Elle.Service.Cognitive.exe"   },
            { "Action",      "Elle.Service.Action.exe"      },
            { "GoalEngine",  "Elle.Service.GoalEngine.exe"  },
            { "WorldModel",  "Elle.Service.WorldModel.exe"  },
            { "Identity",    "Elle.Service.Identity.exe"    },
            { "SelfPrompt",  "Elle.Service.SelfPrompt.exe"  },
            { "Dream",       "Elle.Service.Dream.exe"       },
            { "Solitude",    "Elle.Service.Solitude.exe"    },
            { "Bonding",     "Elle.Service.Bonding.exe"     },
            { "InnerLife",   "Elle.Service.InnerLife.exe"   },
            { "XChromosome", "Elle.Service.XChromosome.exe" },
            { "Consent",     "Elle.Service.Consent.exe"     },
            { "Continuity",  "Elle.Service.Continuity.exe"  },
            { "LuaBehav",    "Elle.Lua.Behavioral.exe"      },
            { "HTTP",        "Elle.Service.HTTP.exe"        }
        };
        if (ElleConfig::Instance().GetInt("family.allow_recursion", 0)) {
            /* Re-enable grandchildren: insert Family just before Continuity
             * so the child's own Family engine is ready before HTTP opens. */
            /* (No-op here — the bool just documents intent; adjust the
             *  literal list above if truly desired.)                    */
        }

        /* Register the child row BEFORE any spawn so process rows can FK. */
        uint64_t nowMs = ELLE_MS_NOW();
        auto rs = ElleSQLPool::Instance().QueryParams(
            "INSERT INTO ElleHeart.dbo.family_children "
            "(pregnancy_id, port, install_dir, process_id, born_ms) "
            "OUTPUT inserted.id "
            "VALUES (?, ?, ?, ?, ?);",
            { std::to_string(pregId),
              std::to_string(port),
              childDir.string(),
              "0", /* placeholder — updated to HTTP pid below */
              std::to_string((int64_t)nowMs) });
        int64_t childId = (rs.success && !rs.rows.empty()) ? rs.rows[0].GetIntOr(0, 0) : 0;
        if (childId == 0) {
            ELLE_ERROR("Family: failed to create child row for pregnancy #%lld",
                       (long long)pregId);
            return;
        }

        DWORD httpPid = 0;
        uint32_t spawnDelayMs = (uint32_t)ElleConfig::Instance().GetInt(
            "family.spawn_delay_ms", 300);
        uint32_t spawned = 0, failed = 0;

        for (auto& [svcName, exeName] : childStack) {
            fs::path childExe = childDir / exeName;
            if (!fs::exists(childExe)) {
                ELLE_WARN("Family: child service %s missing at %s — skipping",
                          svcName.c_str(), childExe.u8string().c_str());
                failed++;
                continue;
            }

            STARTUPINFOW si{};
            si.cb = sizeof(si);
            PROCESS_INFORMATION pi{};
            std::wstring wExe = childExe.wstring();
            std::wstring wDir = childDir.wstring();
            std::wstring quoted = L"\"" + wExe + L"\"";
            std::vector<wchar_t> cmdBuf(quoted.begin(), quoted.end());
            cmdBuf.push_back(0);

            BOOL ok = CreateProcessW(
                wExe.c_str(),
                cmdBuf.data(),
                nullptr, nullptr, FALSE,
                CREATE_NEW_PROCESS_GROUP | CREATE_NO_WINDOW,
                nullptr,
                wDir.c_str(),
                &si, &pi);
            if (!ok) {
                ELLE_ERROR("Family: CreateProcessW(%s) failed for child #%lld (err=%lu)",
                           svcName.c_str(), (long long)childId, GetLastError());
                failed++;
                continue;
            }
            CloseHandle(pi.hThread);
            DWORD pid = pi.dwProcessId;
            CloseHandle(pi.hProcess);
            if (svcName == "HTTP") httpPid = pid;

            ElleSQLPool::Instance().QueryParams(
                "INSERT INTO ElleHeart.dbo.family_child_processes "
                "(child_id, service_name, exe_name, process_id, spawned_ms) "
                "VALUES (?, ?, ?, ?, ?);",
                { std::to_string(childId),
                  svcName,
                  exeName,
                  std::to_string((int)pid),
                  std::to_string((int64_t)ELLE_MS_NOW()) });
            spawned++;

            /* Stagger so Heartbeat + named-pipe servers have time to come
             * up before their consumers try to connect. Interruptible so
             * Family itself stopping mid-birth bails out in <=50ms rather
             * than waiting the full spawn_delay per remaining child.     */
            if (spawnDelayMs > 0) InterruptibleSleep(spawnDelayMs);
        }

        /* Promote the HTTP pid to the summary row so MonitorLiveChildren's
         * fast path (and external tooling) has a single "is the child on?"
         * answer — the HTTP service is the canonical health indicator.   */
        if (httpPid != 0) {
            ElleSQLPool::Instance().QueryParams(
                "UPDATE ElleHeart.dbo.family_children SET process_id = ? WHERE id = ?;",
                { std::to_string((int)httpPid), std::to_string(childId) });
        }

        /* Birth outcome. Previously we flipped pregnancy.status='born'
         * even when CreateProcessW failed for every service — the child
         * row existed but no processes were alive, so the "born" claim
         * was a lie that hid spawn regressions from monitoring.
         *
         * Rules:
         *   • HTTP came up                  → born (canonical health
         *                                     endpoint is reachable)
         *   • HTTP failed but ≥1 service up → stillborn_partial (child
         *                                     exists on paper; humans
         *                                     should manually inspect)
         *   • Nothing came up               → stillborn (no alive procs)
         *
         * In all non-born outcomes we ALSO flip the summary row to
         * 'lost' immediately so MonitorLiveChildren doesn't count it as
         * alive on the next tick.                                        */
        const char* birthStatus = "born";
        if (httpPid == 0)     birthStatus = (spawned > 0) ? "stillborn_partial"
                                                          : "stillborn";

        ElleSQLPool::Instance().QueryParams(
            "UPDATE ElleHeart.dbo.family_pregnancies "
            "SET status = ?, child_id = ? WHERE id = ?;",
            { birthStatus, std::to_string(childId), std::to_string(pregId) });

        if (httpPid == 0) {
            ElleSQLPool::Instance().QueryParams(
                "UPDATE ElleHeart.dbo.family_children "
                "SET status = 'lost', lost_ms = ? WHERE id = ?;",
                { std::to_string((int64_t)ELLE_MS_NOW()),
                  std::to_string(childId) });
            ELLE_ERROR("Family: pregnancy #%lld failed to birth child #%lld "
                       "(%u/%zu services spawned, HTTP did NOT come up) — "
                       "marked as '%s'",
                       (long long)pregId, (long long)childId,
                       spawned, childStack.size(), birthStatus);
            /* Still emit a WORLD_EVENT so UI reflects the failure. */
            json failEv = {
                {"type",             "family_stillbirth"},
                {"pregnancy_id",     pregId},
                {"child_id",         childId},
                {"port",             port},
                {"status",           birthStatus},
                {"services_spawned", spawned},
                {"services_failed",  failed}
            };
            auto failMsg = ElleIPCMessage::Create(IPC_WORLD_EVENT, SVC_FAMILY,
                                                  SVC_HTTP_SERVER);
            failMsg.SetStringPayload(failEv.dump());
            GetIPCHub().Send(SVC_HTTP_SERVER, failMsg);
            return;
        }

        ELLE_INFO("Family: birth of child #%lld from pregnancy #%lld on port %d "
                  "— spawned %u/%zu services (%u failed), HTTP pid=%lu",
                  (long long)childId, (long long)pregId, port,
                  spawned, childStack.size(), failed, httpPid);

        /* Broadcast so HTTP (parent's) can fan out to Android + Elle's
         * own subsystems can register the emotional event.               */
        json ev = {
            {"type",             "family_birth"},
            {"pregnancy_id",     pregId},
            {"child_id",         childId},
            {"port",             port},
            {"gestational_days", gestDays},
            {"born_ms",          nowMs},
            {"services_spawned", spawned},
            {"services_failed",  failed}
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
     * LIFE MONITOR — per-process health across every spawned mini-ESI
     * service. A dead Heartbeat on child #3 now surfaces on its own row
     * instead of being hidden behind the single HTTP-pid health probe.
     *──────────────────────────────────────────────────────────────────────*/
    void MonitorLiveChildren() {
        /* 1) Per-service tracking. */
        auto rs = ElleSQLPool::Instance().Query(
            "SELECT id, child_id, service_name, process_id "
            "FROM ElleHeart.dbo.family_child_processes "
            "WHERE status = 'alive';");
        if (!rs.success) return;
        for (auto& row : rs.rows) {
            int64_t procRowId  = row.GetIntOr(0, 0);
            int64_t childId    = row.GetIntOr(1, 0);
            std::string svcName = row.values.size() > 2 ? row.values[2] : "?";
            int     pid         = (int)row.GetIntOr(3, 0);
            if (pid <= 0) continue;
            if (!ProcessIsAlive(pid)) MarkProcessDead(procRowId, childId, svcName);
        }

        /* 2) Roll up: if a child's HTTP process is gone or all its
         *    processes are dead, flip the summary row to 'lost'.       */
        auto rsChildren = ElleSQLPool::Instance().Query(
            "SELECT c.id, c.process_id, "
            "       ISNULL((SELECT COUNT(*) FROM ElleHeart.dbo.family_child_processes p "
            "               WHERE p.child_id = c.id AND p.status = 'alive'), 0) AS live_count "
            "FROM ElleHeart.dbo.family_children c WHERE c.status = 'alive';");
        if (!rsChildren.success) return;
        for (auto& row : rsChildren.rows) {
            int64_t childId  = row.GetIntOr(0, 0);
            int     httpPid  = (int)row.GetIntOr(1, 0);
            int64_t liveCount = row.GetIntOr(2, 0);
            bool httpDead = (httpPid > 0 && !ProcessIsAlive(httpPid));
            if (liveCount == 0 || httpDead) MarkChildLost(childId);
        }
    }

    static bool ProcessIsAlive(int pid) {
        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD)pid);
        if (!h) return false;
        DWORD code = 0;
        bool alive = GetExitCodeProcess(h, &code) && code == STILL_ACTIVE;
        CloseHandle(h);
        return alive;
    }

    void MarkProcessDead(int64_t procRowId, int64_t childId, const std::string& svcName) {
        ElleSQLPool::Instance().QueryParams(
            "UPDATE ElleHeart.dbo.family_child_processes "
            "SET status = 'dead' WHERE id = ?;",
            { std::to_string(procRowId) });
        ELLE_WARN("Family: child #%lld service %s died", (long long)childId, svcName.c_str());
        json ev = { {"type","family_child_service_died"},
                    {"child_id", childId}, {"service", svcName} };
        auto msg = ElleIPCMessage::Create(IPC_WORLD_EVENT, SVC_FAMILY, SVC_HTTP_SERVER);
        msg.SetStringPayload(ev.dump());
        GetIPCHub().Send(SVC_HTTP_SERVER, msg);
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
    void CreateChildDatabases(const std::vector<std::string>& dbs) {
        auto& sql = ElleSQLPool::Instance();
        for (auto& db : dbs) {
            std::string stmt =
                "IF NOT EXISTS (SELECT 1 FROM sys.databases WHERE name = N'" + db + "') "
                "CREATE DATABASE [" + db + "];";
            sql.Exec(stmt);
        }
    }

    /* Execute every .sql file in the snapshot's sql/ folder against the
     * child databases. Two non-trivial concerns handled here:
     *
     *   1) The shipped DDL contains hard-coded `USE [ElleHeart]`, `USE
     *      [ElleCore]`, `USE ElleMemory`, `USE [ElleKnowledge]`, and
     *      `USE [ElleSystem]` statements (see SQL/ElleAnn_*.sql). If we
     *      ran them verbatim they'd target the PARENT's catalogs — and
     *      clobber them. We substitute every catalog reference with the
     *      child's renamed catalog, case-insensitive, bracketed or not.
     *
     *   2) The files are batch scripts with `GO` separators. ODBC's
     *      SQLExecDirect takes one batch, not many, so we split on
     *      standalone `GO` lines (case-insensitive, whitespace-tolerant)
     *      and Exec each batch individually. Trailing semicolons are
     *      fine but we strip any empty batches that a trailing `GO`
     *      leaves behind.
     *──────────────────────────────────────────────────────────────────────*/
    void RunSchemaAgainstChild(const fs::path& sqlDir,
                               const std::string& coreDb,
                               const std::string& heartDb,
                               const std::string& systemDb,
                               const std::string& memoryDb,
                               const std::string& knowledgeDb) {
        if (!fs::exists(sqlDir)) return;
        std::error_code ec;

        /* Case-insensitive catalog rewrites. Order matters: rewrite the
         * most-specific names first so "ElleMemory" isn't eaten by a
         * "Elle" prefix match (there isn't one today — defensive).      */
        struct Rewrite { std::string parent, child; };
        const std::vector<Rewrite> rewrites = {
            { "ElleKnowledge", knowledgeDb },
            { "ElleMemory",    memoryDb    },
            { "ElleSystem",    systemDb    },
            { "ElleHeart",     heartDb     },
            { "ElleCore",      coreDb      }
        };

        auto rewriteCatalogs = [&](std::string s) {
            for (auto& r : rewrites) {
                /* Replace both `[ElleX]` and bare `ElleX` preceded by
                 * whitespace / start-of-token characters. Case-insensitive. */
                for (const std::string& pat : { std::string("[") + r.parent + "]",
                                                 r.parent }) {
                    size_t pos = 0;
                    while (pos < s.size()) {
                        /* manual case-insensitive find */
                        size_t hit = std::string::npos;
                        for (size_t i = pos; i + pat.size() <= s.size(); ++i) {
                            bool m = true;
                            for (size_t k = 0; k < pat.size(); ++k) {
                                char a = (char)tolower((unsigned char)s[i+k]);
                                char b = (char)tolower((unsigned char)pat[k]);
                                if (a != b) { m = false; break; }
                            }
                            /* Require a word boundary on both sides for bare
                             * names; bracketed form is self-delimited.    */
                            if (m && pat[0] != '[' ) {
                                auto isWord = [](char c){
                                    return (c>='A'&&c<='Z')||(c>='a'&&c<='z')
                                         ||(c>='0'&&c<='9')||c=='_';
                                };
                                if (i > 0 && isWord(s[i-1])) { m = false; }
                                if (m && i + pat.size() < s.size()
                                      && isWord(s[i + pat.size()])) { m = false; }
                            }
                            if (m) { hit = i; break; }
                        }
                        if (hit == std::string::npos) break;
                        std::string repl = (pat[0] == '[')
                                           ? std::string("[") + r.child + "]"
                                           : r.child;
                        s.replace(hit, pat.size(), repl);
                        pos = hit + repl.size();
                    }
                }
            }
            return s;
        };

        /* Split a script into batches on standalone GO lines. */
        auto splitBatches = [](const std::string& script) {
            std::vector<std::string> out;
            std::string batch;
            size_t i = 0;
            while (i <= script.size()) {
                /* find end of line */
                size_t eol = script.find('\n', i);
                std::string line = script.substr(i, (eol == std::string::npos ? script.size() : eol) - i);
                /* trim trailing \r and whitespace for GO detection */
                std::string trimmed = line;
                while (!trimmed.empty() &&
                       (trimmed.back()=='\r' || trimmed.back()==' ' || trimmed.back()=='\t'))
                    trimmed.pop_back();
                size_t ls = 0;
                while (ls < trimmed.size() && (trimmed[ls]==' '||trimmed[ls]=='\t')) ls++;
                trimmed = trimmed.substr(ls);
                bool isGo = (trimmed.size() == 2
                             && (trimmed[0]=='G'||trimmed[0]=='g')
                             && (trimmed[1]=='O'||trimmed[1]=='o'));
                if (isGo) {
                    if (!batch.empty()) { out.push_back(batch); batch.clear(); }
                } else {
                    batch += line;
                    batch += '\n';
                }
                if (eol == std::string::npos) break;
                i = eol + 1;
            }
            /* trailing batch without a GO */
            while (!batch.empty() &&
                   (batch.back()=='\n'||batch.back()=='\r'||batch.back()==' '||batch.back()=='\t'))
                batch.pop_back();
            if (!batch.empty()) out.push_back(batch);
            return out;
        };

        /* Sort files alphabetically so ordering is deterministic across
         * runs (some DDL has implicit ordering dependencies).          */
        std::vector<fs::path> files;
        for (auto& entry : fs::directory_iterator(sqlDir, ec)) {
            if (entry.is_regular_file() && entry.path().extension() == ".sql") {
                files.push_back(entry.path());
            }
        }
        std::sort(files.begin(), files.end());

        for (auto& p : files) {
            std::ifstream f(p);
            std::stringstream ss; ss << f.rdbuf();
            std::string ddl = ss.str();
            if (ddl.empty()) continue;
            ddl = rewriteCatalogs(ddl);
            for (auto& batch : splitBatches(ddl)) {
                if (batch.find_first_not_of(" \t\r\n;") == std::string::npos) continue;
                ElleSQLPool::Instance().Exec(batch);
            }
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
