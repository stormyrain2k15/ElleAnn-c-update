#!/usr/bin/env python3
"""
build_dispatch_table.py — Phase 6a foundation tool.

Cross-references three authority sources to produce the *single*
opcode → PROTO_NC_* dispatch table that Elle's headless Fiesta client
will use for every inbound packet:

  Sources (read-only):
    1. extracted/MERGED_protos.json       — PDB-derived struct catalogue
                                            (2 723 PROTO_NC_* entries with
                                            sizeof + member offsets).
    2. extracted/PDB_OPCODES.json         — opcode-name → uint16_t mapping
                                            from PROTOCOL_COMMAND enum
                                            (~2 687 entries).
    3. ../wire_captures/parsed_captures.json
                                          — live wire observations
                                            (`(direction, opcode, plen)`).

  Output (written):
    extracted/dispatch_table.json
        [ { "opcode_hex":   "0x0438",
            "opcode_dec":   1080,
            "dept":         4,
            "subid":        56,
            "opcode_name":  "NC_CHAR_CLIENT_BASE_CMD",
            "struct_name":  "PROTO_NC_CHAR_CLIENT_BASE_CMD" | null,
            "pdb_sizeof":   105 | null,
            "wire_lengths": [97, 97, 97, 97],
            "wire_count":   4,
            "directions":   ["Inbound"],
            "category":     "char" | "act" | "briefinfo" | "misc" | …,
            "drift":        "wire 97 vs pdb 105 (delta 8)"     | null,
            "decoder":      "fixed" | "head+tail" | "opaque"   |
                            "unknown_struct" | "unknown_opcode" } ]

  The dispatch_table.json is consumed by:
    * gen_proto_table.py            — emits FiestaProtoTable.h X-macro list
    * test_fiesta_proto_coverage.cpp — runtime replay assertions
    * Cognitive's "what does Elle know about packet X" introspection

Strict NO-STUB: this script does not invent opcodes, structs, or
sizes. Every record traces back to a verifiable line in one of the
three input files. A `null` field means "no source agrees on this"
which is itself a meaningful, actionable signal.
"""
from __future__ import annotations
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
EXTRACTED = HERE / 'extracted'
WIRE = HERE.parent / 'wire_captures'

PROTOS_JSON = EXTRACTED / 'MERGED_protos.json'
OPCODES_JSON = EXTRACTED / 'PDB_OPCODES.json'
WIRE_JSON = WIRE / 'parsed_captures.json'
OUT_JSON = EXTRACTED / 'dispatch_table.json'

# ───────────────────────────────────────────────────────────────────────────────
# Category inference — purely lexical (NC_CHAR_* → "char", etc.). Used by the
# C++ side to bucket handlers by feature area.
# ───────────────────────────────────────────────────────────────────────────────
_CAT_RE = re.compile(r'^NC_([A-Z]+(?:_[A-Z]+)?)_')

def category_of(name: str) -> str:
    m = _CAT_RE.match(name)
    if not m:
        return 'unknown'
    head = m.group(1).lower()
    # Compress USER_CONNECTION → user_connection, GUILD → guild, etc.
    return head


# ───────────────────────────────────────────────────────────────────────────────
# Opcode-name → struct-name resolution.
#
# By Fiesta convention every opcode `NC_FOO_BAR_REQ` has a sibling
# struct `PROTO_NC_FOO_BAR_REQ`, BUT a handful of opcodes share a
# struct (e.g. SHOUT_CMD reuses CHAT_REQ's payload shape) and a few
# are payload-less. We do the lookup naïvely first, then fall back
# to the empty-payload case if no struct is present.
# ───────────────────────────────────────────────────────────────────────────────
def struct_for(opcode_name: str, structs_by_name: dict) -> tuple[str | None, int | None]:
    candidate = f'PROTO_{opcode_name}'
    if candidate in structs_by_name:
        s = structs_by_name[candidate]
        return candidate, s['sizeof']

    # Some _ACK opcodes alias the matching _REQ struct on the wire; we
    # only flag this when the _ACK has no own struct.
    if opcode_name.endswith('_ACK'):
        sibling = f'PROTO_{opcode_name[:-4]}_REQ'
        if sibling in structs_by_name:
            return sibling, structs_by_name[sibling]['sizeof']

    return None, None


