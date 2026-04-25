/*══════════════════════════════════════════════════════════════════════════════
 * FiestaClient.cpp — implementation of the high-level state machine.
 *
 *   Strategy:
 *     - Every action method builds an opcode payload, calls Connection::Send,
 *       and lets the inbound packet handler advance the state machine on
 *       the corresponding reply.
 *     - The packet handler uses an opcode switch with documented names
 *       (see FiestaPacket.h::Op). Anything we don't know the parser for
 *       yet is surfaced as a raw event (kind="raw") so Cognitive can
 *       observe it without us pretending we decoded it.
 *
 *   Payload structures below are the typical Fiesta layouts but the
 *   exact byte order / field set of any ONE server build can differ.
 *   Treat this as the framework; adapt one struct at a time as you
 *   capture real packets from the target server.
 *══════════════════════════════════════════════════════════════════════════════*/
#include "FiestaClient.h"

#include <cstdio>
#include <sstream>

namespace Fiesta {

/* JSON-escape a string for embedding in a quoted JSON value. */
static std::string JsonEsc(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default:
                if ((unsigned char)c < 0x20) {
                    /* Two-digit hex escape \u00HL where H = high nibble,
                     * L = low nibble. The hex lookup uses 4-bit masked
                     * indices so we never read past the 16-char table.   */
                    static const char hexdig[] = "0123456789abcdef";
                    const unsigned u = (unsigned char)c;
                    out += "\\u00";
                    out += hexdig[(u >> 4) & 0xF];
                    out += hexdig[u & 0xF];
                } else {
                    out += c;
                }
        }
    }
    return out;
}

static std::string ToHex(const std::vector<uint8_t>& bytes) {
    static const char* hex = "0123456789abcdef";
    std::string s;
    s.reserve(bytes.size() * 2);
    for (uint8_t b : bytes) {
        s += hex[b >> 4];
        s += hex[b & 0xF];
    }
    return s;
}

/*──────────────────────────────────────────────────────────────────────────────
 * Setup / lifecycle
 *──────────────────────────────────────────────────────────────────────────────*/
bool Client::Connect(const std::string& loginHost, uint16_t loginPort,
                     const std::string& user, const std::string& pass) {
    Disconnect("re-connect");
    {
        std::lock_guard<std::mutex> lk(m_mx);
        m_pendingUser = user;
        m_pendingPass = pass;
    }
    SetState(State::LOGIN_CONNECTING);

    m_conn.SetOnPacket([this](const InPacket& pkt) {
        if (m_onRawPacket) m_onRawPacket(pkt, /*inbound=*/true);
        HandlePacket(pkt);
    });
    m_conn.SetOnDisconnect([this](const std::string& why) {
        EmitEvent(std::string("{\"kind\":\"disconnected\",\"reason\":\"") +
                  JsonEsc(why) + "\"}");
        SetState(State::DISCONNECTED);
        StopHeartbeat();
    });

    if (!m_conn.Connect(loginHost, loginPort)) {
        SetState(State::DISCONNECTED);
        return false;
    }
    /* ShineEngine flow: TCP starts cleartext. The server pushes
     * NC_MISC_SEED_ACK first, carrying the 32-byte seed. Our handler
     * for that opcode flips the cipher on and then auto-fires
     * SendLoginRequest() with the credentials we cached above. So we
     * just park in SEED_WAIT here and wait for the seed to arrive. */
    m_conn.MutableCipher().SetEnabled(false);
    SetState(State::SEED_WAIT);
    return true;
}

void Client::Disconnect(const std::string& why) {
    StopHeartbeat();
    m_conn.Disconnect(why);
    SetState(State::DISCONNECTED);
}

void Client::SetState(State s) {
    State previous;
    {
        std::lock_guard<std::mutex> lk(m_mx);
        previous = m_state;
        m_state  = s;
    }
    if (previous != s) {
        std::ostringstream o;
        o << "{\"kind\":\"login_state\",\"state\":\""
          << StateName(s) << "\"}";
        EmitEvent(o.str());
    }
}

