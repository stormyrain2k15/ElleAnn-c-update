/*******************************************************************************
 * test_passive_mesh.cpp — Portable smoke test for the passive-mesh state
 * machine used by ElleServiceBase::TickReconnector.
 *
 * The real reconnector consults `m_ipcHub.IsConnectedTo()` (which is
 * Win32-only) and updates `m_everConnectedTo` (a std::set guarded by a
 * mutex). This test inlines the same state-transition logic against a
 * scriptable peer-state model so we can exercise:
 *
 *   - initial: all peers down → silent (no log spam)
 *   - peer comes up → "first contact" emitted EXACTLY ONCE
 *   - peer stays up → no further events
 *   - peer drops → "lost" emitted, set entry removed
 *   - peer comes back → "first contact" emitted again
 *
 * Compiles with: g++ -std=c++17 -Wall -Wextra -Werror test_passive_mesh.cpp
 *
 * Linux-friendly: no Win32, no SCM, no IOCP. Pure state-machine equivalence
 * test for the bit of code that doesn't depend on the OS pipe primitives.
 ******************************************************************************/
#include <cassert>
#include <iostream>
#include <set>
#include <string>
#include <vector>

enum Event { FIRST_CONTACT, LOST };

struct ReconnectorMock {
    std::set<int> everConnectedTo;
    std::vector<std::pair<Event, int>> log;

    /* Mirror of TickReconnector(): given a snapshot of peer liveness,
     * emit FIRST_CONTACT / LOST events on transitions and update the
     * tracker.  A peer that stays up or stays down emits nothing.    */
    void Tick(const std::vector<std::pair<int, bool>>& peerStates) {
        for (auto& [peer, alive] : peerStates) {
            const bool wasUp = everConnectedTo.count(peer) > 0;
            if (wasUp && !alive) {
                log.push_back({LOST, peer});
                everConnectedTo.erase(peer);
            }
            if (alive && !wasUp) {
                log.push_back({FIRST_CONTACT, peer});
                everConnectedTo.insert(peer);
            }
        }
    }
};

#define EXPECT(cond) do { if (!(cond)) { std::cerr << "FAIL line " << __LINE__ \
    << ": " #cond "\n"; std::exit(1); } } while (0)

int main() {
    /* --- Test 1: cold boot. Peers all down → no events. -------------- */
    {
        ReconnectorMock r;
        r.Tick({{1, false}, {2, false}, {3, false}});
        EXPECT(r.log.empty());
        EXPECT(r.everConnectedTo.empty());
        std::cout << "1. cold-boot silence: OK\n";
    }

    /* --- Test 2: peers come up in arbitrary order ------------------- */
    {
        ReconnectorMock r;
        // Memory comes up first; Heartbeat later. Order intentionally
        // reversed from the old SCM dependency chain — Heartbeat used
        // to have to be up before Memory could even be queried.
        r.Tick({{1, false}, {2, true}, {3, false}}); // Memory up
        EXPECT(r.log.size() == 1);
        EXPECT(r.log[0] == std::make_pair(FIRST_CONTACT, 2));
        r.Tick({{1, true}, {2, true}, {3, false}});  // Heartbeat up
        EXPECT(r.log.size() == 2);
        EXPECT(r.log[1] == std::make_pair(FIRST_CONTACT, 1));
        r.Tick({{1, true}, {2, true}, {3, true}});   // Cognitive up
        EXPECT(r.log.size() == 3);
        EXPECT(r.log[2] == std::make_pair(FIRST_CONTACT, 3));
        // Mesh fully formed.  No further events on idle ticks.
        for (int i = 0; i < 5; i++) r.Tick({{1, true}, {2, true}, {3, true}});
        EXPECT(r.log.size() == 3);
        std::cout << "2. arbitrary-order convergence: OK\n";
    }

    /* --- Test 3: a peer crashes mid-flight, then recovers ------------ */
    {
        ReconnectorMock r;
        r.Tick({{1, true}, {2, true}});             // both up
        EXPECT(r.log.size() == 2);
        r.Tick({{1, true}, {2, false}});            // peer 2 crashes
        EXPECT(r.log.size() == 3);
        EXPECT(r.log.back() == std::make_pair(LOST, 2));
        EXPECT(r.everConnectedTo.count(2) == 0);
        r.Tick({{1, true}, {2, false}});            // still down — silent
        EXPECT(r.log.size() == 3);
        r.Tick({{1, true}, {2, true}});             // peer 2 recovers
        EXPECT(r.log.size() == 4);
        EXPECT(r.log.back() == std::make_pair(FIRST_CONTACT, 2));
        EXPECT(r.everConnectedTo.count(2) == 1);
        std::cout << "3. crash-and-recover: OK\n";
    }

    /* --- Test 4: idempotent — same state across many ticks --------- */
    {
        ReconnectorMock r;
        for (int i = 0; i < 100; i++) r.Tick({{1, true}});
        EXPECT(r.log.size() == 1); // exactly ONE first-contact, ever
        std::cout << "4. idempotent steady state: OK\n";
    }

    std::cout << "\nALL PASSIVE-MESH TESTS PASSED\n";
    return 0;
}
