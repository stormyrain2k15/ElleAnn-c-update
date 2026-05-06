#!/usr/bin/env python3
"""
gen_proto_table.py — Phase 6a foundation tool.

Reads `extracted/dispatch_table.json` (produced by build_dispatch_table.py)
and emits the C++ X-macro list `Elle.Service.Fiesta/Generated/FiestaProtoTable.inc`
plus the wire-observation header `Elle.Service.Fiesta/Generated/FiestaWireFixtures.inc`.

The X-macro layout is:
    FIESTA_PROTO_TABLE(X) \\
        X(0x0306, NC_USER_LOGIN_REQ,    PROTO_NC_USER_LOGIN_REQ,    272, "user")    \\
        X(0x030A, NC_USER_LOGIN_ACK,    PROTO_NC_USER_LOGIN_ACK,      1, "user")    \\
        ...

Consumers:
    * FiestaProtoTable.h        — translates the macro into either a
                                  switch/case dispatch, an opcode-name
                                  pretty-printer, or a sizeof self-check.
    * test_fiesta_proto_coverage.cpp
                                — emits a coverage report showing every
                                  observed wire opcode that the table
                                  knows how to decode.

The wire fixtures file emits each captured (port, dir, opcode, plen,
hex) as a `static constexpr WireFixture` so the test binary can replay
the sessions without parsing JSON at runtime.
"""
from __future__ import annotations
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent              # Elle.Service.Fiesta/
DT_JSON = HERE / 'extracted' / 'dispatch_table.json'
WIRE_JSON = HERE.parent / 'wire_captures' / 'parsed_captures.json'

OUT_TABLE = ROOT / 'Generated' / 'FiestaProtoTable.inc'
OUT_FIXTURES = ROOT / 'Generated' / 'FiestaWireFixtures.inc'


HEADER = '''/* ╔════════════════════════════════════════════════════════════════════════╗
 * ║  FiestaProtoTable.inc                                                    ║
 * ║  AUTO-GENERATED — DO NOT EDIT.  Regen:                                   ║
 * ║      python3 _re_artifacts/pdb/gen_proto_table.py                        ║
 * ║                                                                          ║
 * ║  X-macro list of every recovered Fiesta opcode crossed against PDB     ║
 * ║  struct catalogue (MERGED_protos.json) and live wire observations      ║
 * ║  (parsed_captures.json).                                                ║
 * ║                                                                          ║
 * ║  Use:                                                                    ║
 * ║      #define X(opcode_hex, opcode_name, struct_name,                    ║
 * ║                 pdb_sizeof, category) ...                                ║
 * ║      FIESTA_PROTO_TABLE(X)                                              ║
 * ║      #undef X                                                            ║
 * ║                                                                          ║
 * ║  Rows where the PDB does not know a struct render `void` for the        ║
 * ║  struct slot and -1 for sizeof — the consuming switch must default     ║
 * ║  these to a raw-bytes handler.                                         ║
 * ╚════════════════════════════════════════════════════════════════════════╝ */
#pragma once
'''


FIXTURES_HEADER = '''/* ╔════════════════════════════════════════════════════════════════════════╗
 * ║  FiestaWireFixtures.inc                                                  ║
 * ║  AUTO-GENERATED.  Live wire packet capture fixtures.                    ║
 * ║                                                                          ║
 * ║  Replays every (port, direction, opcode, payload) tuple captured       ║
 * ║  during the user's 2026-02-05 login walk.  Format:                      ║
 * ║      FIESTA_WIRE_FIXTURE(port, direction_tag, opcode_hex,               ║
 * ║                          payload_len, hex_payload_literal)              ║
 * ║                                                                          ║
 * ║  Consumers (test_fiesta_proto_coverage.cpp) iterate this list to:       ║
 * ║    * verify every observed opcode appears in FIESTA_PROTO_TABLE         ║
 * ║    * confirm sizeof(struct) matches payload_len for fixed-len ops      ║
 * ║    * surface drift / unknown-opcode lists for backlog grooming         ║
 * ╚════════════════════════════════════════════════════════════════════════╝ */
#pragma once
'''


