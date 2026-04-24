/*══════════════════════════════════════════════════════════════════════════════
 * ElleCrypto.h — Production crypto primitives via Windows CNG (BCrypt)
 *
 *   All-in-one cover for the crypto needs of the auth + IPC security surface:
 *     • SHA-256          — general digest (file fingerprints, id hashing)
 *     • HMAC-SHA-256     — JWT signing + pairing-code MAC
 *     • Secure random    — pairing-code generation, JWT nonces, device IDs
 *     • Constant-time compare — for MAC / tag comparisons (timing-safe)
 *     • Base64URL        — for JWT / URL-safe token encoding
 *
 *   Why Windows CNG (BCrypt) instead of the Elle.ASM.Crypto MASM DLL?
 *     The MASM SHA-256 + AES-256 are scaffolds (see ASM/Elle.ASM.Crypto/
 *     Crypto.asm header). BCrypt is:
 *       - FIPS 140-2 validated
 *       - Part of Windows since Vista (no dependency to install)
 *       - Hardware-accelerated (SHA-NI / AES-NI) by the OS where available
 *       - Thread-safe, multi-call-per-process safe
 *     When a real MASM implementation is benchmarked and proven faster
 *     on the target hardware, callers can swap at the ElleCrypto level
 *     without touching any consumer.
 *
 *   Error policy:
 *     Every function returns bool. `true` = success with result written to
 *     the out-parameter. `false` = BCrypt error OR invalid-argument. The
 *     BCrypt NTSTATUS is logged at WARN level via ELLE_WARN with the
 *     function name; callers should fail-closed on false.
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ElleCrypto {

/*──────── Hashing ────────────────────────────────────────────────────────────*/

/** SHA-256. `out` must be exactly 32 bytes. Returns true on success.         */
bool Sha256(const void* data, size_t len, uint8_t out[32]);

/** SHA-256 convenience: operate on a std::string and return hex digest.      */
std::string Sha256Hex(const std::string& data);

/*──────── HMAC-SHA-256 ───────────────────────────────────────────────────────*/

/** HMAC-SHA-256. `out` must be exactly 32 bytes.                             */
bool HmacSha256(const void* key, size_t keyLen,
                const void* data, size_t dataLen,
                uint8_t out[32]);

/*──────── Secure random ──────────────────────────────────────────────────────*/

/** Fill `out[0..len)` with cryptographically secure random bytes.            */
bool RandomBytes(void* out, size_t len);

/** Generate a zero-padded N-digit decimal string (e.g. "429156").
 *  Uses rejection sampling so the distribution is uniform — no modulo bias. */
std::string RandomDigits(uint32_t digits);

/** Generate a URL-safe random token of exactly `bytes` bytes, base64url-
 *  encoded. Suitable for device_id / session nonce.                          */
std::string RandomUrlToken(size_t bytes);

/*──────── Constant-time compare (timing-safe) ────────────────────────────────*/

/** Compare two byte buffers in constant time relative to the compared length.
 *  Returns true iff a and b have identical bytes over [0..len). Use this
 *  for MAC / tag / signature comparisons where early-exit memcmp would
 *  leak timing information.                                                  */
bool ConstantTimeEquals(const void* a, const void* b, size_t len);

/*──────── Base64URL (no padding) ─────────────────────────────────────────────*/

/** Base64URL-encode (RFC 4648 §5). No padding, no line breaks.               */
std::string Base64UrlEncode(const void* data, size_t len);

/** Base64URL-decode. Returns empty vector on malformed input. Caller can
 *  detect malformed by comparing result.size() to the expected size.         */
std::vector<uint8_t> Base64UrlDecode(const std::string& s);

}  /* namespace ElleCrypto */
