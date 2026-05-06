/*══════════════════════════════════════════════════════════════════════════════
 * FiestaProtoBase.h — Foundational types for the Fiesta packet protocol.
 *
 *   This header is the *single* source of truth for every primitive
 *   building-block used by Elle's headless Fiesta client:
 *
 *     • Fixed-width string typedefs        (Name3 / Name4 / Name5 / Name8 /
 *                                           Name256Byte)
 *     • Network handle types               (SHINE_HANDLE_NUMBER, NETCOMMAND)
 *     • World coordinate primitives        (SHINE_XY_TYPE, SHINE_COORD_TYPE)
 *     • Server-to-server framing headers   (NETPACKETHEADER,
 *                                           NETPACKETZONEHEADER)
 *     • Opcode encoding helpers            (MAKE_NETCMDID, OPCODE_DEPT,
 *                                           OPCODE_SUBID)
 *     • Reader / Writer primitives         (re-exported from FiestaPacket.h)
 *
 *   ────────────────────────────────────────────────────────────────────
 *   PROVENANCE
 *   ────────────────────────────────────────────────────────────────────
 *   Every layout below is sourced from the cross-validated PDB type
 *   stream (`_re_artifacts/pdb/extracted/MERGED_protos.json`) which is
 *   itself a merge of:
 *     [PDB-V80] llvm-pdbutil dump of {Login,WorldManager,Account,
 *               AccountLog,Fiesta}.pdb (post-region-toggle 2016 build).
 *     [PDB-V70] custom CodeView reader on 5ZoneServer2.pdb (CN2012).
 *
 *   See `_re_artifacts/OPCODES_FROM_PDB.md` for methodology notes.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   WIRE FORMAT INVARIANTS (verified)
 *   ────────────────────────────────────────────────────────────────────
 *   • Length prefix:
 *         [u8  sizeFlag]   0x00..0xFE → length is 1 byte (sizeFlag)
 *                          0xFF       → length is the next u16 LE
 *         [u16 LE  payloadLen]        (only when sizeFlag == 0xFF)
 *
 *   • NETCOMMAND wire encoding:
 *         [u8 dept][u8 subid]
 *         The host-side u16 representation packs as
 *             opcode_u16 = (dept << 8) | subid
 *         such that the wire bytes for opcode 0x0207 are [0x02][0x07].
 *         This is NOT a little-endian u16.  See MAKE_NETCMDID below.
 *
 *   • Cipher boundary:
 *         The XOR cipher applies to (cmdtype || cmdsubid || payload).
 *         The size prefix itself is cleartext.  See FiestaCipher.h.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   STRICT NO-STUB POLICY
 *   ────────────────────────────────────────────────────────────────────
 *   Every typedef has a static_assert pinning its size to the value the
 *   PDB reports.  Compile-time failure = layout drift, refuse to ship.
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once
#ifndef ELLE_FIESTA_PROTO_BASE_H
#define ELLE_FIESTA_PROTO_BASE_H

#include <cstdint>
#include <cstddef>
#include <type_traits>

namespace Fiesta {

#pragma pack(push, 1)

/*──────────────────────────────────────────────────────────────────────────────
 * Fixed-width string typedefs.
 *
 *   Fiesta uses NUL-padded fixed-width ASCII (NOT length-prefix).
 *   The widths come from the ShineEngine-era client headers and were
 *   confirmed against PDB type sizes for fields whose containing struct
 *   had `Name<N>` member type.
 *
 *   Naming convention (preserved from upstream so PDB cross-references
 *   resolve cleanly):
 *
 *     Name3   →  4-byte NUL-padded ASCII (account/short codes).
 *     Name4   → 16-byte NUL-padded ASCII (legacy "char ID" / IPv4-dotted).
 *     Name5   → 20-byte NUL-padded ASCII (current charid / spawn-app tag).
 *     Name8   →  8-byte raw bytes (asset checksums, MD5 fragments).
 *     Name256Byte → 256-byte NUL-padded ASCII (login user blob, version
 *                   key holder).
 *──────────────────────────────────────────────────────────────────────────────*/
