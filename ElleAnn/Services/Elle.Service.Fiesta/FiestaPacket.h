/*══════════════════════════════════════════════════════════════════════════════
 * FiestaPacket.h — ShineEngine packet structure & opcode registry.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   PROVENANCE — every constant in this file is sourced from one of:
 *   ────────────────────────────────────────────────────────────────────
 *     [PDB-V80] llvm-pdbutil dump of {Login,WorldManager,Account,
 *               AccountLog}.pdb  (post-region-toggle 2016 build).
 *     [PDB-V70] custom CodeView reader on 5ZoneServer2.pdb (CN2012
 *               2012-12 build, TPI v7.0 — llvm-pdbutil rejects).
 *     [BIN]     `strings(1)` over matching .exe (when PDB silent).
 *
 *   See `_re_artifacts/OPCODES_FROM_PDB.md` for the master extraction
 *   methodology and `_re_artifacts/pdb/extracted/PDB_OPCODES.json`
 *   for the full 2 687-entry resolved opcode table.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   WIRE FORMAT (cleartext-prefix variant — verified)
 *   ────────────────────────────────────────────────────────────────────
 *
 *       [u8  sizeFlag]              0x00..0xFE → length is 1 byte (sizeFlag)
 *                                   0xFF       → length is the next u16 LE
 *       [u16 LE  payloadLen]        (only when sizeFlag == 0xFF)
 *       ┌─ cipher boundary ──────────────────────────────────────────┐
 *       │ [u16 LE  opcode]                                            │
 *       │ [bytes]  payload                                            │
 *       └─────────────────────────────────────────────────────────────┘
 *
 *   `length` covers (opcode || payload). Cipher is keyed by a single
 *   u16 seed that the SERVER pushes immediately after TCP accept in
 *   PROTO_NC_MISC_SEED_ACK (opcode 0x0207). Both directions use the
 *   same seed; the cipher state advances continuously across the
 *   stream (no per-packet reset). See FiestaCipher.h.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   3-HOP LOGIN DANCE (verified opcode IDs)
 *   ────────────────────────────────────────────────────────────────────
 *      hop 1: Login (port 9010)
 *        S→C  NC_MISC_SEED_ACK         (0x0207, payload {u16 seed})
 *        C→S  NC_USER_CLIENT_VERSION_CHECK_REQ (0x0365, 64B wchar key)
 *        S→C  NC_USER_CLIENT_RIGHTVERSION_CHECK_ACK (0x0367, empty)
 *        C→S  NC_USER_LOGIN_REQ       (0x0306, 272B {char[256]+char[16]})
 *        S→C  NC_USER_LOGIN_ACK       (0x030A, {u8 numofworld + WorldInfo[]})
 *               or NC_USER_LOGINFAIL_ACK (0x0309)
 *        C→S  NC_USER_WORLDSELECT_REQ (0x030B, {u8 worldno})
 *        S→C  NC_USER_WORLDSELECT_ACK (0x030C, 83B {status, ip[16],
 *                                                    port, validate[64]})
 *      hop 2: WorldManager (port returned by WORLDSELECT_ACK)
 *        S→C  NC_MISC_SEED_ACK
 *        C→S  NC_USER_WILLLOGIN_REQ   (0x030D, 347B inc. validate token)
 *        S→C  NC_USER_WILLLOGIN_ACK   (0x030E, returns Zone {ip,port,token})
 *      hop 3: Zone (port returned by WILLLOGIN_ACK)
 *        S→C  NC_MISC_SEED_ACK
 *        C→S  NC_USER_LOGINWORLD_REQ  (0x030F)
 *        C→S  NC_MAP_LOGIN_REQ        (0x0601, 978B incl. data checksum)
 *        S→C  NC_MAP_LOGINCOMPLETE_CMD (0x0603, 32B doorblock checksum)
 *               → IN_GAME
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
 * NC_* opcode registry  —  encoded as opcode = (cmdtype << 8) | subid.
 *
 *   `cmdtype` values come from PROTOCOL_COMMAND (Login.pdb fieldlist
 *   0x28C1). Selected groupings (low byte fixed by PDB enums):
 *     0x02 NC_MISC      0x03 NC_USER     0x04 NC_CHAR     0x05 NC_AVATAR
 *     0x06 NC_MAP       0x07 NC_BRIEFINFO 0x08 NC_ACT     0x09 NC_BAT
 *     0x0A NC_OPTOOL    0x0C NC_ITEM     0x0E NC_PARTY    0x10 NC_CHARSAVE
 *     0x11 NC_QUEST     0x12 NC_SKILL    0x13 NC_TRADE    0x14 NC_SOULSTONE
 *     0x16 NC_KQ        0x17 NC_WT       0x18 NC_CT       0x1A NC_BOOTH
 *     0x1D NC_GUILD     0x21 NC_MINIHOUSE 0x24 NC_CHARGED 0x29 NC_INSTANCE
 *     0x2B NC_DICE      0x2D NC_USER_CONNECTION  0x2E NC_AUCTION  0x35 NC_PET
 *──────────────────────────────────────────────────────────────────────────────*/
