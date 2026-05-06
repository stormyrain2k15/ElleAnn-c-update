/*══════════════════════════════════════════════════════════════════════════════
 * test_fiesta_cipher_calibrate.cpp — Phase 6c step 0: identify which
 * cipher family the user's Fiesta server uses by trying every seed
 * against the rosetta-stone (encrypted, plaintext) packet pair and
 * checking which produces a matching keystream.
 *
 *   Sources
 *   ────────
 *   * Encrypted bytes:   _re_artifacts/wire_captures/Port_61483.txt event 1.
 *   * Plaintext  bytes:  _re_artifacts/wire_captures/server_side/
 *                          login_session1_p9010.txt event 1.
 *
 *   Procedure
 *   ─────────
 *   For each cipher kind ∈ {LCG, XOR499}:
 *     For seed in 0..0xFFFF:
 *       Init cipher with that seed.
 *       Compute the keystream for len = 7 bytes.
 *       If keystream == enc XOR pt for those 7 bytes → match.
 *
 *   If the actual server cipher is LCG with some particular seed,
 *   we'll find ONE seed in the 64K range that produces the right
 *   keystream.  If it's XOR499 with a particular start position,
 *   we'll find one position.  If neither match, the cipher is
 *   something else — likely the per-packet variant we hypothesised
 *   in README §15.
 *
 *   Compile (Linux portable):
 *      g++ -std=c++17 -I . -I ../../Debug/_winstub \
 *          Services/Elle.Service.Fiesta/test_fiesta_cipher_calibrate.cpp \
 *          -o /tmp/test_fiesta_cipher_calibrate
 *══════════════════════════════════════════════════════════════════════════════*/

#include "FiestaProtoBase.h"
#include "FiestaCipher.h"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>

/*--- The rosetta-stone packet pair (Port 61483 event 1 client+server) ------*/
/* Encrypted bytes seen on the wire (client side, post-cipher).
 * NOTE: client opcode = 0x5101, plaintext opcode = 0x0C01.  Wire
 * order is [opcode_high][opcode_low][payload], so the bytes the
 * cipher transformed are:
 *     enc:  51 01 E1 36 CB 3A 8B
 *     pt:   0C 01 D6 07 04 0A 00
 * Recovered keystream = enc XOR pt = 5D 00 37 31 CF 30 8B           */
static const uint8_t kEncrypted[] = { 0x51, 0x01, 0xE1, 0x36, 0xCB, 0x3A, 0x8B };
static const uint8_t kPlaintext[] = { 0x0C, 0x01, 0xD6, 0x07, 0x04, 0x0A, 0x00 };
static_assert(sizeof(kEncrypted) == sizeof(kPlaintext), "");
static constexpr std::size_t kLen = sizeof(kEncrypted);

static const uint8_t kExpectedKeystream[kLen] = {
    0x5D, 0x00, 0x37, 0x31, 0xCF, 0x30, 0x8B
};


/* Compute the keystream of length `kLen` produced by a fresh Cipher
 * keyed by `seed` and `kind`.  Returns true if the keystream matches
 * `kExpectedKeystream` byte-for-byte.                                */
static bool TryCipher(Fiesta::CipherKind kind, uint16_t seed,
                      uint8_t out_keystream[kLen]) {
    Fiesta::Cipher c;
    c.SetKind(kind);
    c.Reset(seed);

    /* Apply the cipher to a zero buffer — the result IS the
     * keystream because 0 XOR k = k. */
    uint8_t buf[kLen] = {0};
    c.EncryptOut(buf, kLen);

    std::memcpy(out_keystream, buf, kLen);
    return std::memcmp(buf, kExpectedKeystream, kLen) == 0;
}

int main() {
    std::printf("──── Phase 6c step 0 — cipher calibration ────\n");
    std::printf("Wire enc bytes : ");
    for (auto b : kEncrypted) { std::printf("%02X ", b); }
    std::printf("\n");
    std::printf("Plaintext      : ");
    for (auto b : kPlaintext) { std::printf("%02X ", b); }
    std::printf("\n");
    std::printf("Expected mask  : ");
    for (auto b : kExpectedKeystream) { std::printf("%02X ", b); }
    std::printf("\n\n");

    int total_matches = 0;

    /* Try LCG family — full 64K seed space. */
    std::printf("Scanning LCG seeds 0..0xFFFF...\n");
    for (uint32_t seed = 0; seed <= 0xFFFF; ++seed) {
        uint8_t ks[kLen];
        if (TryCipher(Fiesta::CipherKind::LCG, (uint16_t)seed, ks)) {
            std::printf("  ★ MATCH  LCG    seed=0x%04X\n", seed);
            ++total_matches;
        }
    }

    /* Try XOR499 family. */
    std::printf("Scanning XOR499 seeds 0..0xFFFF...\n");
    for (uint32_t seed = 0; seed <= 0xFFFF; ++seed) {
        uint8_t ks[kLen];
        if (TryCipher(Fiesta::CipherKind::XOR499, (uint16_t)seed, ks)) {
            std::printf("  ★ MATCH  XOR499 seed=0x%04X\n", seed);
            ++total_matches;
        }
    }

    /* Diagnostics — show the first few keystream bytes from each
     * cipher with seed=0 to confirm the cipher is producing data. */
    std::printf("\nDiagnostic: first 8 keystream bytes per cipher (seed=0):\n");
    for (auto kind : {Fiesta::CipherKind::LCG, Fiesta::CipherKind::XOR499}) {
        Fiesta::Cipher c;
        c.SetKind(kind);
        c.Reset(0);
        uint8_t buf[8] = {0};
        c.EncryptOut(buf, 8);
        std::printf("  %s : ",
                    kind == Fiesta::CipherKind::LCG ? "LCG   " : "XOR499");
        for (auto b : buf) { std::printf("%02X ", b); }
        std::printf("\n");
    }

    std::printf("\nResult: %d cipher/seed combinations match the rosetta stone.\n",
                total_matches);
    if (total_matches == 0) {
        std::printf("→ Neither in-tree cipher matches.  The user's server\n"
                    "  uses a different family — see\n"
                    "  _re_artifacts/cipher/README.md for IDA hunt steps.\n");
        return 1;
    }
    return 0;
}
