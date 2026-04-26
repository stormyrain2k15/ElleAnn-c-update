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

class Cipher {
public:
    /** A default-constructed cipher is *disabled* (no-op).  Calling
     *  Reset(seed) keys it and enables it in a single step.           */
    Cipher() = default;

    /** Re-key with a fresh u16 seed (e.g. after PROTO_NC_MISC_SEED_ACK).
     *  Both inbound and outbound stream states reset to the same seed,
     *  matching the server's per-direction symmetric initialisation.  */
    void Reset(uint16_t seed) {
        m_state_in  = (uint32_t)seed;
        m_state_out = (uint32_t)seed;
        m_enabled   = true;
    }

    /** Disable cipher (used during the very first frame on a fresh
     *  socket, before SEED_ACK arrives).                              */
    void SetEnabled(bool en) { m_enabled = en; }
    bool Enabled() const     { return m_enabled; }

    /** Encrypt outbound bytes in-place.  Advances out-direction state. */
    void EncryptOut(uint8_t* data, size_t len) {
        if (!m_enabled) return;
        for (size_t i = 0; i < len; i++) data[i] ^= NextByte(m_state_out);
    }

    /** Decrypt inbound bytes in-place.  Advances in-direction state.   */
    void DecryptIn(uint8_t* data, size_t len) {
        if (!m_enabled) return;
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
    bool     m_enabled   = false;
};

}  /* namespace Fiesta */
#endif
