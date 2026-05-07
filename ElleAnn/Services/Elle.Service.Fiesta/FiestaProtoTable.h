/*══════════════════════════════════════════════════════════════════════════════
 * FiestaProtoTable.h — Authoritative opcode-to-struct dispatch table.
 *
 *   This header consumes the auto-generated X-macro list
 *   `Generated/FiestaProtoTable.inc` (regen via
 *   `_re_artifacts/pdb/gen_proto_table.py`) and surfaces three views
 *   over it:
 *
 *     1. `Fiesta::OpcodeMetaTable` — constexpr array of OpcodeMeta
 *        records covering EVERY recovered opcode (~2300 entries).
 *
 *     2. `Fiesta::HotOpcodeMetaTable` — same shape, restricted to
 *        opcodes Elle has actually observed on the wire (~15
 *        entries from the 2026-02-05 capture session).  This is
 *        the SHORT list the headless dispatcher hot-paths.
 *
 *     3. `Fiesta::OpcodeName(opcode)` — pretty-printer that returns
 *        the PROTO_NC_* string for an opcode, or "(unknown)" when
 *        the opcode has no PDB entry.  O(log N) binary search on
 *        the sorted-by-id table.
 *
 *   The X-macro pattern lets us add lookup helpers (size assertions,
 *   per-category enumeration, etc.) without a code-generation step —
 *   each helper is a single re-expansion of FIESTA_PROTO_TABLE.
 *══════════════════════════════════════════════════════════════════════════════*/
#pragma once
#ifndef ELLE_FIESTA_PROTO_TABLE_H
#define ELLE_FIESTA_PROTO_TABLE_H

#include "FiestaProtoBase.h"

#include <cstdint>
#include <cstddef>
#include <string_view>

