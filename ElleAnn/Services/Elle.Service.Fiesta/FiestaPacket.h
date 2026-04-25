/*══════════════════════════════════════════════════════════════════════════════
 * FiestaPacket.h — Packet structure and opcode registry.
 *
 *   Wire format (consistent across the login + world stages):
 *
 *       [u8 sizeFlag]              0x00..0xFE → length is 1 byte (sizeFlag)
 *                                  0xFF       → length is the next u16 LE
 *       [u16/u8  payloadLen]       (omitted when sizeFlag < 0xFF)
 *       [u16 LE  opcode]           Opcode dispatched to handlers
 *       [bytes]  payload           Opcode-specific body
 *
 *   This Variable-length-prefix framing is the conceptual layout used
 *   by every FiestaPS-derived server; the bytes are interpreted
 *   identically by login + world. Payload (and opcode) are XOR-stream
 *   encrypted by [FiestaCipher] AFTER the size prefix is read.
 *
 *   Opcode hex values below are placeholders pulled from the public
 *   private-server documentation and should be confirmed against the
 *   target server build before relying on them — they are stable
 *   across most JFOL/CN revisions but NOT guaranteed.
 *
 *   This file is a clean original write of the framing/opcode shape;
 *   it does not embed third-party server source.
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once
#ifndef ELLE_FIESTA_PACKET_H
#define ELLE_FIESTA_PACKET_H

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace Fiesta {

/*──────────────────────────────────────────────────────────────────────────────
 * Opcode registry. Names are MAJOR_MINOR in the Fiesta convention:
 *   MAJOR is the system (CHAT, MOVE, ITEM…), MINOR is the operation.
 *
 * Server build differences MAY swap a few of these. When a mismatch is
 * detected, override the value in FiestaClient before connecting.
 *──────────────────────────────────────────────────────────────────────────────*/
namespace Op {

    /* Authentication / login server */
    constexpr uint16_t LOGIN_REQ            = 0x0F01;
    constexpr uint16_t LOGIN_REPLY          = 0x0F02;
    constexpr uint16_t WORLD_LIST_REQ       = 0x0F03;
    constexpr uint16_t WORLD_LIST_REPLY     = 0x0F04;
    constexpr uint16_t WORLD_SELECT_REQ     = 0x0F05;
    constexpr uint16_t WORLD_SELECT_REPLY   = 0x0F06;

    /* World server / character */
    constexpr uint16_t CHAR_LIST_REQ        = 0x1010;
    constexpr uint16_t CHAR_LIST_REPLY      = 0x1011;
    constexpr uint16_t CHAR_SELECT_REQ      = 0x1012;
    constexpr uint16_t CHAR_SELECT_REPLY    = 0x1013;

    /* Game heartbeat / keepalive */
    constexpr uint16_t HEARTBEAT_REQ        = 0x2001;
    constexpr uint16_t HEARTBEAT_REPLY      = 0x2002;

    /* Movement */
    constexpr uint16_t MOVE_REQ             = 0x2010;
    constexpr uint16_t MOVE_BROADCAST       = 0x2011;
    constexpr uint16_t TELEPORT_NOTIFY      = 0x2012;

    /* Combat */
    constexpr uint16_t ATTACK_REQ           = 0x2110;
    constexpr uint16_t ATTACK_RESULT        = 0x2111;
    constexpr uint16_t HP_UPDATE            = 0x2112;
    constexpr uint16_t MP_UPDATE            = 0x2113;
    constexpr uint16_t DEATH_NOTIFY         = 0x2114;
    constexpr uint16_t RESPAWN_REQ          = 0x2115;

    /* Chat */
    constexpr uint16_t CHAT_NORMAL          = 0x2210;
    constexpr uint16_t CHAT_WHISPER         = 0x2211;
    constexpr uint16_t CHAT_PARTY           = 0x2212;
    constexpr uint16_t CHAT_GUILD           = 0x2213;
    constexpr uint16_t CHAT_SHOUT           = 0x2214;

    /* Inventory */
    constexpr uint16_t ITEM_PICKUP_REQ      = 0x2310;
    constexpr uint16_t ITEM_PICKUP_REPLY    = 0x2311;
    constexpr uint16_t ITEM_USE_REQ         = 0x2312;
    constexpr uint16_t ITEM_USE_REPLY       = 0x2313;
    constexpr uint16_t ITEM_DROP_REQ        = 0x2314;
    constexpr uint16_t INVENTORY_SNAPSHOT   = 0x2315;

