/*══════════════════════════════════════════════════════════════════════════════
 * FiestaCipher.h — Symmetric stream cipher used by Fiesta-style MMOs.
 *
 *   Implements the well-known 32-byte-key XOR scheme used by every
 *   FiestaPS-derived server — the cipher is a per-direction stream:
 *   each emitted/received byte is XOR'd against `key[index % keylen]`,
 *   then `index` is incremented. Login server and world server use
 *   the same algorithm with different keys negotiated during the
 *   handshake.
 *
 *   The class keeps two independent counters (one per direction) so
 *   the same cipher instance can be used full-duplex without races.
 *   Mutation is internal; callers don't manage the index.
 *
 *   This is a clean original implementation of the conceptual cipher
 *   shape (length-keyed XOR stream), not a copy of any third-party
 *   server code.
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once
#ifndef ELLE_FIESTA_CIPHER_H
#define ELLE_FIESTA_CIPHER_H

#include <cstdint>
#include <cstddef>
#include <vector>
#include <array>

namespace Fiesta {

class Cipher {
public:
    /* Default key length — most Fiesta variants use 16 or 32 bytes.
     * The actual key bytes MUST be set via Reset() once the handshake
     * delivers them. The default-constructed cipher acts as a no-op
     * (key full of zeros, but the caller can also pass enabled=false).*/
    explicit Cipher(bool enabled = true) : m_enabled(enabled) {
        m_key.fill(0);
    }

    /* Replace the key and reset both stream counters. Call this when
     * the server supplies a fresh session key (e.g. after auth or on
     * world-server handoff). Key must be exactly kKeyLen bytes; pass
     * a shorter buffer and the tail is zero-padded.                   */
    void Reset(const uint8_t* keyBytes, size_t keyLen) {
        m_key.fill(0);
        const size_t copy = (keyLen < kKeyLen) ? keyLen : kKeyLen;
        for (size_t i = 0; i < copy; i++) m_key[i] = keyBytes[i];
        m_inIdx  = 0;
        m_outIdx = 0;
    }

    void SetEnabled(bool en) { m_enabled = en; }
    bool Enabled() const     { return m_enabled; }

    /* Encrypt outbound bytes in-place. Advances the outbound counter. */
    void EncryptOut(uint8_t* data, size_t len) {
        if (!m_enabled) return;
        for (size_t i = 0; i < len; i++) {
            data[i] ^= m_key[(m_outIdx + i) % kKeyLen];
        }
        m_outIdx = (m_outIdx + len) % kKeyLen;
    }

    /* Decrypt inbound bytes in-place. Advances the inbound counter.   */
    void DecryptIn(uint8_t* data, size_t len) {
        if (!m_enabled) return;
        for (size_t i = 0; i < len; i++) {
            data[i] ^= m_key[(m_inIdx + i) % kKeyLen];
        }
        m_inIdx = (m_inIdx + len) % kKeyLen;
    }

    /* Convenience overloads taking std::vector<uint8_t>.              */
    void EncryptOut(std::vector<uint8_t>& v) { if (!v.empty()) EncryptOut(v.data(), v.size()); }
    void DecryptIn(std::vector<uint8_t>& v)  { if (!v.empty()) DecryptIn(v.data(), v.size()); }

    static constexpr size_t kKeyLen = 32;

private:
    std::array<uint8_t, kKeyLen> m_key;
    bool   m_enabled = true;
    size_t m_inIdx   = 0;
    size_t m_outIdx  = 0;
};

}  /* namespace Fiesta */
#endif
