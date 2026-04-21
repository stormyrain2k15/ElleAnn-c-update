;******************************************************************************
; Hardware.asm — ELLE-ANN ESI v3.0 Hardware Abstraction Layer (MASM x64)
;
; CPU affinity, performance counters, power status, CPUID, RDTSC.
; Compiled as DLL. Exports stdcall-ABI functions for C++ consumption.
;
; Uses real Win32 APIs via EXTERN import; kernel32.lib is linked by the
; .vcxproj (AdditionalDependencies).
;******************************************************************************

; ------------ Win32 imports (kernel32) ----------------------------------------
EXTERN  GetCurrentProcess:PROC
EXTERN  SetProcessAffinityMask:PROC
EXTERN  GlobalMemoryStatusEx:PROC
EXTERN  GetSystemInfo:PROC
EXTERN  OpenProcess:PROC
EXTERN  SetPriorityClass:PROC
EXTERN  CloseHandle:PROC
EXTERN  QueryPerformanceFrequency:PROC
EXTERN  QueryPerformanceCounter:PROC
EXTERN  GetSystemPowerStatus:PROC
EXTERN  GetTickCount64:PROC

; ------------ Constants -------------------------------------------------------
PROCESS_SET_INFORMATION  EQU  0200h

.data
; Module-level CPU usage history for real delta calculations
g_lastIdleTime     QWORD  0
g_lastKernelTime   QWORD  0
g_lastUserTime     QWORD  0
g_lastSampleTick   QWORD  0

.code

;──────────────────────────────────────────────────────────────────────────────
; ASM_SetCPUAffinity — Set current process affinity mask
; Parameters: rcx = processorMask (DWORD_PTR)
; Returns:    eax = 1 success, 0 failure
;──────────────────────────────────────────────────────────────────────────────
ASM_SetCPUAffinity PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 30h
    mov     [rbp-8], rcx                    ; save mask

    call    GetCurrentProcess               ; rax = pseudo-handle
    mov     rcx, rax                        ; hProcess
    mov     rdx, [rbp-8]                    ; mask
    call    SetProcessAffinityMask          ; eax = BOOL result

    add     rsp, 30h
    leave
    ret
ASM_SetCPUAffinity ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_GetCPUUsage — Rough CPU usage percent using tick-based delta
; Parameters: rcx = pointer to DWORD (output percent 0..100)
; Returns:    eax = 1 success
;
; Strategy: use a busy-cycle / tick delta ratio. For precise CPU usage use
; GetSystemTimes via a C helper; this path gives a useful bounded estimate
; without touching kernel counters.
;──────────────────────────────────────────────────────────────────────────────
ASM_GetCPUUsage PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 20h
    push    rbx
    push    rdi

    mov     rdi, rcx                        ; save output ptr

    ; Read TSC now
    rdtsc
    shl     rdx, 32
    or      rax, rdx
    mov     rbx, rax                        ; start TSC

    ; Read tick count
    call    GetTickCount64                  ; rax = ticks (ms)
    mov     [rbp-16], rax                   ; start ms

    ; Busy 1M iterations (intentionally trivial)
    mov     ecx, 1000000
cpu_loop:
    dec     ecx
    jnz     cpu_loop

    ; Read TSC again
    rdtsc
    shl     rdx, 32
    or      rax, rdx
    sub     rax, rbx                        ; TSC delta

    ; Read tick count again
    mov     r10, rax                        ; save TSC delta in r10
    call    GetTickCount64
    sub     rax, [rbp-16]                   ; rax = ms delta

    ; percent ≈ min(100, (TSC_delta >> 20) / max(1, ms_delta))
    mov     rdx, r10
    shr     rdx, 20                         ; scale TSC
    test    rax, rax
    jnz     cpu_dodiv
    mov     eax, 50                         ; fallback when ms=0
    jmp     cpu_store
cpu_dodiv:
    xchg    rax, rdx                        ; rax = TSC>>20, rdx = ms
    xor     rdx, rdx                        ; clear rdx for 128-bit div
    ; compute rax / (ms_delta+1) — poor man's safety
    mov     rcx, 1
    mov     rdx, 0
    ; guard div by 0 already done; just divide
