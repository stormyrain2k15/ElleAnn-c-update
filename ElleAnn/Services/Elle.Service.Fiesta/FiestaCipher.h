/*══════════════════════════════════════════════════════════════════════════════
 * FiestaCipher.h — ShineEngine packet stream cipher.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   ALGORITHM (verified from disassembly of CN2012 5ZoneServer2.exe)
 *   ────────────────────────────────────────────────────────────────────
 *   ShineEngine uses a Linear Congruential Generator (LCG) keyed by a
 *   single u16 seed delivered via PROTO_NC_MISC_SEED_ACK at session
 *   start.  Per-direction state advances independently; both sides
 *   advance their state by the byte count of every cipher-bounded
 *   transmission they make.
 *
 *   The LCG uses Microsoft Visual C++ runtime's `rand()` constants —
 *   confirmed at file offset 0x1FE4F4 of `5ZoneServer2.exe`:
 *
 *       imul ecx, ecx, 0x000343FD     ; ecx *= 214013
 *       add  ecx, 0x00269EC3          ; ecx += 2531011
 *       mov  [eax+0x14], ecx          ; persist state
 *       mov  eax, ecx
 *       shr  eax, 16                  ; >>= 16
 *       and  eax, 0x7FFF              ; &= 0x7FFF
 *       ret
 *
 *   Each cipher byte = low 8 bits of `(state >> 16) & 0x7FFF` AFTER
 *   stepping the state.  Plaintext byte is XOR'd against this mask.
 *   The same routine is used for encrypt and decrypt (XOR is involutive).
 *
 *   ────────────────────────────────────────────────────────────────────
 *   SEED SCOPING — "continuous stream, NOT per-packet reset"
 *   ────────────────────────────────────────────────────────────────────
 *   The cipher state in `CLoginClientSession::m_PacketEncryptor` (offset
 *   268, single u32 holding the seed/state) is initialised once on
 *   `so_EncSeedSet` and never reset until the connection is dropped.
 *   `Send_NC_MISC_SEED_ACK` is the only known seed-delivery path.
 *   Every login/world/zone hop establishes a fresh socket with its OWN
 *   seed — this class is keyed once per Connection lifetime.
 *
 *   ────────────────────────────────────────────────────────────────────
 *   STATUS
 *   ────────────────────────────────────────────────────────────────────
 *   🟢 PASS — algorithm matches the disassembled `imul/add/shr/and`
 *   sequence bit-exactly.  Byte ordering and stream-direction are
 *   covered by `tests/cipher_self_test.cpp` (validates the first 16
 *   stream bytes for seed=0x4321 against the LCG hand-computation).
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once
#ifndef ELLE_FIESTA_CIPHER_H
#define ELLE_FIESTA_CIPHER_H

#include <cstdint>
#include <cstddef>
#include <vector>

namespace Fiesta {

/* Floating cipher selector — flipped from `elle_settings.lua`
 * (`ElleAnn.fiesta.region`).  The runtime picks a backend at session
 * start; once chosen, the cipher state lives until the next handshake.
 *
 *   LCG     — CN2012 fork (5ZoneServer2.exe, MSVC rand-LCG).
 *   XOR499  — DragonFiesta-Rewrite (zepheus_fiesta 2012).
 *
 * Default is LCG so historical builds keep their behavior; the Fiesta
 * service explicitly calls `SetCipherKind(XOR499)` for the "usa" region. */
enum class CipherKind : uint8_t { LCG = 0, XOR499 = 1 };

class Cipher {
public:
    /** A default-constructed cipher is *disabled* (no-op).  Calling
     *  Reset(seed) keys it and enables it in a single step.           */
    Cipher() = default;

    /** Pick the cipher family BEFORE any traffic is exchanged.  Switching
     *  mid-stream is undefined; call `Reset()` afterward to install the
     *  initial state for the new family.                                */
    void SetKind(CipherKind k) { m_kind = k; }
    CipherKind Kind() const    { return m_kind; }

    /** Re-key with a fresh u16 seed (e.g. after PROTO_NC_MISC_SEED_ACK).
     *  Both inbound and outbound stream states reset to the same seed,
     *  matching the server's per-direction symmetric initialisation.
     *
     *  XOR499 family ignores the seed's high byte — only the low 9 bits
     *  matter (table is 499 bytes ≤ 512).                                */
    void Reset(uint16_t seed) {
        m_state_in  = (uint32_t)seed;
        m_state_out = (uint32_t)seed;
        m_pos_in    = (int16_t)(seed % 499);
        m_pos_out   = (int16_t)(seed % 499);
        m_lastSeed  = seed;
        m_enabled   = true;
    }

    /** Last seed value handed to Reset() — surfaced on /api/diag/fiesta
     *  so the operator can spot mismatched seeds without log diving. */
    uint16_t LastSeed() const { return m_lastSeed; }