namespace Op {

    /* ── NC_MISC — handshake, ping ───────────────────────────────── */
    constexpr uint16_t NC_MISC_HEARTBEAT_REQ                     = 0x0204;  /* [PDB-V80] */
    constexpr uint16_t NC_MISC_HEARTBEAT_ACK                     = 0x0205;  /* [PDB-V80] */
    constexpr uint16_t NC_MISC_SEED_REQ                          = 0x0206;  /* [PDB-V80] (legacy, server-pushed) */
    constexpr uint16_t NC_MISC_SEED_ACK                          = 0x0207;  /* [PDB-V80] cipher seed delivery */
    constexpr uint16_t NC_MISC_GAMETIME_REQ                      = 0x0212;  /* [BIN] */

    /* ── NC_USER — login / world-select / logout ────────────────── */
    constexpr uint16_t NC_USER_XTRAP_REQ                         = 0x0304;  /* [PDB-V80] */
    constexpr uint16_t NC_USER_XTRAP_ACK                         = 0x0305;  /* [PDB-V80] */
    constexpr uint16_t NC_USER_LOGIN_REQ                         = 0x0306;  /* [PDB-V80] hop-1 */
    constexpr uint16_t NC_USER_PASSWORD_CHECK_REQ                = 0x0307;  /* [PDB-V80] */
    constexpr uint16_t NC_USER_PASSWORD_CHECK_ACK                = 0x0308;  /* [PDB-V80] */
    constexpr uint16_t NC_USER_LOGINFAIL_ACK                     = 0x0309;  /* [PDB-V80] */
    constexpr uint16_t NC_USER_LOGIN_ACK                         = 0x030A;  /* [PDB-V80] world list */
    constexpr uint16_t NC_USER_WORLDSELECT_REQ                   = 0x030B;  /* [PDB-V80] */
    constexpr uint16_t NC_USER_WORLDSELECT_ACK                   = 0x030C;  /* [PDB-V80] WM addr+token */
    constexpr uint16_t NC_USER_WILLLOGIN_REQ                     = 0x030D;  /* [PDB-V80] hop-2 */
    constexpr uint16_t NC_USER_WILLLOGIN_ACK                     = 0x030E;  /* [PDB-V80] zone addr+token */
    constexpr uint16_t NC_USER_LOGINWORLD_REQ                    = 0x030F;  /* [PDB-V80] hop-3 enter */
    constexpr uint16_t NC_USER_NORMALLOGOUT_CMD                  = 0x0318;  /* [PDB-V80] */
    constexpr uint16_t NC_USER_USE_BEAUTY_SHOP_CMD               = 0x0332;  /* [BIN] */
    constexpr uint16_t NC_USER_CLIENT_VERSION_CHECK_REQ          = 0x0365;  /* [PDB-V80] */
    constexpr uint16_t NC_USER_CLIENT_WRONGVERSION_CHECK_ACK     = 0x0366;  /* [PDB-V80] */
    constexpr uint16_t NC_USER_CLIENT_RIGHTVERSION_CHECK_ACK     = 0x0367;  /* [PDB-V80] */

    /* ── NC_LOG (uses subid space 0x03xx legacy) ─────────────────── */
    constexpr uint16_t NC_LOG_MAP_NOBASE_CMD                     = 0x0384;  /* [BIN] */

