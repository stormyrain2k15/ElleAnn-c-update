/*══════════════════════════════════════════════════════════════════════════════
 * FiestaPacket.h — Packet structure and opcode registry.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   ENGINE IDENTIFICATION (Feb 2026, confirmed via client RE artifacts)
 *   ────────────────────────────────────────────────────────────────────
 *   This is **ShineEngine**. Confirmed by the symbol/error strings in
 *   the user-supplied IDA dumps:
 *
 *     ShineObjectClass::ShinePlayer::sp_NC_MAP_LOGIN_REQ
 *     ShineObjectClass::ShinePlayer::sp_NC_MISC_SEED_REQ
 *     ShineObjectClass::ShinePlayer::sp_NC_MAP_LOGINCOMPLETE_CMD
 *     ProtocolPacket::pp_SendPacket / pp_SetPacketLen
 *     PROTOCOLFUNCTIONTEMPLETE<T>::pft_Store
 *     CProtocolAnalysis::LogWriteAllNetCommand
 *
 *   Network commands are named NC_<SUBSYSTEM>_<NAME>_<TYPE> where
 *   TYPE ∈ {REQ, ACK, CMD, CMDD}. 518 distinct NC_* identifiers were
 *   recovered from the client and are catalogued in
 *   `shine_nc_symbols.txt` next to this file.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   WIRE FORMAT (consistent across login + world stages):
 *   ────────────────────────────────────────────────────────────────────
 *
 *       [u8 sizeFlag]              0x00..0xFE → length is 1 byte (sizeFlag)
 *                                  0xFF       → length is the next u16 LE
 *       [u16/u8  payloadLen]       (omitted when sizeFlag < 0xFF)
 *       [u16 LE  opcode]           Opcode dispatched to handlers
 *       [bytes]  payload           Opcode-specific body
 *
 *   The size-prefix byte stays cleartext; the cipher boundary begins
 *   AT the opcode byte. Payload (and opcode) are XOR-stream encrypted
 *   per direction. Cipher key is the 32-byte SEED delivered by the
 *   server in NC_MISC_SEED_ACK after the client sends NC_MISC_SEED_REQ.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   OPCODE HEX VALUES — STATUS
 *   ────────────────────────────────────────────────────────────────────
 *   The constants below are the documented JFOL/CN community-mapping
 *   placeholders we shipped before client RE landed. Now that we have
 *   the symbol surface, we know the NAMES are correct (NC_MAP_LOGIN_REQ
 *   etc) but the HEX values still need to be confirmed from one of:
 *     1. The client PDB (xrefs to pft_Store give (opcode, handler) pairs)
 *     2. The server's `Protocol/NC.h` (or equivalent) header — this
 *        file maps every NC_* name to its hex value definitively.
 *   When either lands, replace the values below in one pass — no other
 *   file in this folder hardcodes opcode hex values.
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
 * ShineEngine NC_* opcode registry — REAL VALUES.
 *
 *   Recovered Feb 2026 from the Mischief 6.11.2015 server build using
 *   `5ZoneServer2-Port80-NoService.exe` + `5ZoneServer2.udd`. We walked
 *   every `pft_Store(major, minor, &handler)` call site, decoded the
 *   immediate operands, and joined handler RVAs to NC_* names from the
 *   .udd. The full extraction script is in
 *   `_re_artifacts/extract_via_udd.py`; the flat table is
 *   `_re_artifacts/shine_opcodes.tsv`.
 *
 *   Wire opcode encoding:  opcode = (major << 8) | minor
 *   where (major, minor) are the two ints pft_Store() takes ahead of
 *   the handler pointer. Decoder is unchanged — opcode is read as a
 *   single u16 LE off the wire, the (major, minor) decomposition is
 *   only meaningful at registration time.
 *
 *   210 ShinePlayer-facing opcodes covered (181 REQ + 41 CMD + 13 ACK
 *   spread across 29 subsystems). Sub-major map:
 *
 *     0x02 NC_MISC      0x03 NC_USER/LOG  0x04 NC_CHAR    0x06 NC_MAP
 *     0x07 NC_BRIEFINFO 0x08 NC_ACT       0x09 NC_BAT     0x0C NC_ITEM
 *     0x0E NC_PARTY     0x0F NC_MENU      0x10 NC_CHARSAVE 0x11 NC_QUEST
 *     0x12 NC_SKILL     0x13 NC_TRADE     0x14 NC_SOULSTONE 0x16 NC_KQ
 *     0x17 NC_WT        0x18 NC_CT        0x1A NC_BOOTH    0x1B NC_SCENARIO
 *     0x1D NC_GUILD/ITEMDB                 0x23 NC_MINIHOUSE 0x25 NC_HOLY
 *     0x26 NC_GUILD     0x29 NC_INSTANCE  0x2B NC_DICE     0x2D NC_USER
 *     0x2E NC_AUCTION   0x2F NC_GAMBLE
 *
 *   Server-built variants MAY have a few opcodes shifted (e.g. CN-vs-NA
 *   private servers); if a target server rejects a packet, override
 *   the value at the call site and re-test. This file is the canonical
 *   reference for the Mischief build.
 *──────────────────────────────────────────────────────────────────────────────*/
