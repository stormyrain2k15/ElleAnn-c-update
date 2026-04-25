/*══════════════════════════════════════════════════════════════════════════════
 * FiestaClient.h — High-level headless game client.
 *
 *   State machine:
 *       DISCONNECTED → LOGIN_CONNECTING → LOGIN_AUTH → WORLD_LIST
 *           → WORLD_HANDOFF → WORLD_CONNECTING → WORLD_CHAR_LIST
 *               → IN_GAME ↻
 *
 *   The client owns one [Connection] at a time. After WORLD_SELECT the
 *   login socket closes and a new socket opens against the world host
 *   the server returned. Cipher key is reset on each handoff.
 *
 *   Public surface:
 *     - Connect(host, port, user, pass)           start the chain
 *     - SelectWorld(id), SelectChar(idx)          progress the chain
 *     - Move(x,y,z), Attack(id), Chat(ch,text)    in-game actions
 *     - Pickup(id), UseItem(slot)
 *     - SetOnEvent(cb)                            high-level event sink
 *     - SetOnRawPacket(cb)                        raw debug sink
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
    LOGIN_CONNECTING,
    SEED_WAIT,        /* TCP open, waiting for server-pushed NC_MISC_SEED_ACK   */
    LOGIN_AUTH,       /* seed received, NC_MAP_LOGIN_REQ sent, awaiting char data */
    WORLD_LIST,       /* (legacy two-step protocols only — unused on ShineEngine) */
    WORLD_HANDOFF,
    WORLD_CONNECTING,
    WORLD_CHAR_LIST,
    IN_GAME,
};

inline const char* StateName(State s) {
    switch (s) {
        case State::DISCONNECTED:      return "disconnected";
        case State::LOGIN_CONNECTING:  return "login_connecting";
        case State::SEED_WAIT:         return "seed_wait";
        case State::LOGIN_AUTH:        return "login_auth";
        case State::WORLD_LIST:        return "world_list";
        case State::WORLD_HANDOFF:     return "world_handoff";
        case State::WORLD_CONNECTING:  return "world_connecting";
        case State::WORLD_CHAR_LIST:   return "world_char_list";
        case State::IN_GAME:           return "in_game";
    }
    return "?";
}

/** A high-level decoded event surfaced to subscribers as JSON.        */
struct GameEvent {
    std::string kindJson;   /* full JSON body, ready to ship over IPC */
};

class Client {
public:
    using EventCallback     = std::function<void(const GameEvent&)>;
    using RawPacketCallback = std::function<void(const InPacket&, bool inbound)>;

    Client() = default;
    ~Client() { Disconnect("destructor"); }

    /* ── Setup ─────────────────────────────────────────────────────  */
    void SetOnEvent(EventCallback cb)         { m_onEvent     = std::move(cb); }
    void SetOnRawPacket(RawPacketCallback cb) { m_onRawPacket = std::move(cb); }

    State GetState() const {
        std::lock_guard<std::mutex> lk(m_mx);
        return m_state;
    }

    /* ── Connection lifecycle ─────────────────────────────────────── */
    bool Connect(const std::string& loginHost, uint16_t loginPort,
                 const std::string& user, const std::string& pass);

    void Disconnect(const std::string& why);

    bool SelectWorld(uint32_t worldId);
    bool SelectChar(uint32_t charIndex);

    /* Override the u32 protocol-version field carried by
     * NC_MAP_LOGIN_REQ. Default 0; if your WM drops with a version
     * mismatch, capture the original client's login packet and set
     * this to the value seen on the wire.                            */
    void SetProtocolVersion(uint32_t v) { m_protocolVersion = v; }

    /* ── In-game actions ──────────────────────────────────────────── */
    bool Move(float x, float y, float z);
    bool Attack(uint32_t targetId);
    bool Chat(const std::string& channel, const std::string& text);
    bool Pickup(uint32_t itemId);
    bool UseItem(uint32_t slot);
    bool Respawn();

    /* Send a raw packet — escape hatch for protocol exploration.      */
    bool SendRaw(uint16_t opcode, const std::vector<uint8_t>& payload);

private:
    void HandlePacket(const InPacket& pkt);
    void EmitEvent(const std::string& kindJson);
    void SetState(State s);
    void StartHeartbeat();
    void StopHeartbeat();

    /* Helpers for the wire-format details we know about. */
    bool SendLoginRequest(const std::string& user, const std::string& pass);

    mutable std::mutex     m_mx;
    Connection             m_conn;
    State                  m_state = State::DISCONNECTED;
    std::string            m_pendingUser;
    std::string            m_pendingPass;
    std::string            m_worldHost;
    uint16_t               m_worldPort = 0;
    uint32_t               m_charId    = 0;
    uint32_t               m_protocolVersion = 0;
    std::atomic<bool>      m_heartbeat{false};
    std::thread            m_hbThread;
    EventCallback          m_onEvent;
    RawPacketCallback      m_onRawPacket;
};

}  /* namespace Fiesta */
#endif
