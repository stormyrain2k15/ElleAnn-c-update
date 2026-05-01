/*══════════════════════════════════════════════════════════════════════════════
 * ElleCrypto.cpp — Implementation (Windows CNG / BCrypt)
 *
 *   See ElleCrypto.h for API rationale. This file holds:
 *     • SHA-256 via BCryptHashData
 *     • HMAC-SHA-256 via BCryptHashData with BCRYPT_ALG_HANDLE_HMAC_FLAG
 *     • Secure random via BCryptGenRandom
 *     • Base64URL encode/decode (RFC 4648 §5, no padding)
 *
 *   Algorithm handles are opened lazily once per process and leaked on
 *   exit (BCryptCloseAlgorithmProvider is best-effort and typically
 *   handled by the OS cleanup). This avoids per-call open/close
 *   overhead, which is ~1 µs on modern Windows but still non-zero.
 *══════════════════════════════════════════════════════════════════════════════*/
#include "ElleCrypto.h"
#include "ElleLogger.h"

#include <windows.h>
#include <bcrypt.h>
#include <mutex>
#include <cstring>

#pragma comment(lib, "bcrypt.lib")

namespace {

/*───────────────────────────────────────────────────────────────────────────
 *  Algorithm handle cache. One BCRYPT_ALG_HANDLE per (algorithm, hmac?)
 *  combination, lazily initialised under a double-checked mutex.
 *──────────────────────────────────────────────────────────────────────────*/
BCRYPT_ALG_HANDLE g_sha256    = nullptr;
BCRYPT_ALG_HANDLE g_hmacSha256= nullptr;
BCRYPT_ALG_HANDLE g_rng       = nullptr;
std::once_flag    g_initFlag;

void InitAlgorithmsOnce() {
    std::call_once(g_initFlag, []{
        NTSTATUS st;
        st = BCryptOpenAlgorithmProvider(&g_sha256,
                                          BCRYPT_SHA256_ALGORITHM,
                                          nullptr, 0);
        if (!BCRYPT_SUCCESS(st))
            ELLE_WARN("BCryptOpenAlgorithmProvider(SHA256) NTSTATUS=0x%08X", (unsigned)st);

        st = BCryptOpenAlgorithmProvider(&g_hmacSha256,
                                          BCRYPT_SHA256_ALGORITHM,
                                          nullptr,
                                          BCRYPT_ALG_HANDLE_HMAC_FLAG);
        if (!BCRYPT_SUCCESS(st))
            ELLE_WARN("BCryptOpenAlgorithmProvider(HMAC-SHA256) NTSTATUS=0x%08X", (unsigned)st);

        st = BCryptOpenAlgorithmProvider(&g_rng,
                                          BCRYPT_RNG_ALGORITHM,
                                          nullptr, 0);
        if (!BCRYPT_SUCCESS(st))
            ELLE_WARN("BCryptOpenAlgorithmProvider(RNG) NTSTATUS=0x%08X", (unsigned)st);
    });
}

/* Base64URL alphabet per RFC 4648 §5.                                       */
constexpr char kB64UrlAlphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

}  /* anonymous namespace */

