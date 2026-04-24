;******************************************************************************
; Crypto.asm — ELLE-ANN ESI v3.0 Cryptography DLL (MASM x64)
;
; SHA-256, AES-256 (via AES-NI), XOR cipher, CRC32, CSPRNG.
; Uses hardware-accelerated instructions where available.
;******************************************************************************

.data
    ; ml64 caps segment alignment at 16 bytes. The 64-byte request here
    ; was harmless on some toolchains but is rejected as A2189 on recent
    ; ones. 16-byte alignment still satisfies every AES-NI / SSE access
    ; these constants participate in — SHA-256 K[] and the hash-state
    ; vectors are read 16 bytes at a time in the round functions.
    ALIGN 16
    ; SHA-256 round constants (K)
    sha256_k DWORD 0428a2f98h, 071374491h, 0b5c0fbcfh, 0e9b5dba5h
             DWORD 03956c25bh, 059f111f1h, 0923f82a4h, 0ab1c5ed5h
             DWORD 0d807aa98h, 012835b01h, 0243185beh, 0550c7dc3h
             DWORD 072be5d74h, 080deb1feh, 09bdc06a7h, 0c19bf174h
             DWORD 0e49b69c1h, 0efbe4786h, 00fc19dc6h, 0240ca1cch
             DWORD 02de92c6fh, 04a7484aah, 05cb0a9dch, 076f988dah
             DWORD 0983e5152h, 0a831c66dh, 0b00327c8h, 0bf597fc7h
             DWORD 0c6e00bf3h, 0d5a79147h, 006ca6351h, 014292967h
             DWORD 027b70a85h, 02e1b2138h, 04d2c6dfch, 053380d13h
             DWORD 0650a7354h, 0766a0abbh, 081c2c92eh, 092722c85h
             DWORD 0a2bfe8a1h, 0a81a664bh, 0c24b8b70h, 0c76c51a3h
             DWORD 0d192e819h, 0d6990624h, 0f40e3585h, 0106aa070h
             DWORD 019a4c116h, 01e376c08h, 02748774ch, 034b0bcb5h
             DWORD 0391c0cb3h, 04ed8aa4ah, 05b9cca4fh, 0682e6ff3h
             DWORD 0748f82eeh, 078a5636fh, 084c87814h, 08cc70208h
             DWORD 090befffah, 0a4506cebh, 0bef9a3f7h, 0c67178f2h

    ; SHA-256 initial hash values
    sha256_h0 DWORD 06a09e667h
    sha256_h1 DWORD 0bb67ae85h
    sha256_h2 DWORD 03c6ef372h
    sha256_h3 DWORD 0a54ff53ah
    sha256_h4 DWORD 0510e527fh
    sha256_h5 DWORD 09b05688ch
    sha256_h6 DWORD 01f83d9abh
    sha256_h7 DWORD 05be0cd19h

    ; CRC32 polynomial table
    ALIGN 16
    crc32_table DWORD 256 DUP (0)
    crc32_init  DWORD 0             ; 0 = not initialized

.code

