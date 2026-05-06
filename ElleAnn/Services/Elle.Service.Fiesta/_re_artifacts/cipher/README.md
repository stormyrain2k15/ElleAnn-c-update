# Phase 6c — Fiesta Cipher Hunting Guide

This document is the practical instruction set for recovering the
Shine/Fiesta packet cipher algorithm from the official client binary
**without** spending hours grep-hunting through stripped IDC exports.

You have IDA Pro on your machine (the `client.idc` files prove that).
The cipher will be ~30–80 lines of decompiled C++ once you find the
right function. This doc tells you exactly what to search for and
where.

---

## TL;DR

1. Open `client.idb` in IDA Pro.
2. Auto-analyse if not already done (`Options → Compiler` → confirm
   MSVC, then `Edit → Plugins → Auto-Analyzer → Reanalyze program`).
3. Find any of the four cross-references listed below — they all
   lead to the same `cipher_encrypt` / `cipher_decrypt` pair.
4. Hex-Rays decompile both functions (F5).
5. Paste the decompiled output into a new file
   `_re_artifacts/cipher/decompiled_cipher.c`.

That's it. Total time once IDA is open: 15–30 min.

---

## Cross-references that lead to the cipher

### Method A — `connect()` import → caller chain (fastest)
The cipher is initialised one call up from where the client invokes
WinSock `connect()`.

1. In IDA, `Imports` view → double-click `connect` (from `WS2_32.dll`).
2. Right-click the imported address → `Jump to xref to operand` (X).
3. There is exactly ONE caller (the `CSocket::Connect` wrapper).
   Press F5 to decompile.
4. Look for a member-init pattern like:
   ```c
   this->cipher_state = some_constant;        // OR
   memset(this->cipher_buf, 0, 0x40);
   sub_XXXXXX(this);                          // ← this is cipher_init
   ```
5. Follow `sub_XXXXXX`. That is the **cipher key schedule**.

### Method B — `send()` import → caller (the encrypt path)
The cipher's `encrypt()` is called *immediately before* `send()`.

1. `Imports` view → `send` (from `WS2_32.dll`).
2. The wrapper that calls `send` will look like:
   ```c
   sub_AAAAAA(buf, len);     // ← cipher_encrypt
   send(this->fd, buf, len, 0);
   ```
3. F5 the wrapper. Look for the cipher_encrypt sub-call. That is
   the function we need.

### Method C — `recv()` callback → caller (the decrypt path)
ShineEngine uses overlapped I/O (`WSARecv`). The decrypt happens in
the I/O completion callback.

1. `Imports` view → `WSARecv`.
2. Trace one xref. The callback function will start with a buffer
   length check, then call the decrypt routine BEFORE dispatching
   the opcode:
   ```c
   if (bytes_received < 3) return;
   sub_BBBBBB(this->recv_buf, bytes_received);  // ← cipher_decrypt
   uint16_t opcode = *(uint16_t*)(this->recv_buf + 1);
   switch (opcode) { ... }
   ```
3. `sub_BBBBBB` is the decrypt function (or, more often, calls into
   the SAME bidirectional cipher we found via Method B).

### Method D — XOR-loop pattern scan
The cipher contains a tight loop like:
```c
for (int i = 0; i < len; ++i) {
    buf[i] ^= state;
    state = (state * mul + add) & mask;     // LCG advance
}
```
In assembly this looks like:
```asm
loop_start:
    mov   al, [esi+ecx]      ; load buf[i]
    xor   al, dl              ; XOR with cipher state low byte
    mov   [esi+ecx], al
    imul  edx, <const>        ; LCG advance
    add   edx, <const>
    inc   ecx
    cmp   ecx, ebx
    jl    loop_start
```
Use IDA's `Search → Sequence of bytes → 32 51 ... 6B D2 ...` (the
`xor / imul edx` pair). Limit to .text section. There are typically
< 5 hits and only one is the cipher.

---

## Once you have the decompiled cipher

Save it as `decompiled_cipher.c` and paste it into a new chat. With
the real algorithm in hand the headless implementation is a clean
~20-line C++ port:

```cpp
struct FiestaCipher {
    uint32_t state;
    void Init(uint16_t seed_from_NC_MISC_SEED_ACK);
    void Apply(uint8_t* buf, std::size_t len);  // same fn for E/D
};
```

Then `EncodeChatRequest()` (already in `FiestaDecoders.h`) is wrapped
as:

```cpp
void Elle::SendChat(std::string_view text) {
    auto payload = Fiesta::EncodeChatRequest(text);
    auto frame   = Fiesta::BuildPacket(opcode_NC_ACT_CHAT_REQ, payload);
    cipher_send.Apply(frame.data() + sizeFlagBytes,
                      frame.size() - sizeFlagBytes);
    socket.Send(frame);
}
```

Elle is then capable of saying `"hi"` in-game.

---

## What we already know about the cipher (from the rosetta stone)

From `_re_artifacts/wire_captures/server_side/login_session1_p9010.txt`
aligned against `_re_artifacts/wire_captures/Port_61483.txt`:

| Event | Plaintext (server view) | Encrypted (client wire) | Recovered key (first 7 B) |
|-------|------------------------|------------------------|----------------------------|
| 1     | `0C 01 D6 07 04 0A 00` | `51 01 E1 36 CB 3A 8B` | `5d 00 37 31 cf 30 8b`     |
| 4     | `0C 04 1D 33 33 42 35` | `4B 78 A5 14 35 4C E7` | `47 7c b8 27 06 0e d2`     |

**Properties already inferred** (constrains the search):

* Stream cipher — keystream length grows linearly with packet size.
* Per-packet keys are different → cipher state advances or resets
  per packet, NOT per session-start.
* No visible periodicity in 32-byte recovered keys → if it's an LCG
  the modulus is large (likely 2^32 or 2^31).
* Inbound (server → client) is plaintext on this build → the cipher
  operates only on outbound. So you only need ONE direction working
  to send chat.

These constraints mean the cipher is almost certainly:
```c
state = lcg_seed;
for (i = 0; i < len; ++i) {
    state = state * MULT + INCR;        // 32-bit LCG
    buf[i] ^= (state >> SHIFT);          // emit one byte from state
}
```
The constants `MULT`, `INCR`, `SHIFT`, and the seed source are what
the IDA hunt above will reveal in 15 minutes.

---

## Practical alternative — runtime capture

If IDA hunting feels slow, the same algorithm can be recovered with
**zero reverse engineering** by attaching `x64dbg` (or any debugger)
to a running `Fiesta.exe` and setting a conditional breakpoint on
`send`:

1. Launch Fiesta.exe and log in.
2. Attach x64dbg.
3. `bp send` → `bp recv` → resume.
4. When `send` hits, dump the buffer (encrypted view).
5. Step OUT of `send` once. The buffer pointer was prepared one
   level up — log the pre-`send` calling routine's address. That is
   the cipher_encrypt callsite. Step INTO it on the next `send`
   trigger to capture the un-encrypted buffer too.

This gives you encrypted+plaintext pairs *with* the call-trace,
which is enough to identify the cipher function even without IDA.

---

## Files in this directory after the hunt

```
_re_artifacts/cipher/
├── README.md                       (this file)
├── decompiled_cipher.c             (paste here when you have it)
└── runtime_capture_keys.txt        (optional — debugger output)
```
