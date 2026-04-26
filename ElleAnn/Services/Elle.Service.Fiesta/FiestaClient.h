/*══════════════════════════════════════════════════════════════════════════════
 * FiestaClient.h — High-level headless game client for ShineEngine.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   3-HOP LOGIN STATE MACHINE (verified from PDB)
 *   ────────────────────────────────────────────────────────────────────
 *     DISCONNECTED
 *           │ Connect(host, port, user, pass)
 *           ▼
 *     LOGIN_CONNECTING  ── socket → Login (9010)
 *           │ recv NC_MISC_SEED_ACK  (cipher keyed)
 *           ▼
 *     LOGIN_VERSION     ── send NC_USER_CLIENT_VERSION_CHECK_REQ
 *           │ recv NC_USER_CLIENT_RIGHTVERSION_CHECK_ACK
 *           ▼
 *     LOGIN_AUTH        ── send NC_USER_LOGIN_REQ (272-byte struct)
 *           │ recv NC_USER_LOGIN_ACK   (world list)
 *           ▼
 *     WORLD_LIST        ── send NC_USER_WORLDSELECT_REQ (1B)
 *           │ recv NC_USER_WORLDSELECT_ACK (WM ip+port+token)
 *           ▼
 *     WM_HANDOFF        ── socket close, open → WM
 *           │ recv NC_MISC_SEED_ACK
 *           ▼
 *     WM_AUTH           ── send NC_USER_WILLLOGIN_REQ (347B w/ token)
 *           │ recv NC_USER_WILLLOGIN_ACK (Zone ip+port+token)
 *           ▼
 *     ZONE_HANDOFF      ── socket close, open → Zone
 *           │ recv NC_MISC_SEED_ACK
 *           ▼
 *     ZONE_AUTH         ── send NC_USER_LOGINWORLD_REQ + NC_MAP_LOGIN_REQ
 *           │ recv NC_MAP_LOGINCOMPLETE_CMD
 *           ▼
 *     IN_GAME ↻
 *
 *   The client owns one `Connection` at a time.  Between hops the
 *   socket closes, the cipher resets, and a new socket opens against
 *   the host:port returned by the previous server.
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once
#ifndef ELLE_FIESTA_CLIENT_H
#define ELLE_FIESTA_CLIENT_H

#include "FiestaConnection.h"
#include "FiestaPacket.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <string>
#include <thread>

namespace Fiesta {

enum class State {
    DISCONNECTED,
    LOGIN_CONNECTING,    /* TCP open to Login, waiting for SEED_ACK     */
    LOGIN_VERSION,       /* version handshake in flight                 */
    LOGIN_AUTH,          /* user/pass sent, waiting for LOGIN_ACK       */
    WORLD_LIST,          /* world list received, awaiting select        */
    WM_HANDOFF,          /* WORLDSELECT_ACK received, reconnecting      */
    WM_AUTH,             /* WM seed received, WILLLOGIN_REQ sent        */
    ZONE_HANDOFF,        /* WILLLOGIN_ACK received, reconnecting        */
    ZONE_AUTH,           /* Zone seed received, MAP_LOGIN_REQ sent      */
    IN_GAME,             /* MAP_LOGINCOMPLETE_CMD received              */
};

inline const char* StateName(State s) {
    switch (s) {
        case State::DISCONNECTED:      return "disconnected";
        case State::LOGIN_CONNECTING:  return "login_connecting";
        case State::LOGIN_VERSION:     return "login_version";
        case State::LOGIN_AUTH:        return "login_auth";
        case State::WORLD_LIST:        return "world_list";
        case State::WM_HANDOFF:        return "wm_handoff";
        case State::WM_AUTH:           return "wm_auth";
        case State::ZONE_HANDOFF:      return "zone_handoff";
        case State::ZONE_AUTH:         return "zone_auth";
        case State::IN_GAME:           return "in_game";
    }
    return "?";
}

struct GameEvent {
    std::string kindJson;   /* full JSON body, ready to ship over IPC */
};

class Client {
public:
    using EventCallback     = std::function<void(const GameEvent&)>;
    using RawPacketCallback = std::function<void(const InPacket&, bool inbound)>;

    Client() = default;
    ~Client() { Disconnect("destructor"); }

    /* ── Setup ──────────────────────────────────────────────────── */
    void SetOnEvent(EventCallback cb)         { m_onEvent     = std::move(cb); }
    void SetOnRawPacket(RawPacketCallback cb) { m_onRawPacket = std::move(cb); }

