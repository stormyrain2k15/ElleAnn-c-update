/*══════════════════════════════════════════════════════════════════════════════
 * test_compile_audit_feb2026.cpp
 *
 *   Regression pin for the 6 real /WX bugs the Feb 2026 compile audit
 *   found and fixed.  Each fix lives in production code; this test
 *   asserts the source files no longer carry the bad pattern, so a
 *   future agent can't silently reintroduce one.
 *
 *   Compile + run:
 *     g++ -std=c++17 -Wall -Wextra -Werror \
 *         -I /app/ElleAnn/Debug/_winstub \
 *         test_compile_audit_feb2026.cpp -o tcompile && ./tcompile
 *══════════════════════════════════════════════════════════════════════════════*/
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string Slurp(const char* path) {
    std::ifstream in(path, std::ios::binary);
    std::ostringstream os; os << in.rdbuf();
    return os.str();
}

int g_failures = 0;
void RequireNotContains(const char* path, const char* needle, const char* explanation) {
    auto src = Slurp(path);
    if (src.find(needle) != std::string::npos) {
        std::fprintf(stderr, "FAIL %s contains forbidden pattern: %s\n  %s\n",
                     path, needle, explanation);
        g_failures++;
    } else {
        std::fprintf(stdout, "ok   %s — %s\n", path, explanation);
    }
}
void RequireContains(const char* path, const char* needle, const char* explanation) {
    auto src = Slurp(path);
    if (src.find(needle) == std::string::npos) {
        std::fprintf(stderr, "FAIL %s missing required pattern: %s\n  %s\n",
                     path, needle, explanation);
        g_failures++;
    } else {
        std::fprintf(stdout, "ok   %s — %s\n", path, explanation);
    }
}

}  /* namespace */

