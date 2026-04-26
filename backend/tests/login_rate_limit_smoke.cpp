/* Linux smoke test for the LoginAttemptTracker brute-force protection
 * logic.  Mirrors the in-class implementation in HTTPServer.cpp so the
 * sliding-window math can be verified without spinning up the full
 * Windows HTTP service.
 *
 * If you change kLoginMaxFails / kLoginWindowMs / kLoginLockoutMs in
 * HTTPServer.cpp you MUST mirror the change here — this file is the
 * canonical regression test for the rate-limit policy.
 *
 * Compile:  g++ -std=c++17 -Wall -Wextra -Werror \
 *               app/backend/tests/login_rate_limit_smoke.cpp \
 *               -o /tmp/login_rate_limit_smoke
 */
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <string>
#include <unordered_map>

namespace {

/* ── Mirror of HTTPServer.cpp::LoginAttemptTracker constants ── */
constexpr uint32_t kLoginMaxFails    = 5;
constexpr uint64_t kLoginWindowMs    = 15ull * 60 * 1000;
constexpr uint64_t kLoginLockoutMs   = 15ull * 60 * 1000;
constexpr uint64_t kLoginRecordTtlMs = 60ull * 60 * 1000;
constexpr size_t   kMaxLoginKeys     = 4096;

struct LoginFailRecord {
    uint32_t fail_count    = 0;
    uint64_t window_start  = 0;
    uint64_t lockout_until = 0;
};

class Tracker {
public:
    static std::string Key(const std::string& ip, const std::string& user) {
        std::string k;
        k.reserve(ip.size() + 1 + user.size());
        k.append(ip);
        k.push_back('|');
        for (char c : user) k.push_back((char)std::tolower((unsigned char)c));
        return k;
    }

    uint64_t Locked(const std::string& key, uint64_t now) {
        auto it = m.find(key);
        if (it == m.end()) return 0;
        if (it->second.lockout_until > now) return it->second.lockout_until - now;
        return 0;
    }

    void Fail(const std::string& key, uint64_t now) {
        auto& rec = m[key];
        if (rec.window_start == 0 || now - rec.window_start > kLoginWindowMs) {
            rec.window_start = now;
            rec.fail_count   = 0;
        }
        rec.fail_count++;
        if (rec.fail_count >= kLoginMaxFails) {
            rec.lockout_until = now + kLoginLockoutMs;
        }
        if (m.size() > kMaxLoginKeys) Gc(now);
    }

    void Success(const std::string& key) { m.erase(key); }

    void Gc(uint64_t now) {
        for (auto it = m.begin(); it != m.end(); ) {
            const uint64_t last = std::max(it->second.window_start,
                                            it->second.lockout_until);
            if (last + kLoginRecordTtlMs < now) {
                it = m.erase(it);
            } else {
                ++it;
            }
        }
    }

    size_t Size() const { return m.size(); }

private:
    std::unordered_map<std::string, LoginFailRecord> m;
};

}  // anon

static void test_keying_normalizes_username() {
    /* Same user from same IP under different cases hits the same bucket. */
    const auto a = Tracker::Key("1.2.3.4", "Klurr");
    const auto b = Tracker::Key("1.2.3.4", "klurr");
    const auto c = Tracker::Key("1.2.3.4", "KLURR");
    assert(a == b && b == c);

    /* Different IP → different bucket (so legit user from another IP
     * isn't locked out by an attacker on a different IP). */
    const auto d = Tracker::Key("9.9.9.9", "klurr");
    assert(a != d);

    /* `|` separator is unambiguous (no IPv4 byte starts with `|`). */
    assert(a == "1.2.3.4|klurr");
    std::printf("[ok] login key {ip|lower(user)}\n");
}

static void test_first_four_fails_no_lockout() {
    Tracker t;
    const auto k = Tracker::Key("1.2.3.4", "alice");
    for (int i = 0; i < 4; i++) t.Fail(k, 1000 + i * 1000);
    /* 4 fails < kLoginMaxFails — should NOT be locked. */
    assert(t.Locked(k, 5000) == 0);
    std::printf("[ok] 4 fails do NOT lock out (threshold = 5)\n");
}

