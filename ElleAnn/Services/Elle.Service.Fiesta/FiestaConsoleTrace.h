/*══════════════════════════════════════════════════════════════════════════════
 * FiestaConsoleTrace.h — live-watch console for Elle's headless Fiesta client.
 *
 *   When the service is launched with `--console`, this header's hooks
 *   spit a one-line summary of every packet (RX and TX) plus key
 *   lifecycle events to STDOUT in colour-coded form:
 *
 *       12:34:56.789  RX  0x0207  NC_MISC_SEED_ACK            (   2 B)  1F 03
 *       12:34:56.812  TX  0x0306  NC_USER_LOGIN_REQ           ( 272 B)
 *       12:34:56.834  >>  state -> LOGIN_AWAIT_LOGIN_ACK
 *       12:34:56.997  RX  0x030A  NC_USER_LOGIN_ACK           (  43 B)  03 0A 00 …
 *       12:34:57.250  RX  0x0420  NC_BRIEFINFO_BRIEFINFO_CMD  (  76 B)  …
 *       12:34:57.250  ★   chat   "Crystal" → "hi"
 *
 *   ────────────────────────────────────────────────────────────────────
 *   STRICT NO-STUB POLICY
 *   ────────────────────────────────────────────────────────────────────
 *   Every line printed here corresponds to a REAL packet or REAL state
 *   transition.  When the console isn't allocated (service mode), the
 *   functions are still callable but write nothing — there is no
 *   per-call branch on whether a console exists; we just write to stdout
 *   and rely on the parent's stdout sink (NUL when run as a Windows
 *   service, attached console when launched via `--console`).
 *
 *   ────────────────────────────────────────────────────────────────────
 *   USAGE
 *   ────────────────────────────────────────────────────────────────────
 *   #include "FiestaConsoleTrace.h"
 *
 *   // In FiestaClient::HandlePacket, first line:
 *   Fiesta::Trace::OnRx(pkt.opcode, pkt.payload);
 *
 *   // In Connection::Send, after cipher applied:
 *   Fiesta::Trace::OnTx(opcode, payload);
 *
 *   // In Client::SetState transition handler:
 *   Fiesta::Trace::OnStateChange(prev_name, new_name);
 *
 *   // For decoded events, pretty-print as they are emitted:
 *   Fiesta::Trace::OnChat(sender, text);
 *   Fiesta::Trace::OnMove(handle, fromX, fromY, toX, toY, type);
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once
#ifndef ELLE_FIESTA_CONSOLE_TRACE_H
#define ELLE_FIESTA_CONSOLE_TRACE_H

#include "FiestaProtoTable.h"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <mutex>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

namespace Fiesta {
namespace Trace {

/*──────────────────────────────────────────────────────────────────────────────
 * Toggle set by the launcher.  When false the helpers are a near no-op
 * (one atomic load + early return), so leaving the trace calls compiled
 * into release builds costs ~5 ns per call when not in console mode.
 *──────────────────────────────────────────────────────────────────────────────*/
inline std::atomic<bool>& EnabledFlag() {
    static std::atomic<bool> g{false};
    return g;
}

inline void SetEnabled(bool on) { EnabledFlag().store(on); }
inline bool IsEnabled()          { return EnabledFlag().load(std::memory_order_relaxed); }


/*──────────────────────────────────────────────────────────────────────────────
 * Stdout serialisation guard — multiple worker threads emit traces.
 *──────────────────────────────────────────────────────────────────────────────*/
inline std::mutex& TraceMutex() {
    static std::mutex m;
    return m;
}


/*──────────────────────────────────────────────────────────────────────────────
 * Cheap millisecond timestamp:  HH:MM:SS.mmm
 *──────────────────────────────────────────────────────────────────────────────*/
inline void WriteTimestamp(char buf[16]) {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto t   = system_clock::to_time_t(now);
    auto ms  = duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000;
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::snprintf(buf, 16, "%02d:%02d:%02d.%03lld",
                  tm.tm_hour, tm.tm_min, tm.tm_sec,
                  static_cast<long long>(ms));
}


/*──────────────────────────────────────────────────────────────────────────────
 * Hex preview — writes up to `max_bytes` hex pairs.
 *──────────────────────────────────────────────────────────────────────────────*/
inline std::string HexPreview(const std::vector<uint8_t>& bytes,
                              std::size_t max_bytes = 16) {
    std::string s;
    s.reserve(max_bytes * 3 + 4);
    static const char* hex = "0123456789ABCDEF";
    std::size_t shown = std::min(bytes.size(), max_bytes);
    for (std::size_t i = 0; i < shown; ++i) {
        s += hex[bytes[i] >> 4];
        s += hex[bytes[i] & 0xF];
        if (i + 1 < shown) s += ' ';
    }
    if (bytes.size() > shown) s += " …";
    return s;
}


/*──────────────────────────────────────────────────────────────────────────────
 * Direction / event prefixes (ANSI-coloured if stdout is a TTY/console).
 *
 *   RX  cyan    [server → Elle]
 *   TX  yellow  [Elle → server]
 *   >>  green   [state transition]
 *   ★   magenta [decoded high-level event: chat / whisper / move]
 *   !   red     [error / refusal / disconnect]
 *──────────────────────────────────────────────────────────────────────────────*/
inline const char* RX_PFX() { return "\x1b[36mRX\x1b[0m"; }
inline const char* TX_PFX() { return "\x1b[33mTX\x1b[0m"; }
inline const char* ST_PFX() { return "\x1b[32m>>\x1b[0m"; }
inline const char* HI_PFX() { return "\x1b[35m★\x1b[0m"; }
inline const char* ER_PFX() { return "\x1b[31m!\x1b[0m"; }