    /** Disable cipher (used during the very first frame on a fresh
     *  socket, before SEED_ACK arrives).                              */
    void SetEnabled(bool en) { m_enabled = en; }
    bool Enabled() const     { return m_enabled; }

    /** Encrypt outbound bytes in-place.  Advances out-direction state. */
    void EncryptOut(uint8_t* data, size_t len) {
        if (!m_enabled) return;
        if (m_kind == CipherKind::XOR499) {
            for (size_t i = 0; i < len; ++i) {
                data[i] ^= kXor499Table[m_pos_out];
                if (++m_pos_out == 499) m_pos_out = 0;
            }
            return;
        }
        for (size_t i = 0; i < len; i++) data[i] ^= NextByte(m_state_out);
    }

    /** Decrypt inbound bytes in-place.  Advances in-direction state.   */
    void DecryptIn(uint8_t* data, size_t len) {
        if (!m_enabled) return;
        if (m_kind == CipherKind::XOR499) {
            for (size_t i = 0; i < len; ++i) {
                data[i] ^= kXor499Table[m_pos_in];
                if (++m_pos_in == 499) m_pos_in = 0;
            }
            return;
        }
        for (size_t i = 0; i < len; i++) data[i] ^= NextByte(m_state_in);
    }

    void EncryptOut(std::vector<uint8_t>& v) { if (!v.empty()) EncryptOut(v.data(), v.size()); }
    void DecryptIn(std::vector<uint8_t>& v)  { if (!v.empty()) DecryptIn(v.data(),  v.size()); }

    /* Test-helper: peek the next byte in either direction without
     * consuming.  Used by `cipher_self_test.cpp`.                    */
    uint8_t PeekOut() const { uint32_t s = m_state_out; return PeekByte(s); }
    uint8_t PeekIn()  const { uint32_t s = m_state_in;  return PeekByte(s); }

    /* MSVC rand() LCG constants (verified by disassembly). */
    static constexpr uint32_t kLcgMul = 214013u;     /* 0x000343FDu */
    static constexpr uint32_t kLcgAdd = 2531011u;    /* 0x00269EC3u */

private:
    /** Advance `state` one step and return the cipher byte.
     *
     *  Implementation:  state = state * 214013 + 2531011
     *                   mask  = (state >> 16) & 0x7FFF
     *                   byte  = mask & 0xFF                          */
    static uint8_t NextByte(uint32_t& state) {
        state = state * kLcgMul + kLcgAdd;
        return (uint8_t)((state >> 16) & 0xFF);
    }

    /** Peek without consuming. */
    static uint8_t PeekByte(uint32_t state) {
        state = state * kLcgMul + kLcgAdd;
        return (uint8_t)((state >> 16) & 0xFF);
    }

    uint32_t m_state_in  = 0;
    uint32_t m_state_out = 0;
    int16_t  m_pos_in    = 0;       /* XOR499 inbound table position  */
    int16_t  m_pos_out   = 0;       /* XOR499 outbound table position */
    uint16_t m_lastSeed  = 0;
    bool     m_enabled   = false;
    CipherKind m_kind    = CipherKind::LCG;

    /* The 499-byte XOR table (DragonFiesta-Rewrite canonical).  Same
     * table that lives in FiestaNetCrypto.h — duplicated here as a
     * static constexpr-equivalent so the existing single-file Cipher
     * implementation needs no additional .cpp.  The two MUST stay
     * byte-identical; if you change one, change both.                  */
    static const uint8_t kXor499Table[499];
};

/* Out-of-class definition — header-only library, single TU instantiation
 * via inline.                                                            */
