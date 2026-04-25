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
                if ((unsigned char)c < 0x20)
                    out += "\\u00" + std::string(((unsigned char)c < 0x10) ? "0" : "") +
                           std::string(1, "0123456789abcdef"[(unsigned char)c]);
                else
                    out += c;
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
        EmitEvent(std::string(R"({"kind":"disconnected","reason":")") +
                  JsonEsc(why) + "\"}");
        SetState(State::DISCONNECTED);
        StopHeartbeat();
    });

    if (!m_conn.Connect(loginHost, loginPort)) {
        SetState(State::DISCONNECTED);
        return false;
    }
    /* Login server in most Fiesta builds starts in cleartext; the
     * server emits a session-key handshake packet which flips the
     * cipher on. The default Cipher state is enabled-but-zero-key,
     * which behaves as no-op until Reset() is called.                */
    m_conn.MutableCipher().SetEnabled(false);

    return SendLoginRequest(user, pass);
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
        o << R"({"kind":"login_state","state":")"
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
 * Outbound action methods
 *──────────────────────────────────────────────────────────────────────────────*/
bool Client::SendLoginRequest(const std::string& user, const std::string& pass) {
    Writer w;
    w.Str(user);
    w.Str(pass);
    SetState(State::LOGIN_AUTH);
    return m_conn.Send(Op::LOGIN_REQ, w.Data());
}

bool Client::SelectWorld(uint32_t worldId) {
    if (GetState() != State::WORLD_LIST) return false;
    Writer w; w.U32(worldId);
    return m_conn.Send(Op::WORLD_SELECT_REQ, w.Data());
}

bool Client::SelectChar(uint32_t charIndex) {
    if (GetState() != State::WORLD_CHAR_LIST) return false;
    Writer w; w.U32(charIndex);
    m_charId = charIndex;
    return m_conn.Send(Op::CHAR_SELECT_REQ, w.Data());
}

bool Client::Move(float x, float y, float z) {
    if (GetState() != State::IN_GAME) return false;
    Writer w; w.F32(x); w.F32(y); w.F32(z);
    return m_conn.Send(Op::MOVE_REQ, w.Data());
}

bool Client::Attack(uint32_t targetId) {
    if (GetState() != State::IN_GAME) return false;
    Writer w; w.U32(targetId);
    return m_conn.Send(Op::ATTACK_REQ, w.Data());
}

bool Client::Chat(const std::string& channel, const std::string& text) {
    if (GetState() != State::IN_GAME) return false;
    uint16_t op = Op::CHAT_NORMAL;
    if      (channel == "whisper") op = Op::CHAT_WHISPER;
    else if (channel == "party")   op = Op::CHAT_PARTY;
    else if (channel == "guild")   op = Op::CHAT_GUILD;
    else if (channel == "shout")   op = Op::CHAT_SHOUT;
    Writer w; w.Str(text);
    return m_conn.Send(op, w.Data());
}

bool Client::Pickup(uint32_t itemId) {
    if (GetState() != State::IN_GAME) return false;
    Writer w; w.U32(itemId);
    return m_conn.Send(Op::ITEM_PICKUP_REQ, w.Data());
}

bool Client::UseItem(uint32_t slot) {
    if (GetState() != State::IN_GAME) return false;
    Writer w; w.U32(slot);
    return m_conn.Send(Op::ITEM_USE_REQ, w.Data());
}

bool Client::Respawn() {
    Writer w;
    return m_conn.Send(Op::RESPAWN_REQ, w.Data());
}

bool Client::SendRaw(uint16_t opcode, const std::vector<uint8_t>& payload) {
    return m_conn.Send(opcode, payload);
}

/*──────────────────────────────────────────────────────────────────────────────
 * Inbound packet handler
 *──────────────────────────────────────────────────────────────────────────────*/
