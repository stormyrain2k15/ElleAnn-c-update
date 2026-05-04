/*══════════════════════════════════════════════════════════════════════════════
 * test_fiesta_net_crypto.cpp — verifies the 499-byte XOR table cipher,
 *   FileCrypto SHN routine, and opcode pack/unpack helpers against the
 *   DragonFiesta-Rewrite reference values.
 *
 *   Compile:
 *     g++ -std=c++17 -Wall -Wextra -Werror -o test_fnc test_fiesta_net_crypto.cpp
 *   Run:
 *     ./test_fnc
 *══════════════════════════════════════════════════════════════════════════════*/
#include "/app/ElleAnn/Services/Elle.Service.Fiesta/FiestaNetCrypto.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

static int g_fails = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { std::fprintf(stderr, "FAIL: %s (%s:%d)\n", msg, __FILE__, __LINE__); g_fails++; } \
} while(0)

int main() {
    using namespace Fiesta;

    /* T1: round-trip — encrypt then decrypt with same offset = identity. */
    {
        const char* msg = "Hello Fiesta, the brown fox jumps over the lazy dog.";
        std::vector<uint8_t> a(msg, msg + std::strlen(msg));
        std::vector<uint8_t> orig = a;

        NetCryptoXor499 enc(0);
        enc.Crypt(a.data(), (int)a.size());
        CHECK(a != orig, "T1: cipher must mutate input");

        NetCryptoXor499 dec(0);
        dec.Crypt(a.data(), (int)a.size());
        CHECK(a == orig, "T1: round-trip identity");
    }

    /* T2: first-byte XOR matches s_table[0] = 0x07 for offset 0. */
    {
        std::vector<uint8_t> b = {0x00};
        NetCryptoXor499 enc(0);
        enc.Crypt(b.data(), 1);
        CHECK(b[0] == 0x07, "T2: first byte == 0x07 (table[0])");
    }

    /* T3: position advances and wraps at 499. */
    {
        std::vector<uint8_t> z(NetCryptoXor499::kTableSize + 5, 0);
        NetCryptoXor499 enc(0);
        enc.Crypt(z.data(), (int)z.size());
        CHECK(enc.GetPos() == 5, "T3: pos wraps to 5 after 499+5 bytes");
    }

    /* T4: explicit offset works. */
    {
        std::vector<uint8_t> b = {0x00};
        NetCryptoXor499 enc(1);
        enc.Crypt(b.data(), 1);
        /* table[1] == 0x59 per reference. */
        CHECK(b[0] == 0x59, "T4: offset=1 yields table[1]");
    }

    /* T5: opcode pack/unpack round-trip across the documented byte
     *     ranges (header < 0x40, type fits in 8 bits). */
    {
        for (uint8_t hdr = 0; hdr < 0x10; ++hdr) {
            for (uint16_t t = 0; t < 0x100; ++t) {
                uint16_t op = Packet::MakeOpcode(hdr, (uint8_t)t);
                CHECK(Packet::GetHeader(op) == hdr,  "T5: header round-trip");
                CHECK(Packet::GetType(op)   == (uint8_t)t, "T5: type round-trip");
            }
        }
    }

    /* T6: department constant sanity — CH3 + LOGIN = packed 0x0C38. */
    {
        uint16_t op = Packet::MakeOpcode(CH3::HEADER, CH3::LOGIN);
        CHECK(op == 0x0C38, "T6: CH3 LOGIN packs to 0x0C38");
    }

    /* T7: FileCrypto round-trip on a payload-shaped buffer (skip 36-byte
     *     header in real use; here we exercise the routine directly). */
    {
        std::vector<uint8_t> body(64);
        for (size_t i = 0; i < body.size(); ++i) body[i] = (uint8_t)(i * 17 + 3);
        std::vector<uint8_t> orig = body;
        FileCrypto::Crypt(body.data(), (int)body.size());
        CHECK(body != orig, "T7: FileCrypto must mutate");
        FileCrypto::Crypt(body.data(), (int)body.size());
        CHECK(body == orig, "T7: FileCrypto round-trip");
    }

    /* T8: explicit offset out of range throws. */
    {
        bool threw = false;
        try { NetCryptoXor499 bad(NetCryptoXor499::kTableSize); }
        catch (const std::out_of_range&) { threw = true; }
        CHECK(threw, "T8: offset >= 499 throws");
    }

    if (g_fails == 0) std::printf("ALL TESTS PASS\n");
    else              std::printf("%d test(s) FAILED\n", g_fails);
    return g_fails == 0 ? 0 : 1;
}
