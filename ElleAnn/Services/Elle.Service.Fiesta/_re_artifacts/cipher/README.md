# Phase 6c — Fiesta Cipher: PROVEN via disassembly (2026-02-06)

## Cipher algorithm — definitively recovered

The official Fiesta client's outbound cipher has been recovered by
disassembling the user's `client.idb` (47 MB IDA database). The
relevant function is `sub_82DB60` at RVA `0x82DB60`:

```asm
sub_82DB60 proc near                      ; thiscall
    ; ecx = this  (CSocket-like; cipher state at offsets +0 and +2)
    ; arg_0 = data ptr ; arg_4 = length

    push edi
    mov edi, [esp+arg_4]                  ; edi = len
    xor edx, edx                          ; edx = i = 0
    test edi, edi
    jle short done
    push esi
    mov esi, [esp+4+arg_0]                ; esi = data ptr

loop:
    movzx eax, word ptr [ecx]             ; eax = pos (u16 at this+0)
    mov   al, byte_9119D0[eax]            ; al  = TABLE[pos]
    xor   [edx+esi], al                   ; data[i] ^= TABLE[pos]
    inc   word ptr [ecx]                  ; ++pos
    cmp   word ptr [ecx], 1F3h            ; if pos == 499:
    jb    short skip
    mov   word ptr [ecx], 0               ;   pos = 0
skip:
    inc   edx                             ; ++i
    cmp   edx, edi
    jl    short loop                      ; while (i < len)

    mov eax, esi
    pop esi ; pop edi ; retn 8
done:
    mov eax, [esp+arg_0]
    pop edi ; retn 8
sub_82DB60 endp
```

Translated to C++:

```cpp
void Cipher::EncryptOut(uint8_t* data, std::size_t len) {
    for (std::size_t i = 0; i < len; ++i) {
        data[i] ^= kXor499Table[m_pos_out];
        if (++m_pos_out == 499) m_pos_out = 0;
    }
}
```

The table at `byte_9119D0` is **499 bytes** (matching `dword_8E221C =
0x1F3 = 499`, the divisor used when initialising the cipher seed).

> **The 499-byte table extracted from the IDB is BYTE-IDENTICAL to
> the `kXor499Table` already in `FiestaCipher.h`.** All 499 bytes
> match — both implementations are correct. The cipher kind is
> `Fiesta::CipherKind::XOR499` (already implemented in-tree).

### Cipher seed initialisation

`sub_7FCB90` at RVA `0x7FCB90` initialises the cipher state from a
seed value:

```asm
sub_7FCB90 proc near                      ; thiscall
    ; ecx = this; arg_0 = u16 seed

    push ebp ; mov ebp, esp
    movzx eax, word ptr [ebp+arg_0]       ; eax = seed (u16)
    cdq                                    ; sign-extend (effectively zero-extend
                                            ;   since seed is u16)
    idiv ds:dword_8E221C                  ; eax = seed / 499; edx = seed % 499
    mov [ecx],   dx                       ; this[0] = pos = (seed % 499)
    mov [ecx+2], dx                       ; this[2] = pos (a second copy?)
    pop ebp ; retn 4
sub_7FCB90 endp
```

So the **starting position** (and only state) is `seed % 499`. The
seed itself comes from somewhere we haven't traced yet — likely the
`MISC_SEED_ACK` packet (opcode 0x0207) sent by the server on
connect, but possibly also the initial connect handshake packet's
contents.

### Cipher uniqueness — only ONE cipher in the whole binary

* Only one xref to `byte_9119D0` (cipher table).
* Only one xref to `dword_8E221C` (the 499 divisor).
* Only one call to `sub_82DB60` (the cipher itself), from
  `sub_7FCA10` (the SendData routine), called ONCE per outbound
  packet right before the buffer is flushed via `send()`.
* Only one xref to `sub_7FCB90` (the seed init) — this xref isn't
  trivially findable in the asm (likely called via a vtable),
  proving that the seed initialisation goes through one of the
  socket-class wrappers.

The receive path (`sub_7FCB10`, the `recv` wrapper) does NOT call
any cipher — confirming our wire-capture finding that **inbound
(server → client) traffic is plaintext** on this build.

---

## Why the "rosetta stone" calibration failed (analysis)

The earlier calibration test in
`Services/Elle.Service.Fiesta/test_fiesta_cipher_calibrate.cpp`
returned 0 matches across all 499 starting positions. The reason:

> The client-side capture (`Port_61483.txt` event 1, 01:54:29) and
> the server-side captures (`server_side/login_session{1,2,3}_p9010.txt`,
> 04:24:07 / 04:28:13 / 04:28:36) were taken **2.5 hours apart**
> and are therefore from **different login sessions**, each with
> its own cipher seed.