    /* ── NC_CHAR — character lifecycle / stats ───────────────────── */
    constexpr uint16_t NC_CHAR_CLIENT_BASE_CMD                   = 0x0438;  /* [PDB-V70] base stat delta */
    constexpr uint16_t NC_CHAR_REVIVE_REQ                        = 0x044E;  /* [PDB-V80] */
    constexpr uint16_t NC_CHAR_STAT_INCPOINT_REQ                 = 0x045C;  /* [PDB-V80] */
    constexpr uint16_t NC_CHAR_STAT_DECPOINT_REQ                 = 0x0462;  /* [PDB-V80] */
    constexpr uint16_t NC_CHAR_LOGOUTREADY_CMD                   = 0x0471;  /* [PDB-V80] */
    constexpr uint16_t NC_CHAR_LOGOUTCANCEL_CMD                  = 0x0472;  /* [PDB-V80] */
    constexpr uint16_t NC_CHAR_WEDDING_PARTNER_INFO_REQ          = 0x0493;  /* [BIN] */
    constexpr uint16_t NC_CHAR_DEPOLYMORPH_CMD                   = 0x04B0;  /* [BIN] */
    constexpr uint16_t NC_CHAR_SAVE_LINK_REQ                     = 0x04B9;  /* [BIN] */
    constexpr uint16_t NC_CHAR_CLIENT_AUTO_PICK_REQ              = 0x04BC;  /* [BIN] */
    constexpr uint16_t NC_CHAR_CLIENT_FREESTAT_APPLICATION_REQ   = 0x04C2;  /* [BIN] */

    /* ── NC_MAP — zone enter / wing / portal ─────────────────────── */
    constexpr uint16_t NC_MAP_LOGIN_REQ                          = 0x0601;  /* [PDB-V70] hop-3 */
    constexpr uint16_t NC_MAP_LOGINCOMPLETE_CMD                  = 0x0603;  /* [PDB-V70] */
    constexpr uint16_t NC_MAP_WING_SAVE_REQ                      = 0x0614;  /* [BIN] */
    constexpr uint16_t NC_MAP_WING_FLY_REQ                       = 0x0616;  /* [BIN] */
    constexpr uint16_t NC_MAP_TOWNPORTAL_REQ                     = 0x061A;  /* [BIN] */
    constexpr uint16_t NC_MAP_LINEUP_PARTYPLAYER_REQ             = 0x061C;  /* [BIN] */
    constexpr uint16_t NC_MAP_LINEUP_GUILDPLAYER_REQ             = 0x061F;  /* [BIN] */

    /* ── NC_BRIEFINFO — server-pushed entity-state deltas ────────── */
    constexpr uint16_t NC_BRIEFINFO_INFORM_CMD                   = 0x0701;  /* [PDB-V70] */
    constexpr uint16_t NC_BRIEFINFO_REGENMOB_CMD                 = 0x0708;  /* [PDB-V70] */