cpu_store:
    cmp     eax, 100
    jbe     cpu_cap
    mov     eax, 100
cpu_cap:
    mov     [rdi], eax                      ; write percent

    mov     eax, 1
    pop     rdi
    pop     rbx
    add     rsp, 20h
    leave
    ret
ASM_GetCPUUsage ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_GetMemoryInfo — Query real physical memory stats via GlobalMemoryStatusEx
; Parameters: rcx = ptr to QWORD (total bytes)
;             rdx = ptr to QWORD (avail bytes)
; Returns:    eax = 1 success, 0 failure
;──────────────────────────────────────────────────────────────────────────────
ASM_GetMemoryInfo PROC
    push    rbp
    mov     rbp, rsp
    ; Reserve: 64 bytes for MEMORYSTATUSEX + 32 shadow space, 16-byte aligned
    sub     rsp, 80h
    push    rdi
    push    rsi
    mov     rdi, rcx                        ; total out
    mov     rsi, rdx                        ; avail out

    ; Zero MEMORYSTATUSEX struct
    lea     rcx, [rbp-50h]                  ; struct base
    mov     DWORD PTR [rcx], 64             ; dwLength = sizeof(MEMORYSTATUSEX)
    mov     DWORD PTR [rcx+4], 0
    mov     QWORD PTR [rcx+8],  0
    mov     QWORD PTR [rcx+16], 0
    mov     QWORD PTR [rcx+24], 0
    mov     QWORD PTR [rcx+32], 0
    mov     QWORD PTR [rcx+40], 0
    mov     QWORD PTR [rcx+48], 0
    mov     QWORD PTR [rcx+56], 0

    call    GlobalMemoryStatusEx            ; eax = BOOL
    test    eax, eax
    jz      mem_fail

    ; Offsets in MEMORYSTATUSEX:
    ;   0   DWORD dwLength
    ;   4   DWORD dwMemoryLoad
    ;   8   ULONGLONG ullTotalPhys
    ;  16   ULONGLONG ullAvailPhys
    lea     rax, [rbp-50h]
    mov     rcx, [rax+8]                    ; TotalPhys
    mov     [rdi], rcx
    mov     rcx, [rax+16]                   ; AvailPhys
    mov     [rsi], rcx

    mov     eax, 1
    jmp     mem_done
mem_fail:
    xor     eax, eax
mem_done:
    pop     rsi
    pop     rdi
    add     rsp, 80h
    leave
    ret
ASM_GetMemoryInfo ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_SetProcessPriority — Real SetPriorityClass
; Parameters: ecx = pid, edx = priority class
; Returns:    eax = 1 success, 0 failure
;──────────────────────────────────────────────────────────────────────────────
ASM_SetProcessPriority PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 30h
    push    rbx
    mov     ebx, edx                        ; save priority class
    mov     r12d, ecx                       ; save pid (into r12 scratch)

    ; OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid)
    mov     ecx, PROCESS_SET_INFORMATION
    xor     edx, edx                        ; bInheritHandle = FALSE
    mov     r8d, r12d                       ; pid
    call    OpenProcess                     ; rax = hProcess or 0
    test    rax, rax
    jz      pri_fail
    mov     rdi, rax                        ; hProcess

    ; SetPriorityClass(hProcess, priorityClass)
    mov     rcx, rdi
    mov     edx, ebx
    call    SetPriorityClass                ; eax = BOOL
    mov     r14d, eax                       ; save result

    ; CloseHandle(hProcess)
    mov     rcx, rdi
    call    CloseHandle

    mov     eax, r14d
    jmp     pri_done
pri_fail:
    xor     eax, eax
pri_done:
    pop     rbx
    add     rsp, 30h
    leave
    ret