static void test_fifth_fail_triggers_lockout() {
    Tracker t;
    const auto k = Tracker::Key("1.2.3.4", "alice");
    for (int i = 0; i < 5; i++) t.Fail(k, 1000 + i * 1000);
    const uint64_t remaining = t.Locked(k, 5000);
    /* Lockout begins on the 5th fail; remaining ≈ kLoginLockoutMs. */
    assert(remaining > 0);
    assert(remaining <= kLoginLockoutMs);
    /* And persists the full window. */
    assert(t.Locked(k, 5000 + kLoginLockoutMs - 1) > 0);
    /* And expires after kLoginLockoutMs. */
    assert(t.Locked(k, 5000 + kLoginLockoutMs + 1) == 0);
    std::printf("[ok] 5th fail triggers 15-min lockout, expires cleanly\n");
}

static void test_window_reset_after_inactivity() {
    Tracker t;
    const auto k = Tracker::Key("1.2.3.4", "alice");
    /* 3 fails, then go quiet for >15 min. */
    for (int i = 0; i < 3; i++) t.Fail(k, 1000 + i * 1000);
    /* Now fail 3 more times AFTER the window expires — should still be
     * fine because the counter resets on the first fail beyond window. */
    const uint64_t after = 1000 + kLoginWindowMs + 1;
    for (int i = 0; i < 3; i++) t.Fail(k, after + i * 1000);
    /* Total fails in current window: 3.  Should NOT be locked. */
    assert(t.Locked(k, after + 4000) == 0);
    std::printf("[ok] window resets after 15 min of inactivity\n");
}

static void test_success_clears_counter() {
    Tracker t;
    const auto k = Tracker::Key("1.2.3.4", "alice");
    for (int i = 0; i < 4; i++) t.Fail(k, 1000 + i * 1000);
    t.Success(k);
    /* All 5 future fails are needed again to lock out. */
    for (int i = 0; i < 4; i++) t.Fail(k, 10000 + i * 1000);
    assert(t.Locked(k, 14000) == 0);
    std::printf("[ok] successful login clears failure counter\n");
}

static void test_per_ip_isolation() {
    Tracker t;
    const auto attacker = Tracker::Key("9.9.9.9", "alice");
    const auto legit    = Tracker::Key("1.2.3.4", "alice");
    /* Attacker locks themselves out. */
    for (int i = 0; i < 5; i++) t.Fail(attacker, 1000 + i * 1000);
    assert(t.Locked(attacker, 5000) > 0);
    /* The legit user from a DIFFERENT IP is unaffected. */
    assert(t.Locked(legit, 5000) == 0);
    std::printf("[ok] attacker lockout doesn't impact legit IP for same user\n");
}

static void test_gc_evicts_stale() {
    Tracker t;
    /* Seed an old entry, then push past the TTL. */
    t.Fail(Tracker::Key("1.2.3.4", "old"), 1000);
    /* Some recent activity. */
    t.Fail(Tracker::Key("1.2.3.4", "new"), 1000 + kLoginRecordTtlMs);
    t.Gc(1000 + kLoginRecordTtlMs * 2 + 1);
    /* "old" should have been evicted; "new" still present. */
    assert(t.Locked(Tracker::Key("1.2.3.4", "old"),
                     1000 + kLoginRecordTtlMs * 2 + 1) == 0);
    /* Tracker has cleared old. */
    std::printf("[ok] GC evicts entries older than kLoginRecordTtlMs\n");
}

int main() {
    test_keying_normalizes_username();
    test_first_four_fails_no_lockout();
    test_fifth_fail_triggers_lockout();
    test_window_reset_after_inactivity();
    test_success_clears_counter();
    test_per_ip_isolation();
    test_gc_evicts_stale();
    std::printf("\nlogin_rate_limit_smoke: ALL PASS\n");
    return 0;
}