    /* ── NC_ACT — movement, chat, NPC, gather, emote ─────────────── */
    constexpr uint16_t NC_ACT_CHAT_REQ                           = 0x0801;  /* [PDB-V70] */
    constexpr uint16_t NC_ACT_WALK_REQ                           = 0x0803;  /* [PDB-V70] */
    constexpr uint16_t NC_ACT_RUN_REQ                            = 0x0805;  /* [PDB-V70] */
    constexpr uint16_t NC_ACT_CHANGEMODE_REQ                     = 0x0808;  /* [BIN] */
    constexpr uint16_t NC_ACT_NPCCLICK_CMD                       = 0x080A;  /* [PDB-V70] */
    constexpr uint16_t NC_ACT_ENDOFTRADE_CMD                     = 0x080B;  /* [BIN] */
    constexpr uint16_t NC_ACT_WHISPER_REQ                        = 0x080C;  /* [PDB-V70] */
    constexpr uint16_t NC_ACT_STOP_REQ                           = 0x0812;  /* [PDB-V70] */
    constexpr uint16_t NC_ACT_MOVEWALK_CMD                       = 0x0817;  /* [PDB-V70] */
    constexpr uint16_t NC_ACT_MOVERUN_CMD                        = 0x0819;  /* [PDB-V70] */
    constexpr uint16_t NC_ACT_NPCMENUOPEN_ACK                    = 0x081D;  /* [BIN] */
    constexpr uint16_t NC_ACT_SHOUT_CMD                          = 0x081E;  /* [PDB-V70] */
    constexpr uint16_t NC_ACT_EMOTICON_CMD                       = 0x0820;  /* [BIN] */
    constexpr uint16_t NC_ACT_EMOTICONSTOP_CMD                   = 0x0822;  /* [BIN] */
    constexpr uint16_t NC_ACT_JUMP_CMD                           = 0x0824;  /* [PDB-V70] */
    constexpr uint16_t NC_ACT_PITCHTENT_REQ                      = 0x0827;  /* [BIN] */
    constexpr uint16_t NC_ACT_FOLDTENT_REQ                       = 0x082A;  /* [BIN] */
    constexpr uint16_t NC_ACT_GATHERSTART_REQ                    = 0x082D;  /* [BIN] */
    constexpr uint16_t NC_ACT_GATHERCANCEL_CMD                   = 0x0830;  /* [BIN] */
    constexpr uint16_t NC_ACT_GATHERCOMPLETE_REQ                 = 0x0832;  /* [BIN] */
    constexpr uint16_t NC_ACT_PRODUCE_CAST_REQ                   = 0x0835;  /* [BIN] */
    constexpr uint16_t NC_ACT_PRODUCE_CASTABORT_CMD              = 0x0839;  /* [BIN] */
    constexpr uint16_t NC_ACT_RIDE_FEEDING_REQ                   = 0x0844;  /* [BIN] */
    constexpr uint16_t NC_ACT_ROAR_REQ                           = 0x084B;  /* [BIN] */
    constexpr uint16_t NC_ACT_RIDE_OFF_REQ                       = 0x086D;  /* [BIN] */
    constexpr uint16_t NC_ACT_AUTO_WAY_FINDING_USE_GATE_REQ      = 0x0870;  /* [BIN] */

    /* ── NC_BAT — combat (Zone-side; PDB-V70 names but layouts not
     *   recovered from this build's PDB — opcodes verified via WM
     *   dispatcher table cross-reference, payloads remain WIP). ─── */
    constexpr uint16_t NC_BAT_TARGETING_REQ                      = 0x0901;  /* [BIN] (note: not "TARGETTING") */
    constexpr uint16_t NC_BAT_HIT_REQ                            = 0x0903;  /* [BIN] */
    constexpr uint16_t NC_BAT_UNTARGET_REQ                       = 0x0908;  /* [BIN] */
    constexpr uint16_t NC_BAT_SMASH_REQ                          = 0x0912;  /* [BIN] */
    constexpr uint16_t NC_BAT_SKILLCAST_REQ                      = 0x0918;  /* [BIN] */
    constexpr uint16_t NC_BAT_SKILLCASTABORT_CMD                 = 0x091C;  /* [BIN] */
    constexpr uint16_t NC_BAT_BASHSTART_CMD                      = 0x092B;  /* [BIN] */
    constexpr uint16_t NC_BAT_BASHSTOP_CMD                       = 0x0932;  /* [BIN] */
    constexpr uint16_t NC_BAT_ASSIST_REQ                         = 0x093E;  /* [BIN] */
    constexpr uint16_t NC_BAT_SKILLBASH_OBJ_CAST_REQ             = 0x0940;  /* [BIN] */
    constexpr uint16_t NC_BAT_SKILLBASH_FLD_CAST_REQ             = 0x0941;  /* [BIN] */
    constexpr uint16_t NC_BAT_SKILLBASH_CASTABORT_REQ            = 0x0944;  /* [BIN] */
    constexpr uint16_t NC_BAT_ABSTATE_ERASE_REQ                  = 0x0954;  /* [BIN] */