void Client::HandlePacket(const InPacket& pkt) {
    switch (pkt.opcode) {

        case Op::LOGIN_REPLY: {
            /* Layout (typical): u8 result, [u32 accountId], [key bytes]. */
            Reader r(pkt.payload.data(), pkt.payload.size());
            uint8_t result = 0; r.U8(result);
            if (result != 0) {
                EmitEvent(R"({"kind":"login_failed"})");
                Disconnect("login refused");
                return;
            }
            /* Many builds include a session key here that future
             * packets must be XOR'd against. Pull up to 32 bytes —
             * the Cipher reset zero-pads any shortfall.            */
            uint8_t key[32]{};
            const size_t keyLen =
                (r.Remaining() > 32) ? 32 : r.Remaining();
            if (keyLen > 0) {
                r.Bytes(key, keyLen);
                m_conn.MutableCipher().Reset(key, keyLen);
                m_conn.MutableCipher().SetEnabled(true);
            }
            SetState(State::WORLD_LIST);
            /* Ask for the world list immediately. */
            Writer w;
            m_conn.Send(Op::WORLD_LIST_REQ, w.Data());
            break;
        }

        case Op::WORLD_LIST_REPLY: {
            /* Surface the raw world list to Cognitive — the brain
             * picks one and sends back a SelectWorld() command.    */
            std::ostringstream o;
            o << R"({"kind":"world_list","payload_hex":")"
              << ToHex(pkt.payload) << "\"}";
            EmitEvent(o.str());
            break;
        }

        case Op::WORLD_SELECT_REPLY: {
            /* Layout (typical): u8 result, str host, u16 port. */
            Reader r(pkt.payload.data(), pkt.payload.size());
            uint8_t result = 0; r.U8(result);
            if (result != 0) {
                EmitEvent(R"({"kind":"world_select_failed"})");
                return;
            }
            std::string host; r.Str(host);
            uint16_t port = 0; r.U16(port);
            {
                std::lock_guard<std::mutex> lk(m_mx);
                m_worldHost = host;
                m_worldPort = port;
            }
            SetState(State::WORLD_HANDOFF);

            /* Tear down the login socket and open the world socket. */
            m_conn.Disconnect("world handoff");
            SetState(State::WORLD_CONNECTING);
            if (!m_conn.Connect(host, port)) {
                Disconnect("world connect failed");
                return;
            }
            /* Cipher resets per-session. */
            m_conn.MutableCipher().SetEnabled(false);

            /* Ask for character list. */
            Writer w;
            m_conn.Send(Op::CHAR_LIST_REQ, w.Data());
            SetState(State::WORLD_CHAR_LIST);
            break;
        }

        case Op::CHAR_LIST_REPLY: {
            std::ostringstream o;
            o << R"({"kind":"char_list","payload_hex":")"
              << ToHex(pkt.payload) << "\"}";
            EmitEvent(o.str());
            break;
        }

        case Op::CHAR_SELECT_REPLY: {
            Reader r(pkt.payload.data(), pkt.payload.size());
            uint8_t result = 0; r.U8(result);
            if (result == 0) {
                SetState(State::IN_GAME);
                StartHeartbeat();
            } else {
                EmitEvent(R"({"kind":"char_select_failed"})");
            }
            break;
        }

        case Op::CHAT_NORMAL:
        case Op::CHAT_SHOUT:
        case Op::CHAT_WHISPER:
        case Op::CHAT_PARTY:
        case Op::CHAT_GUILD: {
            const char* channel =
                (pkt.opcode == Op::CHAT_WHISPER) ? "whisper" :
                (pkt.opcode == Op::CHAT_PARTY)   ? "party"   :
                (pkt.opcode == Op::CHAT_GUILD)   ? "guild"   :
                (pkt.opcode == Op::CHAT_SHOUT)   ? "shout"   : "normal";
            Reader r(pkt.payload.data(), pkt.payload.size());
            std::string speaker, text;
            r.Str(speaker); r.Str(text);
            std::ostringstream o;
            o << R"({"kind":"chat","channel":")" << channel
              << R"(","speaker":")" << JsonEsc(speaker)
              << R"(","text":")" << JsonEsc(text) << "\"}";
            EmitEvent(o.str());
            break;
        }

        case Op::HP_UPDATE: {
            Reader r(pkt.payload.data(), pkt.payload.size());
            uint32_t entityId = 0; uint32_t cur = 0; uint32_t max = 0;
            r.U32(entityId); r.U32(cur); r.U32(max);
            float pct = (max > 0) ? ((float)cur / (float)max) : 0.f;
            std::ostringstream o;
            o << R"({"kind":"hp_changed","entity_id":)" << entityId
              << R"(,"cur":)" << cur << R"(,"max":)" << max
              << R"(,"pct":)" << pct << "}";
            EmitEvent(o.str());
            break;
        }

        case Op::DEATH_NOTIFY: {
            Reader r(pkt.payload.data(), pkt.payload.size());
            uint32_t entityId = 0; r.U32(entityId);
            std::ostringstream o;
            o << R"({"kind":"death","entity_id":)" << entityId << "}";
            EmitEvent(o.str());
            break;
        }

        case Op::ENTITY_SPAWN: {
            std::ostringstream o;
            o << R"({"kind":"entity_spawn","payload_hex":")"
              << ToHex(pkt.payload) << "\"}";
            EmitEvent(o.str());
            break;
        }

        case Op::HEARTBEAT_REPLY: {
            /* Quiet — heartbeat is admin noise, no event needed.    */
            break;
        }

        default: {
            /* Anything we don't have a high-level decoder for goes
             * out as a raw event. Cognitive can pattern-match.       */
            std::ostringstream o;
            o << R"({"kind":"raw","direction":"in","opcode":")"
              << "0x" << std::hex << pkt.opcode << std::dec
              << R"(","len":)" << pkt.payload.size()
              << R"(,"hex":")" << ToHex(pkt.payload) << "\"}";
            EmitEvent(o.str());
            break;
        }
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * Heartbeat loop
 *──────────────────────────────────────────────────────────────────────────────*/
void Client::StartHeartbeat() {
    StopHeartbeat();
    m_heartbeat = true;
    m_hbThread = std::thread([this] {
        while (m_heartbeat.load()) {
            for (int i = 0; i < 30 && m_heartbeat.load(); i++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            if (!m_heartbeat.load()) break;
            Writer w;
            m_conn.Send(Op::HEARTBEAT_REQ, w.Data());
        }
    });
}

void Client::StopHeartbeat() {
    m_heartbeat = false;
    if (m_hbThread.joinable() &&
        m_hbThread.get_id() != std::this_thread::get_id()) {
        m_hbThread.join();
    }
}

}  /* namespace Fiesta */