;──────────────────────────────────────────────────────────────────────────────
; ASM_SHA256 — Compute SHA-256 hash
; Parameters: rcx = data pointer
;             edx = data length
;             r8  = output hash (32 bytes)
; Returns:    void
;──────────────────────────────────────────────────────────────────────────────
;──────────────────────────────────────────────────────────────────────────────
; ASM_SHA256_SCAFFOLDED — INTENTIONALLY UNIMPLEMENTED
;
;   The body is a scaffold: prologue/epilogue + constants setup are
;   real, but the 64-round message schedule + compression function are
;   NOT implemented. A real MASM implementation is ~200 lines of
;   schedule-expand + compression-round macro and was deferred to
;   focus on the Windows-service core.
;
;   To prevent silent "all zero" or "initial-hash-value" outputs from
;   being mistaken for real hashes, this stub FILLS the output buffer
;   with a distinctive canary pattern 0xDEAD5CAF (SCAFFOLD) 32 bytes
;   so any caller that accidentally links to it gets a loud, visible
;   wrong-looking hash instead of a plausible-looking bug.
;
;   NOT exported in Crypto.def — nothing can link to this by mistake
;   unless a caller manually adds the export + import. The Shared-side
;   prototype in ElleTypes.h was removed at the same time.
;
;   When a real SHA-256 in MASM is implemented, rename back to
;   ASM_SHA256, re-export, restore the Shared prototype, and run the
;   NIST CAVS test vectors (FIPS 180-4 §5.3.3) before wiring any
;   call sites. For now, production code uses Windows BCrypt via
;   Shared/ElleCrypto.cpp (BCryptHashData / BCryptHmac).
;
; Parameters: rcx = data pointer (unused)
;             edx = data length   (unused)
;             r8  = output hash (32 bytes — filled with canary)
;──────────────────────────────────────────────────────────────────────────────
;──────────────────────────────────────────────────────────────────────────────
; Three cryptographic primitives in this DLL are marked _SCAFFOLDED below
; and have NO real implementation: SHA-256, AES-256 encrypt, AES-256 decrypt.
; They fill the output buffer with a 0xDEAD5CAF canary so accidental callers
; get an obvious wrong-looking result instead of a plausible bug.
;
; The _SCAFFOLDED functions are NOT exported (Crypto.def) and NOT declared
; in Shared/ElleTypes.h — so nothing in the rest of the codebase can
; accidentally link to them.
;
; Production SHA-256 / HMAC-SHA256 lives in Shared/ElleCrypto.{h,cpp} and
; uses Windows CNG (BCrypt*). Replace these MASM scaffolds with real
; AES-NI / SHA-NI implementations only when a hot-path bottleneck is
; measured; the BCrypt wrappers are more than fast enough for JWT-sign /
; config-decrypt / etc.
;──────────────────────────────────────────────────────────────────────────────
ASM_SHA256_SCAFFOLDED PROC
    push    rbp
    mov     rbp, rsp
    ; Fill 32-byte output with 0xDEAD5CAF repeated 8 times.
    mov     rdi, r8
    mov     eax, 0DEAD5CAFh
    mov     ecx, 8
@@fill:
    mov     [rdi], eax
    add     rdi, 4
    dec     ecx
    jnz     @@fill
    leave
    ret
ASM_SHA256_SCAFFOLDED ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_AES256Encrypt_SCAFFOLDED — INTENTIONALLY UNIMPLEMENTED
;
;   Same story as SHA-256: AES-NI aesenc / aesenclast were commented
;   out; no real key schedule, no CBC chaining, no PKCS#7 padding. A
;   real MASM implementation needs the 15-round AES-256 key expansion
;   via aeskeygenassist + the aesenc/aesenclast loop with CBC IV
;   chaining.
;
;   Canary-poison the output with 0xDEAD5CAF so any accidental caller
;   gets an obvious wrong-looking ciphertext rather than a plausible
;   stream. Length comes through [rbp+30h] per the original shadow-
;   space convention.
;
; Parameters: rcx = key (32 bytes, unused)
;             rdx = IV  (16 bytes, unused)
;             r8  = input data       (unused)
;             r9  = output data      (filled with canary)
;             [rbp+30h] = length in bytes
;──────────────────────────────────────────────────────────────────────────────
ASM_AES256Encrypt_SCAFFOLDED PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 40h
    mov     rdi, r9
    mov     r10d, DWORD PTR [rbp+30h]
    test    r10d, r10d
    jz      @@done
    mov     eax, 0DEAD5CAFh
@@fill:
    mov     [rdi], al
    inc     rdi
    dec     r10d
    jnz     @@fill
@@done:
    leave
    ret