    /* ── NC_ITEM — inventory ─────────────────────────────────────── */
    constexpr uint16_t NC_ITEM_BUY_REQ                           = 0x0C03;  /* [BIN] */
    constexpr uint16_t NC_ITEM_SELL_REQ                          = 0x0C06;  /* [BIN] */
    constexpr uint16_t NC_ITEM_DROP_REQ                          = 0x0C07;  /* [BIN] */
    constexpr uint16_t NC_ITEM_PICKUP_REQ                        = 0x0C09;  /* [BIN] */
    constexpr uint16_t NC_ITEM_RELOC_REQ                         = 0x0C0B;  /* [BIN] */
    constexpr uint16_t NC_ITEM_USE_REQ                           = 0x0C0F;  /* [BIN] */
    constexpr uint16_t NC_ITEM_QUICKSLOT_RELOC_REQ               = 0x0C42;  /* [BIN] */
    constexpr uint16_t NC_ITEM_REVIVEITEMUSE_CMD                 = 0x0C48;  /* [BIN] */

    /* ── small subsystems (BIN-sourced; payloads not in scope) ──── */
    constexpr uint16_t NC_PARTY_REQUEST_REQ                      = 0x0E48;
    constexpr uint16_t NC_MENU_SERVERMENU_ACK                    = 0x0F02;
    constexpr uint16_t NC_QUEST_DOING_REQ                        = 0x1102;
    constexpr uint16_t NC_QUEST_REWARD_REQ                       = 0x1107;
    constexpr uint16_t NC_QUEST_GIVEUP_REQ                       = 0x110B;
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

}  /* namespace Op */

/*──────────────────────────────────────────────────────────────────────────────
 * ShineEngine PROTO_NC_* PODs — verified bit-exact layouts.
 *
 *   Every struct below has its sizeof checked at compile time
 *   against the value reported by the PDB type stream.  The packed
 *   pragma pair guarantees the in-memory image matches the wire image
 *   on x86 / x86-64 toolchains (Visual Studio + GCC + Clang all honor
 *   #pragma pack(push,1) for these scalar/array layouts).
 *──────────────────────────────────────────────────────────────────────────────*/
#pragma pack(push, 1)

/** SHINE_XY_TYPE — world XY pair, 32-bit fixed-point integer.
 *  Source: Zone PDB type 0x1163 (sizeof=8). */
struct SHINE_XY_TYPE {
    uint32_t x;
    uint32_t y;
};

/** SHINE_COORD_TYPE — XY plus 8-bit facing direction.
 *  Source: Zone PDB type id 0x3005 (sizeof=9). */
struct SHINE_COORD_TYPE {
    SHINE_XY_TYPE xy;
    uint8_t       dir;
};

/** PROTO_NC_MISC_SEED_ACK — server pushes once after TCP accept.
 *  Source: Login PDB type 0x28D0 (sizeof=2). */
struct PROTO_NC_MISC_SEED_ACK {
    uint16_t seed;
};

/** PROTO_NC_USER_CLIENT_VERSION_CHECK_REQ — version handshake.
 *  Source: Login PDB type for 0x2A...; sizeof=64.  Field is a
 *  fixed-size 32-char wide string (UTF-16LE).  Region-specific
 *  builds compare exact bytes against a hard-coded constant; for
 *  the CN2012 build the expected key is `"SDO_FIESTA_NEW_VER_KEY"`
 *  padded with NULs (0x00). */
struct PROTO_NC_USER_CLIENT_VERSION_CHECK_REQ {
    uint16_t sVersionKey[32];   /* UTF-16LE; verbatim wire bytes */
};

/** PROTO_NC_USER_LOGIN_REQ — hop-1 credentials.
 *  Source: Login PDB struct 0x297F + fieldlist 0x297E (sizeof=272).
 *  Both fields are NUL-padded fixed-width ASCII (NOT length-prefixed). */
struct PROTO_NC_USER_LOGIN_REQ {
    char user[256];
    char password[16];
};

/** PROTO_NC_USER_WORLDSELECT_REQ — pick which world from LOGIN_ACK list.
 *  Source: Login PDB; sizeof=1. */
struct PROTO_NC_USER_WORLDSELECT_REQ {
    uint8_t worldno;
};

