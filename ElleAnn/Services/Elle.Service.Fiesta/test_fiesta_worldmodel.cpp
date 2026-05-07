/*══════════════════════════════════════════════════════════════════════════════
 * test_fiesta_worldmodel.cpp — Phase 6b-Alpha regression test.
 *
 *   Drives the headless WorldModel through the lifecycle a real Fiesta
 *   session produces, and asserts the JSON snapshot reports what
 *   Cognitive will see on `{"op":"get_world"}`.
 *
 *   Scope: WorldModel ONLY — independent of Client / Connection /
 *   cipher, so this test compiles and runs on Linux with no Windows
 *   shim needed (uses only <mutex>, <chrono>, nlohmann/json).
 *
 *   Compile (Linux portable):
 *     g++ -std=c++17 -I . -I ../../Shared \
 *         Services/Elle.Service.Fiesta/test_fiesta_worldmodel.cpp \
 *         -o /tmp/test_fiesta_worldmodel
 *══════════════════════════════════════════════════════════════════════════════*/

#include "FiestaWorldModel.h"

#include <cassert>
#include <cstdio>
#include <string>

using Fiesta::WorldModel;
using Fiesta::EntityKind;

static void test_self_base_populates_snapshot() {
    WorldModel w;
    w.UpdateSelfBase(42, "ElleAnn");
    auto j = w.SnapshotJson();
    assert(j["self"]["chrregnum"] == 42);
    assert(j["self"]["char_name"] == "ElleAnn");
    assert(j["entities"].size() == 0);
    std::fprintf(stderr, "[PASS] self_base populates snapshot\n");
}

static void test_player_upsert_then_remove() {
    WorldModel w;
    w.UpsertPlayer(0x1001, "Crystal");
    w.UpsertPlayer(0x1002, "Bob");
    assert(w.EntityCount() == 2);

    auto j = w.SnapshotJson();
    assert(j["entities"].size() == 2);

    /* Names survive the round-trip and the kind is "player". */
    bool sawCrystal = false, sawBob = false;
    for (const auto& e : j["entities"]) {
        assert(e["kind"] == "player");
        if (e["name"] == "Crystal") sawCrystal = true;
        if (e["name"] == "Bob")     sawBob     = true;
    }
    assert(sawCrystal && sawBob);

    w.RemoveEntity(0x1001);
    assert(w.EntityCount() == 1);
    std::fprintf(stderr, "[PASS] player upsert + remove\n");
}

static void test_mob_carries_mob_id() {
    WorldModel w;
    w.UpsertMob(0x2000, 5012);
    auto j = w.SnapshotJson();
    assert(j["entities"].size() == 1);
    auto& e = j["entities"][0];
    assert(e["kind"]   == "mob");
    assert(e["mob_id"] == 5012);
    /* Mobs don't carry a name — snapshot should omit the field cleanly. */
    assert(!e.contains("name"));
    std::fprintf(stderr, "[PASS] mob carries mob_id (no name)\n");
}

static void test_mob_then_player_same_handle() {
    /* Server reuses handles after REGEN — a mob handle can later be a
     * player (or the opposite) when the slot cycles. Upsert must replace
     * kind + fields, not merge stale ones. */
    WorldModel w;
    w.UpsertMob(0x3000, 4242);
    w.UpsertPlayer(0x3000, "Reused");
    auto j = w.SnapshotJson();
    assert(j["entities"].size() == 1);
    auto& e = j["entities"][0];
    assert(e["kind"] == "player");
    assert(e["name"] == "Reused");
    std::fprintf(stderr, "[PASS] handle reuse replaces kind\n");
}

static void test_zone_and_position_round_trip() {
    WorldModel w;
    w.SetZone("RouN");
    w.UpdateSelfPosition(12345, 67890);
    w.SetLoginState("in_game");

    auto j = w.SnapshotJson();
    assert(j["zone"]["map"] == "RouN");
    assert(j["self"]["x"] == 12345);
    assert(j["self"]["y"] == 67890);
    assert(j["self"]["state"] == "in_game");
    std::fprintf(stderr, "[PASS] zone + position + state round-trip\n");
}

static void test_clear_zone_wipes_entities() {
    WorldModel w;
    w.SetZone("RouN");
    w.UpsertPlayer(1, "A");
    w.UpsertMob(2, 7);
    assert(w.EntityCount() == 2);
    w.ClearZone();
    assert(w.EntityCount() == 0);
    auto j = w.SnapshotJson();
    assert(j["zone"]["map"] == "");
    assert(j["entities"].size() == 0);
    std::fprintf(stderr, "[PASS] clear_zone wipes entities\n");
}

static void test_self_handle_update() {
    WorldModel w;
    w.UpdateSelfHandle(0xABCD);
    auto j = w.SnapshotJson();
    assert(j["self"]["handle"] == 0xABCD);
    std::fprintf(stderr, "[PASS] self handle update\n");
}

static void test_full_session_simulation() {
    /* Walk through a realistic capture order:
     *   1. SEED_ACK → SetLoginState("login_connecting")
     *   2. CHAR_BASE_CMD (self) → UpdateSelfBase
     *   3. MAP_LOGINCOMPLETE_CMD → SetLoginState("in_game")
     *   4. BRIEFINFO login_character (Crystal)
     *   5. BRIEFINFO login_character (Bob)
     *   6. REGEN_MOB goblin
     *   7. MoveTo(100,200) — self position
     *   8. BRIEFINFO briefinfo_delete Crystal leaves
     *   9. get_world snapshot → Bob + goblin + us at (100,200) on RouN
     */
    WorldModel w;
    w.SetLoginState("login_connecting");
    w.UpdateSelfBase(5, "ElleAnn");
    w.SetZone("RouN");
    w.SetLoginState("in_game");
    w.UpsertPlayer(0x1001, "Crystal");
    w.UpsertPlayer(0x1002, "Bob");
    w.UpsertMob   (0x5001, 3010);
    w.UpdateSelfPosition(100, 200);
    w.UpdateSelfHandle(0x0005);
    w.RemoveEntity(0x1001);

    auto j = w.SnapshotJson();
    assert(j["self"]["char_name"] == "ElleAnn");
    assert(j["self"]["chrregnum"] == 5);
    assert(j["self"]["handle"]    == 0x0005);
    assert(j["self"]["state"]     == "in_game");
    assert(j["self"]["x"]         == 100);
    assert(j["self"]["y"]         == 200);
    assert(j["zone"]["map"]       == "RouN");
    assert(j["entities"].size()   == 2);

    /* Verify Crystal is gone, Bob + goblin remain. */
    bool sawBob = false, sawGoblin = false;
    for (const auto& e : j["entities"]) {
        const std::string name = e.value("name", std::string(""));
        if (e["kind"] == "player" && name == "Bob") sawBob = true;
        if (e["kind"] == "mob"    && e.value("mob_id", 0) == 3010) sawGoblin = true;
        assert(name != "Crystal");
    }
    assert(sawBob);
    assert(sawGoblin);
    std::fprintf(stderr, "[PASS] full session simulation\n");
}

int main() {
    test_self_base_populates_snapshot();
    test_player_upsert_then_remove();
    test_mob_carries_mob_id();
    test_mob_then_player_same_handle();
    test_zone_and_position_round_trip();
    test_clear_zone_wipes_entities();
    test_self_handle_update();
    test_full_session_simulation();
    std::fprintf(stderr, "\nALL 8 WORLDMODEL TESTS PASSED\n");
    return 0;
}