    /* Entity (player / NPC / monster) lifecycle */
    constexpr uint16_t ENTITY_SPAWN         = 0x2410;
    constexpr uint16_t ENTITY_DESPAWN       = 0x2411;
    constexpr uint16_t ENTITY_UPDATE        = 0x2412;

}  /* namespace Op */

/*──────────────────────────────────────────────────────────────────────────────
 * Buffer reader/writer helpers for little-endian primitives.
 *──────────────────────────────────────────────────────────────────────────────*/
class Writer {
public:
    void U8(uint8_t v)   { m_buf.push_back(v); }
    void U16(uint16_t v) { m_buf.push_back((uint8_t)(v & 0xFF)); m_buf.push_back((uint8_t)(v >> 8)); }
    void U32(uint32_t v) {
        for (int i = 0; i < 4; i++) m_buf.push_back((uint8_t)((v >> (i * 8)) & 0xFF));
    }
    void F32(float v) {
        uint32_t x; std::memcpy(&x, &v, 4); U32(x);
    }
    void Bytes(const void* p, size_t n) {
        const auto* b = static_cast<const uint8_t*>(p);
        m_buf.insert(m_buf.end(), b, b + n);
    }
    /* Length-prefixed UTF-8 string: u16 length then bytes. */
    void Str(const std::string& s) {
        if (s.size() > 0xFFFF) U16(0xFFFF); else U16((uint16_t)s.size());
        Bytes(s.data(), (s.size() > 0xFFFF) ? 0xFFFF : s.size());
    }
    const std::vector<uint8_t>& Data() const { return m_buf; }
    std::vector<uint8_t>& Mutable() { return m_buf; }

private:
    std::vector<uint8_t> m_buf;
};

class Reader {
public:
    Reader(const uint8_t* data, size_t len) : m_p(data), m_end(data + len) {}
    bool U8(uint8_t& out) {
        if (m_p + 1 > m_end) return false;
        out = *m_p++; return true;
    }
    bool U16(uint16_t& out) {
        if (m_p + 2 > m_end) return false;
        out = (uint16_t)(m_p[0] | (m_p[1] << 8)); m_p += 2; return true;
    }
    bool U32(uint32_t& out) {
        if (m_p + 4 > m_end) return false;
        out = (uint32_t)(m_p[0] | (m_p[1] << 8) | (m_p[2] << 16) | (m_p[3] << 24));
        m_p += 4; return true;
    }
    bool F32(float& out) {
        uint32_t x; if (!U32(x)) return false;
        std::memcpy(&out, &x, 4); return true;
    }
    bool Bytes(void* dst, size_t n) {
        if (m_p + n > m_end) return false;
        std::memcpy(dst, m_p, n); m_p += n; return true;
    }
    bool Str(std::string& out) {
        uint16_t n; if (!U16(n)) return false;
        if (m_p + n > m_end) return false;
        out.assign((const char*)m_p, n); m_p += n; return true;
    }
    size_t Remaining() const { return (size_t)(m_end - m_p); }

private:
    const uint8_t* m_p;
    const uint8_t* m_end;
};

/*──────────────────────────────────────────────────────────────────────────────
 * Build a wire-ready packet (size prefix + opcode + payload) but DO NOT
 * encrypt — caller passes through FiestaCipher::EncryptOut on the final
 * (opcode || payload) bytes after the size prefix.
 *──────────────────────────────────────────────────────────────────────────────*/
inline std::vector<uint8_t> BuildPacket(uint16_t opcode,
                                         const std::vector<uint8_t>& payload) {
    /* The body == opcode (2 bytes LE) + payload. Length prefix sits
     * BEFORE the cipher boundary because the receiver needs to read
     * it cleartext to know how many bytes to decrypt next.            */
    const size_t bodyLen = 2 + payload.size();

    std::vector<uint8_t> out;
    out.reserve(3 + bodyLen);
    if (bodyLen < 0xFF) {
        out.push_back((uint8_t)bodyLen);
    } else {
        out.push_back(0xFF);
        out.push_back((uint8_t)(bodyLen & 0xFF));
        out.push_back((uint8_t)((bodyLen >> 8) & 0xFF));
    }
    out.push_back((uint8_t)(opcode & 0xFF));
    out.push_back((uint8_t)(opcode >> 8));
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

}  /* namespace Fiesta */
#endif