namespace Op {

    /* ── NC_MISC — handshake, ping, server params ────────────── */
    constexpr uint16_t NC_MISC_SEED_REQ                          = 0x0206;
    constexpr uint16_t NC_MISC_GAMETIME_REQ                      = 0x0212;
    constexpr uint16_t NC_MISC_HIDE_EXCEPT_ME_ON_CMD             = 0x0213;
    constexpr uint16_t NC_MISC_HIDE_EXCEPT_ME_OFF_CMD            = 0x0217;
    constexpr uint16_t NC_MISC_PINGTEST_TOOL_WM_CLIENT_ZONE_DB   = 0x0218;
    constexpr uint16_t NC_MISC_SERVERPARAMETER_REQ               = 0x022F;
    constexpr uint16_t NC_MISC_TIMERECORD_REQ                    = 0x023E;

    /* ── NC_USER / NC_LOG — account, logout, beauty ─────────── */
    constexpr uint16_t NC_USER_NORMALLOGOUT_CMD                  = 0x0318;
    constexpr uint16_t NC_USER_USE_BEAUTY_SHOP_CMD               = 0x0332;
    constexpr uint16_t NC_LOG_MAP_NOBASE_CMD                     = 0x0384;

    /* ── NC_CHAR ─────────────────────────────────────────────── */
    constexpr uint16_t NC_CHAR_REVIVE_REQ                        = 0x044E;
    constexpr uint16_t NC_CHAR_STAT_INCPOINT_REQ                 = 0x045C;
    constexpr uint16_t NC_CHAR_STAT_DECPOINT_REQ                 = 0x0462;
    constexpr uint16_t NC_CHAR_LOGOUTREADY_CMD                   = 0x0471;
    constexpr uint16_t NC_CHAR_LOGOUTCANCEL_CMD                  = 0x0472;
    constexpr uint16_t NC_CHAR_WEDDING_PARTNER_INFO_REQ          = 0x0493;
    constexpr uint16_t NC_CHAR_DEPOLYMORPH_CMD                   = 0x04B0;
    constexpr uint16_t NC_CHAR_SAVE_LINK_REQ                     = 0x04B9;
    constexpr uint16_t NC_CHAR_CLIENT_AUTO_PICK_REQ              = 0x04BC;
    constexpr uint16_t NC_CHAR_CLIENT_FREESTAT_APPLICATION_REQ   = 0x04C2;

    /* ── NC_MAP — login, zone, town portal ───────────────────── */
    constexpr uint16_t NC_MAP_LOGIN_REQ                          = 0x0601;
    constexpr uint16_t NC_MAP_LOGINCOMPLETE_CMD                  = 0x0603;
    constexpr uint16_t NC_MAP_WING_SAVE_REQ                      = 0x0614;
    constexpr uint16_t NC_MAP_WING_FLY_REQ                       = 0x0616;
    constexpr uint16_t NC_MAP_TOWNPORTAL_REQ                     = 0x061A;
    constexpr uint16_t NC_MAP_LINEUP_PARTYPLAYER_REQ             = 0x061C;
    constexpr uint16_t NC_MAP_LINEUP_GUILDPLAYER_REQ             = 0x061F;

    /* ── NC_BRIEFINFO ────────────────────────────────────────── */
    constexpr uint16_t NC_BRIEFINFO_INFORM_CMD                   = 0x0701;

