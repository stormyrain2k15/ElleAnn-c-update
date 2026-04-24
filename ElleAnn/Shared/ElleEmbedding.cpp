/*══════════════════════════════════════════════════════════════════════════════
 * ElleEmbedding.cpp — Implementation
 *
 *   See ElleEmbedding.h for design rationale. This file holds only the
 *   hot loops: char-trigram extraction + FNV-1a hashing + L2 normalize
 *   + cosine. No virtual calls, no allocations post-construction.
 *══════════════════════════════════════════════════════════════════════════════*/
#include "ElleEmbedding.h"

#include <cctype>
#include <cmath>
#include <cstring>

namespace {

/* FNV-1a 64-bit. Chosen over std::hash because std::hash is implementation-
 * defined (MSVC and libstdc++ disagree) — we need determinism across
 * Windows services AND the Lua host's embedded scripts in the future. */
constexpr uint64_t FNV_OFFSET = 0xcbf29ce484222325ULL;
constexpr uint64_t FNV_PRIME  = 0x100000001b3ULL;

inline uint64_t fnv1a(const char* data, size_t len) {
    uint64_t h = FNV_OFFSET;
    for (size_t i = 0; i < len; i++) {
        h ^= (uint8_t)data[i];
        h *= FNV_PRIME;
    }
    return h;
}

/* Lowercase + collapse non-alphanumeric to single spaces. Keeps the
 * trigram distribution focused on content morphemes and throws away
 * formatting noise. Works on ASCII; UTF-8 multi-byte chars pass through
 * unchanged (each byte hashes into trigrams, which is imperfect but
 * consistent and deterministic for the same input).                  */
inline std::string Normalize(const std::string& in) {
    std::string out;
    out.reserve(in.size() + 2);
    out.push_back(' ');              /* sentinel: marks word-boundary trigrams at start */
    bool lastSpace = true;
    for (char ch : in) {
        unsigned char uc = (unsigned char)ch;
        if (std::isalnum(uc)) {
            out.push_back((char)std::tolower(uc));
            lastSpace = false;
        } else if (!lastSpace) {
            out.push_back(' ');
            lastSpace = true;
        }
    }
    if (lastSpace && !out.empty() && out.back() == ' ') {
        /* trim trailing double-space */
    } else {
        out.push_back(' ');          /* sentinel: marks word-boundary trigrams at end */
    }
    return out;
}

}  /* anonymous namespace */

namespace ElleEmbeddings {

void Encode(const std::string& text, ElleEmbedding& out) {
    /* Zero the vector. std::fill is loop-optimized by MSVC into a memset. */
    out.fill(0.0f);
    if (text.empty()) return;

    std::string norm = Normalize(text);
    if (norm.size() < 3) return;     /* Not enough for a single trigram */

    /* Char-trigram feature hashing.
     *   - 2 hashes per n-gram: one picks the dimension, one picks the
     *     sign (±1). Signed feature hashing de-biases collisions.      */
    size_t ngrams = 0;
    for (size_t i = 0; i + 3 <= norm.size(); i++) {
        uint64_t h = fnv1a(norm.data() + i, 3);
        size_t   dim  = (size_t)(h % ELLE_EMBEDDING_DIM);
        /* High bit of the hash decides the sign. This uses the upper
         * half-word that `% DIM` discarded, so dim and sign are weakly
         * independent (good hashing-trick property).                    */
        float    sign = (h >> 63) ? -1.0f : 1.0f;
        out[dim] += sign;
        ngrams++;
    }
    if (ngrams == 0) return;

    /* L2 normalize so cosine == dot product at comparison time. */
    float sumSq = 0.0f;
    for (size_t i = 0; i < ELLE_EMBEDDING_DIM; i++) sumSq += out[i] * out[i];
    if (sumSq <= 1e-20f) { out.fill(0.0f); return; }
    float inv = 1.0f / std::sqrt(sumSq);
    for (size_t i = 0; i < ELLE_EMBEDDING_DIM; i++) out[i] *= inv;
}

float Cosine(const ElleEmbedding& a, const ElleEmbedding& b) {
    float dot = 0.0f;
    /* Hot loop: auto-vectorizable by MSVC's /O2 optimizer. No branches. */
    for (size_t i = 0; i < ELLE_EMBEDDING_DIM; i++) dot += a[i] * b[i];
    return dot;
}

}  /* namespace ElleEmbeddings */