namespace ElleCrypto {

/*══════════════ SHA-256 ══════════════════════════════════════════════════════*/

bool Sha256(const void* data, size_t len, uint8_t out[32]) {
    InitAlgorithmsOnce();
    if (!g_sha256) return false;

    BCRYPT_HASH_HANDLE h = nullptr;
    NTSTATUS st = BCryptCreateHash(g_sha256, &h, nullptr, 0, nullptr, 0, 0);
    if (!BCRYPT_SUCCESS(st)) {
        ELLE_WARN("BCryptCreateHash NTSTATUS=0x%08X", (unsigned)st);
        return false;
    }
    bool ok = false;
    do {
        st = BCryptHashData(h, (PUCHAR)data, (ULONG)len, 0);
        if (!BCRYPT_SUCCESS(st)) break;
        st = BCryptFinishHash(h, out, 32, 0);
        if (!BCRYPT_SUCCESS(st)) break;
        ok = true;
    } while (0);
    BCryptDestroyHash(h);
    if (!ok) ELLE_WARN("BCryptHashData/FinishHash NTSTATUS=0x%08X", (unsigned)st);
    return ok;
}

std::string Sha256Hex(const std::string& data) {
    uint8_t digest[32];
    if (!Sha256(data.data(), data.size(), digest)) return {};
    static const char hex[] = "0123456789abcdef";
    std::string out(64, '0');
    for (size_t i = 0; i < 32; i++) {
        out[i*2]   = hex[(digest[i] >> 4) & 0xF];
        out[i*2+1] = hex[digest[i] & 0xF];
    }
    return out;
}

/*══════════════ HMAC-SHA-256 ═════════════════════════════════════════════════*/

bool HmacSha256(const void* key, size_t keyLen,
                const void* data, size_t dataLen,
                uint8_t out[32]) {
    InitAlgorithmsOnce();
    if (!g_hmacSha256) return false;

    BCRYPT_HASH_HANDLE h = nullptr;
    NTSTATUS st = BCryptCreateHash(g_hmacSha256, &h, nullptr, 0,
                                    (PUCHAR)key, (ULONG)keyLen, 0);
    if (!BCRYPT_SUCCESS(st)) {
        ELLE_WARN("BCryptCreateHash(HMAC) NTSTATUS=0x%08X", (unsigned)st);
        return false;
    }
    bool ok = false;
    do {
        st = BCryptHashData(h, (PUCHAR)data, (ULONG)dataLen, 0);
        if (!BCRYPT_SUCCESS(st)) break;
        st = BCryptFinishHash(h, out, 32, 0);
        if (!BCRYPT_SUCCESS(st)) break;
        ok = true;
    } while (0);
    BCryptDestroyHash(h);
    if (!ok) ELLE_WARN("BCryptHashData/FinishHash(HMAC) NTSTATUS=0x%08X", (unsigned)st);
    return ok;
}

/*══════════════ Secure random ════════════════════════════════════════════════*/

bool RandomBytes(void* out, size_t len) {
    InitAlgorithmsOnce();
    if (!g_rng || !out || len == 0) return false;
    NTSTATUS st = BCryptGenRandom(g_rng, (PUCHAR)out, (ULONG)len, 0);
    if (!BCRYPT_SUCCESS(st)) {
        ELLE_WARN("BCryptGenRandom NTSTATUS=0x%08X", (unsigned)st);
        return false;
    }
    return true;
}

std::string RandomDigits(uint32_t digits) {
    /* Rejection sampling via uint32 — generate a uint32, reject values
     * that would introduce modulo bias, format remainder as zero-padded
     * decimal. For digits <= 9 the mod-bias threshold is comfortably
     * inside uint32 range; for larger values rollover to string concat. */
    if (digits == 0 || digits > 18) return {};
    /* Compute 10^digits as uint64 to avoid overflow for digits up to 18. */
    uint64_t modulus = 1;
    for (uint32_t i = 0; i < digits; i++) modulus *= 10;

    /* Largest multiple of `modulus` that fits in uint64, used as the
     * rejection threshold for uniform sampling.                         */
    const uint64_t threshold = (uint64_t)(-1) - ((uint64_t)(-1) % modulus) - 1;

    uint64_t r = 0;
    for (int attempts = 0; attempts < 64; attempts++) {
        if (!RandomBytes(&r, sizeof(r))) return {};
        if (r <= threshold) break;
    }
    r %= modulus;

    std::string out(digits, '0');
    for (int i = (int)digits - 1; i >= 0; i--) {
        out[i] = (char)('0' + (r % 10));
        r /= 10;
    }
    return out;
}

std::string RandomUrlToken(size_t bytes) {
    std::vector<uint8_t> buf(bytes);
    if (!RandomBytes(buf.data(), bytes)) return {};
    return Base64UrlEncode(buf.data(), bytes);
}

std::string RandomHex(size_t bytes) {
    /* Produce `bytes` secure random bytes, return lowercase hex.
     * Used for opaque Bearer session tokens: 32 bytes ⇒ 64 hex chars
     * ⇒ 256 bits of entropy, trivially ASCII-safe in HTTP headers
     * and SQL NVARCHAR columns without escape or encode overhead.  */
    std::vector<uint8_t> buf(bytes);
    if (!RandomBytes(buf.data(), bytes)) return {};
    static const char* kHex = "0123456789abcdef";
    std::string out;
    out.resize(bytes * 2);
    for (size_t i = 0; i < bytes; i++) {
        out[i * 2 + 0] = kHex[(buf[i] >> 4) & 0xF];
        out[i * 2 + 1] = kHex[buf[i] & 0xF];
    }
    return out;
}

/*══════════════ Constant-time compare ════════════════════════════════════════*/

bool ConstantTimeEquals(const void* a, const void* b, size_t len) {
    const uint8_t* pa = (const uint8_t*)a;
    const uint8_t* pb = (const uint8_t*)b;
    uint8_t diff = 0;
    for (size_t i = 0; i < len; i++) diff |= (uint8_t)(pa[i] ^ pb[i]);
    return diff == 0;
}

/*══════════════ Base64URL ════════════════════════════════════════════════════*/

std::string Base64UrlEncode(const void* data, size_t len) {
    const uint8_t* in = (const uint8_t*)data;
    std::string out;
    out.reserve(((len + 2) / 3) * 4);
    size_t i = 0;
    while (i + 3 <= len) {
        uint32_t v = (uint32_t)in[i] << 16 | (uint32_t)in[i+1] << 8 | in[i+2];
        out.push_back(kB64UrlAlphabet[(v >> 18) & 0x3F]);
        out.push_back(kB64UrlAlphabet[(v >> 12) & 0x3F]);
        out.push_back(kB64UrlAlphabet[(v >>  6) & 0x3F]);
        out.push_back(kB64UrlAlphabet[v         & 0x3F]);
        i += 3;
    }
    if (i < len) {
        uint32_t v = (uint32_t)in[i] << 16;
        if (i + 1 < len) v |= (uint32_t)in[i+1] << 8;
        out.push_back(kB64UrlAlphabet[(v >> 18) & 0x3F]);
        out.push_back(kB64UrlAlphabet[(v >> 12) & 0x3F]);
        if (i + 1 < len) out.push_back(kB64UrlAlphabet[(v >> 6) & 0x3F]);
        /* No padding — RFC 4648 §5 "Base 64 Encoding with URL and
         * Filename Safe Alphabet". Decoders must treat length%4 as
         * meaningful.                                                  */
    }
    return out;
}

std::vector<uint8_t> Base64UrlDecode(const std::string& s) {
    /* Build a reverse table lazily. 256 entries, -1 for invalid.        */
    static int8_t rev[256];
    static std::once_flag revInit;
    std::call_once(revInit, []{
        for (int i = 0; i < 256; i++) rev[i] = -1;
        for (int i = 0; i < 64; i++) rev[(uint8_t)kB64UrlAlphabet[i]] = (int8_t)i;
    });

    std::vector<uint8_t> out;
    out.reserve((s.size() * 3) / 4);
    uint32_t buf = 0;
    int bits = 0;
    for (char ch : s) {
        int v = rev[(uint8_t)ch];
        if (v < 0) return {};               /* invalid char — fail-closed */
        buf = (buf << 6) | (uint32_t)v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back((uint8_t)((buf >> bits) & 0xFF));
        }
    }
    /* Remaining `bits` in [0..6) are the zero-pad of the final group —
     * legitimate for unpadded base64url. If any non-zero bits remain,
     * it's malformed.                                                   */
    if (bits > 0 && (buf & ((1u << bits) - 1)) != 0) return {};
    return out;
}

}  /* namespace ElleCrypto */