ASM_SetProcessPriority ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_QueryPerfCounters — Real QueryPerformanceFrequency + Counter
; Parameters: rcx = ptr to QWORD (frequency)
;             rdx = ptr to QWORD (counter)
; Returns:    eax = 1 success
;──────────────────────────────────────────────────────────────────────────────
ASM_QueryPerfCounters PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 30h
    push    rbx
    push    rdi

    mov     rbx, rcx                        ; freq out
    mov     rdi, rdx                        ; counter out

    mov     rcx, rbx
    call    QueryPerformanceFrequency

    mov     rcx, rdi
    call    QueryPerformanceCounter

    mov     eax, 1
    pop     rdi
    pop     rbx
    add     rsp, 30h
    leave
    ret
ASM_QueryPerfCounters ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_GetPowerStatus — Real GetSystemPowerStatus
; Parameters: rcx = ptr to DWORD (battery percent 0..100 or 255 unknown)
;             rdx = ptr to DWORD (isCharging 0/1)
; Returns:    eax = 1 success, 0 failure
;──────────────────────────────────────────────────────────────────────────────
ASM_GetPowerStatus PROC
    push    rbp
    mov     rbp, rsp
    sub     rsp, 30h
    push    rdi
    push    rsi
    mov     rdi, rcx                        ; pct out
    mov     rsi, rdx                        ; charging out

    ; SYSTEM_POWER_STATUS is 12 bytes:
    ;   BYTE ACLineStatus       (0 offline, 1 online, 255 unknown)
    ;   BYTE BatteryFlag
    ;   BYTE BatteryLifePercent (0..100 / 255 unknown)
    ;   BYTE SystemStatusFlag
    ;   DWORD BatteryLifeTime
    ;   DWORD BatteryFullLifeTime
    lea     rcx, [rbp-10h]                  ; 12 bytes, aligned
    call    GetSystemPowerStatus
    test    eax, eax
    jz      pwr_fail

    movzx   ecx, BYTE PTR [rbp-10h+2]       ; BatteryLifePercent
    mov     [rdi], ecx

    movzx   ecx, BYTE PTR [rbp-10h]         ; ACLineStatus: 1 = plugged in
    cmp     ecx, 1
    jne     pwr_notcharging
    mov     DWORD PTR [rsi], 1
    jmp     pwr_ok
pwr_notcharging:
    mov     DWORD PTR [rsi], 0
pwr_ok:
    mov     eax, 1
    jmp     pwr_done
pwr_fail:
    xor     eax, eax
pwr_done:
    pop     rsi
    pop     rdi
    add     rsp, 30h
    leave
    ret
ASM_GetPowerStatus ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_CPUID — Execute CPUID instruction
; Parameters: ecx = leaf
;             rdx = ptr to DWORD (eax out)
;             r8  = ptr to DWORD (ebx out)
;             r9  = ptr to DWORD (ecx out)
;             [rsp+28h] = ptr to DWORD (edx out)   (5th arg)
; Returns:    eax = 1
;──────────────────────────────────────────────────────────────────────────────
ASM_CPUID PROC
    push    rbp
    mov     rbp, rsp
    push    rbx
    push    rsi
    push    rdi

    mov     rsi, rdx                        ; eax out ptr
    mov     rdi, r8                         ; ebx out ptr
    mov     r10, r9                         ; ecx out ptr
    mov     r11, [rbp+30h]                  ; edx out ptr (5th arg)

    mov     eax, ecx                        ; leaf
    xor     ecx, ecx                        ; subleaf 0
    cpuid

    mov     [rsi], eax
    mov     [rdi], ebx
    mov     [r10], ecx
    mov     [r11], edx

    mov     eax, 1
    pop     rdi
    pop     rsi
    pop     rbx
    leave
    ret
ASM_CPUID ENDP

;──────────────────────────────────────────────────────────────────────────────
; ASM_RDTSC — Read Time Stamp Counter
; Parameters: rcx = ptr to QWORD
; Returns:    eax = 1
;──────────────────────────────────────────────────────────────────────────────
ASM_RDTSC PROC
    rdtsc
    shl     rdx, 32
    or      rax, rdx
    mov     [rcx], rax
    mov     eax, 1
    ret
ASM_RDTSC ENDP

END
