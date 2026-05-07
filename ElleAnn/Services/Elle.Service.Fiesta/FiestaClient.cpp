/*══════════════════════════════════════════════════════════════════════════════
 * FiestaClient.cpp — implementation of the 3-hop ShineEngine login dance
 *                    and Zone-side action methods.
 *
 *   See FiestaClient.h for the state machine documentation.  Each
 *   `Send*` and `On*` helper here corresponds to ONE PDB-verified
 *   PROTO_NC_ struct, populated bit-exactly from the recovered
 *   `_re_artifacts/pdb/extracted/` layouts.
 *══════════════════════════════════════════════════════════════════════════════*/
#include "FiestaClient.h"
#include "FiestaConsoleTrace.h"

#include <cstdio>
#include <cstring>
#include <sstream>

namespace Fiesta {

/* ── helpers ──────────────────────────────────────────────────── */
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

/* Read u8-prefixed string (used by chat/shout). */
[[maybe_unused]] static std::string ReadStr8(Reader& r) {
    std::string s;
    r.Str8(s);
    return s;
}

/*──────────────────────────────────────────────────────────────────────────────
 * BRIEFINFO ring updates — handle⇆name maintenance
 *──────────────────────────────────────────────────────────────────────────────*/
void Client::OnLoginCharacter(const InPacket& pkt) {
    /* PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD head: handle:u16 + charid:char[16].
     * Trailing 203 bytes are equipment/coord/etc. — opaque to the ring. */
    if (pkt.payload.size() < sizeof(PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD_HEAD)) {
        return;
    }
    PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD_HEAD head{};
    std::memcpy(&head, pkt.payload.data(), sizeof(head));
    char nameBuf[17] = {0};
    std::memcpy(nameBuf, head.charid, 16);
    std::string name(nameBuf);
    while (!name.empty() && name.back() == 0) name.pop_back();
    m_briefRing.Insert(head.handle, name);
    m_world.UpsertPlayer(head.handle, name);

    std::ostringstream o;
    o << "{\"kind\":\"player_appear\",\"handle\":" << head.handle
      << ",\"name\":\"" << JsonEsc(name) << "\"}";
    EmitEvent(o.str());
}

void Client::OnBriefInfoDelete(const InPacket& pkt) {
    /* PROTO_NC_BRIEFINFO_BRIEFINFODELETE_CMD: hnd:u16 (sizeof=2). */
    if (pkt.payload.size() < 2) return;
    PROTO_NC_BRIEFINFO_BRIEFINFODELETE_CMD del{};
    std::memcpy(&del, pkt.payload.data(), sizeof(del));
    m_briefRing.Remove(del.hnd);
    m_world.RemoveEntity(del.hnd);

    std::ostringstream o;
    o << "{\"kind\":\"entity_disappear\",\"handle\":" << del.hnd << "}";
    EmitEvent(o.str());
}

void Client::OnRegenMob(const InPacket& pkt) {
    /* PROTO_NC_BRIEFINFO_REGENMOB_CMD head: handle:u16 + mob_id:u16.
     * Trailing bytes carry coord/mode/abstate, opaque to the ring. */
    if (pkt.payload.size() < sizeof(PROTO_NC_BRIEFINFO_REGENMOB_CMD_HEAD)) return;
    PROTO_NC_BRIEFINFO_REGENMOB_CMD_HEAD head{};
    std::memcpy(&head, pkt.payload.data(), sizeof(head));
    /* Mobs don't carry a display name in REGENMOB — we keep them out
     * of the player ring (they're handle-only) and surface as an
     * event for Cognitive's spatial model. */
    m_world.UpsertMob(head.handle, head.mob_id);
    std::ostringstream o;
    o << "{\"kind\":\"mob_appear\",\"handle\":" << head.handle
      << ",\"mob_id\":" << head.mob_id << "}";
    EmitEvent(o.str());
}

void Client::OnNpcDisappear(const InPacket& pkt) {
    if (pkt.payload.size() < 2) return;
    PROTO_NC_BRIEFINFO_NPC_DISAPPEAR_CMD del{};
    std::memcpy(&del, pkt.payload.data(), sizeof(del));
    m_briefRing.Remove(del.handle);
    m_world.RemoveEntity(del.handle);
    std::ostringstream o;
    o << "{\"kind\":\"npc_disappear\",\"handle\":" << del.handle << "}";
    EmitEvent(o.str());
}

void Client::OnBriefCharacter(const InPacket& pkt) {
    /* NC_BRIEFINFO_CHARACTER_CMD — equivalent to LOGINCHARACTER for
     * a player already in our zone (mid-session re-broadcast e.g.
     * after a region toggle).  Same head shape. */
    if (pkt.payload.size() < sizeof(PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD_HEAD)) return;
    PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD_HEAD head{};
    std::memcpy(&head, pkt.payload.data(), sizeof(head));
    char nameBuf[17] = {0};
    std::memcpy(nameBuf, head.charid, 16);
    std::string name(nameBuf);
    while (!name.empty() && name.back() == 0) name.pop_back();
    m_briefRing.Insert(head.handle, name);
    m_world.UpsertPlayer(head.handle, name);

    std::ostringstream o;
    o << "{\"kind\":\"player_update\",\"handle\":" << head.handle
      << ",\"name\":\"" << JsonEsc(name) << "\"}";
    EmitEvent(o.str());
}

void Client::OnPlayerListAppear(const InPacket& pkt) {
    /* NC_BRIEFINFO_PLAYER_LIST_INFO_APPEAR_CMD — bulk roster broadcast
     * sent by the zone on first enter. Layout per PDB-V70:
     *   [u8 count][ {u16 handle, char[16] charid} × count ].
     * We feed each into the ring so subsequent chats resolve.       */
    Reader r(pkt.payload.data(), pkt.payload.size());
    uint8_t count = 0;
    if (!r.U8(count)) return;
    int parsed = 0;
    for (uint8_t i = 0; i < count; i++) {
        uint16_t handle = 0;
        char     charid[16] = {0};
        if (!r.U16(handle)) break;
        if (!r.Bytes(charid, sizeof(charid))) break;
        std::string name(charid, sizeof(charid));
        while (!name.empty() && name.back() == 0) name.pop_back();
        if (!name.empty()) {
            m_briefRing.Insert(handle, name);
            m_world.UpsertPlayer(handle, name);
        }
        parsed++;
    }
    std::ostringstream o;
    o << "{\"kind\":\"player_list\",\"count\":" << parsed << "}";
    EmitEvent(o.str());
}

void Client::OnCharBase(const InPacket& pkt) {
    /* PROTO_NC_CHAR_BASE_CMD head: chrregnum:u32 + charid:char[16].
     * This is OUR OWN data; it doesn't carry our entity handle here
     * (handle is assigned later by zone broadcast).  We surface the
     * registration number so Cognitive can pin Elle's own identity. */
    if (pkt.payload.size() < sizeof(PROTO_NC_CHAR_BASE_CMD_HEAD)) return;
    PROTO_NC_CHAR_BASE_CMD_HEAD head{};
    std::memcpy(&head, pkt.payload.data(), sizeof(head));
    char nameBuf[17] = {0};
    std::memcpy(nameBuf, head.charid, 16);
    std::string name(nameBuf);
    while (!name.empty() && name.back() == 0) name.pop_back();

    m_world.UpdateSelfBase(head.chrregnum, name);

    std::ostringstream o;
    o << "{\"kind\":\"self_base\",\"chrregnum\":" << head.chrregnum
      << ",\"charid\":\"" << JsonEsc(name) << "\"}";
    EmitEvent(o.str());
}

/*──────────────────────────────────────────────────────────────────────────────
 * Chat dispatch — verified PROTO_NC_ACT_CHAT_REQ layout
 *──────────────────────────────────────────────────────────────────────────────*/
void Client::OnChatLike(const InPacket& pkt) {
    /* Verified PROTO_NC_ACT_CHAT_REQ wire layout (from Fiesta CLIENT
     * PDB type 0x159AC + fieldlist 0x159AB):
     *   [0] u8 itemLinkDataCount
     *   [1] u8 len
     *   [2] u8 content[len]
     *
     * 🟡 STUB_CHAT_BROADCAST_PARSE — sender identification for
     * server-broadcast chat is NOT in the CHAT_REQ payload itself.
     * The expected envelope is one of:
     *   (a) the zone wraps the frame in NETPACKETZONEHEADER (6B
     *       prefix `clienthandle:u16 + charregistnumber:u32`), then
     *       the CHAT_REQ payload follows; OR
     *   (b) the broadcast uses a different opcode whose payload
     *       carries `[u16 hnd][CHAT_REQ]`.
     * Both shapes are probed here; on PCAP confirmation this site
     * collapses to the verified shape. */
    const char* channel =
        (pkt.opcode == Op::NC_ACT_SHOUT_CMD) ? "shout" : "normal";

    uint16_t senderHandle = 0;
    std::string text;
    bool decoded = false;

    auto tryParseChatReq = [&](const uint8_t* p, size_t n) -> bool {
        if (n < sizeof(PROTO_NC_ACT_CHAT_REQ_HEAD)) return false;
        const uint8_t /*itemLinkDataCount = p[0],*/ len = p[1];
        if ((size_t)len > n - 2) return false;
        text.assign((const char*)(p + 2), len);
        return true;
    };

    /* Shape (a): NETPACKETZONEHEADER (6B) + CHAT_REQ. */
    if (pkt.payload.size() >= 6 + sizeof(PROTO_NC_ACT_CHAT_REQ_HEAD)) {
        const uint8_t* p = pkt.payload.data();
        const uint16_t handle = (uint16_t)(p[0] | (p[1] << 8));
        if (tryParseChatReq(p + 6, pkt.payload.size() - 6)) {
            senderHandle = handle;
            decoded = true;
        }
    }
    /* Shape (b): u16 sender + CHAT_REQ. */
    if (!decoded && pkt.payload.size() >= 2 + sizeof(PROTO_NC_ACT_CHAT_REQ_HEAD)) {
        const uint8_t* p = pkt.payload.data();
        const uint16_t handle = (uint16_t)(p[0] | (p[1] << 8));
        if (tryParseChatReq(p + 2, pkt.payload.size() - 2)) {
            senderHandle = handle;
            decoded = true;
        }
    }
    /* Shape (c): bare CHAT_REQ (our own echo, or client→server). */
    if (!decoded) {
        decoded = tryParseChatReq(pkt.payload.data(), pkt.payload.size());
    }

    /* Resolve speakerHandle → name via the BriefInfoRing. */
    const std::string speakerName = m_briefRing.Resolve(senderHandle);

    std::ostringstream o;
    o << "{\"kind\":\"chat\",\"channel\":\"" << channel
      << "\",\"speaker_handle\":" << senderHandle
      << ",\"speaker_name\":\"" << JsonEsc(speakerName) << "\""
      << ",\"text\":\"" << JsonEsc(text) << "\""
      << ",\"raw_hex\":\"" << ToHex(pkt.payload) << "\"}";
    EmitEvent(o.str());

    /* Console trace — high-level decoded view. */
    Fiesta::Trace::OnChat(speakerName.empty()
                              ? std::string_view{"?"}
                              : std::string_view{speakerName},
                          text, channel);
}

void Client::OnWhisper(const InPacket& pkt) {
    /* PROTO_NC_ACT_WHISPER_REQ in the S→C direction:
     *   [char[16] sender (NUL-pad)][u8 itemLinkDataCount][u8 len][text]
     * The 16-byte sender slot is the speaker's display name — we do
     * NOT need the BriefInfoRing for whispers (the name is right
     * there in the payload). This is canonical evidence that the
     * BriefInfoRing-based shape (a/b) above is purely a chat-channel
     * concern, not a whisper concern. */
    if (pkt.payload.size() < sizeof(PROTO_NC_ACT_WHISPER_REQ_HEAD)) return;
    PROTO_NC_ACT_WHISPER_REQ_HEAD head{};
    std::memcpy(&head, pkt.payload.data(), sizeof(head));

    char nameBuf[17] = {0};
    std::memcpy(nameBuf, head.handle, 16);
    std::string sender(nameBuf);
    while (!sender.empty() && sender.back() == 0) sender.pop_back();

    const size_t want = sizeof(head) + head.len;
    std::string text;
    if (pkt.payload.size() >= want) {
        text.assign((const char*)(pkt.payload.data() + sizeof(head)),
                    head.len);
    }

    std::ostringstream o;
    o << "{\"kind\":\"chat\",\"channel\":\"whisper_in\""
      << ",\"speaker_handle\":0"
      << ",\"speaker_name\":\"" << JsonEsc(sender) << "\""
      << ",\"text\":\"" << JsonEsc(text) << "\"}";
    EmitEvent(o.str());

    Fiesta::Trace::OnChat(sender, text, "whisper");
}

/*──────────────────────────────────────────────────────────────────────────────
 * Lifecycle
 *──────────────────────────────────────────────────────────────────────────────*/
void Client::WireConnectionCallbacks() {
    m_conn.SetOnPacket([this](const InPacket& pkt) {
        if (m_onRawPacket) m_onRawPacket(pkt, /*inbound=*/true);
        HandlePacket(pkt);
    });
    m_conn.SetOnDisconnect([this](const std::string& why) {
        EmitEvent(std::string("{\"kind\":\"disconnected\",\"reason\":\"") +
                  JsonEsc(why) + "\"}");
        SetState(State::DISCONNECTED);
    });
}

bool Client::Connect(const std::string& loginHost, uint16_t loginPort,
                     const std::string& user, const std::string& pass) {
    Disconnect("re-connect");

    {
        std::lock_guard<std::mutex> lk(m_mx);
        m_user = user;
        m_pass = pass;
    }

    WireConnectionCallbacks();
    /* Cipher is disabled until SEED_ACK arrives — first frame is
     * always cleartext.  Reset to "off" explicitly each hop. */
    m_conn.MutableCipher().SetEnabled(false);

    if (!m_conn.Connect(loginHost, loginPort)) {
        SetState(State::DISCONNECTED);
        return false;
    }
    SetState(State::LOGIN_CONNECTING);
    return true;
}

void Client::Disconnect(const std::string& why) {
    StopHeartbeat();
    m_conn.Disconnect(why);
    SetState(State::DISCONNECTED);
}

bool Client::ReconnectTo(const std::string& host, uint16_t port) {
    /* Hop transition: drop the old socket, key-reset cipher, open
     * the new socket.  Callbacks remain wired (m_conn is reused).   */
    m_conn.Disconnect("hop");
    m_conn.MutableCipher().SetEnabled(false);
    if (!m_conn.Connect(host, port)) return false;
    /* Re-wire onPacket/onDisconnect — Connection::Connect reused the
     * existing callback pointers internally, but the new RecvLoop
     * thread reads them fresh, so this is safe. */
    return true;
}

void Client::SetState(State s) {
    State previous;
    {
        std::lock_guard<std::mutex> lk(m_mx);
        previous = m_state;
        m_state  = s;
    }
    if (previous != s) {
        Fiesta::Trace::OnStateChange(StateName(previous), StateName(s));
        m_world.SetLoginState(StateName(s));
        std::ostringstream o;
        o << "{\"kind\":\"login_state\",\"state\":\"" << StateName(s) << "\"}";
        EmitEvent(o.str());
    }
}

void Client::EmitEvent(const std::string& kindJson) {
    if (m_onEvent) {
        GameEvent e; e.kindJson = kindJson; m_onEvent(e);
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * Hop 1 — Login server
 *──────────────────────────────────────────────────────────────────────────────*/
bool Client::SendVersionCheck() {
    PROTO_NC_USER_CLIENT_VERSION_CHECK_REQ req{};
    /* sVersionKey is a fixed 32-character UTF-16LE buffer.
     * Encode the configured ASCII key as UTF-16 (each char → u16). */
    std::string key;
    {
        std::lock_guard<std::mutex> lk(m_mx);
        key = m_versionKey;
    }
    const size_t maxChars = sizeof(req.sVersionKey) / sizeof(req.sVersionKey[0]);
    const size_t copy = (key.size() < maxChars) ? key.size() : maxChars;
    for (size_t i = 0; i < copy; i++) {
        req.sVersionKey[i] = (uint16_t)(unsigned char)key[i];
    }
    SetState(State::LOGIN_VERSION);
    return m_conn.Send(Op::NC_USER_CLIENT_VERSION_CHECK_REQ, ToBytes(req));
}

bool Client::SendLoginRequest() {
    PROTO_NC_USER_LOGIN_REQ req{};
    std::string user, pass;
    {
        std::lock_guard<std::mutex> lk(m_mx);
        user = m_user;
        pass = m_pass;
    }
    /* NUL-padded fixed-width ASCII (verified from PDB layout —
     * NOT a length-prefixed string). */
    const size_t userCopy = (user.size() < sizeof(req.user)) ? user.size() : sizeof(req.user);
    std::memcpy(req.user, user.data(), userCopy);
    const size_t passCopy = (pass.size() < sizeof(req.password)) ? pass.size() : sizeof(req.password);
    std::memcpy(req.password, pass.data(), passCopy);
    SetState(State::LOGIN_AUTH);
    return m_conn.Send(Op::NC_USER_LOGIN_REQ, ToBytes(req));
}

bool Client::SelectWorld(uint8_t worldNo) {
    if (GetState() != State::WORLD_LIST) return false;
    return SendWorldSelect(worldNo);
}

bool Client::SendWorldSelect(uint8_t worldNo) {
    PROTO_NC_USER_WORLDSELECT_REQ req{};
    req.worldno = worldNo;
    {
        std::lock_guard<std::mutex> lk(m_mx);
        m_selectedWorld = worldNo;
    }
    return m_conn.Send(Op::NC_USER_WORLDSELECT_REQ, ToBytes(req));
}

void Client::OnLoginAck(const InPacket& pkt) {
    /* PROTO_NC_USER_LOGIN_ACK = u8 numofworld + WorldInfo[N].
     * WorldInfo per PDB fieldlist 0x25B7 first member is `worldno:u8`.
     * Surface the world list as JSON for Cognitive; private-server
     * builds typically have a single world (worldno=0). */
    Reader r(pkt.payload.data(), pkt.payload.size());
    uint8_t numWorlds = 0;
    r.U8(numWorlds);

    std::ostringstream o;
    o << "{\"kind\":\"world_list\",\"count\":" << (int)numWorlds
      << ",\"worlds\":[";
    for (uint8_t i = 0; i < numWorlds; i++) {
        if (i > 0) o << ",";
        uint8_t worldno = 0;
        r.U8(worldno);
        /* WorldInfo full layout has more fields (status, name, etc.)
         * but the byte offsets vary by region toggle; we forward the
         * first reliable field and tag the rest as raw. */
        o << "{\"worldno\":" << (int)worldno << "}";
        /* Skip ahead by the WorldInfo struct stride.  WorldInfo is
         * 20 bytes per record per PDB type 0x25A0 sizeof comment.
         * We've consumed 1 byte (worldno); skip the remaining 19. */
        const size_t stride = 20 - 1;
        if (r.Remaining() >= stride) {
            uint8_t skip[19];
            r.Bytes(skip, stride);
        }
    }
    o << "]}";
    EmitEvent(o.str());

    SetState(State::WORLD_LIST);

    /* Auto-select world 0 when only one is available — matches the
     * default UX of every CN/JP/NA private server. */
    if (numWorlds == 1) {
        SendWorldSelect(0);
    }
}

void Client::OnWorldSelectAck(const InPacket& pkt) {
    if (pkt.payload.size() < sizeof(PROTO_NC_USER_WORLDSELECT_ACK)) {
        EmitEvent("{\"kind\":\"login_error\",\"reason\":\"worldselect_ack truncated\"}");
        Disconnect("worldselect_ack truncated");
        return;
    }
    std::memcpy(&m_wmHandoff, pkt.payload.data(), sizeof(m_wmHandoff));

    if (m_wmHandoff.worldstatus == 0) {
        std::ostringstream o;
        o << "{\"kind\":\"login_error\",\"reason\":\"world_down\",\"status\":"
          << (int)m_wmHandoff.worldstatus << "}";
        EmitEvent(o.str());
        Disconnect("world_down");
        return;
    }

    /* IP field is NUL-padded ASCIZ inside char[16]. */
    char ipbuf[17] = {0};
    std::memcpy(ipbuf, m_wmHandoff.ip, 16);
    std::string wmHost(ipbuf);
    /* Host is sent NUL-padded; trim trailing zeros. */
    while (!wmHost.empty() && wmHost.back() == 0) wmHost.pop_back();

    std::ostringstream o;
    o << "{\"kind\":\"wm_handoff\",\"ip\":\"" << JsonEsc(wmHost)
      << "\",\"port\":" << m_wmHandoff.port << "}";
    EmitEvent(o.str());

    SetState(State::WM_HANDOFF);
    if (!ReconnectTo(wmHost, m_wmHandoff.port)) {
        Disconnect("wm reconnect failed");
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * Hop 2 — WorldManager
 *──────────────────────────────────────────────────────────────────────────────*/
bool Client::SendWillLogin() {
    PROTO_NC_USER_WILLLOGIN_REQ req{};
    /* netpacketheader stays zero on the client→WM direction (CN2012). */
    /* userid: same string used in hop-1 LOGIN_REQ. */
    std::string user;
    {
        std::lock_guard<std::mutex> lk(m_mx);
        user = m_user;
    }
    const size_t userCopy = (user.size() < sizeof(req.userid)) ? user.size() : sizeof(req.userid);
    std::memcpy(req.userid, user.data(), userCopy);
    /* validate_new: echo verbatim from WORLDSELECT_ACK. */
    std::memcpy(req.validate_new, m_wmHandoff.validate_new, sizeof(req.validate_new));
    /* spawnapps: launcher fingerprint string. */
    std::string sa;
    {
        std::lock_guard<std::mutex> lk(m_mx);
        sa = m_spawnApps;
    }
    const size_t saCopy = (sa.size() < sizeof(req.spawnapps)) ? sa.size() : sizeof(req.spawnapps);
    std::memcpy(req.spawnapps, sa.data(), saCopy);

    SetState(State::WM_AUTH);
    return m_conn.Send(Op::NC_USER_WILLLOGIN_REQ, ToBytes(req));
}

void Client::OnWillLoginAck(const InPacket& pkt) {
    if (pkt.payload.size() < sizeof(PROTO_NC_USER_WILLLOGIN_ACK)) {
        EmitEvent("{\"kind\":\"login_error\",\"reason\":\"willlogin_ack truncated\"}");
        Disconnect("willlogin_ack truncated");
        return;
    }
    std::memcpy(&m_zoneHandoff, pkt.payload.data(), sizeof(m_zoneHandoff));

    if (m_zoneHandoff.status != 0) {
        std::ostringstream o;
        o << "{\"kind\":\"login_error\",\"reason\":\"wm_rejected\",\"status\":"
          << (int)m_zoneHandoff.status << "}";
        EmitEvent(o.str());
        Disconnect("wm_rejected");
        return;
    }

    char ipbuf[17] = {0};
    std::memcpy(ipbuf, m_zoneHandoff.zone_ip, 16);
    std::string zoneHost(ipbuf);
    while (!zoneHost.empty() && zoneHost.back() == 0) zoneHost.pop_back();

    std::ostringstream o;
    o << "{\"kind\":\"zone_handoff\",\"ip\":\"" << JsonEsc(zoneHost)
      << "\",\"port\":" << m_zoneHandoff.zone_port << "}";
    EmitEvent(o.str());

    SetState(State::ZONE_HANDOFF);
    if (!ReconnectTo(zoneHost, m_zoneHandoff.zone_port)) {
        Disconnect("zone reconnect failed");
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * Hop 3 — Zone
 *──────────────────────────────────────────────────────────────────────────────*/
bool Client::SendLoginWorld() {
    /* PROTO_NC_USER_LOGINWORLD_REQ payload shape was not recovered
     * from this build's PDB.  CN2012 zones accept the WILLLOGIN_REQ-
     * shaped struct verbatim — i.e. echo userid + validate_new. */
    PROTO_NC_USER_WILLLOGIN_REQ req{};
    std::string user;
    {
        std::lock_guard<std::mutex> lk(m_mx);
        user = m_user;
    }
    const size_t userCopy = (user.size() < sizeof(req.userid)) ? user.size() : sizeof(req.userid);
    std::memcpy(req.userid, user.data(), userCopy);
    std::memcpy(req.validate_new, m_zoneHandoff.validate_new, sizeof(req.validate_new));
    return m_conn.Send(Op::NC_USER_LOGINWORLD_REQ, ToBytes(req));
}

bool Client::SendMapLogin() {
    /* PROTO_NC_MAP_LOGIN_REQ = chardata (18B) + checksum Name8[120].
     * For a fresh headless session we have no character yet — the
     * Zone server validates checksum bytes only when the build is
     * compiled with CHECK_CLIENT_DATA, which CN2012 is NOT.  Send
     * an all-zero 978-byte body and let the server respond with
     * MAP_LOGINCOMPLETE_CMD. */
    std::vector<uint8_t> payload(978, 0);
    SetState(State::ZONE_AUTH);
    return m_conn.Send(Op::NC_MAP_LOGIN_REQ, payload);
}

void Client::OnMapLoginComplete(const InPacket& pkt) {
    /* MAP_LOGINCOMPLETE_CMD — server signals char is in-world.
     * Payload is 32-byte doorblock checksum which we don't verify. */
    (void)pkt;
    SetState(State::IN_GAME);
    /* Start the keepalive thread now that we're past the auth chain.
     * Idempotent — safe even if a re-Connect fires this twice. */
    StartHeartbeat();
    EmitEvent("{\"kind\":\"in_game\"}");
}

/*──────────────────────────────────────────────────────────────────────────────
 * Cipher key event — single dispatcher for SEED_ACK on all 3 hops
 *──────────────────────────────────────────────────────────────────────────────*/
void Client::OnSeedAck(const InPacket& pkt) {
    /* PROTO_NC_MISC_SEED_ACK = single u16 seed.  Anything longer is
     * a protocol violation we surface, not silently accept. */
    if (pkt.payload.size() < 2) {
        EmitEvent("{\"kind\":\"login_error\",\"reason\":\"seed_ack truncated\"}");
        Disconnect("seed_ack truncated");
        return;
    }
    const uint16_t seed = (uint16_t)(pkt.payload[0] | (pkt.payload[1] << 8));
    m_conn.MutableCipher().Reset(seed);

    std::ostringstream o;
    o << "{\"kind\":\"seed_received\",\"seed\":" << seed
      << ",\"hop\":\"" << StateName(GetState()) << "\"}";
    EmitEvent(o.str());

    /* Per-hop next step.  The state at SEED_ACK time tells us which
     * server we just connected to:
     *   LOGIN_CONNECTING → version-check then LOGIN_REQ
     *   WM_HANDOFF       → WILLLOGIN_REQ
     *   ZONE_HANDOFF     → LOGINWORLD + MAP_LOGIN
     */
    switch (GetState()) {
        case State::LOGIN_CONNECTING:
            SendVersionCheck();
            break;
        case State::WM_HANDOFF:
            SendWillLogin();
            break;
        case State::ZONE_HANDOFF:
            SendLoginWorld();
            SendMapLogin();
            break;
        default:
            /* Mid-stream re-seed?  Server-pushed re-key isn't documented
             * for CN2012; surface as a raw event for Cognitive triage. */
            break;
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * In-game actions — Zone-side opcodes (PDB-V70 verified layouts)
 *──────────────────────────────────────────────────────────────────────────────*/
bool Client::MoveTo(uint32_t toX, uint32_t toY, bool run) {
    if (GetState() != State::IN_GAME) return false;
    PROTO_NC_ACT_WALK_REQ req{};
    req.from = m_lastPos;
    req.to.x = toX;
    req.to.y = toY;
    m_lastPos = req.to;
    m_world.UpdateSelfPosition(toX, toY);
    return m_conn.Send(run ? Op::NC_ACT_RUN_REQ : Op::NC_ACT_WALK_REQ,
                       ToBytes(req));
}

bool Client::Stop(uint32_t atX, uint32_t atY) {
    if (GetState() != State::IN_GAME) return false;
    /* PROTO_NC_ACT_STOP_REQ payload: SHINE_XY_TYPE (8B). The server
     * uses this to authoritatively pin where we stopped — important
     * for bot-detection dampening on private servers that ban for
     * "teleporting" stop-points. */
    SHINE_XY_TYPE xy{ atX, atY };
    m_lastPos = xy;
    m_world.UpdateSelfPosition(atX, atY);
    return m_conn.Send(Op::NC_ACT_STOP_REQ, ToBytes(xy));
}

bool Client::Jump() {
    if (GetState() != State::IN_GAME) return false;
    /* NC_ACT_JUMP_CMD has no payload — the zone server records the
     * jump and broadcasts it. */
    return m_conn.Send(Op::NC_ACT_JUMP_CMD, {});
}

bool Client::NpcClick(uint16_t npcHandle) {
    if (GetState() != State::IN_GAME) return false;
    PROTO_NC_ACT_NPCCLICK_CMD req{};
    req.npchandle = npcHandle;
    return m_conn.Send(Op::NC_ACT_NPCCLICK_CMD, ToBytes(req));
}

/*── Combat ──────────────────────────────────────────────────────*/
bool Client::Target(uint16_t targetHandle) {
    if (GetState() != State::IN_GAME) return false;
    PROTO_NC_BAT_TARGETING_REQ r{}; r.target_handle = targetHandle;
    return m_conn.Send(Op::NC_BAT_TARGETING_REQ, ToBytes(r));
}

bool Client::Untarget(uint16_t targetHandle) {
    if (GetState() != State::IN_GAME) return false;
    PROTO_NC_BAT_TARGETING_REQ r{}; r.target_handle = targetHandle;
    return m_conn.Send(Op::NC_BAT_UNTARGET_REQ, ToBytes(r));
}

bool Client::Hit(uint16_t targetHandle) {
    if (GetState() != State::IN_GAME) return false;
    PROTO_NC_BAT_HIT_REQ r{}; r.target_handle = targetHandle;
    return m_conn.Send(Op::NC_BAT_HIT_REQ, ToBytes(r));
}

bool Client::Smash(uint16_t targetHandle) {
    if (GetState() != State::IN_GAME) return false;
    PROTO_NC_BAT_HIT_REQ r{}; r.target_handle = targetHandle;
    return m_conn.Send(Op::NC_BAT_SMASH_REQ, ToBytes(r));
}

bool Client::Attack(uint16_t targetHandle) {
    /* Canonical "engage in melee" sequence: Target then Hit. The
     * server validates that HIT_REQ.target matches the most-recent
     * TARGETING_REQ.target — issuing them back-to-back from the
     * same thread keeps that invariant trivially. */
    if (!Target(targetHandle)) return false;
    return Hit(targetHandle);
}

bool Client::SkillCast(uint16_t skillId, uint16_t targetHandle) {
    if (GetState() != State::IN_GAME) return false;
    PROTO_NC_BAT_SKILLCAST_REQ r{};
    r.skill_id      = skillId;
    r.target_handle = targetHandle;
    return m_conn.Send(Op::NC_BAT_SKILLCAST_REQ, ToBytes(r));
}

bool Client::SkillCastAbort() {
    if (GetState() != State::IN_GAME) return false;
    return m_conn.Send(Op::NC_BAT_SKILLCASTABORT_CMD, {});
}

bool Client::Assist(uint16_t partnerHandle) {
    if (GetState() != State::IN_GAME) return false;
    PROTO_NC_BAT_TARGETING_REQ r{}; r.target_handle = partnerHandle;
    return m_conn.Send(Op::NC_BAT_ASSIST_REQ, ToBytes(r));
}

/*── Social ──────────────────────────────────────────────────────*/
bool Client::Chat(const std::string& text) {
    if (GetState() != State::IN_GAME) return false;
    /* Verified PROTO_NC_ACT_CHAT_REQ outbound layout:
     *   [u8 itemLinkDataCount=0][u8 len][text bytes]
     * (No item-link blocks attached for plain text.) */
    Writer w;
    w.U8(0);
    const size_t n = (text.size() > 0xFF) ? 0xFF : text.size();
    w.U8((uint8_t)n);
    w.Bytes(text.data(), n);
    return m_conn.Send(Op::NC_ACT_CHAT_REQ, w.Data());
}

bool Client::Shout(const std::string& text) {
    if (GetState() != State::IN_GAME) return false;
    /* Same struct as CHAT_REQ — itemLinkDataCount + len + content. */
    Writer w;
    w.U8(0);
    const size_t n = (text.size() > 0xFF) ? 0xFF : text.size();
    w.U8((uint8_t)n);
    w.Bytes(text.data(), n);
    return m_conn.Send(Op::NC_ACT_SHOUT_CMD, w.Data());
}

bool Client::Whisper(const std::string& recipient, const std::string& text) {
    if (GetState() != State::IN_GAME) return false;
    /* PROTO_NC_ACT_WHISPER_REQ outbound layout:
     *   [char[16] recipient (NUL-pad)][u8 itemLinkDataCount=0]
     *   [u8 len][text bytes].
     * Recipient must be the canonical character display name (Name4).
     * Server resolves it to a handle internally. */
    Writer w;
    w.FixedStr(recipient, 16);
    w.U8(0);
    const size_t n = (text.size() > 0xFF) ? 0xFF : text.size();
    w.U8((uint8_t)n);
    w.Bytes(text.data(), n);
    /* Emit a local `whisper_out` event so Bonding/Cognitive can
     * record outbound whisper without waiting for server echo
     * (echoes are not guaranteed on private CN builds).            */
    {
        std::ostringstream o;
        o << "{\"kind\":\"chat\",\"channel\":\"whisper_out\""
          << ",\"speaker_handle\":" << m_selfHandle
          << ",\"speaker_name\":\"" << JsonEsc(recipient) << "\""
          << ",\"text\":\"" << JsonEsc(text) << "\"}";
        EmitEvent(o.str());
    }
    return m_conn.Send(Op::NC_ACT_WHISPER_REQ, w.Data());
}

bool Client::Emote(uint16_t emoteId) {
    if (GetState() != State::IN_GAME) return false;
    PROTO_NC_ACT_EMOTICON_CMD r{}; r.emote_id = emoteId;
    return m_conn.Send(Op::NC_ACT_EMOTICON_CMD, ToBytes(r));
}

/*── Inventory / lifecycle ──────────────────────────────────────*/
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
    Writer w;
    return m_conn.Send(Op::NC_CHAR_REVIVE_REQ, w.Data());
}

bool Client::Logout() {
    /* Polite logout: tell the server we're going (NORMALLOGOUT_CMD)
     * before yanking the socket. The CN2012 server treats a sudden
     * close as a crash and keeps your character "ghosted" in-zone
     * for ~30s — sending this CMD first releases the slot cleanly. */
    if (m_conn.IsConnected()) {
        m_conn.Send(Op::NC_USER_NORMALLOGOUT_CMD, {});
    }
    Disconnect("logout");
    return true;
}

bool Client::Heartbeat() {
    /* NC_MISC_HEARTBEAT_REQ has no payload on this build — the
     * server replies with NC_MISC_HEARTBEAT_ACK which we don't need
     * to act on (the round-trip is the keepalive). */
    return m_conn.Send(Op::NC_MISC_HEARTBEAT_REQ, {});
}

bool Client::SendRaw(uint16_t opcode, const std::vector<uint8_t>& payload) {
    return m_conn.Send(opcode, payload);
}

/*──────────────────────────────────────────────────────────────────────────────
 * Heartbeat thread — periodic NC_MISC_HEARTBEAT_REQ while IN_GAME.
 *
 *   The CN2012 zone disconnects clients with no traffic for ~60s.
 *   30s cadence keeps us safely under that watchdog without spamming.
 *──────────────────────────────────────────────────────────────────────────────*/
void Client::HeartbeatLoop() {
    using namespace std::chrono_literals;
    while (m_hbRunning.load()) {
        /* Sleep in 1s slices so Stop returns promptly. */
        for (int i = 0; i < 30 && m_hbRunning.load(); i++) {
            std::this_thread::sleep_for(1s);
        }
        if (!m_hbRunning.load()) break;
        if (GetState() == State::IN_GAME) Heartbeat();
    }
}

void Client::StartHeartbeat() {
    if (m_hbRunning.exchange(true)) return;   /* already running */
    m_hbThread = std::thread(&Client::HeartbeatLoop, this);
}

void Client::StopHeartbeat() {
    if (!m_hbRunning.exchange(false)) return;
    if (m_hbThread.joinable() &&
        m_hbThread.get_id() != std::this_thread::get_id()) {
        m_hbThread.join();
    }
}

/*──────────────────────────────────────────────────────────────────────────────
 * Inbound packet dispatcher
 *──────────────────────────────────────────────────────────────────────────────*/
void Client::HandlePacket(const InPacket& pkt) {
    /* Live console trace — visible only when the service is launched
     * with `--console`.  See FiestaConsoleTrace.h. */
    Fiesta::Trace::OnRx(pkt.opcode, pkt.payload);

    switch (pkt.opcode) {

        /* ── Cipher / handshake ──────────────────────────────── */
        case Op::NC_MISC_SEED_ACK:
        case Op::NC_MISC_SEED_REQ:    /* legacy alias on some builds */
            OnSeedAck(pkt);
            break;

        /* ── Hop-1 outcomes ──────────────────────────────────── */
        case Op::NC_USER_CLIENT_RIGHTVERSION_CHECK_ACK:
            /* Version OK — continue with credential auth. */
            SendLoginRequest();
            break;

        case Op::NC_USER_CLIENT_WRONGVERSION_CHECK_ACK:
            EmitEvent("{\"kind\":\"login_error\",\"reason\":\"version_mismatch\"}");
            Disconnect("version_mismatch");
            break;

        case Op::NC_USER_LOGIN_ACK:
            OnLoginAck(pkt);
            break;

        case Op::NC_USER_LOGINFAIL_ACK: {
            /* Payload byte 0 is the failure reason code — surface it
             * verbatim for Cognitive to learn the build's mapping. */
            uint8_t reason = pkt.payload.empty() ? 0xFF : pkt.payload[0];
            std::ostringstream o;
            o << "{\"kind\":\"login_error\",\"reason\":\"loginfail\",\"code\":"
              << (int)reason << "}";
            EmitEvent(o.str());
            Disconnect("loginfail");
            break;
        }

        case Op::NC_USER_WORLDSELECT_ACK:
            OnWorldSelectAck(pkt);
            break;

        /* ── Hop-2 outcome ───────────────────────────────────── */
        case Op::NC_USER_WILLLOGIN_ACK:
            OnWillLoginAck(pkt);
            break;

        /* ── Hop-3 outcome ───────────────────────────────────── */
        case Op::NC_MAP_LOGINCOMPLETE_CMD:
            OnMapLoginComplete(pkt);
            break;

        /* ── In-game broadcasts ──────────────────────────────── */
        case Op::NC_BRIEFINFO_INFORM_CMD: {
            std::ostringstream o;
            o << "{\"kind\":\"brief_info\",\"payload_hex\":\""
              << ToHex(pkt.payload) << "\"}";
            EmitEvent(o.str());
            break;
        }

        /* ── BRIEFINFO ring updates ─────────────────────────── */
        case Op::NC_BRIEFINFO_LOGINCHARACTER_CMD:
            OnLoginCharacter(pkt);
            break;
        case Op::NC_BRIEFINFO_BRIEFINFODELETE_CMD:
            OnBriefInfoDelete(pkt);
            break;
        case Op::NC_BRIEFINFO_REGENMOB_CMD:
            OnRegenMob(pkt);
            break;
        case Op::NC_BRIEFINFO_NPC_DISAPPEAR_CMD:
            OnNpcDisappear(pkt);
            break;
        case Op::NC_BRIEFINFO_CHARACTER_CMD:
            OnBriefCharacter(pkt);
            break;
        case Op::NC_BRIEFINFO_PLAYER_LIST_INFO_APPEAR_CMD:
            OnPlayerListAppear(pkt);
            break;
        case Op::NC_CHAR_BASE_CMD:
            OnCharBase(pkt);
            break;

        case Op::NC_ACT_CHAT_REQ:
        case Op::NC_ACT_SHOUT_CMD:
            OnChatLike(pkt);
            break;

        case Op::NC_ACT_WHISPER_REQ:
            OnWhisper(pkt);
            break;

        case Op::NC_MISC_HEARTBEAT_ACK:
            /* Server confirmed our heartbeat. No action needed; the
             * round-trip itself is what proves the connection is up. */
            break;

        default: {
            /* Unknown opcode: surface as raw for Cognitive to learn. */
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

}  /* namespace Fiesta */
