/*══════════════════════════════════════════════════════════════════════════════
 * ElleEmbedding.h — Hashed-feature text embeddings for novelty / similarity
 *
 *   Principle
 *     Real embeddings without an LLM. We map text to a fixed 256-dim L2-
 *     normalized float vector using the "hashing trick" over character
 *     trigrams. Two texts with overlapping trigram distributions cluster
 *     under cosine similarity; unrelated texts spread apart.
 *
 *   Why not substring search (the old novelty signal)?
 *     Substring matching is lexical and brittle: "cat" doesn't match
 *     "cats", let alone "feline". Character n-grams capture sub-word
 *     morphology. Trigrams specifically give a good balance between
 *     precision and recall for short natural-language inputs.
 *
 *   Why feature hashing vs. keeping a vocabulary map?
 *     Fixed memory (256 floats == 1024 bytes per embedding, regardless
 *     of corpus size), no out-of-vocabulary handling, thread-safe because
 *     there is no shared state. Collisions are rare at 256 dims for the
 *     tens-to-hundreds of trigrams in a short utterance, and collisions
 *     bias embeddings toward similarity, which for novelty is the safe
 *     direction (false negatives > false positives for wonder).
 *
 *   Complexity
 *     Encode:  O(len(text))      — one FNV-1a hash per trigram
 *     Compare: O(dim) == O(256)  — dot product + no branches
 *
 *   Determinism
 *     FNV-1a is deterministic and architecture-independent. Two processes
 *     on different hosts produce identical embeddings for identical text
 *     — property the single-writer Identity fabric relies on.
 *
 *   Not a replacement for a real model
 *     This is a lightweight semantic-ish signal for novelty gating. For
 *     deep memory recall, MemoryEngine's cluster-proximity path is still
 *     the canonical "have I seen something like this?" check. The two
 *     are combined in ElleIdentityCore::EvaluateNovelty.
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once

#include <array>
#include <cstdint>
#include <string>

/* 256 dims chosen because: (a) one cache line worth of floats is 16, so
 * 256 == 16 lines cold-miss; (b) trigram collision rate stays below 5%
 * for typical 30–200 char utterances. If you change this, audit the
 * `novelty_memory` ring buffer size in ElleIdentityCore — 64 entries *
 * 256 floats * 4 bytes == 64 KB per identity core instance, well under
 * any cache/working-set budget. */
constexpr size_t ELLE_EMBEDDING_DIM = 256;

using ElleEmbedding = std::array<float, ELLE_EMBEDDING_DIM>;

namespace ElleEmbeddings {

    /* Encode `text` into a 256-dim L2-normalized float vector. Output
     * is written into `out`. Empty input produces a zero vector (norm
     * == 0, treat as "nothing seen yet"). Callers can check the zero
     * case by dotting with itself. */
    void Encode(const std::string& text, ElleEmbedding& out);

    /* Cosine similarity in [-1, 1]. Both inputs expected to be unit-norm
     * (Encode's contract). For unit-norm vectors cosine == dot product,
     * which we compute branchlessly in a single SIMD-friendly loop. If
     * either vector is zero (empty text), returns 0.                    */
    [[nodiscard]] float Cosine(const ElleEmbedding& a, const ElleEmbedding& b);

}  /* namespace ElleEmbeddings */