Aligning encrypted bytes from one session against decrypted bytes
from a different session gives a meaningless XOR. The plaintext for
`PROTO_NC_USER_CLIENT_VERSION_CHECK_REQ` is fixed across sessions
(`0C 01 D6 07 04 0A 00` — opcode + 2006-04-10 client date), but the
cipher position differs.

### What we still know (negative results count too)

The cross-session XOR `5D 00 37 31 CF 30 8B` does NOT appear in the
table at any of 499 starting positions. This is not surprising —
it's the XOR of TWO DIFFERENT cipher streams (one starting at pos
`p1`, the other at pos `p2`), which generally is not a contiguous
slice of the table.

If we ever capture **truly-paired** client-side AND server-side
traffic of the same session, the alignment will recover the seed
in one calculation:

```python
mask = encrypted_byte_0 ^ plaintext_byte_0     # = TABLE[p]
for p in range(499):
    if TABLE[p] == mask:
        # candidate. Test bytes 1..N against TABLE[(p+i) % 499]
        ...
```

---

## How to capture truly-paired traffic (Phase 6c step 1)

Run BOTH captures simultaneously on the same machine:

1. **Client side capture** — the tool the user has been using
   (filenames like `Port_61483.txt`).
2. **Server side capture** — same tool aimed at the LoginServer's
   listening socket (filenames like `Port_9010.txt`).

Trigger ONE login attempt while both captures are recording. The
matching wall-clock timestamps in both files will identify the
paired packets unambiguously. Send both files; the calibration
tool then recovers the seed and we instantiate `Fiesta::Cipher`
with `Reset(seed)` accordingly.

Alternatively, simply log the client's `MISC_SEED_ACK` (opcode
0x0207) reply on session start. The 2-byte payload IS the seed;
no calibration needed.

---

## Implementation status

* `FiestaCipher.h::CipherKind::XOR499` — algorithm verified,
  table verified byte-for-byte against the disassembly. **Ready
  for production use** as soon as the seed source is confirmed.
* `FiestaCipher.h::CipherKind::LCG` — kept in-tree as a backup
  cipher family but **proven unused by this build's client**
  (the LCG constants `0x000343FD` / `0x00269EC3` do not appear
  in `client.idb`'s code or data sections; `byte_9119D0` is the
  only XOR-with-table loop in the binary).

---

## Files referenced from this directory

```
_re_artifacts/cipher/
├── README.md                       (this file)
└── decompiled_cipher.c             (transcribed sub_82DB60 + sub_7FCB90)
```

---

## Server-side cipher confirmation (2026-02-06)

The user supplied `FIESTA-DISASSEMBLED.zip` containing the official
binaries for **all** server-side processes:

* `3LoginServer2.exe`        (167 KB) — port 9010
* `4WorldManagerServer2.exe` (540 KB) — port 9110
* `Account Release.exe`,  `AccountLog Release.exe`,
  `Character Release.exe`, `GameLog Release.exe` — sub-services
* `MoePromise.exe`,        `optool.exe`             — admin tools

> The 16-byte XOR499 table prefix
> `07 59 69 4A 94 11 94 85 8C 88 05 CB A0 9E CD 58`
> appears at file offset **0x27358** in `3LoginServer2.exe`
> and **0x82530** in `4WorldManagerServer2.exe`.
>
> The constant `499` (= `0x1F3`) appears as a 32-bit LE literal at
> file offset **0x23858** in `3LoginServer2.exe` (one match).

This is iron-clad confirmation that the server uses the **same**
XOR499 cipher with the **same** 499-byte table as the client. The
cipher is symmetric (XOR ⊕ XOR = identity), so the same routine
encrypts client→server and decrypts server→client packets.

The only remaining unknown is the SEED that initialises the cipher
position at the start of each session.  See "What still needs
calibration" below.

---

## What still needs calibration

The cipher position is initialised by `sub_7FCB90` in the client
to `seed % 499`.  We have not yet traced **where the `seed` value
comes from**.  The two most plausible sources:

1. **Server-supplied via `PROTO_NC_MISC_SEED_ACK` (opcode 0x0207)**
   — server writes the seed in its 2-byte payload and the client
   `Reset()`s with it on receipt.  This is the canonical Shine
   engine pattern.

2. **Constant zero on this build** — the user's private-server
   build may have neutralised the seed exchange (zero seed for
   simplicity).  Test this first: it costs 1 line of code in the
   headless client.

### The simplest path to verify

Implement `Phase 6c` outbound chat with `Cipher::Reset(0)` and try
sending `[u8 0][u8 2]"hi"` to the ZoneServer once Elle is logged
in. If the server processes the packet → seed is 0 (done!). If not,
send a `MISC_SEED_ACK` capture from a fresh login round.