/*──────────────────────────────────────────────────────────────────────────────
 * Hooks
 *──────────────────────────────────────────────────────────────────────────────*/
inline void OnRx(uint16_t opcode, const std::vector<uint8_t>& payload) {
    if (!IsEnabled()) return;
    std::string_view name = OpcodeName(opcode);
    char ts[16]; WriteTimestamp(ts);
    std::string hex = HexPreview(payload);
    std::lock_guard<std::mutex> lk(TraceMutex());
    std::printf("%s  %s  0x%04X  %-30.*s (%4zu B)  %s\n",
                ts, RX_PFX(), opcode,
                static_cast<int>(name.size()), name.data(),
                payload.size(), hex.c_str());
    std::fflush(stdout);
}

inline void OnTx(uint16_t opcode, const std::vector<uint8_t>& payload) {
    if (!IsEnabled()) return;
    std::string_view name = OpcodeName(opcode);
    char ts[16]; WriteTimestamp(ts);
    std::lock_guard<std::mutex> lk(TraceMutex());
    std::printf("%s  %s  0x%04X  %-30.*s (%4zu B)\n",
                ts, TX_PFX(), opcode,
                static_cast<int>(name.size()), name.data(),
                payload.size());
    std::fflush(stdout);
}

inline void OnStateChange(std::string_view prev, std::string_view next) {
    if (!IsEnabled()) return;
    char ts[16]; WriteTimestamp(ts);
    std::lock_guard<std::mutex> lk(TraceMutex());
    std::printf("%s  %s   state %.*s -> \x1b[1m%.*s\x1b[0m\n",
                ts, ST_PFX(),
                static_cast<int>(prev.size()), prev.data(),
                static_cast<int>(next.size()), next.data());
    std::fflush(stdout);
}

inline void OnChat(std::string_view sender, std::string_view text,
                   const char* kind = "chat") {
    if (!IsEnabled()) return;
    char ts[16]; WriteTimestamp(ts);
    std::lock_guard<std::mutex> lk(TraceMutex());
    std::printf("%s  %s   %-7s \"%.*s\" -> \"%.*s\"\n",
                ts, HI_PFX(), kind,
                static_cast<int>(sender.size()), sender.data(),
                static_cast<int>(text.size()), text.data());
    std::fflush(stdout);
}

inline void OnMove(uint16_t handle,
                   uint32_t fromX, uint32_t fromY,
                   uint32_t toX,   uint32_t toY,
                   uint8_t  movetype) {
    if (!IsEnabled()) return;
    char ts[16]; WriteTimestamp(ts);
    std::lock_guard<std::mutex> lk(TraceMutex());
    std::printf("%s  %s   move    h=0x%04X (%u,%u) -> (%u,%u) type=0x%02X\n",
                ts, HI_PFX(), handle, fromX, fromY, toX, toY, movetype);
    std::fflush(stdout);
}

inline void OnError(std::string_view what) {
    if (!IsEnabled()) return;
    char ts[16]; WriteTimestamp(ts);
    std::lock_guard<std::mutex> lk(TraceMutex());
    std::printf("%s  %s   %.*s\n",
                ts, ER_PFX(),
                static_cast<int>(what.size()), what.data());
    std::fflush(stdout);
}


/*──────────────────────────────────────────────────────────────────────────────
 * Banner — printed once at console-mode startup.
 *──────────────────────────────────────────────────────────────────────────────*/
inline void Banner(std::string_view who) {
    if (!IsEnabled()) return;
    std::lock_guard<std::mutex> lk(TraceMutex());
    std::printf("\x1b[1;36m"
                "════════════════════════════════════════════════════════════════════\n"
                " Elle.Service.Fiesta — live console trace\n"
                " %.*s\n"
                "════════════════════════════════════════════════════════════════════"
                "\x1b[0m\n",
                static_cast<int>(who.size()), who.data());
    std::printf("Legend:  \x1b[36mRX\x1b[0m server->Elle   "
                "\x1b[33mTX\x1b[0m Elle->server   "
                "\x1b[32m>>\x1b[0m state   "
                "\x1b[35m★\x1b[0m decoded   "
                "\x1b[31m!\x1b[0m error\n\n");
    std::fflush(stdout);
}


/*──────────────────────────────────────────────────────────────────────────────
 * One-time launcher attaches a Windows console window when the service
 * is started with `--console` / `-c`.  If a console is already attached
 * (parent shell on cmd.exe / Powershell), AllocConsole() is a no-op
 * for us — we just bind stdout to the existing one.
 *──────────────────────────────────────────────────────────────────────────────*/
#ifdef _WIN32
inline void EnsureWindowsConsole() {
    /* Attempt to attach to the parent console first (preferred — keeps
     * `Elle.Service.Fiesta.exe --console > log.txt` redirection working). */
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        /* No parent → spawn a fresh console window. */
        AllocConsole();
        SetConsoleTitleA("Elle.Service.Fiesta — live trace");
    }
    /* Rebind C runtime streams to the now-attached console. */
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);
    freopen_s(&f, "CONIN$",  "r", stdin);
    /* Enable ANSI colour escapes (Windows 10+). */
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode)) {
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}
#else
inline void EnsureWindowsConsole() { /* POSIX: stdout is the console. */ }
#endif

}  /* namespace Trace */
}  /* namespace Fiesta */

#endif  /* ELLE_FIESTA_CONSOLE_TRACE_H */
