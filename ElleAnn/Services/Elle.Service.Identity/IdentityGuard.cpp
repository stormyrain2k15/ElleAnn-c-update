/*******************************************************************************
 * IdentityGuard.cpp — Identity Protection & Tamper Detection
 *
 * Monitors Elle's identity file, detects tampering, file changes.
 * Protects against unauthorized modification of core files.
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleIdentityCore.h"
#include "../../Shared/ElleSQLConn.h"
#include <fstream>
#include <sstream>
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

/*──────────────────────────────────────────────────────────────────────────────
 * SHA-256 via Windows CNG (bcrypt). Replaces the previous 32-bit FNV-1a
 * checksum, which:
 *   • had a trivial collision surface (~2^16 messages by birthday bound),
 *   • was transparently re-computable by an attacker (no secret key),
 *   • provided no cryptographic guarantee beyond "these bytes differ".
 * SHA-256 gives us collision resistance; pairing it with a signed manifest
 * (future — see SIGNED_MANIFEST below) gets us full integrity verification.
 *────────────────────────────────────────────────────────────────────────────*/
static bool Sha256Hex(const std::string& data, std::string& outHex) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0) != 0) return false;
    DWORD hashObjectLen = 0, cb = 0;
    if (BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH,
                          (PUCHAR)&hashObjectLen, sizeof(DWORD), &cb, 0) != 0) {
        BCryptCloseAlgorithmProvider(hAlg, 0); return false;
    }
    std::vector<UCHAR> hashObject(hashObjectLen);
    BCRYPT_HASH_HANDLE hHash = nullptr;
    if (BCryptCreateHash(hAlg, &hHash, hashObject.data(), hashObjectLen,
                         nullptr, 0, 0) != 0) {
        BCryptCloseAlgorithmProvider(hAlg, 0); return false;
    }
    if (!data.empty()) {
        BCryptHashData(hHash, (PUCHAR)data.data(), (ULONG)data.size(), 0);
    }
    UCHAR digest[32];
    NTSTATUS st = BCryptFinishHash(hHash, digest, sizeof(digest), 0);
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    if (st != 0) return false;
    char hex[65];
    for (int i = 0; i < 32; i++) snprintf(hex + i * 2, 3, "%02x", digest[i]);
    hex[64] = '\0';
    outHex = hex;
    return true;
}

