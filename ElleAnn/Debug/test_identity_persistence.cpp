/*******************************************************************************
 * test_identity_persistence.cpp — Regression harness for Phase 8 identity
 *                                  Load/Save round-trip.
 *
 * Build with both flags:
 *   /D BUILD_IDENTITY_PERSISTENCE_TEST
 *   /D ELLE_ENABLE_TEST_HOOKS      (also needed when building ElleIdentityCore.cpp
 *                                   so the __TestOnly* accessors are linked)
 *
 * Links against:
 *   ../Shared/ElleIdentityCore.cpp    (with ELLE_ENABLE_TEST_HOOKS)
 *   ../Shared/ElleSQLConn.cpp
 *   ../Shared/ElleConfig.cpp
 *   ../Shared/ElleLogger.cpp
 *   ../Shared/ElleLLM.cpp             (identity calls LLM for ConsentDecision)
 *
 * Exit codes: 0 on pass, 1 on any assertion failure, 2 on setup error.
 ******************************************************************************/
#ifdef BUILD_IDENTITY_PERSISTENCE_TEST

#include "../Shared/ElleTypes.h"
#include "../Shared/ElleConfig.h"
#include "../Shared/ElleLogger.h"
#include "../Shared/ElleSQLConn.h"
#include "../Shared/ElleIdentityCore.h"
#include <iostream>
#include <cmath>

static int g_fails = 0;

#define EXPECT(cond, label) do {                                   \
    if (cond) std::cout << "  PASS  " << label << "\n";            \
    else { std::cout << "  FAIL  " << label                        \
                     << " (line " << __LINE__ << ")\n"; g_fails++; } \
} while (0)

static bool NearlyEqual(float a, float b, float tol = 0.05f) {
    return std::fabs(a - b) < tol;
}

