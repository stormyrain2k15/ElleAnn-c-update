/*══════════════════════════════════════════════════════════════════════════════
 * FiestaService.cpp — SVC_FIESTA Windows Service entry point.
 *
 *   Wires the Fiesta::Client headless game client into Elle's IPC fabric.
 *
 *   Inbound (Cognitive → SVC_FIESTA, IPC_FIESTA_COMMAND):
 *     {"op":"login",       "host":"...","port":1234,
 *                          "username":"...","password":"..."}
 *     {"op":"select_world","world_id":1}
 *     {"op":"select_char", "char_index":0}
 *     {"op":"chat",        "channel":"normal","text":"hi"}
 *     {"op":"move",        "x":1.0,"y":2.0,"z":3.0}
 *     {"op":"attack",      "target_id":42}
 *     {"op":"pickup",      "item_id":17}
 *     {"op":"use_item",    "slot":3}
 *     {"op":"respawn"}
 *     {"op":"raw",         "opcode":"0x2010","hex":"0102..."}
 *     {"op":"disconnect",  "reason":"..."}
 *
 *   Outbound (SVC_FIESTA → subscribers, IPC_FIESTA_EVENT):
 *     - High-level decoded events surfaced by Fiesta::Client (chat, hp,
 *       login_state, world_list, chat, …).
 *     - Raw inbound packets ({"kind":"raw","direction":"in",...}) for
 *       Cognitive's pattern engine to learn from when we don't have a
 *       parser yet.
 *
 *   Subscribers:  SVC_COGNITIVE  (rule engine + LLM context)
 *                 SVC_HTTP_SERVER (websocket fan-out so the Android app
 *                                  can also see the game stream)
 *
 *   Auto-connect:
 *     If `fiesta.auto_login` is true in the config, OnStart kicks off
 *     the login chain immediately using fiesta.host / port / username
 *     / password. Otherwise the service waits idle until Cognitive
 *     sends an IPC_FIESTA_COMMAND `{"op":"login",...}`.
 *
 *   Reconnect:
 *     OnTick polls the client state — if we're DISCONNECTED while
 *     auto-login is enabled, reconnect with exponential backoff.
 *══════════════════════════════════════════════════════════════════════════════*/
/* CRITICAL: winsock2.h MUST be included before windows.h (which
 * ElleTypes.h pulls in). Without this ordering, MSVC emits
 * `C4005: macro redefinition` (which the project's /WX flag turns
 * into a hard error) because <windows.h> transitively includes the
 * legacy <winsock.h>. WIN32_LEAN_AND_MEAN is set in the same gate
 * and redundantly in FiestaConnection.h with an #ifndef guard so
 * a second consumer can't trip the same warning.                    */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>

#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleUserContinuity.h"
#include "../../Shared/json.hpp"

#include "FiestaClient.h"

#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <string>
#include <vector>

/*──────────────────────────────────────────────────────────────────────────────
 * Hex-string → byte-vector for {"op":"raw","hex":"..."}.
 *──────────────────────────────────────────────────────────────────────────────*/
static std::vector<uint8_t> HexToBytes(const std::string& hex) {
    std::vector<uint8_t> out;
    out.reserve(hex.size() / 2);
    auto nibble = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        int hi = nibble(hex[i]);
        int lo = nibble(hex[i + 1]);
        if (hi < 0 || lo < 0) { out.clear(); break; }
        out.push_back((uint8_t)((hi << 4) | lo));
    }
    return out;
}

/*──────────────────────────────────────────────────────────────────────────────
 * SERVICE
 *──────────────────────────────────────────────────────────────────────────────*/