inline const uint8_t Cipher::kXor499Table[499] = {
    0x07,0x59,0x69,0x4A,0x94,0x11,0x94,0x85,0x8C,0x88,0x05,0xCB,0xA0,0x9E,0xCD,0x58,
    0x3A,0x36,0x5B,0x1A,0x6A,0x16,0xFE,0xBD,0xDF,0x94,0x02,0xF8,0x21,0x96,0xC8,0xE9,
    0x9E,0xF7,0xBF,0xBD,0xCF,0xCD,0xB2,0x7A,0x00,0x9F,0x40,0x22,0xFC,0x11,0xF9,0x0C,
    0x2E,0x12,0xFB,0xA7,0x74,0x0A,0x7D,0x78,0x40,0x1E,0x2C,0xA0,0x2D,0x06,0xCB,0xA8,
    0xB9,0x7E,0xEF,0xDE,0x49,0xEA,0x4E,0x13,0x16,0x16,0x80,0xF4,0x3D,0xC2,0x9A,0xD4,
    0x86,0xD7,0x94,0x24,0x17,0xF4,0xD6,0x65,0xBD,0x3F,0xDB,0xE4,0xE1,0x0F,0x50,0xF6,
    0xEC,0x7A,0x9A,0x0C,0x27,0x3D,0x24,0x66,0xD3,0x22,0x68,0x9C,0x9A,0x52,0x0B,0xE0,
    0xF9,0xA5,0x0B,0x25,0xDA,0x80,0x49,0x0D,0xFD,0x3E,0x77,0xD1,0x56,0xA8,0xB7,0xF4,
    0x0F,0x9B,0xE8,0x0F,0x52,0x47,0xF5,0x6F,0x83,0x20,0x22,0xDB,0x0F,0x0B,0xB1,0x43,
    0x85,0xC1,0xCB,0xA4,0x0B,0x02,0x19,0xDF,0xF0,0x8B,0xEC,0xDB,0x6C,0x6D,0x66,0xAD,
    0x45,0xBE,0x89,0x14,0x7E,0x2F,0x89,0x10,0xB8,0x93,0x60,0xD8,0x60,0xDE,0xF6,0xFE,
    0x6E,0x9B,0xCA,0x06,0xC1,0x75,0x95,0x33,0xCF,0xC0,0xB2,0xE0,0xCC,0xA5,0xCE,0x12,
    0xF6,0xE5,0xB5,0xB4,0x26,0xC5,0xB2,0x18,0x4F,0x2A,0x5D,0x26,0x1B,0x65,0x4D,0xF5,
    0x45,0xC9,0x84,0x14,0xDC,0x7C,0x12,0x4B,0x18,0x9C,0xC7,0x24,0xE7,0x3C,0x64,0xFF,
    0xD6,0x3A,0x2C,0xEE,0x8C,0x81,0x49,0x39,0x6C,0xB7,0xDC,0xBD,0x94,0xE2,0x32,0xF7,
    0xDD,0x0A,0xFC,0x02,0x01,0x64,0xEC,0x4C,0x94,0x0A,0xB1,0x56,0xF5,0xC9,0xA9,0x34,
    0xDE,0x0F,0x38,0x27,0xBC,0x81,0x30,0x0F,0x7B,0x38,0x25,0xFE,0xE8,0x3E,0x29,0xBA,
    0x55,0x43,0xBF,0x6B,0x9F,0x1F,0x8A,0x49,0x52,0x18,0x7F,0x8A,0xF8,0x88,0x24,0x5C,
    0x4F,0xE1,0xA8,0x30,0x87,0x8E,0x50,0x1F,0x2F,0xD1,0x0C,0xB4,0xFD,0x0A,0xBC,0xDC,
    0x12,0x85,0xE2,0x52,0xEE,0x4A,0x58,0x38,0xAB,0xFF,0xC6,0x3D,0xB9,0x60,0x64,0x0A,
    0xB4,0x50,0xD5,0x40,0x89,0x17,0x9A,0xD5,0x85,0xCF,0xEC,0x0D,0x7E,0x81,0x7F,0xE3,
    0xC3,0x04,0x01,0x22,0xEC,0x27,0xCC,0xFA,0x3E,0x21,0xA6,0x54,0xC8,0xDE,0x00,0xB6,
    0xDF,0x27,0x9F,0xF6,0x25,0x34,0x07,0x85,0xBF,0xA7,0xA5,0xA5,0xE0,0x83,0x0C,0x3D,
    0x5D,0x20,0x40,0xAF,0x60,0xA3,0x64,0x56,0xF3,0x05,0xC4,0x1C,0x7D,0x37,0x98,0xC3,
    0xE8,0x5A,0x6E,0x58,0x85,0xA4,0x9A,0x6B,0x6A,0xF4,0xA3,0x7B,0x61,0x9B,0x09,0x40,
    0x1E,0x60,0x4B,0x32,0xD9,0x51,0xA4,0xFE,0xF9,0x5D,0x4E,0x4A,0xFB,0x4A,0xD4,0x7C,
    0x33,0x02,0x33,0xD5,0x9D,0xCE,0x5B,0xAA,0x5A,0x7C,0xD8,0xF8,0x05,0xFA,0x1F,0x2B,
    0x8C,0x72,0x57,0x50,0xAE,0x6C,0x19,0x89,0xCA,0x01,0xFC,0xFC,0x29,0x9B,0x61,0x12,
    0x68,0x63,0x65,0x46,0x26,0xC4,0x5B,0x50,0xAA,0x2B,0xBE,0xEF,0x9A,0x79,0x02,0x23,
    0x75,0x2C,0x20,0x13,0xFD,0xD9,0x5A,0x76,0x23,0xF1,0x0B,0xB5,0xB8,0x59,0xF9,0x9F,
    0x7A,0xE6,0x06,0xE9,0xA5,0x3A,0xB4,0x50,0xBF,0x16,0x58,0x98,0xB3,0x9A,0x6E,0x36,
    0xEE,0x8D,0xEB
};

}  /* namespace Fiesta */
#endif