namespace Fiesta {

/*──────────────────────────────────────────────────────────────────────────────
 * Pull in the generated X-macro list.  This file MUST be present —
 * regen it via `python3 _re_artifacts/pdb/gen_proto_table.py` after
 * any update to MERGED_protos.json or PDB_OPCODES.json.
 *──────────────────────────────────────────────────────────────────────────────*/
#include "Generated/FiestaProtoTable.inc"

/* Fallback macro guards — refuse to compile if the regen step was
 * skipped, rather than silently producing an empty table. */
#ifndef FIESTA_PROTO_TABLE
#  error "FiestaProtoTable.inc did not define FIESTA_PROTO_TABLE — \
regen via python3 _re_artifacts/pdb/gen_proto_table.py"
#endif
#ifndef FIESTA_PROTO_HOT_TABLE
#  error "FiestaProtoTable.inc did not define FIESTA_PROTO_HOT_TABLE"
#endif


/*──────────────────────────────────────────────────────────────────────────────
 * Full opcode metadata array.
 *──────────────────────────────────────────────────────────────────────────────*/
namespace detail {

#define FIESTA_PT_EMIT(opcode_hex, opcode_name, struct_name, pdb_sizeof, category) \
    OpcodeMeta{ static_cast<uint16_t>(opcode_hex),                                  \
                #opcode_name, #struct_name,                                         \
                static_cast<int32_t>(pdb_sizeof),                                   \
                category },

inline constexpr OpcodeMeta kOpcodeMetaTable[] = {
    FIESTA_PROTO_TABLE(FIESTA_PT_EMIT)
};

inline constexpr OpcodeMeta kHotOpcodeMetaTable[] = {
    FIESTA_PROTO_HOT_TABLE(FIESTA_PT_EMIT)
};

#undef FIESTA_PT_EMIT

inline constexpr std::size_t kOpcodeMetaCount =
    sizeof(kOpcodeMetaTable) / sizeof(kOpcodeMetaTable[0]);

inline constexpr std::size_t kHotOpcodeMetaCount =
    sizeof(kHotOpcodeMetaTable) / sizeof(kHotOpcodeMetaTable[0]);

}  /* namespace detail */


/*──────────────────────────────────────────────────────────────────────────────
 * Public accessors.
 *──────────────────────────────────────────────────────────────────────────────*/

inline constexpr const OpcodeMeta* OpcodeMetaTable() {
    return detail::kOpcodeMetaTable;
}

inline constexpr std::size_t OpcodeMetaCount() {
    return detail::kOpcodeMetaCount;
}

inline constexpr const OpcodeMeta* HotOpcodeMetaTable() {
    return detail::kHotOpcodeMetaTable;
}

inline constexpr std::size_t HotOpcodeMetaCount() {
    return detail::kHotOpcodeMetaCount;
}


/*──────────────────────────────────────────────────────────────────────────────
 * OpcodeName — O(log N) binary search.
 *
 *   The generator emits the table in ascending opcode order so we can
 *   bisect.  Returns "(unknown)" for opcodes the PDB has never seen.
 *──────────────────────────────────────────────────────────────────────────────*/
inline std::string_view OpcodeName(uint16_t opcode) {
    const OpcodeMeta* lo = detail::kOpcodeMetaTable;
    const OpcodeMeta* hi = lo + detail::kOpcodeMetaCount;
    while (lo < hi) {
        const OpcodeMeta* mid = lo + (hi - lo) / 2;
        if (mid->opcode < opcode)       lo = mid + 1;
        else if (mid->opcode > opcode)  hi = mid;
        else                            return mid->opcode_name;
    }
    return "(unknown)";
}

inline const OpcodeMeta* OpcodeMetaFor(uint16_t opcode) {
    const OpcodeMeta* lo = detail::kOpcodeMetaTable;
    const OpcodeMeta* hi = lo + detail::kOpcodeMetaCount;
    while (lo < hi) {
        const OpcodeMeta* mid = lo + (hi - lo) / 2;
        if (mid->opcode < opcode)       lo = mid + 1;
        else if (mid->opcode > opcode)  hi = mid;
        else                            return mid;
    }
    return nullptr;
}


/*──────────────────────────────────────────────────────────────────────────────
 * ClassifyDecoder — given an opcode and a payload length, return the
 *   handler kind:
 *     Fixed       sizeof matches and is positive
 *     HeadAndTail sizeof positive but != payload_len → variable tail
 *     Opaque      sizeof <= 0 but PDB has the struct (no fields)
 *     Unknown     opcode not in PDB
 *──────────────────────────────────────────────────────────────────────────────*/
inline Decoder ClassifyDecoder(uint16_t opcode, std::size_t payload_len) {
    const OpcodeMeta* m = OpcodeMetaFor(opcode);
    if (!m)                              return Decoder::Unknown;
    if (m->pdb_sizeof < 0)               return Decoder::Unknown;
    if (m->pdb_sizeof == 0)              return Decoder::Opaque;
    if (static_cast<std::size_t>(m->pdb_sizeof) == payload_len) return Decoder::Fixed;
    return Decoder::HeadAndTail;
}


/*──────────────────────────────────────────────────────────────────────────────
 * Compile-time sanity checks.
 *
 *   • Table is non-empty.
 *   • Sorted by opcode ascending (required for binary search).
 *──────────────────────────────────────────────────────────────────────────────*/
static_assert(detail::kOpcodeMetaCount > 0,
              "FiestaProtoTable: empty opcode table — generator broken");

namespace detail {
inline constexpr bool TableIsSorted() {
    for (std::size_t i = 1; i < kOpcodeMetaCount; ++i) {
        if (kOpcodeMetaTable[i].opcode < kOpcodeMetaTable[i-1].opcode) return false;
    }
    return true;
}
}  /* namespace detail */
static_assert(detail::TableIsSorted(),
              "FiestaProtoTable: rows must be sorted by opcode ascending");


}  /* namespace Fiesta */
#endif  /* ELLE_FIESTA_PROTO_TABLE_H */