class ElleIdentityService : public ElleServiceBase {
public:
    ElleIdentityService()
        : ElleServiceBase(SVC_IDENTITY, "ElleIdentity",
                          "Elle-Ann Identity Guard",
                          "Identity file monitoring and tamper detection") {}

protected:
    bool OnStart() override {
        /* Take ownership of the identity singleton for the whole system.
         * Every other process running ElleIdentityCore::Instance() is a
         * thin client that sends IPC_IDENTITY_MUTATE here and applies the
         * returning IPC_IDENTITY_DELTA broadcast to its local mirror.   */
        ElleIdentityCore::Instance().Initialize();
        ElleIdentityCore::Instance().BecomeAuthoritative();
        ELLE_INFO("Identity service is now AUTHORITATIVE (single-writer fabric)");

        auto identityPath = ElleConfig::Instance().GetString("security.identity_file_path",
                                                              "C:\\ElleAnn\\identity.sig");
        m_identityPath = identityPath;
        m_checkInterval = (uint32_t)ElleConfig::Instance().GetInt(
            "security.identity_check_interval_seconds", 60) * 1000;

        /* Ensure the DB-side trusted-source table exists. This is our
         * anti-self-reference substrate: the expected hash lives in
         * SQL Server, not in this process's own memory. An attacker
         * who modifies identity.sig must ALSO have SQL Server write
         * access under the same account to corrupt the reference —
         * which requires privileges strictly higher than "can
         * overwrite the identity file".                              */
        ElleSQLPool::Instance().Query(
            "IF NOT EXISTS (SELECT 1 FROM sys.tables t "
            "               JOIN sys.schemas s ON s.schema_id = t.schema_id "
            "               WHERE t.name = 'identity_integrity' AND s.name = 'dbo') "
            "CREATE TABLE ElleCore.dbo.identity_integrity ("
            "  path          NVARCHAR(512) NOT NULL PRIMARY KEY,"
            "  expected_hash NVARCHAR(128) NOT NULL,"
            "  algorithm     NVARCHAR(16)  NOT NULL,"
            "  seeded_ms     BIGINT        NOT NULL,"
            "  updated_ms    BIGINT        NOT NULL"
            ");");

        /* Compute current file hash */
        std::string currentHash;
        if (!ComputeIdentityHash(currentHash)) {
            ELLE_WARN("No identity file found — creating initial identity");
            CreateIdentityFile();
            if (!ComputeIdentityHash(currentHash)) {
                ELLE_ERROR("Identity file still unreadable after create — "
                           "refusing to start without a verifiable identity.");
                return false;
            }
        }

        /* Look up the authoritative expected-hash row. First run seeds
         * it; subsequent runs compare against it. Note the
         * m_originalHash we keep in-memory is a CACHE of the DB row,
         * not the source of truth — OnTick re-fetches on every check. */
        auto rs = ElleSQLPool::Instance().QueryParams(
            "SELECT expected_hash, algorithm FROM ElleCore.dbo.identity_integrity "
            "WHERE path = ?;",
            { m_identityPath });
        if (!rs.success) {
            ELLE_ERROR("Identity: integrity table read failed: %s — refusing start",
                       rs.error.c_str());
            return false;
        }
        if (rs.rows.empty()) {
            /* First-run seed: record the current hash as authoritative.
             * This is the only moment the in-memory hash becomes the
             * "trusted source", and it happens exactly once when the
             * user first installs the service.                         */
            uint64_t now = ELLE_MS_NOW();
            auto ir = ElleSQLPool::Instance().QueryParams(
                "INSERT INTO ElleCore.dbo.identity_integrity "
                "(path, expected_hash, algorithm, seeded_ms, updated_ms) "
                "VALUES (?, ?, 'sha256', ?, ?);",
                { m_identityPath, currentHash,
                  std::to_string((int64_t)now),
                  std::to_string((int64_t)now) });
            if (!ir.success) {
                ELLE_ERROR("Identity: failed to seed trusted hash: %s",
                           ir.error.c_str());
                return false;
            }
            ELLE_INFO("Identity: seeded authoritative SHA-256 hash for %s",
                      m_identityPath.c_str());
            m_originalHash = currentHash;
        } else {
            const std::string& expected = rs.rows[0].values[0];
            std::string algo = rs.rows[0].values.size() > 1
                                 ? rs.rows[0].values[1]
                                 : std::string("sha256");
            if (algo != "sha256") {
                ELLE_ERROR("Identity: trusted hash uses unsupported algorithm '%s'",
                           algo.c_str());
                return false;
            }
            if (expected != currentHash) {
                /* TAMPER DETECTED AT STARTUP — fail closed immediately;
                 * do not even enter ServiceLoop. */
                ELLE_FATAL("IDENTITY TAMPER AT STARTUP — hash mismatch vs DB.");
                ELLE_FATAL("  Expected (SQL): %s", expected.c_str());
                ELLE_FATAL("  Got (file):     %s", currentHash.c_str());
                return false;
            }
            m_originalHash = expected;
            ELLE_INFO("Identity: hash verified against authoritative SQL row");
        }

        /* Set up watched paths */
        m_watchedPaths.push_back(identityPath);
        m_watchedPaths.push_back(ElleConfig::Instance().GetConfigPath());

        SetTickInterval(m_checkInterval);
        ELLE_INFO("Identity guard started — monitoring %d paths", (int)m_watchedPaths.size());
        return true;
    }

    void OnStop() override {
        /* Final persist — captures any delta that arrived between the
         * last periodic save and now.                                   */
        ElleIdentityCore::Instance().SaveToDatabase();
        ELLE_INFO("Identity guard stopped");
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID /*sender*/) override {
        if (msg.header.msg_type == IPC_IDENTITY_MUTATE) {
            /* Apply authoritatively — ElleIdentityCore persists + broadcasts
             * the delta back out to every peer's mirror.                */
            ElleIdentityCore::Instance().ApplyMutate(msg.GetStringPayload());
        }
    }