# ───────────────────────────────────────────────────────────────────────────────
def main() -> int:
    if not PROTOS_JSON.exists():
        print(f'FATAL: {PROTOS_JSON} not found', file=sys.stderr)
        return 1
    if not OPCODES_JSON.exists():
        print(f'FATAL: {OPCODES_JSON} not found', file=sys.stderr)
        return 1
    if not WIRE_JSON.exists():
        print(f'FATAL: {WIRE_JSON} not found', file=sys.stderr)
        return 1

    structs = json.loads(PROTOS_JSON.read_text())
    structs_by_name = {s['name']: s for s in structs}

    raw_opcodes = json.loads(OPCODES_JSON.read_text())
    # PDB_OPCODES.json shape varies by gen-step; accept either:
    #   { "NC_FOO_REQ": 0x0306, ... }
    # or a list of { "name": "NC_FOO_REQ", "opcode": 774 } entries.
    opcodes_by_id: dict[int, str] = {}
    if isinstance(raw_opcodes, dict):
        for n, v in raw_opcodes.items():
            try:
                opcodes_by_id[int(v)] = n
            except (TypeError, ValueError):
                continue
    elif isinstance(raw_opcodes, list):
        for entry in raw_opcodes:
            n = entry.get('name')
            v = entry.get('opcode') or entry.get('value') or entry.get('id')
            if n is None or v is None:
                continue
            try:
                opcodes_by_id[int(v)] = n
            except (TypeError, ValueError):
                continue

    wire_events = json.loads(WIRE_JSON.read_text())

    # Aggregate wire observations: opcode_dec → list of (direction, plen)
    obs: dict[int, list[tuple[str, int]]] = defaultdict(list)
    for ev in wire_events:
        try:
            # capture's `flag` and `opcode` are the two NETCOMMAND bytes as
            # decimals.  Reassemble into the host-side u16 the PDB uses:
            #   wire bytes [dept][cmdsubid] → u16 opcode = (dept<<8)|cmdsubid
            flag = int(ev['flag'])
            sub = int(ev['opcode'])
            wire_op = (flag << 8) | sub
        except (KeyError, ValueError):
            continue
        obs[wire_op].append((ev.get('direction', ''), int(ev.get('payload_bytes', 0))))

    # Build the unified table — union of every opcode known to either source.
    all_ids: set[int] = set(opcodes_by_id.keys()) | set(obs.keys())
    rows: list[dict] = []
    for op_id in sorted(all_ids):
        name = opcodes_by_id.get(op_id, '')
        struct_name, pdb_sizeof = (None, None)
        if name:
            struct_name, pdb_sizeof = struct_for(name, structs_by_name)

        wire_pairs = obs.get(op_id, [])
        wire_lengths = sorted({plen for _, plen in wire_pairs})
        wire_directions = sorted({d for d, _ in wire_pairs if d})

        # Drift detection: if PDB says one size but every wire occurrence
        # consistently shows a different fixed size, surface it.
        drift = None
        if pdb_sizeof is not None and wire_lengths:
            # Allow a head-only struct: wire len == sizeof of struct head if
            # there is a trailing variable array. We can't compute that here
            # without member-walking; for now flag any mismatch.
            uniq = set(wire_lengths)
            if uniq != {pdb_sizeof}:
                obs_sizes = ','.join(str(x) for x in wire_lengths)
                delta = (next(iter(uniq)) - pdb_sizeof) if len(uniq) == 1 else None
                drift = f'wire {obs_sizes} vs pdb {pdb_sizeof}'
                if delta is not None:
                    drift += f' (delta {delta:+d})'

        # Decoder classification:
        if struct_name is None and not name:
            decoder = 'unknown_opcode'
        elif struct_name is None:
            decoder = 'unknown_struct'
        elif pdb_sizeof == 0:
            decoder = 'opaque'
        elif drift is None:
            decoder = 'fixed'
        else:
            # Mismatched sizes → almost always a head + variable-len tail
            decoder = 'head+tail'

        rows.append({
            'opcode_hex':   f'0x{op_id:04X}',
            'opcode_dec':   op_id,
            'dept':         (op_id >> 8) & 0xFF,
            'subid':         op_id & 0xFF,
            'opcode_name':   name or None,
            'struct_name':   struct_name,
            'pdb_sizeof':    pdb_sizeof,
            'wire_lengths':  wire_lengths,
            'wire_count':    len(wire_pairs),
            'directions':    wire_directions,
            'category':      category_of(name) if name else 'unknown',
            'drift':         drift,
            'decoder':       decoder,
        })

    OUT_JSON.write_text(json.dumps(rows, indent=2))

    # Coverage summary
    n_total = len(rows)
    n_named = sum(1 for r in rows if r['opcode_name'])
    n_struct = sum(1 for r in rows if r['struct_name'])
    n_observed = sum(1 for r in rows if r['wire_count'])
    n_drift = sum(1 for r in rows if r['drift'])

    print(f'Wrote {OUT_JSON.relative_to(HERE.parent.parent)}')
    print(f'  total rows       : {n_total}')
    print(f'  with opcode name : {n_named}')
    print(f'  with PDB struct  : {n_struct}')
    print(f'  with wire obs    : {n_observed}')
    print(f'  size drift flags : {n_drift}')

    # Surface every observed-opcode-without-known-struct: these are the
    # opcodes Elle WILL receive in production but cannot decode yet.
    holes = [r for r in rows if r['wire_count'] and not r['struct_name']]
    if holes:
        print('\nObserved opcodes lacking a PDB struct (Phase-6a backlog):')
        for r in holes:
            print(f'  {r["opcode_hex"]} '
                  f'({r["opcode_name"] or "??"}) '
                  f'wire_lengths={r["wire_lengths"]} count={r["wire_count"]}')

    drifts = [r for r in rows if r['drift']]
    if drifts:
        print('\nSize-drift opcodes (PDB ≠ wire — head+tail or new shape):')
        for r in drifts:
            print(f'  {r["opcode_hex"]} '
                  f'({r["opcode_name"] or "??"}) {r["drift"]}')

    return 0


if __name__ == '__main__':
    raise SystemExit(main())