def main() -> int:
    if not DT_JSON.exists():
        print(f'FATAL: {DT_JSON} not found — run build_dispatch_table.py first',
              file=sys.stderr)
        return 1

    rows = json.loads(DT_JSON.read_text())
    rows = [r for r in rows if r.get('opcode_name')]
    rows.sort(key=lambda r: r['opcode_dec'])

    OUT_TABLE.parent.mkdir(parents=True, exist_ok=True)
    with OUT_TABLE.open('w') as f:
        f.write(HEADER)
        f.write(f'/* {len(rows)} named opcodes total. */\n\n')
        f.write('#define FIESTA_PROTO_TABLE(X) \\\n')
        for i, r in enumerate(rows):
            struct = r['struct_name'] or 'void'
            sz = r['pdb_sizeof'] if r['pdb_sizeof'] is not None else -1
            cat = r['category']
            cont = '\\' if i < len(rows) - 1 else ' '
            f.write(f'    X({r["opcode_hex"]:<8}, {r["opcode_name"]:<60}, '
                    f'{struct:<60}, {sz:>5}, "{cat}") {cont}\n')
        f.write('\n')
        # Also emit a "hot" subset = wire-observed opcodes only.  These
        # are the opcodes Elle has actually seen; the headless dispatcher
        # focuses here first.
        hot = [r for r in rows if r.get('wire_count')]
        f.write(f'/* {len(hot)} wire-observed opcodes (hot subset). */\n\n')
        f.write('#define FIESTA_PROTO_HOT_TABLE(X) \\\n')
        for i, r in enumerate(hot):
            struct = r['struct_name'] or 'void'
            sz = r['pdb_sizeof'] if r['pdb_sizeof'] is not None else -1
            cat = r['category']
            cont = '\\' if i < len(hot) - 1 else ' '
            f.write(f'    X({r["opcode_hex"]:<8}, {r["opcode_name"]:<60}, '
                    f'{struct:<60}, {sz:>5}, "{cat}") {cont}\n')

    print(f'Wrote {OUT_TABLE.relative_to(ROOT.parent.parent)}  '
          f'({len(rows)} rows, {len(hot)} hot)')

    # ── Wire fixture inc ─────────────────────────────────────────────
    if not WIRE_JSON.exists():
        print(f'WARN: {WIRE_JSON} not found, skipping fixtures.', file=sys.stderr)
        return 0

    events = json.loads(WIRE_JSON.read_text())
    OUT_FIXTURES.parent.mkdir(parents=True, exist_ok=True)
    with OUT_FIXTURES.open('w') as f:
        f.write(FIXTURES_HEADER)
        f.write(f'/* {len(events)} wire events from 4 ports. */\n\n')
        f.write('#define FIESTA_WIRE_FIXTURES(X) \\\n')
        for i, ev in enumerate(events):
            try:
                flag = int(ev['flag']); sub = int(ev['opcode'])
                op_id = (flag << 8) | sub
            except (KeyError, ValueError):
                continue
            direction = ev.get('direction', '?')
            plen = int(ev.get('payload_bytes', 0))
            # truncate payload hex literal at 40 bytes — full hex bloats
            # the test binary; the *length* and *opcode* are what we need
            # to assert struct alignment.
            short_hex = ev.get('payload_hex', '')[:40 * 3].replace(' ', '')
            cont = '\\' if i < len(events) - 1 else ' '
            f.write(f'    X({ev.get("port", 0)}, {direction:<8}, '
                    f'0x{op_id:04X}, {plen:>5}, '
                    f'"{short_hex}") {cont}\n')

    print(f'Wrote {OUT_FIXTURES.relative_to(ROOT.parent.parent)}  '
          f'({len(events)} fixtures)')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