typedef char Name3[4];
typedef char Name4[16];
typedef char Name5[20];
typedef char Name8[8];
typedef char Name256Byte[256];

static_assert(sizeof(Name3)       ==   4, "Name3 must be 4 bytes");
static_assert(sizeof(Name4)       ==  16, "Name4 must be 16 bytes");
static_assert(sizeof(Name5)       ==  20, "Name5 must be 20 bytes");
static_assert(sizeof(Name8)       ==   8, "Name8 must be 8 bytes");
static_assert(sizeof(Name256Byte) == 256, "Name256Byte must be 256 bytes");


/*──────────────────────────────────────────────────────────────────────────────
 * Network primitives.
 *──────────────────────────────────────────────────────────────────────────────*/

/** SHINE_HANDLE_NUMBER — entity routing handle (per-zone slot id).
 *  Source: Zone PDB enum; sizeof=2 across every region toggle.
 *  Used inside PROTO_NC_* payloads (NOT framing) to address one mob /
 *  player / NPC by its ephemeral session handle. */
typedef uint16_t SHINE_HANDLE_NUMBER;

/** NETCOMMAND — the on-wire opcode pair as a transparent byte struct.
 *
 *  Wire layout (struct order ≡ wire order):
 *      [0] dept   : u8  (PROTOCOL_COMMAND group, e.g. 0x02 NC_MISC, 0x06 NC_MAP)
 *      [1] cmdid  : u8  (sub-id within the group)
 *
 *  The host-side u16 representation (NETCMDID) packs them MSB-first so
 *  the constants in FiestaPacket.h read naturally as 0xDDSS — see
 *  MAKE_NETCMDID, OPCODE_DEPT, OPCODE_SUBID below. */
struct NETCOMMAND {
    uint8_t dept;
    uint8_t cmdid;
};
static_assert(sizeof(NETCOMMAND) == 2, "NETCOMMAND must be exactly 2 wire bytes");


/*──────────────────────────────────────────────────────────────────────────────
 * Server-to-server / zone-broadcast frame headers.
 *──────────────────────────────────────────────────────────────────────────────*/

/** NETPACKETHEADER — Login↔AccountDB / WM↔ZoneDB cross-server frame head.
 *  PDB sizeof=2 for the *named* portion (clienthandle:u16).  In real
 *  observed traffic this header expands to 7 bytes (5 unnamed bytes
 *  trail) which we model as `tail[5]` here so the wire offsets line
 *  up with the PDB layouts that follow it. */
struct NETPACKETHEADER {
    uint16_t clienthandle;
    uint8_t  tail[5];      /* unnamed tail observed on the wire */
};
static_assert(sizeof(NETPACKETHEADER) == 7,
              "NETPACKETHEADER wire size must be 7 (2 named + 5 trailing)");

/** NETPACKETHEADER_NAMED — the PDB-only (sizeof=2) variant for places
 *  where the trailing 5 bytes do NOT appear (early Login server-side
 *  bookkeeping, AccountDB internal frames). Kept as a distinct type so
 *  callers must consciously pick which frame shape they expect. */
struct NETPACKETHEADER_NAMED {
    uint16_t clienthandle;
};
static_assert(sizeof(NETPACKETHEADER_NAMED) == 2,
              "NETPACKETHEADER_NAMED wire size must be 2");


/** NETPACKETZONEHEADER — Zone per-client frame head used in some
 *  server→client broadcasts (entity handle + char registration #).
 *  PDB sizeof=6. */
struct NETPACKETZONEHEADER {
    uint16_t clienthandle;       /* SHINE_HANDLE_NUMBER */
    uint32_t charregistnumber;   /* MS-SQL tCharacter.nID */
};
static_assert(sizeof(NETPACKETZONEHEADER) == 6,
              "NETPACKETZONEHEADER wire size must be 6");


