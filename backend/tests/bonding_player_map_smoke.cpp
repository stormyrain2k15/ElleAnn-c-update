/* Linux smoke test for FiestaPlayerBondMap — the per-in-world-player
 * affective profile cache that lives inside the Bonding service.
 *
 * Compile:  g++ -std=c++17 -Wall -Wextra -Werror \
 *               -Iapp/ElleAnn/Services/Elle.Service.Bonding \
 *               app/backend/tests/bonding_player_map_smoke.cpp \
 *               -o /tmp/bonding_player_map_smoke
 */
#include "FiestaPlayerBondMap.h"

#include <cassert>
#include <cstdio>

using Elle::Bonding::FiestaPlayerBondMap;
using Elle::Bonding::PlayerBond;

static void test_appear_and_chat() {
    FiestaPlayerBondMap m;
    m.OnAppear("Bob",   0x0010, 1000);
    m.OnAppear("Alice", 0x0020, 1010);
    /* Same player coming back with a NEW handle (zone change). */
    m.OnAppear("Bob",   0x0030, 5000);

    PlayerBond bob = m.Get("Bob");
    assert(bob.name == "Bob");
    assert(bob.encounter_count == 2);
    assert(bob.last_handle == 0x0030);   /* most-recent handle wins */
    assert(bob.first_seen_ms == 1000);
    assert(bob.last_seen_ms  == 5000);
    assert(bob.familiarity > 0.0f);

    m.OnChat("Bob", 0x0030, "normal", 5100);
    m.OnChat("Bob", 0x0030, "shout",  5200);
    m.OnChat("Bob", 0x0030, "whisper_in",  5300);
    m.OnChat("Bob", 0x0030, "whisper_out", 5400);

    bob = m.Get("Bob");
    assert(bob.chat_count       == 1);
    assert(bob.shout_count      == 1);
    assert(bob.whispers_to_us   == 1);
    assert(bob.whispers_from_us == 1);
    /* Whispers count double for familiarity vs normal/shout. */
    assert(bob.familiarity > 0.10f);
    std::printf("[ok] appear + chat update bond record\n");
}

static void test_unknown_speaker_skipped() {
    FiestaPlayerBondMap m;
    /* Empty name should be a no-op (we never key bonds by anonymous
     * handles).                                                       */
    m.OnChat("", 0x0010, "normal", 1000);
    m.OnAppear("", 0x0010, 1000);
    assert(m.Size() == 0);
    std::printf("[ok] empty name is a no-op (anonymous handles ignored)\n");
}

static void test_affinity_clamp() {
    FiestaPlayerBondMap m;
    m.OnAppear("Carol", 0x0040, 1000);
    /* Pile on positive nudges past the +1.0 ceiling. */
    for (int i = 0; i < 50; i++) m.NudgeAffinity("Carol", 0.1f, 1000);
    assert(m.Get("Carol").affinity == 1.0f);
    /* And clamp at -1.0 too. */
    for (int i = 0; i < 50; i++) m.NudgeAffinity("Carol", -0.2f, 1000);
    assert(m.Get("Carol").affinity == -1.0f);
    std::printf("[ok] affinity clamp [-1.0, 1.0]\n");
}

static void test_eviction_under_load() {
    FiestaPlayerBondMap m;
    /* Spam well past kMaxBonds — interleave a single high-engagement
     * "Anchor" entry that should NEVER be evicted. */
    m.OnAppear("Anchor", 0x0001, 1000);
    for (int i = 0; i < 100; i++) m.OnChat("Anchor", 0x0001, "whisper_in", 1000);

    for (uint32_t i = 0; i < FiestaPlayerBondMap::kMaxBonds + 500; i++) {
        char name[16]; std::snprintf(name, sizeof(name), "noise_%u", i);
        m.OnAppear(name, (uint16_t)i, 1000 + i);
    }
    assert(m.Size() <= FiestaPlayerBondMap::kMaxBonds);
    /* Anchor should have survived because it's the most engaged. */
    assert(!m.Get("Anchor").name.empty());
    std::printf("[ok] eviction respects engagement (Anchor survived %zu inserts)\n",
                m.Size());
}

static void test_snapshot_sorted_by_familiarity() {
    FiestaPlayerBondMap m;
    m.OnAppear("Stranger",      0x0001, 1000);
    /* Frequent appearances → high familiarity. */
    for (int i = 0; i < 30; i++) m.OnAppear("Friend",  0x0002, 1000);
    for (int i = 0; i < 10; i++) m.OnAppear("Acquaint", 0x0003, 1000);

    auto snap = m.SnapshotByFamiliarity(3);
    assert(snap.size() == 3);
    assert(snap[0].name == "Friend");
    assert(snap[1].name == "Acquaint");
    assert(snap[2].name == "Stranger");
    std::printf("[ok] SnapshotByFamiliarity orders by familiarity desc\n");
}

int main() {
    test_appear_and_chat();
    test_unknown_speaker_skipped();
    test_affinity_clamp();
    test_eviction_under_load();
    test_snapshot_sorted_by_familiarity();
    std::printf("\nbonding_player_map_smoke: ALL PASS\n");
    return 0;
}
