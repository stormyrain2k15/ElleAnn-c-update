;******************************************************************************
; FileIO.asm — ELLE-ANN ESI v3.0 File I/O DLL (MASM x64)
;
; Real Win32 file I/O: read, write, append, delete, exists, size, copy, lock.
; Uses EXTERN imports from kernel32 (linker resolves via kernel32.lib).
;******************************************************************************

EXTERN  CreateFileA:PROC
EXTERN  ReadFile:PROC
EXTERN  WriteFile:PROC
EXTERN  CloseHandle:PROC
EXTERN  DeleteFileA:PROC
EXTERN  GetFileAttributesA:PROC
EXTERN  SetFilePointerEx:PROC
EXTERN  GetFileSizeEx:PROC
EXTERN  CopyFileA:PROC
EXTERN  FindFirstChangeNotificationA:PROC

.code

GENERIC_READ                 EQU 80000000h
GENERIC_WRITE                EQU 40000000h
FILE_APPEND_DATA             EQU 00000004h
FILE_SHARE_READ              EQU 00000001h
FILE_SHARE_WRITE             EQU 00000002h
FILE_ATTRIBUTE_NORMAL        EQU 00000080h
OPEN_EXISTING                EQU 3
CREATE_ALWAYS                EQU 2
OPEN_ALWAYS                  EQU 4
INVALID_HANDLE_VALUE         EQU -1
INVALID_FILE_ATTRIBUTES      EQU -1
FILE_END                     EQU 2

;──────────────────────────────────────────────────────────────────────────────
; ASM_ReadFile — Read file into buffer
; Parameters: rcx = path, rdx = buf, r8d = max bytes, r9 = ptr DWORD bytesRead
;──────────────────────────────────────────────────────────────────────────────
ASM_ReadFile PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 50h
    push    rbx
    push    rdi
    push    rsi

    mov     rsi, rdx                    ; buffer
    mov     r12d, r8d                   ; max bytes
    mov     rdi, r9                     ; bytes read out

    ; CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NORMAL, NULL)
    ; rcx already = path
    mov     edx, GENERIC_READ
    mov     r8d, FILE_SHARE_READ
    xor     r9, r9
    mov     DWORD PTR [rsp+20h], OPEN_EXISTING
    mov     DWORD PTR [rsp+28h], FILE_ATTRIBUTE_NORMAL
    mov     QWORD PTR [rsp+30h], 0
    call    CreateFileA
    cmp     rax, INVALID_HANDLE_VALUE
    je      rd_fail
    mov     rbx, rax                    ; hFile

    ; ReadFile(hFile, buf, maxBytes, &bytesRead, NULL)
    mov     rcx, rbx
    mov     rdx, rsi
    mov     r8d, r12d
    mov     r9, rdi
    mov     QWORD PTR [rsp+20h], 0
    call    ReadFile
    mov     r14d, eax

    mov     rcx, rbx
    call    CloseHandle
    mov     eax, r14d
    jmp     rd_done
rd_fail:
    mov     DWORD PTR [rdi], 0
    xor     eax, eax
rd_done:
    pop     rsi
    pop     rdi
    pop     rbx
    add     rsp, 50h
    leave
    ret
ASM_ReadFile ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_WriteFile — Create/overwrite
;──────────────────────────────────────────────────────────────────────────────
ASM_WriteFile PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 50h
    push    rbx
    push    rdi

    mov     rdi, rdx                    ; data buf
    mov     r12d, r8d                   ; num bytes

    mov     edx, GENERIC_WRITE
    xor     r8, r8
    xor     r9, r9
    mov     DWORD PTR [rsp+20h], CREATE_ALWAYS
    mov     DWORD PTR [rsp+28h], FILE_ATTRIBUTE_NORMAL
    mov     QWORD PTR [rsp+30h], 0
    call    CreateFileA
    cmp     rax, INVALID_HANDLE_VALUE
    je      wr_fail
    mov     rbx, rax

    mov     rcx, rbx
    mov     rdx, rdi
    mov     r8d, r12d
    lea     r9, [rbp-8]
    mov     QWORD PTR [rsp+20h], 0
    call    WriteFile
    mov     r14d, eax

    mov     rcx, rbx
    call    CloseHandle
    mov     eax, r14d
    jmp     wr_done
wr_fail:
    xor     eax, eax
wr_done:
    pop     rdi
    pop     rbx
    add     rsp, 50h
    leave
    ret
ASM_WriteFile ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_AppendFile — Open-or-create + seek-end + WriteFile
;──────────────────────────────────────────────────────────────────────────────
ASM_AppendFile PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 60h
    push    rbx
    push    rdi

    mov     rdi, rdx
    mov     r12d, r8d

    mov     edx, FILE_APPEND_DATA
    xor     r8, r8
    xor     r9, r9
    mov     DWORD PTR [rsp+20h], OPEN_ALWAYS
    mov     DWORD PTR [rsp+28h], FILE_ATTRIBUTE_NORMAL
    mov     QWORD PTR [rsp+30h], 0
    call    CreateFileA
    cmp     rax, INVALID_HANDLE_VALUE
    je      ap_fail
    mov     rbx, rax

    ; Seek to end: SetFilePointerEx(h, 0, NULL, FILE_END)
    mov     rcx, rbx
    xor     rdx, rdx
    xor     r8, r8
    mov     r9d, FILE_END
    call    SetFilePointerEx

    mov     rcx, rbx
    mov     rdx, rdi
    mov     r8d, r12d
    lea     r9, [rbp-8]
    mov     QWORD PTR [rsp+20h], 0
    call    WriteFile
    mov     r14d, eax

    mov     rcx, rbx
    call    CloseHandle
    mov     eax, r14d
    jmp     ap_done