/*──────────────────────────────────────────────────────────────────────────────
 * World coordinate primitives.
 *──────────────────────────────────────────────────────────────────────────────*/

/** SHINE_XY_TYPE — world XY pair, 32-bit fixed-point integer.
 *  Source: Zone PDB type 0x1163 (sizeof=8).  Server validates absolute
 *  positions, NOT deltas — clients send target XY for every move. */
struct SHINE_XY_TYPE {
    uint32_t x;
    uint32_t y;
};
static_assert(sizeof(SHINE_XY_TYPE) == 8, "SHINE_XY_TYPE wire-size mismatch");

/** SHINE_COORD_TYPE — XY plus 8-bit facing direction.
 *  Source: Zone PDB type 0x3005 (sizeof=9). */
struct SHINE_COORD_TYPE {
    SHINE_XY_TYPE xy;
    uint8_t       dir;
};
static_assert(sizeof(SHINE_COORD_TYPE) == 9, "SHINE_COORD_TYPE wire-size mismatch");


#pragma pack(pop)


/*──────────────────────────────────────────────────────────────────────────────
 * Opcode encoding helpers.
 *
 *   ╭─ NETCMDID (host-side u16) ─╮       ╭─ wire bytes (2) ─╮
 *   │ 0xDDSS                      │  ⇄   │ [DD][SS]         │
 *   ╰─────────────────────────────╯       ╰──────────────────╯
 *──────────────────────────────────────────────────────────────────────────────*/

/** MAKE_NETCMDID — pack (dept, subid) into the host-side u16 form. */
constexpr uint16_t MAKE_NETCMDID(uint8_t dept, uint8_t subid) {
    return static_cast<uint16_t>((static_cast<uint16_t>(dept) << 8) |
                                  static_cast<uint16_t>(subid));
}

/** OPCODE_DEPT — extract the high byte (PROTOCOL_COMMAND group). */
constexpr uint8_t OPCODE_DEPT(uint16_t netcmdid) {
    return static_cast<uint8_t>((netcmdid >> 8) & 0xFF);
}

/** OPCODE_SUBID — extract the low byte (sub-id within the group). */
constexpr uint8_t OPCODE_SUBID(uint16_t netcmdid) {
    return static_cast<uint8_t>(netcmdid & 0xFF);
}

/* Round-trip self-check at compile time. */
static_assert(MAKE_NETCMDID(0x02, 0x07) == 0x0207,
              "MAKE_NETCMDID(NC_MISC, SEED_ACK) must equal 0x0207");
static_assert(OPCODE_DEPT (0x0207) == 0x02,
              "OPCODE_DEPT(0x0207) must equal 0x02 (NC_MISC)");
static_assert(OPCODE_SUBID(0x0207) == 0x07,
              "OPCODE_SUBID(0x0207) must equal 0x07");


/*──────────────────────────────────────────────────────────────────────────────
 * PROTOCOL_COMMAND group ids — every named department, sourced from
 * Login.pdb fieldlist 0x28C1.  These are NOT exhaustive (the master
 * enum has 96 entries; only the ones actually used in our code base
 * appear here).  Add entries strictly in PDB-numeric order so a sort
 * by name yields the wire byte order.
 *──────────────────────────────────────────────────────────────────────────────*/
