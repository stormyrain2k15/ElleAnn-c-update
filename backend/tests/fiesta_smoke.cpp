/* Linux smoke test for the portable parts of Elle.Service.Fiesta —
 * Fiesta::Cipher (LCG stream), Fiesta::Writer/Reader (LE primitives),
 * Fiesta::BuildPacket (size-prefix framing), the PDB-derived
 * PROTO_NC_* PODs (sizeof + offset assertions), and the BriefInfoRing
 * handle⇆name cache.
 *
 *   The Connection/Client layers depend on <winsock2.h> and only
 *   build on Windows; this file deliberately stops at the wire-format
 *   boundary so we can run it inside the Linux container as a CI
 *   guard against protocol-level regressions.
 *
 * Compile:  g++ -std=c++17 -Wall -Wextra -Werror \
 *               -Iapp/ElleAnn/Services/Elle.Service.Fiesta \
 *               app/backend/tests/fiesta_smoke.cpp -o /tmp/fiesta_smoke
 */
#include "FiestaCipher.h"
#include "FiestaPacket.h"
#include "FiestaBriefInfoRing.h"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>

using namespace Fiesta;

/* ─── Cipher tests ────────────────────────────────────────────── */

static void test_cipher_roundtrip() {
    /* Two cipher instances seeded identically must encrypt and
     * decrypt symmetrically across multiple Send calls without
     * losing alignment. */
    Cipher a; Cipher b;
    a.Reset(0x1234);
    b.Reset(0x1234);

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
    Cipher c;   /* default-constructed → disabled */
    std::vector<uint8_t> bytes{ 0x10, 0x20, 0x30 };
    c.EncryptOut(bytes);
    assert((bytes == std::vector<uint8_t>{ 0x10, 0x20, 0x30 }));
    std::printf("[ok] cipher disabled passthrough\n");
}

static void test_cipher_lcg_first_bytes() {
    /* Validate the LCG matches the disassembly bit-exactly.  For
     * seed=0 the MSVC rand() reference sequence (state >> 16 & 0xFF)
     * produces the well-known leading bytes.  Hand-computed:
     *
     *    state0 = 0
     *    state1 = 0 * 214013 + 2531011 = 2531011  (0x269EC3)
     *             (state1 >> 16) & 0xFF = 0x26  → cipher_byte[0]
     *    state2 = 2531011 * 214013 + 2531011 = 505908858 (0x1E27E73A)
     *             (state2 >> 16) & 0xFF = 0x27  → cipher_byte[1]
     */
    Cipher c;
    c.Reset(0);
    /* Encrypt a sled of zeros — the resulting bytes ARE the cipher mask. */
    std::vector<uint8_t> mask(2, 0);
    c.EncryptOut(mask);
    assert(mask[0] == 0x26);
    assert(mask[1] == 0x27);
    std::printf("[ok] cipher LCG matches MSVC rand() (seed=0 → 0x26 0x27)\n");
}

/* ─── Writer / Reader primitives ──────────────────────────────── */

static void test_writer_reader_primitives() {
    Writer w;
    w.U8(0xAB);
    w.U16(0x1234);
    w.U32(0xDEADBEEF);
    w.FixedStr("elle", 8);

    const auto& d = w.Data();
    Reader r(d.data(), d.size());
    uint8_t  u8 = 0;  uint16_t u16 = 0;  uint32_t u32 = 0;
    std::string s;

    assert(r.U8(u8));            assert(u8  == 0xAB);
    assert(r.U16(u16));          assert(u16 == 0x1234);
    assert(r.U32(u32));          assert(u32 == 0xDEADBEEF);
    assert(r.FixedStr(s, 8));    assert(s   == "elle");
    assert(r.Remaining() == 0);
    std::printf("[ok] writer/reader primitives\n");
}

static void test_str8_roundtrip() {
    Writer w;
    w.Str8("hello world");
    Reader r(w.Data().data(), w.Data().size());
    std::string s;
    assert(r.Str8(s));
    assert(s == "hello world");
    assert(r.Remaining() == 0);
    std::printf("[ok] u8-prefixed string roundtrip\n");
}

/* ─── Packet framing ──────────────────────────────────────────── */