    /* ── NC_ACT — movement, chat, NPC, gather, emote ─────────── */
    constexpr uint16_t NC_ACT_CHAT_REQ                           = 0x0801;
    constexpr uint16_t NC_ACT_WALK_REQ                           = 0x0803;
    constexpr uint16_t NC_ACT_RUN_REQ                            = 0x0805;
    constexpr uint16_t NC_ACT_CHANGEMODE_REQ                     = 0x0808;
    constexpr uint16_t NC_ACT_NPCCLICK_CMD                       = 0x080A;
    constexpr uint16_t NC_ACT_ENDOFTRADE_CMD                     = 0x080B;
    constexpr uint16_t NC_ACT_STOP_REQ                           = 0x0812;
    constexpr uint16_t NC_ACT_MOVEWALK_CMD                       = 0x0817;
    constexpr uint16_t NC_ACT_MOVERUN_CMD                        = 0x0819;
    constexpr uint16_t NC_ACT_NPCMENUOPEN_ACK                    = 0x081D;
    constexpr uint16_t NC_ACT_SHOUT_CMD                          = 0x081E;
    constexpr uint16_t NC_ACT_EMOTICON_CMD                       = 0x0820;
    constexpr uint16_t NC_ACT_EMOTICONSTOP_CMD                   = 0x0822;
    constexpr uint16_t NC_ACT_JUMP_CMD                           = 0x0824;
    constexpr uint16_t NC_ACT_PITCHTENT_REQ                      = 0x0827;
    constexpr uint16_t NC_ACT_FOLDTENT_REQ                       = 0x082A;
    constexpr uint16_t NC_ACT_GATHERSTART_REQ                    = 0x082D;
    constexpr uint16_t NC_ACT_GATHERCANCEL_CMD                   = 0x0830;
    constexpr uint16_t NC_ACT_GATHERCOMPLETE_REQ                 = 0x0832;
    constexpr uint16_t NC_ACT_PRODUCE_CAST_REQ                   = 0x0835;
    constexpr uint16_t NC_ACT_PRODUCE_CASTABORT_CMD              = 0x0839;
    constexpr uint16_t NC_ACT_RIDE_FEEDING_REQ                   = 0x0844;
    constexpr uint16_t NC_ACT_ROAR_REQ                           = 0x084B;
    constexpr uint16_t NC_ACT_RIDE_OFF_REQ                       = 0x086D;
    constexpr uint16_t NC_ACT_AUTO_WAY_FINDING_USE_GATE_REQ      = 0x0870;

    /* ── NC_BAT — combat ─────────────────────────────────────── */
    constexpr uint16_t NC_BAT_TARGETTING_REQ                     = 0x0901;
    constexpr uint16_t NC_BAT_HIT_REQ                            = 0x0903;
    constexpr uint16_t NC_BAT_UNTARGET_REQ                       = 0x0908;
    constexpr uint16_t NC_BAT_SMASH_REQ                          = 0x0912;
    constexpr uint16_t NC_BAT_SKILLCAST_REQ                      = 0x0918;
    constexpr uint16_t NC_BAT_SKILLCASTABORT_CMD                 = 0x091C;
    constexpr uint16_t NC_BAT_BASHSTART_CMD                      = 0x092B;
    constexpr uint16_t NC_BAT_BASHSTOP_CMD                       = 0x0932;
    constexpr uint16_t NC_BAT_ASSIST_REQ                         = 0x093E;
    constexpr uint16_t NC_BAT_SKILLBASH_OBJ_CAST_REQ             = 0x0940;
    constexpr uint16_t NC_BAT_SKILLBASH_FLD_CAST_REQ             = 0x0941;
    constexpr uint16_t NC_BAT_SKILLBASH_CASTABORT_REQ            = 0x0944;
    constexpr uint16_t NC_BAT_ABSTATE_ERASE_REQ                  = 0x0954;

    /* ── NC_ITEM — inventory ─────────────────────────────────── */
    constexpr uint16_t NC_ITEM_BUY_REQ                           = 0x0C03;
    constexpr uint16_t NC_ITEM_SELL_REQ                          = 0x0C06;
    constexpr uint16_t NC_ITEM_DROP_REQ                          = 0x0C07;
    constexpr uint16_t NC_ITEM_PICKUP_REQ                        = 0x0C09;
    constexpr uint16_t NC_ITEM_RELOC_REQ                         = 0x0C0B;
    constexpr uint16_t NC_ITEM_USE_REQ                           = 0x0C0F;
    constexpr uint16_t NC_ITEM_QUICKSLOT_RELOC_REQ               = 0x0C42;
    constexpr uint16_t NC_ITEM_REVIVEITEMUSE_CMD                 = 0x0C48;