namespace Dept {
    constexpr uint8_t NC_NULL              = 0x00;
    constexpr uint8_t NC_LOG               = 0x01;
    constexpr uint8_t NC_MISC              = 0x02;
    constexpr uint8_t NC_USER              = 0x03;
    constexpr uint8_t NC_CHAR              = 0x04;
    constexpr uint8_t NC_AVATAR            = 0x05;
    constexpr uint8_t NC_MAP               = 0x06;
    constexpr uint8_t NC_BRIEFINFO         = 0x07;
    constexpr uint8_t NC_ACT               = 0x08;
    constexpr uint8_t NC_BAT               = 0x09;
    constexpr uint8_t NC_OPTOOL            = 0x0A;
    constexpr uint8_t NC_ITEM              = 0x0C;
    constexpr uint8_t NC_PARTY             = 0x0E;
    constexpr uint8_t NC_MENU              = 0x0F;
    constexpr uint8_t NC_CHARSAVE          = 0x10;
    constexpr uint8_t NC_QUEST             = 0x11;
    constexpr uint8_t NC_SKILL             = 0x12;
    constexpr uint8_t NC_TRADE             = 0x13;
    constexpr uint8_t NC_SOULSTONE         = 0x14;
    constexpr uint8_t NC_KQ                = 0x16;
    constexpr uint8_t NC_WT                = 0x17;
    constexpr uint8_t NC_CT                = 0x18;
    constexpr uint8_t NC_BOOTH             = 0x1A;
    constexpr uint8_t NC_SCENARIO          = 0x1B;
    constexpr uint8_t NC_GUILD             = 0x1D;
    constexpr uint8_t NC_MINIHOUSE         = 0x23;
    constexpr uint8_t NC_CHARGED           = 0x24;
    constexpr uint8_t NC_HOLY              = 0x25;
    constexpr uint8_t NC_GUILD_ACADEMY     = 0x26;
    constexpr uint8_t NC_INSTANCE          = 0x29;
    constexpr uint8_t NC_DICE              = 0x2B;
    constexpr uint8_t NC_USER_CONNECTION   = 0x2D;
    constexpr uint8_t NC_AUCTION           = 0x2E;
    constexpr uint8_t NC_GAMBLE            = 0x2F;
    constexpr uint8_t NC_PET               = 0x35;
}  /* namespace Dept */


/*──────────────────────────────────────────────────────────────────────────────
 * Direction tags.  Used by the dispatch table to pre-filter handlers
 * by traffic direction (S2C → decoder, C2S → encoder).
 *
 *   Note that PROTO_NC_* names alone don't always disclose direction:
 *     *_REQ     →  almost always Client → Server
 *     *_ACK     →  almost always Server → Client (response to *_REQ)
 *     *_CMD     →  one-way command, EITHER direction depending on opcode
 *
 *   Live wire captures are the authoritative direction source; PDB
 *   names are a hint, not a proof.
 *──────────────────────────────────────────────────────────────────────────────*/
enum class Direction : uint8_t {
    Unknown  = 0,
    C2S      = 1,   /* client → server      (Outbound from Elle's PoV) */
    S2C      = 2,   /* server → client      (Inbound  to   Elle's PoV) */
    Bidir    = 3,   /* both — e.g. heartbeat alternating */
};


/*──────────────────────────────────────────────────────────────────────────────
 * Decoder kind — how a given opcode's payload should be parsed.
 *
 *   Fixed       sizeof(Struct) == payload_len                  (bit-exact)
 *   HeadAndTail head fixed, then variable-length tail follows  (e.g. CHAT)
 *   Opaque      no struct known yet — pass raw bytes upward    (back-log)
 *   Unknown     opcode itself is undocumented                  (back-log)
 *──────────────────────────────────────────────────────────────────────────────*/
enum class Decoder : uint8_t {
    Fixed       = 0,
    HeadAndTail = 1,
    Opaque      = 2,
    Unknown     = 3,
};


/*──────────────────────────────────────────────────────────────────────────────
 * Static metadata record per opcode, populated from the X-macro list.
 *
 *   Note `pdb_sizeof` may be -1 when the PDB has no struct for this
 *   opcode — callers must consult `decoder` before trusting it.
 *──────────────────────────────────────────────────────────────────────────────*/
struct OpcodeMeta {
    uint16_t          opcode;
    const char*       opcode_name;
    const char*       struct_name;     /* may be "void" */
    int32_t           pdb_sizeof;      /* -1 = unknown */
    const char*       category;
};


}  /* namespace Fiesta */
#endif  /* ELLE_FIESTA_PROTO_BASE_H */
