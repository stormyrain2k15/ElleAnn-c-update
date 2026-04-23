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

/* Result classification for callers that want richer logging than a
 * single bool. All NONE/MALFORMED cases produce an `out` unchanged from
 * its pre-call state. Audit item #80, Feb 2026.                        */
enum class JsonExtractResult {
    Ok              = 0,  /* a `{...}` region was found and parsed as an object */
    NoBraceFound    = 1,  /* input contained no `{` at all                      */
    Unbalanced      = 2,  /* every candidate ran off the end without closing    */
    ParseFailed     = 3,  /* one+ candidate(s) had balanced braces but nlohmann
                             refused the body (e.g. `{not json at all}`)       */
    FailClosed      = 4,  /* embedded NUL outside a string OR runaway nesting
                             (>1024). Input is hostile; caller should not
                             retry.                                             */
    RootNotObject   = 5   /* parse succeeded but root was not a JSON object
                             (top-level array / number / string etc.)          */
};

inline const char* JsonExtractResultName(JsonExtractResult r) {
    switch (r) {
        case JsonExtractResult::Ok:              return "Ok";
        case JsonExtractResult::NoBraceFound:    return "NoBraceFound";
        case JsonExtractResult::Unbalanced:      return "Unbalanced";
        case JsonExtractResult::ParseFailed:     return "ParseFailed";
        case JsonExtractResult::FailClosed:      return "FailClosed";
        case JsonExtractResult::RootNotObject:   return "RootNotObject";
    }
    return "?";
}

/* Rich variant of ExtractJsonObject. Returns the classified result and
 * populates `out` only on Ok.                                            */
inline JsonExtractResult ExtractJsonObjectEx(const std::string& s, nlohmann::json& out) {
    if (s.empty()) return JsonExtractResult::NoBraceFound;
    constexpr int kMaxDepth = 1024;
    size_t i = 0;
    bool saw_any_brace     = false;
    bool saw_balanced      = false;
    bool saw_parse_failure = false;
    while (i < s.size()) {
        size_t start = s.find('{', i);
        if (start == std::string::npos) break;
        saw_any_brace = true;
        int depth = 0;
        bool inStr = false;
        size_t end = std::string::npos;
        bool aborted = false;
        for (size_t j = start; j < s.size(); j++) {
            unsigned char c = static_cast<unsigned char>(s[j]);
            if (inStr) {
                if (c == '\\') {
                    if (j + 1 >= s.size()) { aborted = true; break; }
                    char esc_char = s[j + 1];
                    if (esc_char == 'u') {
                        if (j + 5 >= s.size()) { aborted = true; break; }
                        j += 5;
                    } else {
                        j += 1;
                    }
                    continue;
                }
                if (c == '"') { inStr = false; continue; }
            } else {
                if (c == 0) { aborted = true; break; }
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
        if (aborted) return JsonExtractResult::FailClosed;
        if (end == std::string::npos) { i = start + 1; continue; }
        saw_balanced = true;
        try {
            out = nlohmann::json::parse(s.substr(start, end - start + 1));
            if (out.is_object()) return JsonExtractResult::Ok;
            /* Balanced + parsed but a non-object root -- keep searching;
             * the caller asked for an object. If nothing later parses as
             * an object, we report RootNotObject (more specific than
             * ParseFailed).                                               */
            saw_parse_failure = true;
        } catch (...) {
            saw_parse_failure = true;
        }
        i = start + 1;
    }
    if (!saw_any_brace)     return JsonExtractResult::NoBraceFound;
    if (saw_parse_failure)  return JsonExtractResult::ParseFailed;
    if (!saw_balanced)      return JsonExtractResult::Unbalanced;
    return JsonExtractResult::RootNotObject;
}

/* Scan `s` for a brace-balanced `{...}` region and parse it as JSON.
 * Returns true on success with `out` populated. Kept as the simple
 * bool entry point; callers wanting richer diagnostics should call
 * `ExtractJsonObjectEx` directly.                                      */
inline bool ExtractJsonObject(const std::string& s, nlohmann::json& out) {
    return ExtractJsonObjectEx(s, out) == JsonExtractResult::Ok;
}

/* ── Legacy implementation retained below only to keep unit-test
 * parity in case someone inlines it. All production call sites go
 * through ExtractJsonObject / ExtractJsonObjectEx above.              */
inline bool ExtractJsonObject_Legacy_UNUSED(const std::string& s, nlohmann::json& out) {
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
        if (end == std::string::npos) {
            /* Candidate ran off the end without closing. Per the
             * documented contract ("tries the next `{` until
             * something parses or the string is exhausted"), advance
             * past THIS `{` and keep looking -- a later brace run may
             * still balance (e.g. prose `{ dangling... {"real":1}`).
             * Previously we returned false immediately, so well-formed
             * JSON appearing after a stray brace was invisible.       */
            i = start + 1;
            continue;
        }
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
