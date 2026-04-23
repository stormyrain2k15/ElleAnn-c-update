/*------------------------------------------------------------------------------
 * test_json_extract.cpp -- strictness tests for Elle::ExtractJsonObject.
 *
 * Standalone executable. Build standalone on any C++17 compiler:
 *
 *   cl  /std:c++17 /EHsc /I ..\Shared test_json_extract.cpp
 *   g++ -std=c++17 -I ../Shared test_json_extract.cpp -o test_json_extract
 *
 * Exits non-zero on any failing assertion so it slots directly into CI /
 * MSBuild post-build steps.
 *------------------------------------------------------------------------------*/
#include "ElleJsonExtract.h"

#include <cassert>
#include <cstdio>
#include <string>

using Elle::ExtractJsonObject;
using nlohmann::json;

static int g_pass = 0;
static int g_fail = 0;

#define EXPECT(cond, label) do {                                     \
    if (cond) { ++g_pass; std::printf("  ok    %s\n", label); }       \
    else      { ++g_fail; std::printf("  FAIL  %s\n", label); }       \
} while (0)

int main() {
    /* 1. Plain object, no prose.                                          */
    {
        json out;
        EXPECT(ExtractJsonObject("{\"x\":1}", out) && out["x"] == 1,
               "plain object");
    }

    /* 2. Prose wrapping.                                                  */
    {
        json out;
        EXPECT(ExtractJsonObject("Here you go: {\"y\":2}. Hope that helps!", out)
               && out["y"] == 2,
               "prose wrapping");
    }

    /* 3. Prose with DECOY braces BEFORE the real JSON.                    */
    {
        json out;
        EXPECT(ExtractJsonObject(
                   "Example input: {not valid}\nActual: {\"z\":3}", out)
               && out["z"] == 3,
               "decoy braces before real JSON");
    }

    /* 4. Nested objects -- must close at the outermost `}`, not the first.*/
    {
        json out;
        EXPECT(ExtractJsonObject("{\"a\":{\"b\":{\"c\":42}}} trailing", out)
               && out["a"]["b"]["c"] == 42,
               "nested objects");
    }

    /* 5. Braces INSIDE strings must not affect depth counting.            */
    {
        json out;
        EXPECT(ExtractJsonObject("{\"s\":\"} }} { {{\"}", out)
               && out["s"] == "} }} { {{",
               "braces inside strings");
    }

    /* 6. Escaped quote inside a string must not break string scan.        */
    {
        json out;
        EXPECT(ExtractJsonObject("{\"s\":\"he said \\\"hi\\\" to me\"}", out)
               && out["s"] == "he said \"hi\" to me",
               "escaped quotes inside string");
    }

    /* 7. \uXXXX surrogate-pair round trip (grinning face emoji U+1F600).  */
    {
        json out;
        /* The two-codeunit surrogate pair \uD83D\uDE00 encodes U+1F600.
         * nlohmann::json decodes it to the 4-byte UTF-8 sequence
         * F0 9F 98 80.                                                    */
        const std::string in =
            "noise {\"emoji\":\"\\uD83D\\uDE00\",\"ok\":true} more noise";
        const bool ok = ExtractJsonObject(in, out);
        const std::string expected = "\xF0\x9F\x98\x80";
        EXPECT(ok && out["emoji"].get<std::string>() == expected && out["ok"] == true,
               "surrogate-pair \\uD83D\\uDE00 round trip");
    }

    /* 8. \u escape that LOOKS like a close-brace must not terminate the
     *    object prematurely. U+007D = '}'.                                */
    {
        json out;
        EXPECT(ExtractJsonObject("{\"s\":\"\\u007D\"}", out)
               && out["s"] == "}",
               "\\u007D inside string does not close the object");
    }

    /* 9. Embedded raw NUL outside a string -- must fail closed.           */
    {
        json out;
        std::string in;
        in.push_back('{');
        in.push_back(static_cast<char>(0));
        in.push_back('}');
        EXPECT(!ExtractJsonObject(in, out),
               "raw NUL outside string fails closed");
    }

    /* 10. Unbalanced -- no close brace ever arrives. Scanner now
     *     continues past unbalanced candidates, so an input with ONLY
     *     an unbalanced brace returns false; but a later valid object
     *     must still be found (covered in test 16).                   */
    {
        json out;
        EXPECT(!ExtractJsonObject("{\"k\":\"v\"", out),
               "unbalanced input returns false");
    }

    /* 11. Empty string.                                                   */
    {
        json out;
        EXPECT(!ExtractJsonObject("", out),
               "empty string");
    }

    /* 12. Malformed then valid -- scanner advances past the first `{`
     *     and finds the second.                                           */
    {
        json out;
        EXPECT(ExtractJsonObject("{not json at all}...{\"ok\":1}", out)
               && out["ok"] == 1,
               "malformed candidate skipped, second candidate succeeds");
    }

    /* 13. Array at top-level is NOT an object -- extractor returns false.*/
    {
        json out;
        EXPECT(!ExtractJsonObject("[1,2,3]", out),
               "top-level array rejected");
    }

    /* 14. Deeply nested but under the kMaxDepth ceiling.                  */
    {
        json out;
        std::string in;
        const int N = 64;
        for (int k = 0; k < N; ++k) in += "{\"a\":";
        in += "42";
        for (int k = 0; k < N; ++k) in += "}";
        EXPECT(ExtractJsonObject(in, out),
               "deeply nested within depth budget");
    }

    /* 15. Over-nested -- must abort fail-closed, not stack-overflow.      */
    {
        json out;
        std::string in(2048, '{');
        EXPECT(!ExtractJsonObject(in, out),
               "runaway nesting fails closed");
    }

    /* 16. Unbalanced prefix followed by a valid object -- scanner must
     *     walk past the stray `{` and still find the real JSON.        */
    {
        json out;
        EXPECT(ExtractJsonObject("preamble { dangling forever... "
                                  "then later {\"real\":1}", out)
               && out["real"] == 1,
               "unbalanced prefix does not mask a later valid object");
    }

    /* 17-22. Rich classification via ExtractJsonObjectEx.              */
    {
        using Elle::ExtractJsonObjectEx;
        using Elle::JsonExtractResult;
        json out;
        EXPECT(ExtractJsonObjectEx("no braces here at all", out)
               == JsonExtractResult::NoBraceFound,
               "Ex: no-brace classification");
        EXPECT(ExtractJsonObjectEx("{\"k\":1}", out) == JsonExtractResult::Ok
               && out["k"] == 1,
               "Ex: ok classification");
        EXPECT(ExtractJsonObjectEx("{\"k\":", out)
               == JsonExtractResult::Unbalanced,
               "Ex: unbalanced classification");
        EXPECT(ExtractJsonObjectEx("{definitely not json}", out)
               == JsonExtractResult::ParseFailed,
               "Ex: parse-failed classification");
        std::string nulStr;
        nulStr.push_back('{'); nulStr.push_back(0); nulStr.push_back('}');
        EXPECT(ExtractJsonObjectEx(nulStr, out) == JsonExtractResult::FailClosed,
               "Ex: fail-closed on embedded NUL");
        EXPECT(ExtractJsonObjectEx(std::string(2048, '{'), out)
               == JsonExtractResult::FailClosed,
               "Ex: fail-closed on runaway depth");
    }

    std::printf("\n%d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
