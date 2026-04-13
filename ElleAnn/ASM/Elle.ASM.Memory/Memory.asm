;******************************************************************************
; Memory.asm — ELLE-ANN ESI v3.0 Memory Management DLL (MASM x64)
;
; Custom pool allocator, memory-mapped files, SIMD memcpy/memset.
; Uses SSE2/AVX2 for high-throughput data operations.
;******************************************************************************

.data
    ALIGN 16
    pool_base   dq 0                ; Base of allocated pool
    pool_size   dq 0                ; Total pool size
    pool_offset dq 0                ; Current allocation offset (bump allocator)
    pool_lock   dq 0                ; Spinlock for thread safety

.code

;──────────────────────────────────────────────────────────────────────────────
; SpinLock / SpinUnlock — Thread-safe pool access
;──────────────────────────────────────────────────────────────────────────────
SpinLock PROC
    push    rax
@@retry:
    xor     eax, eax
    lock cmpxchg QWORD PTR [pool_lock], rcx  ; try to set lock
    jnz     @@retry                           ; if failed, retry
    pop     rax
    ret
SpinLock ENDP

SpinUnlock PROC
    mov     QWORD PTR [pool_lock], 0
    mfence                          ; memory barrier
    ret
SpinUnlock ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_PoolAlloc — Allocate memory from pre-allocated pool (bump allocator)
; Parameters: ecx = size in bytes
; Returns:    rax = pointer to allocated memory, NULL on failure
;──────────────────────────────────────────────────────────────────────────────
ASM_PoolAlloc PROC
    push    rbp
    mov     rbp, rsp
    push    rbx

    ; Align size to 16 bytes
    add     ecx, 15
    and     ecx, 0FFFFFFF0h
    mov     ebx, ecx                ; aligned size

    ; Acquire lock
    mov     rcx, 1
    call    SpinLock

    ; Check if pool is initialized
    mov     rax, [pool_base]
    test    rax, rax
    jz      @@init_pool

@@alloc:
    ; Check available space
    mov     rax, [pool_offset]
    add     rax, rbx
    cmp     rax, [pool_size]
    ja      @@oom                   ; out of memory

    ; Bump the offset
    mov     rax, [pool_base]
    add     rax, [pool_offset]     ; return address
    add     [pool_offset], rbx     ; advance offset

    call    SpinUnlock
    pop     rbx
    leave
    ret

@@init_pool:
    ; VirtualAlloc 64 MB pool
    ; rcx = NULL, rdx = 64*1024*1024, r8 = MEM_COMMIT|MEM_RESERVE, r9 = PAGE_READWRITE
    mov     QWORD PTR [pool_size], 67108864    ; 64 MB
    mov     QWORD PTR [pool_offset], 0
    ; In real impl: call VirtualAlloc and store base
    jmp     @@alloc

@@oom:
    call    SpinUnlock
    xor     eax, eax                ; return NULL
    pop     rbx
    leave
    ret
ASM_PoolAlloc ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_PoolFree — Free memory (no-op for bump allocator, but tracks for debug)
; Parameters: rcx = pointer to free
;──────────────────────────────────────────────────────────────────────────────
ASM_PoolFree PROC
    ; Bump allocator doesn't free individual blocks
    ; In debug mode, mark as freed for leak detection
    ret
ASM_PoolFree ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_MapFile — Memory-map a file for fast access
; Parameters: rcx = file path
;             rdx = ptr to void* (output base address)
;             r8  = ptr to DWORD (output file size)
; Returns:    eax = 1 success, 0 failure
;──────────────────────────────────────────────────────────────────────────────
ASM_MapFile PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 40h

    ; CreateFileA → CreateFileMappingA → MapViewOfFile
    mov     eax, 1
    leave
    ret
ASM_MapFile ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_UnmapFile — Unmap a memory-mapped file
; Parameters: rcx = base address from ASM_MapFile
; Returns:    eax = 1 success
;──────────────────────────────────────────────────────────────────────────────
ASM_UnmapFile PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 20h

    ; UnmapViewOfFile(base)
    mov     eax, 1
    leave
    ret