class ElleFiestaService : public ElleServiceBase {
public:
    ElleFiestaService()
        : ElleServiceBase(SVC_FIESTA, "ElleFiesta",
                          "Elle-Ann Fiesta Game Client",
                          "Headless Fiesta-protocol game client. Connects "
                          "to a Fiesta-style MMO server and bridges the "
                          "session to Elle's cognitive engine over IPC.") {}

protected:
    bool OnStart() override {
        /* Pull connection settings from the master config. The actual
         * `fiesta.*` keys are populated by Deploy/Configs/fiesta.json
         * — see ElleConfig::Load. Defaults are deliberately empty so a
         * missing config does NOT silently connect somewhere wrong. */
        const auto& cfg = ElleConfig::Instance();
        m_loginHost  = cfg.GetString("fiesta.host", "");
        m_loginPort  = (uint16_t)cfg.GetInt("fiesta.port", 9010);
        m_username   = cfg.GetString("fiesta.username", "");
        m_password   = cfg.GetString("fiesta.password", "");
        m_autoLogin  = cfg.GetBool("fiesta.auto_login", false);
        m_nUserNo    = (int64_t)cfg.GetInt("fiesta.user_no", 0);

        m_client.SetProtocolVersion(
            (uint32_t)cfg.GetInt("fiesta.protocol_version", 0));

        m_client.SetOnEvent([this](const Fiesta::GameEvent& e) {
            BroadcastEvent(e.kindJson);
        });
        m_client.SetOnRawPacket([this](const Fiesta::InPacket& pkt,
                                       bool inbound) {
            /* Re-emit raw inbound packets as JSON so subscribers don't
             * have to know the binary format. The high-level decoder
             * has already published a "kind":"chat" / "hp_changed" /…
             * event for opcodes it understands; this stream is the
             * "everything else" debug feed. Outbound raw is reserved
             * for a future Connection-level hook.                    */
            if (!inbound) return;
            nlohmann::json j;
            j["kind"]      = "raw";
            j["direction"] = "in";
            j["opcode"]    = OpcodeHex(pkt.opcode);
            j["len"]       = (uint64_t)pkt.payload.size();
            j["hex"]       = BytesToHex(pkt.payload);
            BroadcastEvent(j.dump());
        });

        SetTickInterval(1000);  /* Reconnect-watchdog cadence. */

        if (m_autoLogin) {
            if (m_loginHost.empty() || m_username.empty()) {
                ELLE_WARN("Fiesta auto_login enabled but fiesta.host or "
                          "fiesta.username is empty — staying idle until "
                          "an IPC_FIESTA_COMMAND login arrives.");
            } else {
                ELLE_INFO("Fiesta auto_login → %s:%u as %s",
                          m_loginHost.c_str(), (unsigned)m_loginPort,
                          m_username.c_str());
                if (!m_client.Connect(m_loginHost, m_loginPort,
                                      m_username, m_password)) {
                    ELLE_WARN("Fiesta initial connect failed; tick loop "
                              "will retry with backoff.");
                }
            }
        }

        ELLE_INFO("Fiesta service started");
        return true;
    }

    void OnStop() override {
        m_client.Disconnect("service stop");
        ELLE_INFO("Fiesta service stopped");
    }

    void OnTick() override {
        /* Reconnect watchdog. Only auto-reconnect when:
         *   1. fiesta.auto_login is true, AND
         *   2. we have host + username, AND
         *   3. the client has fully fallen off the wire.
         * Skip if a connect attempt is already in flight (anything that
         * is NOT DISCONNECTED). Backoff uses a config-driven floor +
         * doubling, capped at fiesta.reconnect_max_ms.                 */
        if (!m_autoLogin) return;
        if (m_loginHost.empty() || m_username.empty()) return;

        if (m_client.GetState() != Fiesta::State::DISCONNECTED) {
            /* Healthy — collapse the backoff so the next disconnect
             * starts from the floor.                                  */
            m_backoffMs = 0;
            return;
        }

        const auto& cfg = ElleConfig::Instance();
        const uint64_t floorMs =
            (uint64_t)cfg.GetInt("fiesta.reconnect_min_ms", 5000);
        const uint64_t ceilMs =
            (uint64_t)cfg.GetInt("fiesta.reconnect_max_ms", 60000);

        const uint64_t now = ELLE_MS_NOW();
        if (m_backoffMs == 0) {
            m_backoffMs       = floorMs;
            m_nextAttemptMs   = now + m_backoffMs;
            ELLE_INFO("Fiesta disconnected — first reconnect in %llums",
                      (unsigned long long)m_backoffMs);
            return;
        }
        if (now < m_nextAttemptMs) return;

        ELLE_INFO("Fiesta reconnect attempt → %s:%u",
                  m_loginHost.c_str(), (unsigned)m_loginPort);
        if (!m_client.Connect(m_loginHost, m_loginPort,
                              m_username, m_password)) {
            m_backoffMs = std::min<uint64_t>(m_backoffMs * 2, ceilMs);
            m_nextAttemptMs = now + m_backoffMs;
            ELLE_WARN("Fiesta reconnect failed — next attempt in %llums",
                      (unsigned long long)m_backoffMs);
        }
    }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID /*sender*/) override {
        if (msg.header.msg_type != IPC_FIESTA_COMMAND) return;

        nlohmann::json j;
        try {
            j = nlohmann::json::parse(msg.GetStringPayload());
        } catch (const std::exception& e) {
            ELLE_WARN("Fiesta: bad IPC_FIESTA_COMMAND JSON: %s", e.what());
            return;
        }

        const std::string op = j.value("op", std::string(""));
        if (op == "login") {
            const std::string host = j.value("host",     m_loginHost);
            const uint16_t    port = (uint16_t)j.value("port", (int)m_loginPort);
            const std::string user = j.value("username", m_username);
            const std::string pass = j.value("password", m_password);
            ELLE_INFO("Fiesta IPC login → %s:%u as %s",
                      host.c_str(), (unsigned)port, user.c_str());
            m_loginHost = host; m_loginPort = port;
            m_username  = user; m_password  = pass;
            m_client.Connect(host, port, user, pass);
        } else if (op == "select_world") {
            m_client.SelectWorld((uint32_t)j.value("world_id", 0));
        } else if (op == "select_char") {
            m_client.SelectChar((uint32_t)j.value("char_index", 0));
        } else if (op == "chat") {
            m_client.Chat(j.value("channel", std::string("normal")),
                          j.value("text",    std::string("")));
        } else if (op == "move") {
            m_client.Move((float)j.value("x", 0.0),
                          (float)j.value("y", 0.0),
                          (float)j.value("z", 0.0));
        } else if (op == "attack") {
            m_client.Attack((uint32_t)j.value("target_id", 0));
        } else if (op == "pickup") {
            m_client.Pickup((uint32_t)j.value("item_id", 0));
        } else if (op == "use_item") {
            m_client.UseItem((uint32_t)j.value("slot", 0));
        } else if (op == "respawn") {
            m_client.Respawn();
        } else if (op == "raw") {
            const std::string opcStr = j.value("opcode", std::string(""));
            const uint16_t opcode =
                (uint16_t)std::strtoul(opcStr.c_str(), nullptr, 0);
            const auto bytes = HexToBytes(j.value("hex", std::string("")));
            m_client.SendRaw(opcode, bytes);
        } else if (op == "disconnect") {
            m_client.Disconnect(j.value("reason", std::string("ipc disconnect")));
        } else {
            ELLE_WARN("Fiesta: unknown IPC op '%s'", op.c_str());
        }
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        /* Heartbeat is the universal dependency. We talk TO Cognitive
         * and HTTP_SERVER but neither is a hard precondition for the
         * game socket starting — they can come up later and miss only
         * the events emitted before they connected.                   */
        return { SVC_HEARTBEAT };
    }