ap_fail:
    xor     eax, eax
ap_done:
    pop     rdi
    pop     rbx
    add     rsp, 60h
    leave
    ret
ASM_AppendFile ENDP

ASM_DeleteFile PROC
    sub     rsp, 28h
    call    DeleteFileA
    add     rsp, 28h
    ret
ASM_DeleteFile ENDP

ASM_FileExists PROC
    sub     rsp, 28h
    call    GetFileAttributesA
    add     rsp, 28h
    cmp     eax, INVALID_FILE_ATTRIBUTES
    jne     fe_yes
    xor     eax, eax
    ret
fe_yes:
    mov     eax, 1
    ret
ASM_FileExists ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_WatchDirectory — Return Win32 change-notification HANDLE
; Parameters: rcx = directory path, edx = notification flags, r8 = unused
; Returns:    eax = 1 if handle opened, 0 on failure (handle stored at r8 if ptr non-null)
;──────────────────────────────────────────────────────────────────────────────
ASM_WatchDirectory PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 30h
    push    rdi
    mov     rdi, r8                     ; out ptr (may be 0)

    ; FindFirstChangeNotificationA(path, TRUE, flags)
    mov     r8d, edx                    ; flags in r8d
    mov     edx, 1                      ; bWatchSubtree = TRUE
    call    FindFirstChangeNotificationA
    cmp     rax, INVALID_HANDLE_VALUE
    je      wd_fail
    test    rdi, rdi
    jz      wd_ok
    mov     [rdi], rax
wd_ok:
    mov     eax, 1
    jmp     wd_done
wd_fail:
    xor     eax, eax
wd_done:
    pop     rdi
    add     rsp, 30h
    leave
    ret
ASM_WatchDirectory ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_LockFile — exclusive CreateFileA(share=0); caller closes with ASM_UnlockFile
;──────────────────────────────────────────────────────────────────────────────
ASM_LockFile PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 50h
    push    rdi
    mov     rdi, rdx                    ; out handle ptr

    mov     edx, GENERIC_READ OR GENERIC_WRITE
    xor     r8, r8                      ; share = 0 (exclusive)
    xor     r9, r9
    mov     DWORD PTR [rsp+20h], OPEN_ALWAYS
    mov     DWORD PTR [rsp+28h], FILE_ATTRIBUTE_NORMAL
    mov     QWORD PTR [rsp+30h], 0
    call    CreateFileA
    cmp     rax, INVALID_HANDLE_VALUE
    je      lk_fail
    mov     [rdi], rax
    mov     eax, 1
    jmp     lk_done
lk_fail:
    mov     QWORD PTR [rdi], 0
    xor     eax, eax
lk_done:
    pop     rdi
    add     rsp, 50h
    leave
    ret
ASM_LockFile ENDP

ASM_UnlockFile PROC
    sub     rsp, 28h
    call    CloseHandle
    add     rsp, 28h
    ret
ASM_UnlockFile ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_GetFileSize — Real GetFileSizeEx
;──────────────────────────────────────────────────────────────────────────────
ASM_GetFileSize PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 50h
    push    rbx
    push    rdi
    mov     rdi, rdx                    ; size out

    mov     edx, GENERIC_READ
    mov     r8d, FILE_SHARE_READ OR FILE_SHARE_WRITE
    xor     r9, r9
    mov     DWORD PTR [rsp+20h], OPEN_EXISTING
    mov     DWORD PTR [rsp+28h], FILE_ATTRIBUTE_NORMAL
    mov     QWORD PTR [rsp+30h], 0
    call    CreateFileA
    cmp     rax, INVALID_HANDLE_VALUE
    je      sz_fail
    mov     rbx, rax

    mov     rcx, rbx
    mov     rdx, rdi
    call    GetFileSizeEx
    mov     r14d, eax

    mov     rcx, rbx
    call    CloseHandle
    mov     eax, r14d
    jmp     sz_done
sz_fail:
    mov     QWORD PTR [rdi], 0
    xor     eax, eax
sz_done:
    pop     rdi
    pop     rbx
    add     rsp, 50h
    leave
    ret
ASM_GetFileSize ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_CopyFileFast — Delegate to CopyFileA (kernel already optimizes via MMF)
;──────────────────────────────────────────────────────────────────────────────
ASM_CopyFileFast PROC
    sub     rsp, 28h
    mov     r8d, 0                      ; bFailIfExists = FALSE (overwrite)
    call    CopyFileA
    add     rsp, 28h
    ret
ASM_CopyFileFast ENDP

END