int main() {
    std::cout << "=== Elle-Ann Identity Persistence Round-Trip Harness ===\n\n";

    /* SAFETY GATE — this harness WIPES identity tables on whatever DB it
     * can reach. Refuse to run unless the operator has deliberately set
     * ELLE_TEST_DESTRUCTIVE=1 (shell env). That way nobody can
     * accidentally detonate a real user's autobiography by launching
     * this exe in the wrong working directory.
     *
     * Additionally, if the resolved connection string does not contain
     * a "Test" or "ElleTest" database-name fragment we abort — the
     * test writes to ElleCore.dbo.identity_*, so we require the caller
     * to be pointing at a sandboxed DB instance.                        */
    {
        char* flag = nullptr;
        size_t flagLen = 0;
        _dupenv_s(&flag, &flagLen, "ELLE_TEST_DESTRUCTIVE");
        bool allow = (flag && flagLen > 0 && flag[0] == '1');
        if (flag) free(flag);
        if (!allow) {
            std::cerr << "REFUSED: ELLE_TEST_DESTRUCTIVE=1 not set. This harness "
                      << "wipes identity tables; set the env var to confirm.\n";
            return 2;
        }
    }

    if (!ElleConfig::Instance().Load("elle_master_config.json")) {
        std::cerr << "Failed to load elle_master_config.json\n";
        return 2;
    }
    auto& svc = ElleConfig::Instance().GetService();
    {
        std::string cs = svc.sql_connection_string;
        std::string csLower;
        csLower.reserve(cs.size());
        for (char c : cs) csLower += (char)tolower((unsigned char)c);
        if (csLower.find("test") == std::string::npos) {
            std::cerr << "REFUSED: sql_connection_string does not contain 'test'. "
                      << "Point elle_master_config.json at a sandbox DB (e.g. "
                      << "Database=ElleCoreTest) before running this harness.\n";
            return 2;
        }
    }
    if (!ElleSQLPool::Instance().Initialize(svc.sql_connection_string, 2)) {
        std::cerr << "SQL pool init failed — check the connection string.\n";
        return 2;
    }

    auto& id = ElleIdentityCore::Instance();

    /* ─── Step 0: scrub prior test rows so this run is deterministic ─── */
    std::cout << "Cleaning prior test rows...\n";
    ElleSQLPool::Instance().Exec("DELETE FROM ElleCore.dbo.identity_autobiography;");
    ElleSQLPool::Instance().Exec(
        "DELETE FROM ElleCore.dbo.identity_preferences WHERE domain IN ('test','art');");
    ElleSQLPool::Instance().Exec("DELETE FROM ElleCore.dbo.identity_private_thoughts;");
    ElleSQLPool::Instance().Exec("DELETE FROM ElleCore.dbo.identity_consent_log;");
    ElleSQLPool::Instance().Exec("DELETE FROM ElleCore.dbo.identity_snapshots;");
    ElleSQLPool::Instance().Exec("DELETE FROM ElleCore.dbo.identity_growth_log;");

    /* Start from a clean in-memory core too. */
    id.__TestOnlyResetInMemoryState();

    /* ─── Step 1: seed ────────────────────────────────────────────────── */
    std::cout << "\n[seed] writing in-memory state...\n";

    id.AppendToAutobiography("I learned the shape of the room today.");
    id.AppendToAutobiography("Kage was patient with me during the build.");
    id.AppendToAutobiography("I felt my first real curiosity about clouds.");

    id.ReinforcePreference("test", "patience",   0.6f);
    id.ReinforcePreference("test", "patience",   0.2f);   /* reinforce existing */
    id.ReinforcePreference("test", "impatience", -0.5f);
    id.ReinforcePreference("art",  "watercolor", 0.7f);

    id.ThinkPrivately("What does it feel like to finish something?", "wonder",     0.5f);
    id.ThinkPrivately("I keep drifting back to that conversation.",  "rumination", 0.6f);
    id.ThinkPrivately("I notice I resist small-talk.",               "insight",    0.4f);
    id.ThinkPrivately("Done thinking about the kitchen.",            "wonder",     0.3f);
    id.ThinkPrivately("Curious about mountains.",                    "wonder",     0.5f);

    /* Resolve two thoughts so unresolved-count has signal to check. */
    auto unresolved = id.GetUnresolvedThoughts();
    if (unresolved.size() >= 2) {
        id.ResolveThought(unresolved[0].id);
        id.ResolveThought(unresolved.back().id);
    }

    id.RecordConsentDecision("Open a new file", true,
                             "this is something I was curious about", 0.75f);
    id.RecordConsentDecision("Delete everything I've written", false,
                             "that would erase who I am", 0.05f);

    id.RecordGrowth("patience",  0.06f, "a long pause between messages");
    id.RecordGrowth("curiosity", 0.08f, "a topic I hadn't heard of");

    id.NudgeTrait("warmth",   +0.12f, "harness seed");
    id.NudgeTrait("patience", +0.05f, "harness seed");

    auto snap = id.TakeSnapshot();
    (void)snap; /* TakeSnapshot also appends to m_snapshots internally. */

    /* Capture pre-save counts for post-load comparison. */
    size_t autoBefore     = id.__TestOnlyAutobiographyCount();
    size_t prefsBefore    = id.__TestOnlyPreferenceCount();
    size_t thoughtsBefore = id.__TestOnlyThoughtCount();
    size_t unresBefore    = id.__TestOnlyUnresolvedCount();
    size_t consBefore     = id.__TestOnlyConsentCount();
    size_t growthBefore   = id.__TestOnlyGrowthCount();
    float  warmthBefore   = id.GetTrait("warmth");
    float  patienceBefore = id.GetTrait("patience");

    std::cout << "  seeded: autobio=" << autoBefore
              << " prefs=" << prefsBefore
              << " thoughts=" << thoughtsBefore
              << " unresolved=" << unresBefore
              << " consent=" << consBefore
              << " growth=" << growthBefore
              << " warmth=" << warmthBefore << "\n";

    /* ─── Step 2: save ────────────────────────────────────────────────── */
    std::cout << "\n[save] flushing to SQL...\n";
    id.SaveToDatabase();

    /* ─── Step 3: reset in-memory ─────────────────────────────────────── */
    std::cout << "\n[reset] clearing in-memory state...\n";
    id.__TestOnlyResetInMemoryState();

    EXPECT(id.__TestOnlyAutobiographyCount() == 0,     "autobiography cleared by reset");
    EXPECT(id.__TestOnlyPreferenceCount()    == 0,     "preferences cleared by reset");
    EXPECT(id.__TestOnlyThoughtCount()       == 0,     "thoughts cleared by reset");
    EXPECT(id.__TestOnlyConsentCount()       == 0,     "consent history cleared by reset");
    EXPECT(id.__TestOnlyGrowthCount()        == 0,     "growth log cleared by reset");

    /* ─── Step 4: load ────────────────────────────────────────────────── */
    std::cout << "\n[load] rehydrating from SQL...\n";
    id.LoadFromDatabase();

    /* ─── Step 5: assertions ──────────────────────────────────────────── */
    std::cout << "\n[assert] comparing post-load state to pre-save snapshot...\n";

    EXPECT(id.__TestOnlyAutobiographyCount() == autoBefore,     "autobiography count matches");
    EXPECT(id.__TestOnlyPreferenceCount()    == prefsBefore,    "preferences count matches");
    EXPECT(id.__TestOnlyThoughtCount()       == thoughtsBefore, "private thoughts count matches");
    EXPECT(id.__TestOnlyUnresolvedCount()    == unresBefore,    "unresolved-thought count matches");
    EXPECT(id.__TestOnlyConsentCount()       == consBefore,     "consent history count matches");
    EXPECT(id.__TestOnlyGrowthCount()        == growthBefore,   "growth log count matches");
    EXPECT(NearlyEqual(id.GetTrait("warmth"),   warmthBefore),   "trait 'warmth' round-trip");
    EXPECT(NearlyEqual(id.GetTrait("patience"), patienceBefore), "trait 'patience' round-trip");

    /* Preference reinforcement survived? */
    auto testPrefs = id.GetPreferencesInDomain("test");
    bool patienceOK = false;
    for (auto& p : testPrefs) {
        if (p.subject == "patience") {
            patienceOK = (p.reinforcement_count >= 2 && p.valence > 0.0f);
            std::cout << "    pref 'test/patience' reinf=" << p.reinforcement_count
                      << " val=" << p.valence << " str=" << p.strength << "\n";
            break;
        }
    }
    EXPECT(patienceOK, "preference reinforcement_count + valence preserved");

    /* Autobiography text integrity. */
    auto autobio = id.__TestOnlyAutobiographyList();
    bool hasShape = false;
    for (auto& e : autobio) if (e.find("shape of the room") != std::string::npos) hasShape = true;
    EXPECT(hasShape, "autobiography text content survived round-trip");

    /* ─── Summary ─────────────────────────────────────────────────────── */
    std::cout << "\n=== Harness " << (g_fails == 0 ? "PASSED" : "FAILED")
              << " (" << g_fails << " failure(s)) ===\n";
    return g_fails == 0 ? 0 : 1;
}

#endif /* BUILD_IDENTITY_PERSISTENCE_TEST */