int main() {
    /* Bug 1 — orphaned block comment in HTTPServer.cpp diag/routes section.
     *         The asterisk lines at the start of the comment had no
     *         opening slash-star and were being parsed as multiplications
     *         of the m_router registration call.                         */
    RequireNotContains(
        "/app/ElleAnn/Services/Elle.Service.HTTP/HTTPServer.cpp",
        " * greppingfor Register() calls.",
        "orphaned block comment fragment must not return");

    /* Bug 2 — ElleConfig::LoadDefaults() called 5 PopulateXxxConfig methods
     *         that didn't exist.  Real method is PopulateFromJSON.       */
    RequireNotContains(
        "/app/ElleAnn/Shared/ElleConfig.cpp",
        "PopulateLLMConfig();",
        "non-existent populate methods must not return");
    RequireContains(
        "/app/ElleAnn/Shared/ElleConfig.cpp",
        "PopulateFromJSON(m_root);",
        "real PopulateFromJSON is the correct call");

    /* Bug 3 — JsonType::Number / num_val don't exist.  Enum has Int+Float
     *         with int_val/float_val.                                    */
    RequireNotContains(
        "/app/ElleAnn/Shared/ElleConfig.cpp",
        "JsonType::Number",
        "JsonType::Number is a typo (real is Int/Float)");
    RequireNotContains(
        "/app/ElleAnn/Shared/ElleConfig.cpp",
        "v.num_val",
        "num_val is a typo (real is int_val/float_val)");

    /* Bug 4 — base virtual OnMessage had named params that several
     *         services don't override → /WX unused-parameter.            */
    RequireContains(
        "/app/ElleAnn/Shared/ElleServiceBase.h",
        "OnMessage(const ElleIPCMessage& /*msg*/, ELLE_SERVICE_ID /*sender*/)",
        "base virtual OnMessage must mark its params unused-OK");

    /* Bug 5 — GoalEngine::source_drive is uint32_t; `>= 0` was always true.
     *         /WX -Wtype-limits flag.                                    */
    RequireNotContains(
        "/app/ElleAnn/Services/Elle.Service.GoalEngine/GoalEngine.cpp",
        "g.source_drive >= 0 &&",
        "uint32 >= 0 redundant comparison");

    /* Bug 6 — comment-within-comment hits MSVC -Wcomment.                */
    const char* commentNests[][2] = {
        { "/app/ElleAnn/Services/Elle.Service.HTTP/HTTPServer.cpp",       "/api/diag/* is dev-only"        },
        { "/app/ElleAnn/Services/Elle.Service.HTTP/HTTPServer.cpp",       "/api/identity/*"                },
        { "/app/ElleAnn/Shared/ElleDB_Content.cpp",                        "/api/memory/* endpoints"        },
        { "/app/ElleAnn/Services/Elle.Service.Family/Family.cpp",          "bin/*.exe"                      },
        { "/app/ElleAnn/Services/Elle.Service.XChromosome/XEngine.h",      "(HTTP /api/x/*"                 },
        { "/app/ElleAnn/Services/Elle.Service.Cognitive/CognitiveEngine.cpp", "/api/diag/* (and operator log scans)" },
        { "/app/ElleAnn/Services/Elle.Service.Fiesta/FiestaClient.cpp",    "_re_artifacts/pdb/extracted/*.md" },
    };
    for (const auto& nc : commentNests) {
        RequireNotContains(nc[0], nc[1],
            "comment-within-comment must stay rewritten (-Wcomment under MSVC /WX)");
    }

    /* Bug 7 — CountTable whitelist included tables not in the schema. */
    RequireNotContains(
        "/app/ElleAnn/Shared/ElleDB_Content.cpp",
        "\"InternalNarrative\"",
        "non-existent table InternalNarrative must not return to the whitelist");
    RequireNotContains(
        "/app/ElleAnn/Shared/ElleDB_Content.cpp",
        "\"DreamIntegration\"",
        "non-existent table DreamIntegration must not return to the whitelist");

    /* Bug 8 — Directory.Build.props had narrow-conv warnings PROMOTED
     *         to errors via /WX with no source-side fixes, killing the
     *         entire ElleCore.Shared build (cascading to 22 LNK1181s
     *         on every consumer service). The suppression list MUST
     *         carry 4244;4267;4996 until those source fixes land.   */
    RequireContains(
        "/app/ElleAnn/Directory.Build.props",
        "4244;4267;4996",
        "narrow-conv suppressions must stay until source-level cleanup is done");
    RequireContains(
        "/app/ElleAnn/Shared/ElleCore.Shared.vcxproj",
        "<TargetName>ElleCore.Shared</TargetName>",
        "ElleCore.Shared must pin TargetName explicitly so consumers find the lib");
    RequireContains(
        "/app/ElleAnn/Shared/ElleCore.Shared.vcxproj",
        "<TargetExt>.lib</TargetExt>",
        "ElleCore.Shared must pin TargetExt to avoid VS2026 Insiders import-order race");

    /* Bug 9 — Per-project <PlatformToolset>v145</PlatformToolset>
     *         overrides locked the build to VS 2026 only. CI runners
     *         (VS 2022 Enterprise) only have v143, fail with MSB8020.
     *         All overrides stripped; everyone inherits v143 from
     *         Directory.Build.props (works on VS 2022 + VS 2026).   */
    {
        const char* projects[] = {
            "/app/ElleAnn/Shared/ElleCore.Shared.vcxproj",
            "/app/ElleAnn/Services/Elle.Service.HTTP/Elle.Service.HTTP.vcxproj",
            "/app/ElleAnn/Services/Elle.Service.Fiesta/Elle.Service.Fiesta.vcxproj",
            "/app/ElleAnn/ASM/Elle.ASM.Hardware/Elle.ASM.Hardware.vcxproj",
            "/app/ElleAnn/Lua/Elle.Lua.Behavioral/Elle.Lua.Behavioral.vcxproj",
        };
        for (const char* p : projects) {
            RequireNotContains(p, "<PlatformToolset>v145</PlatformToolset>",
                "v145 toolset override must NOT pin to VS 2026 only — CI uses v143");
        }
    }

    if (g_failures) {
        std::fprintf(stderr, "\nFAIL — %d audit pin(s) regressed.\n", g_failures);
        return 1;
    }
    std::fprintf(stdout, "\nALL AUDIT PINS HOLD\n");
    return 0;
}