void Client::EmitEvent(const std::string& kindJson) {
    if (m_onEvent) {
        GameEvent e; e.kindJson = kindJson; m_onEvent(e);
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * Outbound action methods — ShineEngine NC_* opcodes.
 *
 *   Login flow per JHR_ServerInfo.txt (Mischief 6.11.2015 ports):
 *     hop #1 connect 9010 PG_Login  →  SEED  →  NC_MAP_LOGIN_REQ
 *                                        ↓
 *                              login_ack delivers WM IP+port
 *                                        ↓
 *     hop #2 connect 9110 PG_WM     →  SEED  →  char list, char select
 *                                        ↓
 *                                 WM delivers Zone IP+port
 *                                        ↓
 *     hop #3 connect 9120 PG_Zone   →  SEED  →  NC_MAP_LOGINCOMPLETE_CMD
 *                                        ↓
 *                                     IN_GAME
 *
 *   STATUS: only hop #1 is wired in this revision. The login_ack
 *   opcode that carries the WM hand-off info is not yet in our
 *   recovered handler set (it's a server-pushed CMD, not a client-
 *   sent REQ, so it didn't appear in the pft_Store sweep). One
 *   Wireshark capture of an original-client login will reveal the
 *   exact opcode and payload layout — at that point, the WM and
 *   Zone hops drop in by reusing m_conn.Connect() / SetEnabled(false)
 *   from this same Client object, in the spirit of the legacy
 *   World handoff that lived here in earlier revisions.
 *──────────────────────────────────────────────────────────────────────────────*/
bool Client::SendLoginRequest(const std::string& user, const std::string& pass) {
    /* NC_MAP_LOGIN_REQ payload (Mischief 6.11.2015 build):
     *   ASCII string  username   (length-prefixed via Writer::Str —
     *                              this is the `requestid` returned
     *                              by UserAuthentication.php's XML,
     *                              NOT the plaintext User field)
     *   ASCII string  password   (kept for protocol parity; many
     *                              builds ignore this field once the
     *                              HTTP-auth requestid validates)
     *   u32           protocol-version (0 by default; override via
     *                              fiesta.protocol_version if the WM
     *                              rejects with "version mismatch")
     *
     * The Mischief WM's "SERVER IS NO-APEX VERSION" log is an internal
     * guild-data load failure on the SERVER side — it never rejects
     * client packets based on that flag, so we don't need to set it
     * in the wire payload either way.                              */
    Writer w;
    w.Str(user);
    w.Str(pass);
    w.U32(m_protocolVersion);
    SetState(State::LOGIN_AUTH);
    return m_conn.Send(Op::NC_MAP_LOGIN_REQ, w.Data());
}

bool Client::SelectWorld(uint32_t /*worldId*/) {
    /* No-op on ShineEngine — world selection happens before TCP
     * connect (you connect to a specific zone server). Surface a
     * benign event so Cognitive knows the call was acknowledged.   */
    EmitEvent("{\"kind\":\"select_world_skipped\",\"reason\":\"shineengine single-zone\"}");
    return true;
}

bool Client::SelectChar(uint32_t charIndex) {
    /* Once the server has pushed all char data after login, the
     * client confirms readiness with NC_MAP_LOGINCOMPLETE_CMD. The
     * char_index is a server-assigned slot; we forward it as the
     * sole payload — most server builds only care that the CMD
     * arrived.                                                     */
    Writer w;
    w.U32(charIndex);
    m_charId = charIndex;
    return m_conn.Send(Op::NC_MAP_LOGINCOMPLETE_CMD, w.Data());
}

bool Client::Move(float x, float y, float z) {
    if (GetState() != State::IN_GAME) return false;
    /* NC_ACT_RUN_REQ is the canonical "I'm moving with run-speed"
     * client request. ShineEngine clients alternate between
     * NC_ACT_WALK_REQ (0x0803) and NC_ACT_RUN_REQ (0x0805) by
     * speed; for an autonomous agent run is the right default.    */
    Writer w; w.F32(x); w.F32(y); w.F32(z);
    return m_conn.Send(Op::NC_ACT_RUN_REQ, w.Data());
}

bool Client::Attack(uint32_t targetId) {
    if (GetState() != State::IN_GAME) return false;
    /* NC_BAT_HIT_REQ — single basic-attack request. The client is
     * expected to first NC_BAT_TARGETTING_REQ (0x0901) the target,
     * then issue HIT_REQs at attack-speed cadence. We collapse the
     * common case here and target+hit in one call.                */
    {
        Writer t; t.U32(targetId);
        m_conn.Send(Op::NC_BAT_TARGETTING_REQ, t.Data());
    }
    Writer w; w.U32(targetId);
    return m_conn.Send(Op::NC_BAT_HIT_REQ, w.Data());
}

bool Client::Chat(const std::string& channel, const std::string& text) {
    if (GetState() != State::IN_GAME) return false;
    /* Channel-to-opcode mapping for ShineEngine:
     *   normal/say  → NC_ACT_CHAT_REQ  (per-tile broadcast)
     *   shout       → NC_ACT_SHOUT_CMD (zone-wide)
     * Whisper/party/guild are routed through a different subsystem
     * (NC_CHAT_*) which is not in the ShinePlayer-handler set;
     * forward them as raw NC_ACT_CHAT_REQ until those are wired.   */
    uint16_t op = Op::NC_ACT_CHAT_REQ;
    if (channel == "shout") op = Op::NC_ACT_SHOUT_CMD;
    Writer w; w.Str(text);
    return m_conn.Send(op, w.Data());
}

bool Client::Pickup(uint32_t itemId) {
    if (GetState() != State::IN_GAME) return false;
    Writer w; w.U32(itemId);
    return m_conn.Send(Op::NC_ITEM_PICKUP_REQ, w.Data());
}

bool Client::UseItem(uint32_t slot) {
    if (GetState() != State::IN_GAME) return false;
    Writer w; w.U32(slot);
    return m_conn.Send(Op::NC_ITEM_USE_REQ, w.Data());
}

bool Client::Respawn() {
    /* ShineEngine has two distinct "I'm dead, send me back" flows:
     *   NC_CHAR_REVIVE_REQ        — full revive (graveyard / town)
     *   NC_SKILL_REPLYREVIVE_CMD  — accept a priest/ress offer
     * We default to the safe one (REVIVE_REQ); a future refinement
     * can decide based on whether a ress offer is pending.         */
    Writer w;
    return m_conn.Send(Op::NC_CHAR_REVIVE_REQ, w.Data());
}

bool Client::SendRaw(uint16_t opcode, const std::vector<uint8_t>& payload) {
    return m_conn.Send(opcode, payload);
}

/*──────────────────────────────────────────────────────────────────────────────
 * Inbound packet handler — ShineEngine NC_* opcodes.
 *──────────────────────────────────────────────────────────────────────────────*/
void Client::HandlePacket(const InPacket& pkt) {
    switch (pkt.opcode) {

        case Op::NC_MISC_SEED_REQ: {
            /* ShineEngine actually flips the convention on its head:
             * the SERVER pushes 0x0206 to the client immediately after
             * connect, carrying the seed bytes. The opcode name kept
             * its "_REQ" suffix from a long-defunct earlier flow.    */
            const size_t keyLen =
                (pkt.payload.size() > 32) ? 32 : pkt.payload.size();
            if (keyLen > 0) {
                m_conn.MutableCipher().Reset(pkt.payload.data(), keyLen);
                m_conn.MutableCipher().SetEnabled(true);
            }
            std::ostringstream o;
            o << "{\"kind\":\"seed_received\",\"len\":" << keyLen << "}";
            EmitEvent(o.str());

            /* Auto-progress: send the login request now that we can
             * actually encrypt it.                                   */
            if (GetState() == State::SEED_WAIT) {
                std::string u, p;
                {
                    std::lock_guard<std::mutex> lk(m_mx);
                    u = m_pendingUser; p = m_pendingPass;
                }
                SendLoginRequest(u, p);
            }
            break;
        }

        case Op::NC_MAP_LOGIN_REQ: {
            /* Server may echo this back with a result code on some
             * builds — surface as login_state for Cognitive.         */
            std::ostringstream o;
            o << "{\"kind\":\"login_state\",\"state\":\"acknowledged\"}";
            EmitEvent(o.str());
            break;
        }

        case Op::NC_BRIEFINFO_INFORM_CMD: {
            /* Server-pushed brief player info during the login
             * sequence. Forward as raw — Cognitive learns the layout
             * from repeated captures. */
            std::ostringstream o;
            o << "{\"kind\":\"brief_info\",\"payload_hex\":\""
              << ToHex(pkt.payload) << "\"}";
            EmitEvent(o.str());
            break;
        }

        case Op::NC_ACT_CHAT_REQ:
        case Op::NC_ACT_SHOUT_CMD: {
            const char* channel =
                (pkt.opcode == Op::NC_ACT_SHOUT_CMD) ? "shout" : "normal";
            Reader r(pkt.payload.data(), pkt.payload.size());
            std::string speaker, text;
            r.Str(speaker); r.Str(text);
            std::ostringstream o;
            o << "{\"kind\":\"chat\",\"channel\":\"" << channel
              << "\",\"speaker\":\"" << JsonEsc(speaker)
              << "\",\"text\":\"" << JsonEsc(text) << "\"}";
            EmitEvent(o.str());
            break;
        }

        default: {
            /* Anything we don't have a high-level decoder for goes
             * out as a raw event. Cognitive can pattern-match.       */
            std::ostringstream o;
            o << "{\"kind\":\"raw\",\"direction\":\"in\",\"opcode\":\""
              << "0x" << std::hex << pkt.opcode << std::dec
              << "\",\"len\":" << pkt.payload.size()
              << ",\"hex\":\"" << ToHex(pkt.payload) << "\"}";
            EmitEvent(o.str());
            break;
        }
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * Heartbeat loop — ShineEngine has no application-level keepalive.
 * The server uses TCP keepalive at the OS layer plus implicit
 * traffic; there is no NC_*_KEEPALIVE in the recovered handler set.
 * StartHeartbeat()/StopHeartbeat() stay as no-ops to keep the call
 * sites in Connect/Disconnect compiling cleanly.
 *──────────────────────────────────────────────────────────────────────────────*/
void Client::StartHeartbeat() {
    /* deliberately empty — see comment above */
}

void Client::StopHeartbeat() {
    m_heartbeat = false;
    if (m_hbThread.joinable() &&
        m_hbThread.get_id() != std::this_thread::get_id()) {
        m_hbThread.join();
    }
}

}  /* namespace Fiesta */