private:
    /*──────────────────────────────────────────────────────────────────
     * Helpers
     *──────────────────────────────────────────────────────────────────*/
    void BroadcastEvent(const std::string& jsonPayload) {
        /* Fan out IPC_FIESTA_EVENT to every subscriber that wants the
         * stream. Sending to a service that isn't connected is a no-op
         * inside ElleIPCHub::Send — safe and cheap.                    */
        const ELLE_SERVICE_ID subscribers[] = {
            SVC_COGNITIVE, SVC_HTTP_SERVER, SVC_BONDING
        };
        for (auto target : subscribers) {
            auto m = ElleIPCMessage::Create(IPC_FIESTA_EVENT,
                                            SVC_FIESTA, target);
            m.SetStringPayload(jsonPayload);
            GetIPCHub().Send(target, m);
        }

        /* Fork a side-effect into ElleCore.dbo.GameSessionState so a
         * service restart can resume gracefully. We only persist the
         * coarse "where am I, who am I, how alive am I" snapshot —
         * not every chat or hp tick, which would thrash the DB.       */
        if (m_nUserNo > 0) {
            try {
                auto j = nlohmann::json::parse(jsonPayload);
                const std::string kind = j.value("kind", "");
                if (kind == "char_selected") {
                    ElleDB::GameSessionStateRow row;
                    row.nUserNo         = m_nUserNo;
                    row.char_index      = j.value("char_index", -1);
                    row.char_name       = j.value("char_name",  std::string(""));
                    row.zone_id         = j.value("zone_id",    -1);
                    row.zone_name       = j.value("zone_name",  std::string(""));
                    row.last_hp         = j.value("hp",         0);
                    row.last_hp_max     = j.value("hp_max",     0);
                    row.last_session_ms = (uint64_t)ELLE_MS_NOW();
                    ElleDB::UpsertGameSession(row);
                } else if (kind == "disconnect") {
                    ElleDB::MarkGameSessionDisconnected(
                        m_nUserNo, j.value("reason", std::string("unknown")));
                }
            } catch (const std::exception& e) {
                /* Best-effort persistence; never crash the IPC
                 * broadcast on a malformed event. JSON parse errors
                 * are the only realistic source here.                */
                ELLE_DEBUG("FiestaService: GameSession persist skipped: %s",
                           e.what());
            }
        }
    }

    static std::string OpcodeHex(uint16_t op) {
        char buf[8];
        std::snprintf(buf, sizeof(buf), "0x%04x", (unsigned)op);
        return std::string(buf);
    }

    static std::string BytesToHex(const std::vector<uint8_t>& bytes) {
        static const char* hex = "0123456789abcdef";
        std::string s; s.reserve(bytes.size() * 2);
        for (uint8_t b : bytes) {
            s += hex[b >> 4];
            s += hex[b & 0xF];
        }
        return s;
    }

    Fiesta::Client m_client;

    std::string m_loginHost;
    uint16_t    m_loginPort = 0;
    std::string m_username;
    std::string m_password;
    bool        m_autoLogin = false;

    /* nUserNo bound to this Fiesta session (from fiesta.user_no in
     * config, or 0 = unbound). When > 0, FiestaService persists
     * GameSessionState rows keyed on it so service restart can
     * resume gracefully and Cognitive can reason about continuity. */
    int64_t     m_nUserNo   = 0;

    /* Reconnect backoff state — driven entirely from OnTick. */
    uint64_t m_backoffMs     = 0;
    uint64_t m_nextAttemptMs = 0;
};

ELLE_SERVICE_MAIN(ElleFiestaService)
