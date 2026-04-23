/*------------------------------------------------------------------------------
 * ElleJsonExtract.h -- brace-balanced JSON object extraction from LLM text.
 *
 * LLMs often wrap structured output in prose / code fences:
 *   "Here's the answer: ```json\n{\"x\":1}\n``` hope that helps"
 * Our old approach was `response.substr(first_{, last_} - first_{ + 1)`,
 * which breaks the moment the prose ALSO contains braces ("Example: {a}").
 * This extractor walks the string, counting nesting while respecting
 * string literals and escape sequences, and returns the first top-level
 * `{...}` block that parses as JSON.
 *
 * Strictness contract (OpSec audit):
 *   - String-literal scan respects ALL JSON escape sequences including
 *     the 6-char \uXXXX form (incl. surrogate pairs \uD83D\uDE00).
 *     A stray `{` or `}` inside a string can never change the nesting
 *     counter.
 *   - A raw NUL byte (0x00) outside a string literal aborts the scan --
 *     JSON does not permit it, and letting it through risks downstream
 *     C string handlers truncating the buffer.
 *   - We never parse more than one candidate region per starting `{`;
 *     malformed candidates advance by exactly one character, so the
 *     worst case is O(N^2) in the length of the prose -- bounded and
 *     deterministic, no infinite loop on pathological input.
 *------------------------------------------------------------------------------*/
#ifndef ELLE_JSON_EXTRACT_H
#define ELLE_JSON_EXTRACT_H

#include "json.hpp"
#include <string>

namespace Elle {

/* Scan `s` for a brace-balanced `{...}` region and parse it as JSON.
 * Returns true on success with `out` populated. Malformed spans are
 * skipped -- the scan tries the next `{` until something parses or the
 * string is exhausted.
 *
 * On fail-closed conditions (embedded NUL outside a string, runaway
 * nesting > 1024) the scan terminates immediately and returns false.
 */
inline bool ExtractJsonObject(const std::string& s, nlohmann::json& out) {
    if (s.empty()) return false;
    constexpr int kMaxDepth = 1024;
    size_t i = 0;
    while (i < s.size()) {
        size_t start = s.find('{', i);
        if (start == std::string::npos) return false;
        int depth = 0;
        bool inStr = false;
        size_t end = std::string::npos;
        bool aborted = false;
        for (size_t j = start; j < s.size(); j++) {
            unsigned char c = static_cast<unsigned char>(s[j]);
            if (inStr) {
                if (c == '\\') {
                    /* Consume the escape sequence in full so a bogus
                     * byte pattern like `\"` or `\u007D` inside a
                     * string can never be mistaken for a close-brace
                     * or close-quote at the outer level.           */
                    if (j + 1 >= s.size()) { aborted = true; break; }
                    char esc_char = s[j + 1];
                    if (esc_char == 'u') {
                        /* \uXXXX -- 4 hex digits must follow.      */
                        if (j + 5 >= s.size()) { aborted = true; break; }
                        j += 5; /* skip `u` + 4 hex; loop `j++` skips `\\` */
                    } else {
                        /* \" \\ \/ \b \f \n \r \t -- one char.      */
                        j += 1;
                    }
                    continue;
                }
                if (c == '"') { inStr = false; continue; }
                /* JSON forbids raw control chars < 0x20 inside a
                 * string literal; but we intentionally tolerate them
                 * here because the final parse step will reject the
                 * candidate, and the outer loop will move on.      */
            } else {
                if (c == 0) { aborted = true; break; }  /* embedded NUL -- fail closed */
                if (c == '"') { inStr = true; continue; }
                if (c == '{') {
                    if (++depth > kMaxDepth) { aborted = true; break; }
                } else if (c == '}') {
                    depth--;
                    if (depth < 0) { aborted = true; break; }
                    if (depth == 0) { end = j; break; }
                }
            }
        }
        if (aborted) return false;
        if (end == std::string::npos) return false; /* unbalanced */
        try {
            out = nlohmann::json::parse(s.substr(start, end - start + 1));
            return out.is_object();
        } catch (...) {
            /* this candidate didn't parse -- advance past it and try again */
            i = start + 1;
        }
    }
    return false;
}

} /* namespace Elle */

#endif /* ELLE_JSON_EXTRACT_H */