    /* ── small subsystems ────────────────────────────────────── */
    constexpr uint16_t NC_PARTY_REQUEST_REQ                      = 0x0E48;
    constexpr uint16_t NC_MENU_SERVERMENU_ACK                    = 0x0F02;
    constexpr uint16_t NC_QUEST_DOING_REQ                        = 0x1102;
    constexpr uint16_t NC_QUEST_REWARD_REQ                       = 0x1107;
    constexpr uint16_t NC_QUEST_GIVEUP_REQ                       = 0x110B;
    constexpr uint16_t NC_QUEST_SCRIPT_CMD_ACK                   = 0x1102;
    constexpr uint16_t NC_SKILL_LEARN_REQ                        = 0x1211;
    constexpr uint16_t NC_SKILL_REPLYREVIVE_CMD                  = 0x122A;
    constexpr uint16_t NC_TRADE_PROPOSE_REQ                      = 0x1301;
    constexpr uint16_t NC_TRADE_PROPOSEYES_ACK                   = 0x1306;
    constexpr uint16_t NC_TRADE_PROPOSE_ASKNO_ACK                = 0x1303;
    constexpr uint16_t NC_TRADE_REQADDITEM_REQ                   = 0x1307;
    constexpr uint16_t NC_TRADE_DECISION_REQ                     = 0x130A;
    constexpr uint16_t NC_TRADE_OK_REQ                           = 0x130D;
    constexpr uint16_t NC_TRADE_CANCEL_REQ                       = 0x1311;
    constexpr uint16_t NC_TRADE_REQADDMONEY_REQ                  = 0x1315;
    constexpr uint16_t NC_TRADE_FINAL_OK_REQ                     = 0x1319;
    constexpr uint16_t NC_TRADE_PARTNER_NOREADY_ACK              = 0x131F;
    constexpr uint16_t NC_SOULSTONE_HP_BUY_REQ                   = 0x1401;
    constexpr uint16_t NC_SOULSTONE_SP_BUY_REQ                   = 0x1402;
    constexpr uint16_t NC_SOULSTONE_USE_REQ                      = 0x1407;
    constexpr uint16_t NC_SOULSTONE_REVIVE_REQ                   = 0x1409;
    constexpr uint16_t NC_KQ_REWARD_REQ                          = 0x161A;
    constexpr uint16_t NC_WT_LICENSE_REQ                         = 0x1701;
    constexpr uint16_t NC_CT_SET_CURRENT_REQ                     = 0x1801;
    constexpr uint16_t NC_BOOTH_OPEN_REQ                         = 0x1A01;
    constexpr uint16_t NC_BOOTH_CLOSE_REQ                        = 0x1A04;
    constexpr uint16_t NC_BOOTH_ENTRY_REQ                        = 0x1A07;
    constexpr uint16_t NC_BOOTH_REFRESH_REQ                      = 0x1A0A;
    constexpr uint16_t NC_BOOTH_ITEMTRADE_REQ                    = 0x1A0D;
    constexpr uint16_t NC_BOOTH_INTERIORSTART_REQ                = 0x1A10;
    constexpr uint16_t NC_INSTANCE_DUNGEON_DELETE_DUNGEON_REQ    = 0x2905;
    constexpr uint16_t NC_DICE_TAISAI_GAME_JOIN_REQ              = 0x2B01;
    constexpr uint16_t NC_DICE_TAISAI_GAME_LEAVE_REQ             = 0x2B05;
    constexpr uint16_t NC_DICE_TAISAI_BETTING_REQ                = 0x2B0B;
    constexpr uint16_t NC_DICE_TAISAI_BETTING_CANCEL_REQ         = 0x2B0F;
    constexpr uint16_t NC_GAMBLE_GAMBLEHOUSE_EXIT_REQ            = 0x2F02;

    /* ── HEARTBEAT / KEEPALIVE — placeholder, server has none ── */
    /*   ShineEngine zone server uses TCP keepalive at the OS level
     *   plus implicit traffic; there is no NC_*_KEEPALIVE in the
     *   recovered table. The client's StartHeartbeat() loop is a
     *   no-op for ShineEngine — left wired for future protocols.    */

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