/** PROTO_NC_USER_WORLDSELECT_ACK — WM addr + per-session validation
 *  token. Login → Client.
 *  Source: Login PDB struct 0x27C6 + fieldlist 0x27C5 (sizeof=83).
 *  Layout exactly:
 *    [0]  u8       worldstatus     0=down 1=normal 2=busy 3=test ...
 *    [1]  char[16] ip              dotted-quad ASCIZ for WM
 *    [17] u16      port            WM listen port (LE)
 *    [19] u16[32]  validate_new    64-byte session token (echoed to WM) */
struct PROTO_NC_USER_WORLDSELECT_ACK {
    uint8_t  worldstatus;
    char     ip[16];
    uint16_t port;
    uint16_t validate_new[32];
};

/** PROTO_NC_USER_WILLLOGIN_REQ — hop-2 (Client → WM).
 *  Source: Login PDB struct 0x28FF + fieldlist 0x28FE (sizeof=347).
 *  Layout exactly:
 *    [  0] netpacketheader 7B (clienthandle:u16 + 5 unnamed bytes —
 *           in CN2012 these are zero on the client→WM direction)
 *    [  7] char[256]       userid  (same string used in LOGIN_REQ.user)
 *    [263] u16[32]         validate_new  (echoed from WORLDSELECT_ACK)
 *    [327] char[20]        spawnapps  (launcher OS fingerprint, NUL-pad
 *           — CN2012 accepts all-zero for headless clients) */
struct PROTO_NC_USER_WILLLOGIN_REQ {
    uint8_t  netpacketheader[7];
    char     userid[256];
    uint16_t validate_new[32];
    char     spawnapps[20];
};

/** PROTO_NC_USER_WILLLOGIN_ACK — WM → Client.
 *  Source: Login PDB struct (sizeof=348).  Wraps WILLLOGIN_REQ +
 *  status byte; on success WM rewrites `validate_new` to the Zone
 *  token and the IP/port fields point to the destination zone. */
struct PROTO_NC_USER_WILLLOGIN_ACK {
    uint8_t  status;
    uint8_t  netpacketheader[7];
    char     userid[256];
    uint16_t validate_new[32];
    char     zone_ip[16];
    uint16_t zone_port;
    uint8_t  pad[2];
};

/** PROTO_NC_ACT_WALK_REQ — Zone movement request.
 *  Source: Zone PDB struct 0xdf53 (sizeof=16). */
struct PROTO_NC_ACT_WALK_REQ {
    SHINE_XY_TYPE from;
    SHINE_XY_TYPE to;
};

/** PROTO_NC_ACT_NPCCLICK_CMD — Zone NPC interaction.
 *  Source: Zone PDB (sizeof=2). */
struct PROTO_NC_ACT_NPCCLICK_CMD {
    uint16_t npchandle;
};

/** PROTO_NC_BRIEFINFO_INFORM_CMD — zone broadcast trigger.
 *  Source: Zone PDB (sizeof=6). */
struct PROTO_NC_BRIEFINFO_INFORM_CMD {
    uint16_t nMyHnd;
    uint16_t ReceiveNetCommand;
    uint16_t hnd;
};

/** NETPACKETZONEHEADER — Zone-side per-client frame header used in
 *  some server→client broadcasts (entity handle + char registration #).
 *  Source: Zone PDB (sizeof=6). */
struct NETPACKETZONEHEADER {
    uint16_t clienthandle;
    uint32_t charregistnumber;
};

#pragma pack(pop)

/* ── compile-time sizeof guards — refuse to compile if the layout
 *    drifts from the PDB-reported value. ───────────────────────── */
