/*══════════════════════════════════════════════════════════════════════════════
 * FiestaDecoders.h — Phase 6a Step 3: high-level packet decoders for the
 *                    inbound (server → client) opcodes whose wire shape
 *                    is now empirically PROVEN by paired client+server
 *                    captures.
 *
 *   Each decoder takes a raw payload buffer (pointer + length, AFTER the
 *   2-byte opcode header has been stripped by the dispatcher) and
 *   returns a fully-typed C++ struct populated from the bytes. Decoders
 *   are zero-allocation, zero-throw, single-pass, and refuse silently
 *   on short buffers (returning `false` so the dispatcher can decide
 *   whether to drop or escalate).
 *
 *   ────────────────────────────────────────────────────────────────────
 *   COVERED OPCODES (Phase 6a Step 3 scope)
 *   ────────────────────────────────────────────────────────────────────
 *
 *     0x201F  CHAT broadcast        →  DecodeChatBroadcast
 *             [Name4 sender (16)] [u8 itemLinkDataCount=0]
 *             [u8 len] [content[len]]
 *             Wire shape proven by EllaAnn vs Crystal "hi" pair
 *             (wire_captures/README.md §13).
 *
 *     0x1038  CHAR_BASE_CMD         →  DecodeCharBase
 *             [u32 chrregnum]  [char[16] charid]  [u8 0] [u8 0x96]
 *             [14×0 reserved]  [73 B character-state body]
 *             Wire shape proven by EllaAnn (uno=5) vs Crystal (uno=6)
 *             pair (wire_captures/README.md §8).
 *             N.B. 0x1038 is the build-specific opcode — same struct
 *             as PROTO_NC_CHAR_BASE_CMD (PDB names it 0x0407 in the
 *             reference build; this server's region toggle moved it).
 *
 *     0x201A  MOVEWALK broadcast    →  DecodeMoveWalk
 *             [u16 handle] [u32 fromX] [u32 fromY]
 *             [u32 toX] [u32 toY] [u8 movetype=0x32] [u16 flags]
 *             Wire shape derived from 2 122 in-session observations
 *             (wire_captures/README.md §14).
 *
 *   ────────────────────────────────────────────────────────────────────
 *   STRICT NO-STUB POLICY
 *   ────────────────────────────────────────────────────────────────────
 *   No fake/mocked branches. Every field is read from a real wire
 *   offset with a sizeof guard. Truncated buffers return `false`; we
 *   never invent data.
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once
#ifndef ELLE_FIESTA_DECODERS_H
#define ELLE_FIESTA_DECODERS_H

#include "FiestaProtoBase.h"
#include "FiestaPacket.h"

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <string_view>

namespace Fiesta {

/*══════════════════════════════════════════════════════════════════════════════
 * 0x201F — Chat broadcast (S → C)
 *══════════════════════════════════════════════════════════════════════════════*/

struct ChatBroadcast {
    char         sender[17] = {};   /* Name4 + NUL terminator for safe printf */
    uint8_t      itemLinkDataCount = 0;
    uint8_t      content_len = 0;
    std::string  content;           /* UTF-8 text body, length == content_len */
};

inline bool DecodeChatBroadcast(const uint8_t* payload,
                                std::size_t len,
                                ChatBroadcast& out) {
    /* Minimum: 16 sender + 1 itemLinks + 1 len = 18 bytes */
    if (len < 18) return false;
    std::memcpy(out.sender, payload, 16);
    out.sender[16] = '\0';
    out.itemLinkDataCount = payload[16];
    out.content_len       = payload[17];
    if (len < static_cast<std::size_t>(18) + out.content_len) return false;
    out.content.assign(reinterpret_cast<const char*>(payload + 18),
                       out.content_len);
    return true;
}

/* Symmetric encoder — used by Phase 6c when Elle wants to send chat.
 * The CLIENT sends WITHOUT the sender field (server prepends).  Build
 * the payload as `[u8 itemLinkDataCount=0][u8 len][text[len]]`. */
inline std::vector<uint8_t>
EncodeChatRequest(std::string_view text, uint8_t itemLinkDataCount = 0) {
    std::vector<uint8_t> buf;
    /* Fiesta caps chat at 0x7F bytes per packet — clip; the official
     * client does the same (client-side validation layer). */
    uint8_t n = static_cast<uint8_t>(text.size() > 0x7F ? 0x7F : text.size());
    buf.reserve(2 + n);
    buf.push_back(itemLinkDataCount);
    buf.push_back(n);
    buf.insert(buf.end(), text.begin(), text.begin() + n);
    return buf;
}

/* End-to-end: build the full wire frame for an outbound chat packet,
 * apply the XOR499 cipher to the [opcode + payload] region, and
 * return the bytes ready to write to the socket.  This is the
 * function `Elle.Service.Fiesta::OnTickSendChat` will call.
 *
 * Invariants (verified against client.asm disassembly):
 *   * Length prefix is plaintext on the wire.
 *   * Opcode (2 B) and payload are XOR-encrypted with TABLE[pos++].
 *   * The cipher state advances by exactly (2 + payload.size())
 *     bytes per packet — the table position carries across packets.
 *
 * Returns a buffer ready for `send()`:
 *   [len-prefix 1 or 3 B PLAINTEXT][opcode 2 B XOR'd][payload XOR'd]
 */
inline std::vector<uint8_t>
EncodeChatRequestEncrypted(class Cipher& cipher,
                           uint16_t opcode_chat_req,
                           std::string_view text,
                           uint8_t itemLinkDataCount = 0);
