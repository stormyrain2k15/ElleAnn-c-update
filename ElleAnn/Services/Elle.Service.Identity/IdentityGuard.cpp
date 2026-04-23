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

        /* FNV-1a hash as hex string */
        uint32_t hash = Elle_IPC_Checksum((const uint8_t*)content.c_str(), (uint32_t)content.size());
        char hex[16];
        snprintf(hex, sizeof(hex), "%08X", hash);
        outHash = hex;
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

        uint32_t hash = Elle_IPC_Checksum((const uint8_t*)content.c_str(), (uint32_t)content.size());
        char hex[16];
        snprintf(hex, sizeof(hex), "%08X", hash);

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