    void OnTick() override {
        /* Periodic persist of authoritative state. */
        ElleIdentityCore::Instance().SaveToDatabase();

        /* Re-fetch the trusted hash from SQL every tick so an operator
         * who legitimately re-seeds the identity file (via an admin
         * tool that also updates identity_integrity) takes effect on
         * the next tick. DON'T use m_originalHash as the reference —
         * that would be a self-referential check, which is exactly the
         * audit finding we're fixing.                                    */
        std::string expected;
        {
            auto rs = ElleSQLPool::Instance().QueryParams(
                "SELECT expected_hash FROM ElleCore.dbo.identity_integrity "
                "WHERE path = ?;",
                { m_identityPath });
            if (!rs.success || rs.rows.empty()) {
                ELLE_ERROR("Identity: trusted-source row missing on tick — "
                           "failing closed.");
                Running().store(false);
                return;
            }
            expected = rs.rows[0].values[0];
        }
        m_originalHash = expected;    /* keep cache in sync for diagnostics */

        std::string currentHash;
        if (ComputeIdentityHash(currentHash)) {
            if (currentHash != expected) {
                ELLE_FATAL("IDENTITY TAMPER DETECTED! Hash mismatch.");
                ELLE_FATAL("  Expected (SQL): %s", expected.c_str());
                ELLE_FATAL("  Got (file):     %s", currentHash.c_str());

                /* Alert all services */
                auto msg = ElleIPCMessage::Create(IPC_SHUTDOWN, SVC_IDENTITY, (ELLE_SERVICE_ID)0);
                msg.SetStringPayload("IDENTITY_TAMPER_DETECTED");
                msg.header.flags = ELLE_IPC_FLAG_BROADCAST | ELLE_IPC_FLAG_URGENT;
                GetIPCHub().Broadcast(msg);

                /* Fail closed locally. */
                Running().store(false);
            }
        } else {
            ELLE_ERROR("Identity file missing or unreadable — failing closed.");
            Running().store(false);
        }

        /* Check config file for unexpected changes */
        CheckConfigIntegrity();
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT };
    }

private:
    std::string m_identityPath;
    std::string m_originalHash;
    std::string m_configHash;
    uint32_t    m_checkInterval = 60000;
    std::vector<std::string> m_watchedPaths;

    bool ComputeIdentityHash(std::string& outHash) {
        std::ifstream file(m_identityPath, std::ios::binary);
        if (!file.is_open()) return false;

        std::stringstream ss;
        ss << file.rdbuf();
        std::string content = ss.str();

        /* SHA-256 — see Sha256Hex() above for why this replaced the
         * 32-bit FNV-1a checksum. Tamper response escalates to LOCAL
         * shutdown (not just log + broadcast) so a modified identity
         * file stops the service instead of continuing to operate with
         * a compromised self-description.                               */
        if (!Sha256Hex(content, outHash)) {
            ELLE_ERROR("IdentityGuard: SHA-256 failed (CNG error)");
            return false;
        }
        return true;
    }

    void CreateIdentityFile() {
        std::ofstream file(m_identityPath, std::ios::binary);
        if (file.is_open()) {
            file << "ELLE-ANN ESI v" << ELLE_VERSION_STRING << "\n"
                 << "GUID: " << ELLE_IDENTITY_GUID << "\n"
                 << "Created: " << ELLE_MS_NOW() << "\n"
                 << "Identity: " << ELLE_IDENTITY_NAME << "\n"
                 << "Signature: AUTHENTIC\n";
        }
    }

    void CheckConfigIntegrity() {
        auto& configPath = ElleConfig::Instance().GetConfigPath();
        if (configPath.empty()) return;

        std::ifstream file(configPath, std::ios::binary);
        if (!file.is_open()) return;

        std::stringstream ss;
        ss << file.rdbuf();
        std::string content = ss.str();

        std::string hex;
        if (!Sha256Hex(content, hex)) {
            ELLE_WARN("IdentityGuard: config SHA-256 failed");
            return;
        }

        if (m_configHash.empty()) {
            m_configHash = hex;
        } else if (m_configHash != hex) {
            ELLE_INFO("Config file changed — triggering reload");
            m_configHash = hex;
            ElleConfig::Instance().Reload();
        }
    }
};

ELLE_SERVICE_MAIN(ElleIdentityService)
