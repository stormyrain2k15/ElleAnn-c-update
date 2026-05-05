# Generated/preview/ — Phase 6a Foundation (WIP)

**Status:** Preview only — DO NOT add to the .vcxproj yet.

These two files are the first-pass output of `_re_artifacts/pdb/gen_packet_headers.py`.
They demonstrate the codegen pipeline works end-to-end but they DO NOT YET compile
clean — there are two known issues that get resolved in Phase 6a proper:

1. **Missing dependency types** — `NETCOMMAND`, `SHINE_XY_TYPE`, etc. are
   referenced by wire structs but the codegen's transitive closure walker
   doesn't pick them up because they're typedefs, not LF_STRUCTUREs in the
   PDB. Phase 6a will hand-write a small `FiestaProtoBase.h` with these
   ~10 typedefs and `#include` it before the generated headers.

2. **Size drift on ~30 structs** — e.g. `PROTO_NC_ACT_ANIMATION_START_CMD`
   computes to 130 bytes but the PDB says 34. Root cause is fields whose
   types are unresolved (return 0 from `field_size()`), letting the next
   field's offset land past the declared sizeof. Once (1) is fixed, almost
   all of these resolve automatically.

## What IS production-ready right now

- **`extracted/MERGED_protos.json`** — 2,723 unique struct definitions with
  offsets and sizeof, merged from Fiesta + Account + AccountLog PDBs. This
  is the canonical source of truth for the wire format and is hand-readable.
- **`gen_packet_headers.py`** — codegen pipeline. Idempotent, deterministic,
  re-runnable any time the JSON is updated. About 200 LOC of well-commented
  Python.

## When Phase 6a starts for real

The 6a kickoff will be:
1. Hand-write `FiestaProtoBase.h` (the 10ish missing typedefs)
2. Patch the codegen to emit `#include "FiestaProtoBase.h"` and to treat
   typedef-only references as size-known via a small lookup table
3. Re-run `gen_packet_headers.py`; static_asserts should now pass for
   ≥95% of populated structs
4. Move headers from `preview/` to `Generated/`, add to .vcxproj
5. Manually patch the final 5% of size-drift structs as they're encountered
   in live-traffic decoding (Phase 6b)

## Don't touch

- `FiestaProtoStructs.h` and `FiestaProtoOpcodes.h` in this folder are
  *snapshots* showing the current state of the pipeline. They are NOT in
  the build. The next codegen run overwrites them.