static void test_build_short_and_long_packet() {
    std::vector<uint8_t> shortPayload{ 1, 2, 3 };
    auto wire = BuildPacket(0x2010, shortPayload);
    assert(wire.size() == 1 + 2 + 3);
    assert(wire[0] == 5);
    assert(wire[1] == 0x10);
    assert(wire[2] == 0x20);
    assert(wire[3] == 1 && wire[4] == 2 && wire[5] == 3);

    std::vector<uint8_t> longPayload(300, 0x42);
    auto wireLong = BuildPacket(0x2210, longPayload);
    assert(wireLong[0] == 0xFF);
    const size_t bodyLen = (size_t)wireLong[1] | ((size_t)wireLong[2] << 8);
    assert(bodyLen == 2 + 300);
    assert(wireLong.size() == 3 + bodyLen);
    assert(wireLong[3] == 0x10);
    assert(wireLong[4] == 0x22);
    std::printf("[ok] build packet (short + long framing)\n");
}

static void test_round_trip_through_cipher() {
    /* The size prefix must stay cleartext — confirm decrypt aligns. */
    Cipher tx; Cipher rx;
    tx.Reset(0xC0DE);
    rx.Reset(0xC0DE);

    std::vector<uint8_t> payload{ 'h', 'i', '!', 0, 0, 0, 0 };
    auto wire = BuildPacket(0x2210, payload);
    const size_t headerLen = (wire[0] == 0xFF) ? 3 : 1;
    tx.EncryptOut(wire.data() + headerLen, wire.size() - headerLen);

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

/* ─── PROTO_NC_* layout assertions (PDB cross-check) ──────────── */

static void test_proto_layouts() {
    /* These mirror the static_asserts in FiestaPacket.h but at runtime
     * so any out-of-pragma drift surfaces here too.                  */
    assert(sizeof(SHINE_XY_TYPE)                            ==   8);
    assert(sizeof(SHINE_COORD_TYPE)                         ==   9);
    assert(sizeof(PROTO_NC_MISC_SEED_ACK)                   ==   2);
    assert(sizeof(PROTO_NC_USER_CLIENT_VERSION_CHECK_REQ)   ==  64);
    assert(sizeof(PROTO_NC_USER_LOGIN_REQ)                  == 272);
    assert(sizeof(PROTO_NC_USER_WORLDSELECT_REQ)            ==   1);
    assert(sizeof(PROTO_NC_USER_WORLDSELECT_ACK)            ==  83);
    assert(sizeof(PROTO_NC_USER_WILLLOGIN_REQ)              == 347);
    assert(sizeof(PROTO_NC_ACT_WALK_REQ)                    ==  16);
    assert(sizeof(PROTO_NC_ACT_NPCCLICK_CMD)                ==   2);
    assert(sizeof(PROTO_NC_BRIEFINFO_INFORM_CMD)            ==   6);
    assert(sizeof(NETPACKETZONEHEADER)                      ==   6);

    /* offset checks: PDB-derived field positions must be byte-exact. */
    assert(offsetof(PROTO_NC_USER_LOGIN_REQ, user)            ==   0);
    assert(offsetof(PROTO_NC_USER_LOGIN_REQ, password)        == 256);
    assert(offsetof(PROTO_NC_USER_WORLDSELECT_ACK, ip)        ==   1);
    assert(offsetof(PROTO_NC_USER_WORLDSELECT_ACK, port)      ==  17);
    assert(offsetof(PROTO_NC_USER_WORLDSELECT_ACK, validate_new) == 19);
    assert(offsetof(PROTO_NC_USER_WILLLOGIN_REQ, userid)      ==   7);
    assert(offsetof(PROTO_NC_USER_WILLLOGIN_REQ, validate_new) == 263);
    assert(offsetof(PROTO_NC_USER_WILLLOGIN_REQ, spawnapps)   == 327);
    assert(offsetof(PROTO_NC_ACT_WALK_REQ, from)              ==   0);
    assert(offsetof(PROTO_NC_ACT_WALK_REQ, to)                ==   8);
    assert(offsetof(SHINE_COORD_TYPE, dir)                    ==   8);
    assert(offsetof(NETPACKETZONEHEADER, charregistnumber)    ==   2);
    assert(offsetof(PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD_HEAD, handle) == 0);
    assert(offsetof(PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD_HEAD, charid) == 2);
    assert(offsetof(PROTO_NC_CHAR_BASE_CMD_HEAD, chrregnum)   ==   0);
    assert(offsetof(PROTO_NC_CHAR_BASE_CMD_HEAD, charid)      ==   4);
    assert(sizeof(PROTO_NC_BRIEFINFO_BRIEFINFODELETE_CMD)     ==   2);
    std::printf("[ok] PROTO_NC_* sizeof + offsetof match PDB layouts\n");
}

static void test_login_req_serialization() {
    /* Build a real LOGIN_REQ payload, send it through the same code
     * path FiestaClient uses (ToBytes), and assert byte-for-byte. */
    PROTO_NC_USER_LOGIN_REQ req{};
    std::memcpy(req.user, "elle", 4);
    std::memcpy(req.password, "secret", 6);

    auto bytes = ToBytes(req);
    assert(bytes.size() == 272);
    assert(bytes[0] == 'e' && bytes[1] == 'l' && bytes[2] == 'l' && bytes[3] == 'e');
    assert(bytes[4] == 0);                /* NUL pad */
    assert(bytes[256] == 's' && bytes[257] == 'e');
    assert(bytes[262] == 0);              /* password NUL pad */
    std::printf("[ok] LOGIN_REQ serializes 272B with correct field offsets\n");
}

static void test_walk_req_serialization() {
    PROTO_NC_ACT_WALK_REQ req{};
    req.from.x = 0x11223344;
    req.from.y = 0x55667788;
    req.to.x   = 0x99AABBCC;
    req.to.y   = 0xDDEEFF00;

    auto bytes = ToBytes(req);
    assert(bytes.size() == 16);
    /* All u32 fields are little-endian on x86 — sanity-check first u32. */
    assert(bytes[0] == 0x44 && bytes[1] == 0x33 && bytes[2] == 0x22 && bytes[3] == 0x11);
    assert(bytes[8] == 0xCC && bytes[12] == 0x00);
    std::printf("[ok] WALK_REQ serializes 16B with little-endian u32 fields\n");
}

static void test_seed_ack_payload_size_guard() {
    /* The runtime guard in OnSeedAck demands at least 2 bytes.  Build
     * a fake SEED_ACK and confirm its on-the-wire length is exactly 2.*/
    PROTO_NC_MISC_SEED_ACK ack{};
    ack.seed = 0xC0DE;
    auto bytes = ToBytes(ack);
    assert(bytes.size() == 2);
    assert(bytes[0] == 0xDE && bytes[1] == 0xC0);  /* LE u16 */
    std::printf("[ok] SEED_ACK serializes to exactly 2 bytes (LE u16 seed)\n");
}

static void test_briefinforing_lifecycle() {
    BriefInfoRing ring;
    ring.Insert(0x1234, "elle");
    ring.Insert(0x5678, "ann");
    assert(ring.Resolve(0x1234) == "elle");
    assert(ring.Resolve(0x5678) == "ann");
    assert(ring.Resolve(0x0042).empty());

    /* Update */
    ring.Insert(0x1234, "elle-ann");
    assert(ring.Resolve(0x1234) == "elle-ann");

    /* Remove */
    ring.Remove(0x5678);
    assert(ring.Resolve(0x5678).empty());
    assert(ring.Size() == 1);

    /* Clear */
    ring.Clear();
    assert(ring.Size() == 0);
    std::printf("[ok] BriefInfoRing insert/resolve/update/remove/clear\n");
}

static void test_briefinforing_capped() {
    /* Hammer the ring with > 4096 distinct handles and confirm
     * we never blow past kMaxEntries.  This is the griefer-safety
     * cap referenced in the class docstring. */
    BriefInfoRing ring;
    for (uint32_t i = 0; i < 8192; i++) {
        ring.Insert((uint16_t)(i & 0xFFFF), "x");
    }
    assert(ring.Size() <= BriefInfoRing::kMaxEntries);
    std::printf("[ok] BriefInfoRing memory cap (size=%zu, max=%zu)\n",
                ring.Size(), BriefInfoRing::kMaxEntries);
}

static void test_chat_req_outbound_layout() {
    /* Replicates the exact bytes Client::Chat() now writes:
     *   [u8 0=itemLinkDataCount][u8 len][bytes]              */
    std::string text = "hi all";
    Writer w;
    w.U8(0);
    w.U8((uint8_t)text.size());
    w.Bytes(text.data(), text.size());
    const auto& bytes = w.Data();
    assert(bytes.size() == 2 + text.size());
    assert(bytes[0] == 0);
    assert(bytes[1] == (uint8_t)text.size());
    assert(std::memcmp(bytes.data() + 2, text.data(), text.size()) == 0);
    std::printf("[ok] CHAT_REQ outbound layout = [0][len][text]\n");
}

int main() {
    test_cipher_roundtrip();
    test_cipher_disabled_is_passthrough();
    test_cipher_lcg_first_bytes();

    test_writer_reader_primitives();
    test_str8_roundtrip();

    test_build_short_and_long_packet();
    test_round_trip_through_cipher();

    test_proto_layouts();
    test_login_req_serialization();
    test_walk_req_serialization();
    test_seed_ack_payload_size_guard();

    test_briefinforing_lifecycle();
    test_briefinforing_capped();
    test_chat_req_outbound_layout();

    std::printf("\nfiesta_smoke: ALL PASS\n");
    return 0;
}
