/* sub_82DB60 — the Fiesta client outbound cipher, recovered from
 * client.idb (47 MB IDA database) via client.asm disassembly.
 *
 * RVA: 0x82DB60.  Calling convention: __thiscall (ecx = this).
 * Args (stack):
 *     arg_0 = uint8_t* data
 *     arg_4 = int      len
 * Returns: pointer to data (eax)
 *
 * The cipher state lives at offset 0 of `this`:
 *     u16 m_pos_out;       // offset 0  — position into the 499-byte table
 *     u16 m_pos_unused;    // offset 2  — second copy from sub_7FCB90;
 *                                          NOT read by sub_82DB60
 *
 * Algorithm: byte-by-byte XOR with `byte_9119D0` (a 499-byte
 * constant table); position wraps at 499.
 *
 * The 499-byte table is byte-identical to `Fiesta::kXor499Table` in
 * `Services/Elle.Service.Fiesta/FiestaCipher.h` — the in-tree
 * implementation is correct.
 */

#include <cstdint>

extern uint8_t byte_9119D0[499];   // global cipher table

struct FiestaSocket {
    uint16_t pos_out;       // offset 0 — read+advanced by cipher
    uint16_t pos_unused;    // offset 2 — written by seed init; never read
    /* ... rest of CSocket ... */
};

void* __thiscall sub_82DB60(FiestaSocket* this_, uint8_t* data, int len) {
    int i = 0;
    if (len > 0) {
        do {
            uint8_t k = byte_9119D0[this_->pos_out];
            data[i] ^= k;
            this_->pos_out = (uint16_t)(this_->pos_out + 1);
            if (this_->pos_out >= 0x1F3 /* 499 */) {
                this_->pos_out = 0;
            }
            ++i;
        } while (i < len);
    }
    return data;
}

/* sub_7FCB90 — cipher seed initialiser (RVA 0x7FCB90).
 * Sets pos_out and pos_unused to (seed % 499).  The seed source has
 * not yet been traced (likely the 2-byte payload of
 * `PROTO_NC_MISC_SEED_ACK`, opcode 0x0207).
 */
void __thiscall sub_7FCB90(FiestaSocket* this_, uint16_t seed) {
    uint16_t pos = (uint16_t)(seed % 499);
    this_->pos_out    = pos;
    this_->pos_unused = pos;
}