    State GetState() const {
        std::lock_guard<std::mutex> lk(m_mx);
        return m_state;
    }

    /* Override the wide-string version key sent in
     * NC_USER_CLIENT_VERSION_CHECK_REQ.  Default is the CN2012
     * key "SDO_FIESTA_NEW_VER_KEY"; private servers may publish
     * their own key string. */
    void SetVersionKey(const std::string& utf8Key) {
        std::lock_guard<std::mutex> lk(m_mx);
        m_versionKey = utf8Key;
    }

    /* Override the WILLLOGIN_REQ.spawnapps fingerprint string.
     * CN2012 accepts an empty string for headless clients. */
    void SetSpawnApps(const std::string& s) {
        std::lock_guard<std::mutex> lk(m_mx);
        m_spawnApps = s;
    }

    /* ── Connection lifecycle ──────────────────────────────────── */
    bool Connect(const std::string& loginHost, uint16_t loginPort,
                 const std::string& user, const std::string& pass);

    void Disconnect(const std::string& why);

    /** Pick a world from the list received in NC_USER_LOGIN_ACK.
     *  worldNo is the `worldno` field of the desired WorldInfo entry
     *  (typically 0 for single-world private servers). */
    bool SelectWorld(uint8_t worldNo);

    /* ── In-game actions ────────────────────────────────────────── */
    /** Walk / run to (x, y) in u32 fixed-point world coordinates.
     *  Pass `run=true` for NC_ACT_RUN_REQ, otherwise NC_ACT_WALK_REQ.
     *  `from` defaults to the last known position (set internally). */
    bool MoveTo(uint32_t toX, uint32_t toY, bool run = true);

    bool Attack(uint16_t targetHandle);
    bool Chat(const std::string& text);
    bool Shout(const std::string& text);
    bool Pickup(uint32_t itemId);
    bool UseItem(uint32_t slot);
    bool Respawn();

    /** Send a raw packet — escape hatch for protocol exploration. */
    bool SendRaw(uint16_t opcode, const std::vector<uint8_t>& payload);

private:
    void HandlePacket(const InPacket& pkt);
    void EmitEvent(const std::string& kindJson);
    void SetState(State s);
    void WireConnectionCallbacks();

    /* Each hop's outbound entry-point. */
    bool SendVersionCheck();
    bool SendLoginRequest();
    bool SendWorldSelect(uint8_t worldNo);
    bool SendWillLogin();
    bool SendLoginWorld();
    bool SendMapLogin();

    /* SEED_ACK keys cipher and dispatches the per-hop next-step. */
    void OnSeedAck(const InPacket& pkt);
    /* LOGIN_ACK contains worldinfo[]; WORLDSELECT_ACK has WM addr+token;
     * WILLLOGIN_ACK has Zone addr+token; MAP_LOGINCOMPLETE_CMD ends auth. */
    void OnLoginAck(const InPacket& pkt);
    void OnWorldSelectAck(const InPacket& pkt);
    void OnWillLoginAck(const InPacket& pkt);
    void OnMapLoginComplete(const InPacket& pkt);

    /* Reconnect to a new host:port (used between hops).  Drops the
     * current socket, resets cipher, opens new socket. */
    bool ReconnectTo(const std::string& host, uint16_t port);

    mutable std::mutex     m_mx;
    Connection             m_conn;
    State                  m_state = State::DISCONNECTED;

    /* Login credentials cached until first SEED_ACK arrives. */
    std::string            m_user;
    std::string            m_pass;

    /* Token + addresses passed between hops (verbatim from the server). */
    PROTO_NC_USER_WORLDSELECT_ACK  m_wmHandoff{};   /* hop 1 → hop 2 */
    PROTO_NC_USER_WILLLOGIN_ACK    m_zoneHandoff{}; /* hop 2 → hop 3 */

    /* Selected character / world. */
    uint8_t                m_selectedWorld = 0;

    /* Last known position for movement deltas. */
    SHINE_XY_TYPE          m_lastPos{0, 0};

    /* Region-toggle config. */
    std::string            m_versionKey = "SDO_FIESTA_NEW_VER_KEY";
    std::string            m_spawnApps;

    EventCallback          m_onEvent;
    RawPacketCallback      m_onRawPacket;
};

}  /* namespace Fiesta */
#endif
