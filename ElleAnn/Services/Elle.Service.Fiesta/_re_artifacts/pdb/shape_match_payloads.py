#!/usr/bin/env python3
"""
shape_match_payloads.py — Phase 6a step-2 calibration tool.

The opcode IDs in our extracted PDBs do NOT match the opcode IDs
the user's running Fiesta server uses (different region toggle).
But the PROTO_NC_* STRUCT SHAPES are stable across region toggles —
the server developers don't usually touch those.

This tool ignores opcode IDs entirely and instead matches each
observed (wire_opcode, payload_len) tuple against the PDB struct
catalogue by sizeof, surfacing the *candidate struct names* whose
size matches the wire length.

Output: prints a table to stdout AND writes
        extracted/payload_shape_matches.json

For each observed wire opcode it lists:
  • exact-size matches (sizeof == payload_len)
  • near matches      (|sizeof - payload_len| <= 8 — head + small tail)
  • category-filtered subset (heuristic by opcode-byte plausibility)

The human's job is then to scan the candidate list per opcode and
pick the one that semantically fits (e.g. 97-byte client-identity
payload → PROTO_NC_CHAR_BASE_CMD, not PROTO_NC_AUCTION_REGISTER_REQ
even though both happen to be 97 bytes).
"""
from __future__ import annotations
import json
from pathlib import Path
from collections import defaultdict

HERE = Path(__file__).resolve().parent
EXTRACTED = HERE / 'extracted'
WIRE = HERE.parent / 'wire_captures'

PROTOS = json.loads((EXTRACTED / 'MERGED_protos.json').read_text())
WIRE_EVENTS = json.loads((WIRE / 'parsed_captures.json').read_text())

# size → list of struct names (so we can answer "which structs are
# exactly N bytes?" in O(1)).
size_index: dict[int, list[str]] = defaultdict(list)
for s in PROTOS:
    size_index[s['sizeof']].append(s['name'])


def candidates_for(plen: int, slack: int = 0) -> list[tuple[int, str]]:
    """Return [(sizeof, name)] for structs of size in [plen-slack, plen+slack]."""
    out: list[tuple[int, str]] = []
    for sz in range(plen - slack, plen + slack + 1):
        if sz <= 0:
            continue
        for name in size_index.get(sz, []):
            out.append((sz, name))
    out.sort()
    return out


# Aggregate observations
obs: dict[tuple[int, int], dict] = {}
for ev in WIRE_EVENTS:
    try:
        flag = int(ev['flag']); op = int(ev['opcode'])
    except (KeyError, ValueError):
        continue
    plen = int(ev.get('payload_bytes', 0))
    direction = ev.get('direction', '?')
    key = ((flag << 8) | op, plen)
    rec = obs.setdefault(key, {'count': 0, 'directions': set(), 'flag': flag, 'op': op})
    rec['count'] += 1
    rec['directions'].add(direction)


# Print a per-(opcode,size) candidate report
print('═' * 110)
print(f'  Phase 6a step-2 — payload-shape matches (struct catalogue: {len(PROTOS)} entries)')
print('═' * 110)
print()
print(f'{"WIRE OP":<10} {"PLEN":>5} {"DIR":<10} {"#":>3}  CANDIDATE PROTO_NC_* STRUCTS '
      f'(by sizeof)')
print('-' * 110)

results: list[dict] = []
for (op_id, plen), rec in sorted(obs.items()):
    exact = candidates_for(plen, slack=0)
    near = candidates_for(plen, slack=8)
    near_only = [c for c in near if c not in exact]

    dirs = ','.join(sorted(rec['directions']))
    print(f'0x{op_id:04X}     {plen:>5} {dirs:<10} {rec["count"]:>3}  ', end='')

    if not exact and not near_only:
        print('(no PDB struct of any close size — UNKNOWN)')
        results.append({
            'opcode_hex': f'0x{op_id:04X}', 'plen': plen,
            'directions': sorted(rec['directions']),
            'count': rec['count'], 'exact': [], 'near': [],
        })
        continue

    if exact:
        names = [n for _, n in exact]
        # Print first 3 inline; the JSON will hold the rest.
        head = names[:3]
        more = '' if len(names) <= 3 else f'  (+{len(names)-3} more)'
        print(f'EXACT[{len(names)}]: {", ".join(head)}{more}')
    else:
        # near-only fallback
        head = [f'{nm}(sz={sz})' for sz, nm in near_only[:3]]
        more = '' if len(near_only) <= 3 else f'  (+{len(near_only)-3} more)'
        print(f'NEAR ±8: {", ".join(head)}{more}')

    results.append({
        'opcode_hex':  f'0x{op_id:04X}',
        'plen':        plen,
        'directions':  sorted(rec['directions']),
        'count':       rec['count'],
        'exact':       [n for _, n in exact],
        'near':        [{'sizeof': sz, 'name': nm} for sz, nm in near_only],
    })

print()
out_path = EXTRACTED / 'payload_shape_matches.json'
out_path.write_text(json.dumps(results, indent=2))
print(f'Wrote {out_path.relative_to(HERE.parent.parent)} '
      f'({len(results)} opcode×size pairs).')

# Standout: for the 97-byte mystery (wire op 0x0438) — print ALL exact
# matches.  This is the packet whose payload starts with [u32 5][char[16]
# "EllaAnn"]... and is the entry point for the calibration loop.
print()
print('Detail for opcode 0x0438 / 97 bytes (the "EllaAnn" identity packet):')
ninetyseven = candidates_for(97, slack=0)
for sz, name in ninetyseven:
    print(f'  • {name}  (sizeof={sz})')
if not ninetyseven:
    print('  (none — try ±8 below)')
    for sz, name in candidates_for(97, slack=8):
        print(f'  • {name}  (sizeof={sz})')