/* Implemented inline below after Cipher.h is included so the header
 * fits the existing layered-include pattern.  See FiestaDecoders.cpp
 * companion definitions if cyclic include becomes an issue. */


/*══════════════════════════════════════════════════════════════════════════════
 * 0x1038 — CHAR_BASE_CMD (S → C, build-specific opcode)
 *══════════════════════════════════════════════════════════════════════════════*/

struct CharBase {
    uint32_t chrregnum     = 0;     /* tUser.nUserNo (account number)        */
    char     charid[17]    = {};    /* Name4 + NUL term                       */
    uint8_t  pad1          = 0;     /* always 0 in observations               */
    uint8_t  marker        = 0;     /* always 0x96 in observations            */
    /* offsets 22..35 reserved (14 zero bytes) */
    /* offsets 36..96 character-state body — surfaced as raw_state for now;
     * decoded incrementally in Phase 6b as fields are individually proven. */
    const uint8_t* raw_state = nullptr;
    std::size_t    raw_state_len = 0;
};

inline bool DecodeCharBase(const uint8_t* payload,
                           std::size_t len,
                           CharBase& out) {
    /* Total wire length proven = 97 bytes for this build. */
    if (len < 22) return false;        /* head+marker minimum */
    std::memcpy(&out.chrregnum, payload, 4);
    std::memcpy(out.charid, payload + 4, 16);
    out.charid[16] = '\0';
    out.pad1   = payload[20];
    out.marker = payload[21];
    if (len > 22) {
        out.raw_state     = payload + 22;
        out.raw_state_len = len - 22;
    }
    return true;
}


/*══════════════════════════════════════════════════════════════════════════════
 * 0x201A — MOVEWALK broadcast (S → C — `someone` walks; observed 2 122x)
 *══════════════════════════════════════════════════════════════════════════════*/

struct MoveWalk {
    uint16_t handle    = 0;
    uint32_t fromX     = 0;
    uint32_t fromY     = 0;
    uint32_t toX       = 0;
    uint32_t toY       = 0;
    uint8_t  movetype  = 0;     /* 0x32 = walk in observations */
    uint16_t flags     = 0;
};

inline bool DecodeMoveWalk(const uint8_t* payload,
                           std::size_t len,
                           MoveWalk& out) {
    if (len < 21) return false;
    std::memcpy(&out.handle,   payload + 0,  2);
    std::memcpy(&out.fromX,    payload + 2,  4);
    std::memcpy(&out.fromY,    payload + 6,  4);
    std::memcpy(&out.toX,      payload + 10, 4);
    std::memcpy(&out.toY,      payload + 14, 4);
    out.movetype = payload[18];
    std::memcpy(&out.flags,    payload + 19, 2);
    return true;
}


/*══════════════════════════════════════════════════════════════════════════════
 * End-to-end encrypted chat builder.
 *
 *   Produces the FULL wire frame (length prefix + encrypted opcode +
 *   encrypted payload) for an outbound `NC_ACT_CHAT_REQ`.  Cipher
 *   state advances per byte exactly as `sub_82DB60` does in the
 *   official client.
 *
 *   ⚠  Phase 6c step 0: the cipher seed source is not yet confirmed
 *   (see _re_artifacts/cipher/README.md "What still needs
 *   calibration").  Call `cipher.Reset(0)` first to start at table
 *   position 0; if the server rejects the packet, capture
 *   `MISC_SEED_ACK` and pass its 2-byte payload to `Reset()`.
 *══════════════════════════════════════════════════════════════════════════════*/

}  /* namespace Fiesta — temporarily close so we can include cipher header */

#include "FiestaCipher.h"

namespace Fiesta {

inline std::vector<uint8_t>
EncodeChatRequestEncrypted(Cipher& cipher,
                           uint16_t opcode_chat_req,
                           std::string_view text,
                           uint8_t itemLinkDataCount) {
    /* Build the plaintext payload [items=0][len][content[len]]. */
    auto payload = EncodeChatRequest(text, itemLinkDataCount);

    /* Compose the [opcode 2 B][payload] region the cipher transforms.
     * Wire byte order is [dept = high byte][cmdid = low byte]. */
    std::vector<uint8_t> ciphered;
    ciphered.reserve(2 + payload.size());
    ciphered.push_back(static_cast<uint8_t>(opcode_chat_req >> 8));
    ciphered.push_back(static_cast<uint8_t>(opcode_chat_req & 0xFF));
    ciphered.insert(ciphered.end(), payload.begin(), payload.end());

    /* Apply the XOR499 cipher in place.  Advances `cipher`'s
     * internal position by exactly `ciphered.size()` table slots. */
    cipher.EncryptOut(ciphered.data(), ciphered.size());

    /* Prepend the length prefix.  Length is the number of
     * post-cipher bytes (opcode + payload). */
    std::vector<uint8_t> frame;
    if (ciphered.size() < 0xFF) {
        frame.reserve(1 + ciphered.size());
        frame.push_back(static_cast<uint8_t>(ciphered.size()));
    } else {
        /* Long-form prefix per Shine convention: [0xFF][u16 LE len]. */
        frame.reserve(3 + ciphered.size());
        frame.push_back(0xFF);
        frame.push_back(static_cast<uint8_t>(ciphered.size() & 0xFF));
        frame.push_back(static_cast<uint8_t>(ciphered.size() >> 8));
    }
    frame.insert(frame.end(), ciphered.begin(), ciphered.end());
    return frame;
}

}  /* namespace Fiesta */
#endif  /* ELLE_FIESTA_DECODERS_H */
