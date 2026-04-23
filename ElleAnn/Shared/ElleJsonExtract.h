/*──────────────────────────────────────────────────────────────────────────────
 * ElleJsonExtract.h — brace-balanced JSON object extraction from LLM text.
 *
 * LLMs often wrap structured output in prose / code fences:
 *   "Here's the answer: ```json\n{\"x\":1}\n``` hope that helps"
 * Our old approach was `response.substr(first_{, last_} - first_{ + 1)`,
 * which breaks the moment the prose ALSO contains braces ("Example: {a}").
 * This extractor walks the string, counting nesting while respecting
 * string literals and escape sequences, and returns the first top-level
 * `{...}` block that parses as JSON.
 *──────────────────────────────────────────────────────────────────────────────*/
#ifndef ELLE_JSON_EXTRACT_H
#define ELLE_JSON_EXTRACT_H

#include "json.hpp"
#include <string>

namespace Elle {

/* Scan `s` for a brace-balanced `{...}` region and parse it as JSON.
 * Returns true on success with `out` populated. Malformed spans are
 * skipped — the scan tries the next `{` until something parses or the
 * string is exhausted.                                                  */
inline bool ExtractJsonObject(const std::string& s, nlohmann::json& out) {
    if (s.empty()) return false;
    size_t i = 0;
    while (i < s.size()) {
        size_t start = s.find('{', i);
        if (start == std::string::npos) return false;
        int depth = 0;
        bool inStr = false;
        bool esc   = false;
        size_t end = std::string::npos;
        for (size_t j = start; j < s.size(); j++) {
            char c = s[j];
            if (inStr) {
                if (esc) { esc = false; continue; }
                if (c == '\\') { esc = true; continue; }
                if (c == '"')  { inStr = false; continue; }
            } else {
                if (c == '"') { inStr = true; continue; }
                if (c == '{') depth++;
                else if (c == '}') {
                    depth--;
                    if (depth == 0) { end = j; break; }
                }
            }
        }
        if (end == std::string::npos) return false; /* unbalanced */
        try {
            out = nlohmann::json::parse(s.substr(start, end - start + 1));
            return out.is_object();
        } catch (...) {
            /* this candidate didn't parse — advance past it and try again */
            i = start + 1;
        }
    }
    return false;
}

} /* namespace Elle */

#endif /* ELLE_JSON_EXTRACT_H */