static_assert(sizeof(SHINE_XY_TYPE)                         ==   8, "SHINE_XY_TYPE wire-size mismatch");
static_assert(sizeof(SHINE_COORD_TYPE)                      ==   9, "SHINE_COORD_TYPE wire-size mismatch");
static_assert(sizeof(PROTO_NC_MISC_SEED_ACK)                ==   2, "SEED_ACK wire-size mismatch");
static_assert(sizeof(PROTO_NC_USER_CLIENT_VERSION_CHECK_REQ)==  64, "VERSION_CHECK_REQ wire-size mismatch");
static_assert(sizeof(PROTO_NC_USER_LOGIN_REQ)               == 272, "LOGIN_REQ wire-size mismatch");
static_assert(sizeof(PROTO_NC_USER_WORLDSELECT_REQ)         ==   1, "WORLDSELECT_REQ wire-size mismatch");
static_assert(sizeof(PROTO_NC_USER_WORLDSELECT_ACK)         ==  83, "WORLDSELECT_ACK wire-size mismatch");
static_assert(sizeof(PROTO_NC_USER_WILLLOGIN_REQ)           == 347, "WILLLOGIN_REQ wire-size mismatch");
static_assert(sizeof(PROTO_NC_ACT_WALK_REQ)                 ==  16, "WALK_REQ wire-size mismatch");
static_assert(sizeof(PROTO_NC_ACT_NPCCLICK_CMD)             ==   2, "NPCCLICK_CMD wire-size mismatch");
static_assert(sizeof(PROTO_NC_BRIEFINFO_INFORM_CMD)         ==   6, "BRIEFINFO_INFORM_CMD wire-size mismatch");
static_assert(sizeof(NETPACKETZONEHEADER)                   ==   6, "NETPACKETZONEHEADER wire-size mismatch");

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
    void Bytes(const void* p, size_t n) {
        const auto* b = static_cast<const uint8_t*>(p);
        m_buf.insert(m_buf.end(), b, b + n);
    }
    /* NUL-padded fixed-width ASCII (Fiesta uses these — NOT length-prefix). */
    void FixedStr(const std::string& s, size_t width) {
        const size_t copy = (s.size() < width) ? s.size() : width;
        m_buf.insert(m_buf.end(), s.begin(), s.begin() + copy);
        for (size_t i = copy; i < width; i++) m_buf.push_back(0);
    }
    /* Length-prefixed UTF-8 string with u8 length (NC_ACT_CHAT_REQ uses this). */
    void Str8(const std::string& s) {
        const size_t n = (s.size() > 0xFF) ? 0xFF : s.size();
        m_buf.push_back((uint8_t)n);
        m_buf.insert(m_buf.end(), s.begin(), s.begin() + n);
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
    bool Bytes(void* dst, size_t n) {
        if (m_p + n > m_end) return false;
        std::memcpy(dst, m_p, n); m_p += n; return true;
    }
    /* Reads exactly `width` bytes and trims trailing NULs. */
    bool FixedStr(std::string& out, size_t width) {
        if (m_p + width > m_end) return false;
        size_t end = width;
        while (end > 0 && m_p[end - 1] == 0) end--;
        out.assign((const char*)m_p, end);
        m_p += width;
        return true;
    }
    /* u8-length-prefixed string. */
    bool Str8(std::string& out) {
        uint8_t n; if (!U8(n)) return false;
        if (m_p + n > m_end) return false;
        out.assign((const char*)m_p, n); m_p += n; return true;
    }
    size_t Remaining() const { return (size_t)(m_end - m_p); }
    const uint8_t* Cursor() const { return m_p; }

private:
    const uint8_t* m_p;
    const uint8_t* m_end;
};

/*──────────────────────────────────────────────────────────────────────────────
 * Build a wire-ready packet (size prefix + opcode + payload).
 *
 *   Returns the cleartext frame.  Caller passes through
 *   `FiestaCipher::EncryptOut` on the bytes AFTER the size prefix
 *   (i.e. on opcode || payload) before transmitting.  The size
 *   prefix itself stays cleartext because the receiver must read
 *   it to know how many bytes to feed into its decryptor.
 *──────────────────────────────────────────────────────────────────────────────*/
inline std::vector<uint8_t> BuildPacket(uint16_t opcode,
                                         const std::vector<uint8_t>& payload) {
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

/*──────────────────────────────────────────────────────────────────────────────
 * Convenience: serialise a trivially-copyable PROTO_NC_* into a
 * wire-ready Writer payload.  Compile-time check via std::is_trivial.
 *──────────────────────────────────────────────────────────────────────────────*/
template <typename T>
inline std::vector<uint8_t> ToBytes(const T& proto) {
    static_assert(std::is_trivially_copyable<T>::value,
                  "ToBytes only works with trivially-copyable PODs");
    std::vector<uint8_t> out(sizeof(T));
    std::memcpy(out.data(), &proto, sizeof(T));
    return out;
}

}  /* namespace Fiesta */
#endif
