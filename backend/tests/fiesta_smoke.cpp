/* Linux smoke test for the portable parts of Elle.Service.Fiesta —
 * Fiesta::Cipher (XOR stream), Fiesta::Writer/Reader (LE primitives),
 * Fiesta::BuildPacket (size-prefix framing). The Connection/Client
 * layers depend on <winsock2.h> and only build on Windows; this file
 * deliberately stops at the wire-format boundary so we can run it
 * inside the Linux container as a CI guard.
 *
 * Compile:  g++ -std=c++17 -Wall -Wextra -Werror -Iapp/ElleAnn/Services/Elle.Service.Fiesta \
 *               app/backend/tests/fiesta_smoke.cpp -o /tmp/fiesta_smoke
 */
#include "FiestaCipher.h"
#include "FiestaPacket.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>

using namespace Fiesta;

static void test_cipher_roundtrip() {
    Cipher a; Cipher b;
    const uint8_t key[16] = { 1,2,3,4,5,6,7,8, 9,10,11,12,13,14,15,16 };
    a.Reset(key, sizeof(key));
    b.Reset(key, sizeof(key));

    /* Two adjacent encrypt calls so the index is non-zero — make sure
     * the matching decrypt advances the same way. */
    std::vector<uint8_t> p1{ 0xAA, 0xBB, 0xCC, 0xDD };
    std::vector<uint8_t> p2{ 0x11, 0x22, 0x33 };
    a.EncryptOut(p1);
    a.EncryptOut(p2);

    b.DecryptIn(p1);
    b.DecryptIn(p2);

    assert(p1 == (std::vector<uint8_t>{ 0xAA, 0xBB, 0xCC, 0xDD }));
    assert(p2 == (std::vector<uint8_t>{ 0x11, 0x22, 0x33 }));
    std::printf("[ok] cipher roundtrip\n");
}

static void test_cipher_disabled_is_passthrough() {
    Cipher c(false);
    std::vector<uint8_t> bytes{ 0x10, 0x20, 0x30 };
    c.EncryptOut(bytes);
    assert((bytes == std::vector<uint8_t>{ 0x10, 0x20, 0x30 }));
    std::printf("[ok] cipher disabled passthrough\n");
}

static void test_writer_reader_primitives() {
    Writer w;
    w.U8(0xAB);
    w.U16(0x1234);
    w.U32(0xDEADBEEF);
    w.F32(3.14159f);
    w.Str("elle");

    const auto& d = w.Data();
    Reader r(d.data(), d.size());
    uint8_t  u8 = 0;  uint16_t u16 = 0;  uint32_t u32 = 0;
    float    f  = 0;  std::string s;

    assert(r.U8(u8));   assert(u8  == 0xAB);
    assert(r.U16(u16)); assert(u16 == 0x1234);
    assert(r.U32(u32)); assert(u32 == 0xDEADBEEF);
    assert(r.F32(f));   assert(f   == 3.14159f);
    assert(r.Str(s));   assert(s   == "elle");
    assert(r.Remaining() == 0);
    std::printf("[ok] writer/reader primitives\n");
}

static void test_build_short_and_long_packet() {
    /* Short body (< 0xFF) → 1-byte size prefix. */
    std::vector<uint8_t> shortPayload{ 1, 2, 3 };
    auto wire = BuildPacket(0x2010 /*MOVE_REQ*/, shortPayload);
    /* size flag = body length = 2 (opcode) + 3 (payload) = 5. */
    assert(wire.size() == 1 + 2 + 3);
    assert(wire[0] == 5);
    assert(wire[1] == 0x10);  /* opcode LO */
    assert(wire[2] == 0x20);  /* opcode HI */
    assert(wire[3] == 1 && wire[4] == 2 && wire[5] == 3);

    /* Long body (>= 0xFF) → 0xFF + u16 LE length. */
    std::vector<uint8_t> longPayload(300, 0x42);
    auto wireLong = BuildPacket(0x2210 /*CHAT_NORMAL*/, longPayload);
    assert(wireLong[0] == 0xFF);
    const size_t bodyLen = (size_t)wireLong[1] | ((size_t)wireLong[2] << 8);
    assert(bodyLen == 2 + 300);
    assert(wireLong.size() == 3 + bodyLen);
    assert(wireLong[3] == 0x10);
    assert(wireLong[4] == 0x22);
    std::printf("[ok] build packet (short + long framing)\n");
}

static void test_round_trip_through_cipher() {
    /* Build a packet, encrypt the body in-place, then frame-parse it
     * exactly the way Connection::RecvLoop does — confirm we get the
     * original opcode/payload back. This is the CRITICAL contract:
     * the size prefix must remain cleartext, only opcode+payload XOR. */
    Cipher tx; Cipher rx;
    const uint8_t key[8] = { 7, 9, 11, 13, 17, 19, 23, 29 };
    tx.Reset(key, sizeof(key));
    rx.Reset(key, sizeof(key));

    std::vector<uint8_t> payload{ 'h', 'i', '!', 0, 0, 0, 0 };
    auto wire = BuildPacket(0x2210, payload);
    const size_t headerLen = (wire[0] == 0xFF) ? 3 : 1;
    tx.EncryptOut(wire.data() + headerLen, wire.size() - headerLen);

    /* Now act like the receiver. */
    size_t bodyLen   = wire[0];
    size_t hdr       = 1;
    if (bodyLen == 0xFF) {
        bodyLen = (size_t)wire[1] | ((size_t)wire[2] << 8);
        hdr = 3;
    }
    assert(wire.size() == hdr + bodyLen);
    std::vector<uint8_t> body(wire.begin() + hdr, wire.begin() + hdr + bodyLen);
    rx.DecryptIn(body);
    uint16_t opcode = (uint16_t)(body[0] | (body[1] << 8));
    std::vector<uint8_t> recoveredPayload(body.begin() + 2, body.end());
    assert(opcode == 0x2210);
    assert(recoveredPayload == payload);
    std::printf("[ok] full encrypt-and-parse round trip\n");
}

int main() {
    test_cipher_roundtrip();
    test_cipher_disabled_is_passthrough();
    test_writer_reader_primitives();
    test_build_short_and_long_packet();
    test_round_trip_through_cipher();
    std::printf("\nfiesta_smoke: ALL PASS\n");
    return 0;
}