ASM_AES256Encrypt_SCAFFOLDED ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_AES256Decrypt_SCAFFOLDED — INTENTIONALLY UNIMPLEMENTED
;   Same rationale as the Encrypt scaffold above. Canary-poisons the
;   output buffer. Not exported.
;──────────────────────────────────────────────────────────────────────────────
ASM_AES256Decrypt_SCAFFOLDED PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 40h
    mov     rdi, r9
    mov     r10d, DWORD PTR [rbp+30h]
    test    r10d, r10d
    jz      @@done
    mov     eax, 0DEAD5CAFh
@@fill:
    mov     [rdi], al
    inc     rdi
    dec     r10d
    jnz     @@fill
@@done:
    leave
    ret
ASM_AES256Decrypt_SCAFFOLDED ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_XorCipher — Simple XOR encryption/decryption
; Parameters: rcx = input data
;             rdx = output data
;             r8d = data length
;             r9  = key bytes
;             [rbp+30h] = key length
;──────────────────────────────────────────────────────────────────────────────
ASM_XorCipher PROC
    push    rbp
    mov     rbp, rsp
    push    rbx
    push    rdi
    push    rsi

    mov     rsi, rcx                ; input
    mov     rdi, rdx                ; output
    mov     ebx, r8d                ; data len
    mov     rcx, r9                 ; key
    mov     edx, DWORD PTR [rbp+30h] ; key len

    xor     eax, eax                ; key index

    ALIGN 16
@@xor_loop:
    test    ebx, ebx
    jz      @@done

    movzx   r8d, BYTE PTR [rsi]
    ; Get key byte (circular)
    push    rax
    xor     edx, edx
    div     DWORD PTR [rbp+30h]     ; eax/keyLen, remainder in edx
    movzx   r9d, BYTE PTR [rcx+rdx]
    pop     rax
    
    xor     r8d, r9d
    mov     [rdi], r8b

    inc     rsi
    inc     rdi
    inc     eax
    dec     ebx
    jmp     @@xor_loop

@@done:
    pop     rsi
    pop     rdi
    pop     rbx
    leave
    ret
ASM_XorCipher ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_CRC32 — Hardware-accelerated CRC32
; Parameters: rcx = data pointer
;             edx = data length
; Returns:    eax = CRC32 value
;──────────────────────────────────────────────────────────────────────────────
ASM_CRC32 PROC
    push    rbp
    mov     rbp, rsp

    mov     eax, 0FFFFFFFFh         ; Initial CRC

    ; Use hardware CRC32 instruction (SSE4.2)
    ALIGN 16
@@crc_loop:
    test    edx, edx
    jz      @@finish
    
    cmp     edx, 8
    jb      @@byte_crc

    ; Process 8 bytes at a time
    crc32   rax, QWORD PTR [rcx]
    add     rcx, 8
    sub     edx, 8
    jmp     @@crc_loop

@@byte_crc:
    crc32   eax, BYTE PTR [rcx]
    inc     rcx
    dec     edx
    jnz     @@byte_crc

@@finish:
    not     eax                     ; Final XOR

    leave
    ret
ASM_CRC32 ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_RandomBytes — Generate cryptographically secure random bytes
; Parameters: rcx = output buffer
;             edx = number of bytes
;──────────────────────────────────────────────────────────────────────────────
ASM_RandomBytes PROC
    push    rbp
    mov     rbp, rsp
    push    rbx
    push    rdi

    mov     rdi, rcx                ; output
    mov     ebx, edx                ; count

    ALIGN 16
@@rng_loop:
    cmp     ebx, 8
    jb      @@rng_small

    ; Use RDRAND instruction for hardware random
    rdrand  rax
    jnc     @@rng_loop              ; retry if CF=0 (not ready)
    mov     [rdi], rax
    add     rdi, 8
    sub     ebx, 8
    jmp     @@rng_loop

@@rng_small:
    test    ebx, ebx
    jz      @@rng_done
    rdrand  rax
    jnc     @@rng_small
@@rng_byte:
    mov     [rdi], al
    shr     rax, 8
    inc     rdi
    dec     ebx
    jnz     @@rng_byte

@@rng_done:
    pop     rdi
    pop     rbx
    leave
    ret
ASM_RandomBytes ENDP

END