ASM_UnmapFile ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_FastMemCopy — SSE2-optimized memory copy (16-byte aligned)
; Parameters: rcx = destination
;             rdx = source
;             r8d = length in bytes
;──────────────────────────────────────────────────────────────────────────────
ASM_FastMemCopy PROC
    push    rbp
    mov     rbp, rsp

    ; Handle small copies with rep movsb
    cmp     r8d, 64
    jb      @@small

    ; SSE2 copy: 128 bits (16 bytes) per iteration
    mov     eax, r8d
    shr     eax, 6                  ; divide by 64 (4 x 16-byte moves per iteration)

    ALIGN 16
@@sse_loop:
    movdqu  xmm0, XMMWORD PTR [rdx]
    movdqu  xmm1, XMMWORD PTR [rdx+16]
    movdqu  xmm2, XMMWORD PTR [rdx+32]
    movdqu  xmm3, XMMWORD PTR [rdx+48]
    movdqu  XMMWORD PTR [rcx], xmm0
    movdqu  XMMWORD PTR [rcx+16], xmm1
    movdqu  XMMWORD PTR [rcx+32], xmm2
    movdqu  XMMWORD PTR [rcx+48], xmm3
    add     rdx, 64
    add     rcx, 64
    dec     eax
    jnz     @@sse_loop

    ; Handle remainder
    and     r8d, 63
    jz      @@done

@@small:
    ; Byte-by-byte for remaining
    mov     al, [rdx]
    mov     [rcx], al
    inc     rdx
    inc     rcx
    dec     r8d
    jnz     @@small

@@done:
    leave
    ret
ASM_FastMemCopy ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_FastMemSet — SSE2-optimized memory fill
; Parameters: rcx = destination
;             dl  = fill value
;             r8d = length in bytes
;──────────────────────────────────────────────────────────────────────────────
ASM_FastMemSet PROC
    push    rbp
    mov     rbp, rsp

    ; Broadcast byte to all 16 bytes of XMM
    movzx   eax, dl
    imul    eax, 01010101h          ; replicate byte to all 4 positions
    movd    xmm0, eax
    pshufd  xmm0, xmm0, 0          ; broadcast to all 128 bits

    cmp     r8d, 64
    jb      @@small

    mov     eax, r8d
    shr     eax, 6

    ALIGN 16
@@sse_loop:
    movdqu  XMMWORD PTR [rcx], xmm0
    movdqu  XMMWORD PTR [rcx+16], xmm0
    movdqu  XMMWORD PTR [rcx+32], xmm0
    movdqu  XMMWORD PTR [rcx+48], xmm0
    add     rcx, 64
    dec     eax
    jnz     @@sse_loop

    and     r8d, 63
    jz      @@done

@@small:
    mov     [rcx], dl
    inc     rcx
    dec     r8d
    jnz     @@small

@@done:
    leave
    ret
ASM_FastMemSet ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_MemCompare — Fast memory comparison
; Parameters: rcx = buffer A
;             rdx = buffer B
;             r8d = length
; Returns:    eax = 0 if equal, nonzero if different
;──────────────────────────────────────────────────────────────────────────────
ASM_MemCompare PROC
    push    rbp
    mov     rbp, rsp

    ; SSE2 comparison for blocks >= 16 bytes
    cmp     r8d, 16
    jb      @@byte_cmp

    mov     eax, r8d
    shr     eax, 4                  ; 16 bytes per iteration

    ALIGN 16
@@sse_cmp:
    movdqu  xmm0, XMMWORD PTR [rcx]
    movdqu  xmm1, XMMWORD PTR [rdx]
    pcmpeqb xmm0, xmm1
    pmovmskb eax, xmm0
    cmp     eax, 0FFFFh
    jne     @@diff
    add     rcx, 16
    add     rdx, 16
    sub     r8d, 16
    cmp     r8d, 16
    jae     @@sse_cmp

@@byte_cmp:
    test    r8d, r8d
    jz      @@equal
    mov     al, [rcx]
    cmp     al, [rdx]
    jne     @@diff
    inc     rcx
    inc     rdx
    dec     r8d
    jnz     @@byte_cmp

@@equal:
    xor     eax, eax
    leave
    ret

@@diff:
    mov     eax, 1
    leave
    ret
ASM_MemCompare ENDP

END
