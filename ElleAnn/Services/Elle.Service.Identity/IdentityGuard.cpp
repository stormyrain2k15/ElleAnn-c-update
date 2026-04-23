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

        /* Compute initial identity hash */
        if (!ComputeIdentityHash(m_originalHash)) {
            ELLE_WARN("No identity file found — creating initial identity");
            CreateIdentityFile();
            ComputeIdentityHash(m_originalHash);
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
        /* Periodic persist of authoritative state. Peers are live via
         * broadcast — this is durability only.                          */
        ElleIdentityCore::Instance().SaveToDatabase();
        std::string currentHash;
        if (ComputeIdentityHash(currentHash)) {
            if (currentHash != m_originalHash) {
                ELLE_FATAL("IDENTITY TAMPER DETECTED! Hash mismatch.");
                ELLE_FATAL("  Expected: %s", m_originalHash.c_str());
                ELLE_FATAL("  Got:      %s", currentHash.c_str());

                /* Alert all services */
                auto msg = ElleIPCMessage::Create(IPC_SHUTDOWN, SVC_IDENTITY, (ELLE_SERVICE_ID)0);
                msg.SetStringPayload("IDENTITY_TAMPER_DETECTED");
                msg.header.flags = ELLE_IPC_FLAG_BROADCAST | ELLE_IPC_FLAG_URGENT;
                GetIPCHub().Broadcast(msg);

                /* FAIL CLOSED LOCALLY — stop our own service loop. Previous
                 * behaviour only broadcast a shutdown notice; peers who
                 * ignored or missed the message continued to operate
                 * against a tampered identity file. By flipping our own
                 * Running() flag we guarantee at least THIS service stops,
                 * and the SCM dependency chain brings the others down
                 * behind us.                                              */
                Running().store(false);
            }
        } else {
            ELLE_ERROR("Identity file missing or unreadable!");
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
    uint32_t    m_checkInterval;
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
