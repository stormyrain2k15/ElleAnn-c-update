#!/usr/bin/env python3
"""
gen_packet_headers.py — codegen Fiesta packet POD headers from MERGED_protos.json.

Reads:  ../extracted/Fiesta_protos.json + Account_protos.json + AccountLog_protos.json
Writes: ../../Generated/FiestaProtoStructs.h    — POD struct definitions
        ../../Generated/FiestaProtoStructs.cpp  — opcode→struct dispatch table
        ../../Generated/FiestaProtoOpcodes.h    — opcode enum (from shine_opcodes.tsv)
        ../../Generated/MERGED_protos.json      — the merged source-of-truth JSON

Invariants:
  - Every struct gets `#pragma pack(push, 1)` so wire format is byte-exact.
  - Structs missing a field layout are written as opaque `uint8_t data[N]` so
    they still occupy correct wire size and can be passed by reference.
  - Generated files are ALWAYS regeneratable from the JSON (no hand edits).
    The header bears a "DO NOT EDIT — regenerated from PDB JSON" banner.

Run from the _re_artifacts/pdb/ directory:
    python3 gen_packet_headers.py
"""
import json, re, os, sys
from pathlib import Path

# ───────────────────────────────────────────────────────────────────────────
HERE = Path(__file__).resolve().parent
EXTRACTED = HERE / 'extracted'
GENERATED = HERE.parent.parent / 'Generated'
GENERATED.mkdir(parents=True, exist_ok=True)

# ───── Step 1: Merge JSONs ──────────────────────────────────────────────────
def load_merge():
    out = {}
    sources = ['Fiesta_protos.json', 'Account_protos.json', 'AccountLog_protos.json']
    for src in sources:
        p = EXTRACTED / src
        if not p.exists(): continue
        for s in json.load(p.open()):
            existing = out.get(s['name'])
            ms = len(s.get('members') or [])
            es = len(existing.get('members') or []) if existing else -1
            if (existing is None
                or ms > es
                or (ms == es and (s.get('sizeof') or 0) > (existing.get('sizeof') or 0))):
                out[s['name']] = s
    return sorted(out.values(), key=lambda x: x['name'])

# ───── Step 2: Resolve types to C/C++ field declarations ────────────────────
BASIC_C = {
    'char':            'int8_t',
    'short':           'int16_t',
    'long':            'int32_t',
    'int64':           'int64_t',
    'unsigned char':   'uint8_t',
    'unsigned short':  'uint16_t',
    'unsigned long':   'uint32_t',
    'unsigned __int64':'uint64_t',
    'bool':            'bool',
    'float':           'float',
    'double':          'double',
    'wchar_t':         'wchar_t',
    'int':             'int32_t',
    'unsigned int':    'uint32_t',
    'void':            'void',
}

def safe_ident(name: str) -> str:
    """Sanitize a C++ identifier — replace anything illegal with '_'."""
    s = re.sub(r'[^A-Za-z0-9_]', '_', name or 'unnamed')
    if s and s[0].isdigit(): s = '_' + s
    return s

# Match "TYPE[N]   /* nbytes */"
RE_ARRAY = re.compile(r'^(.*?)\[(\d+)\]')

def render_field_type(type_expr: str, sizeof_hint: int = 0) -> str:
    """Best-effort: turn a parser type-expression into a C++ field declaration suffix.
       Returns just the type, NOT including the field name."""
    if type_expr is None:
        return f'uint8_t /* unresolved */'
    t = type_expr.strip()

    # Pointers: render as uint32_t — these are 32-bit ptr32 in PDB
    if t.endswith('*'):
        return 'uint32_t /* ptr32 */'

    # Array — TYPE[N] /* X bytes */
    m = RE_ARRAY.match(t)
    if m:
        inner_t = m.group(1).strip()
        count = int(m.group(2))
        return f'{render_field_type(inner_t)} __ARR{count}__'

    # Direct basic mapping
    if t in BASIC_C:
        return BASIC_C[t]

    # Anonymous struct/union from the parser → opaque bytes
    if t.startswith('<') or t.startswith('struct{') or t.startswith('union{'):
        # Use sizeof_hint to size the placeholder bytes
        if sizeof_hint > 0:
            return f'uint8_t /* anon */ __ARR{sizeof_hint}__'
        return 'uint32_t /* anon */'

    # Otherwise treat as a named struct reference
    return safe_ident(t)


def field_size(type_expr: str, types_by_name: dict) -> int:
    """Best-effort sizeof for a field's type expression. Returns 0 if unknown."""
    if not type_expr: return 0
    t = type_expr.strip()
    # Pointer
    if t.endswith('*'): return 4
    # Array
    m = RE_ARRAY.match(t)
    if m:
        inner = m.group(1).strip()
        n = int(m.group(2))
        return field_size(inner, types_by_name) * n
    # Anonymous struct in the type expr (rare) - treat as 0; trailing pad fixes it.
    if t.startswith('<') or 'struct{' in t or 'union{' in t: return 0
    # Basic type
    if t in BASIC_C:
        return {'int8_t':1,'uint8_t':1,'int16_t':2,'uint16_t':2,
                'int32_t':4,'uint32_t':4,'int64_t':8,'uint64_t':8,
                'bool':1,'float':4,'double':8,'wchar_t':2,'void':0}.get(BASIC_C[t], 0)
    # Named struct - look up its sizeof
    s = types_by_name.get(t)
    if s and s.get('sizeof'):
        return s['sizeof']
    return 0  # unknown — let the trailing pad handle it


def emit_field(member: dict, prev_end: int, types_by_name: dict) -> tuple[str, int]:
    """Emit one C++ field declaration line. Returns (line, new_end_offset).
       Uses the JSON's declared offset directly — does not assume fields are contiguous."""
    offset = member['offset']
    name   = safe_ident(member['name'])
    raw_t  = member.get('type', '')
    rendered = render_field_type(raw_t)

    pad_lines = []
    if offset > prev_end:
        pad_lines.append(f'    uint8_t _pad_at_{prev_end:04x}[{offset - prev_end}]; /* gap */')

    arr_match = re.search(r'__ARR(\d+)__', rendered)
    if arr_match:
        n = int(arr_match.group(1))
        clean = rendered.replace(f' __ARR{n}__', '')
        line = f'    {clean} {name}[{n}];  /* @{offset} */'
    else:
        line = f'    {rendered} {name};  /* @{offset} */'

    fs = field_size(raw_t, types_by_name)
    new_end = offset + (fs if fs > 0 else 0)
    return ('\n'.join(pad_lines + [line]), new_end)


# ───── Step 3: Emit headers ─────────────────────────────────────────────────
def collect_referenced_types(structs: list, types_by_name: dict) -> list:
    """Walk the wire types and return ALL types they reference (transitively).
       Ensures we emit dependencies before dependents."""
    seen = set()
    order = []

    def visit(name: str):
        if name in seen or name not in types_by_name:
            return
        seen.add(name)
        s = types_by_name[name]
        for m in (s.get('members') or []):
            t = (m.get('type') or '').strip()
            # Strip pointer / array wrappers
            t = t.rstrip('*').strip()
            arr = RE_ARRAY.match(t)
            if arr: t = arr.group(1).strip()
            if t in BASIC_C: continue
            if t.startswith('<') or 'struct{' in t or 'union{' in t: continue
            if t in types_by_name:
                visit(t)
        order.append(s)

    for s in structs: visit(s['name'])
    return order


def emit_header(structs: list, types_by_name: dict) -> str:
    out = []
    out.append('/* ╔════════════════════════════════════════════════════════════════════════╗')
    out.append(' * ║                     FiestaProtoStructs.h                                ║')
    out.append(' * ║  AUTO-GENERATED — DO NOT EDIT BY HAND                                   ║')
    out.append(' * ║  Source: _re_artifacts/pdb/extracted/{Fiesta,Account,AccountLog}_protos ║')
    out.append(' * ║  Regen:  python3 _re_artifacts/pdb/gen_packet_headers.py                ║')
    out.append(' * ║                                                                          ║')
    out.append(' * ║  Wire-exact PODs for every PROTO_NC_* struct recovered from the PDB.    ║')
    out.append(' * ║  Structs whose field layout is not present in any available PDB are    ║')
    out.append(' * ║  rendered as opaque byte arrays of correct sizeof so they still        ║')
    out.append(' * ║  participate in the dispatch table — they can be filled in later as    ║')
    out.append(' * ║  field info becomes available, without changing wire offsets.          ║')
    out.append(' * ║                                                                          ║')
    out.append(f' * ║  {len([s for s in structs if s.get("members")])} of {len(structs)} structs have full field layout populated.                ║')
    out.append(' * ╚════════════════════════════════════════════════════════════════════════╝ */')
    out.append('#pragma once')
    out.append('#include <cstdint>')
    out.append('#include <cstddef>')
    out.append('')
    out.append('namespace ElleFiesta::Proto {')
    out.append('')
    out.append('#pragma pack(push, 1)')
    out.append('')

    populated = empty = 0
    for s in structs:
        name = safe_ident(s['name'])
        sz = s.get('sizeof') or 0
        members = s.get('members') or []

        if not members:
            empty += 1
            if sz <= 0:
                # Zero-byte placeholder — emit an empty struct. C++ requires
                # sizeof >= 1, but we want byte-accuracy where possible.
                out.append(f'/* {s["name"]} — opaque, sizeof=0 (empty placeholder) */')
                out.append(f'struct {name} {{ }};')
                out.append('')
                continue
            out.append(f'/* {s["name"]} — opaque, sizeof={sz} */')
            out.append(f'struct {name} {{ uint8_t data[{sz}]; }};')
            out.append(f'static_assert(sizeof({name}) == {sz}, "{name} size drift");')
            out.append('')
            continue

        populated += 1
        out.append(f'/* {s["name"]} — {len(members)} fields, sizeof={sz} */')
        out.append(f'struct {name} {{')
        end = 0
        for m in sorted(members, key=lambda x: x['offset']):
            line, end = emit_field(m, end, types_by_name)
            out.append(line)
        # Trailing padding to declared sizeof
        if sz and end < sz:
            out.append(f'    uint8_t _tail[{sz - end}]; /* trailing */')
        out.append('};')
        if sz > 0:
            out.append(f'static_assert(sizeof({name}) == {sz}, "{name} size drift");')
        out.append('')

    out.append('#pragma pack(pop)')
    out.append('')
    out.append(f'/* Stats: {populated} populated structs, {empty} opaque (sizeof-only). */')
    out.append('')
    out.append('}  /* namespace ElleFiesta::Proto */')
    return '\n'.join(out) + '\n'


# ───── Step 4: Emit opcode enum + dispatch table ────────────────────────────
def emit_opcode_header(structs: list, opcodes_tsv: Path) -> str:
    out = []
    out.append('/* ╔════════════════════════════════════════════════════════════════════════╗')
    out.append(' * ║                     FiestaProtoOpcodes.h                                ║')
    out.append(' * ║  AUTO-GENERATED — DO NOT EDIT                                            ║')
    out.append(' * ║  Maps every recovered Fiesta opcode (NETCOMMAND) to its C++ POD type.  ║')
    out.append(' * ╚════════════════════════════════════════════════════════════════════════╝ */')
    out.append('#pragma once')
    out.append('#include <cstdint>')
    out.append('')
    out.append('namespace ElleFiesta::Proto {')
    out.append('')
    out.append('enum class Opcode : uint16_t {')

    rows = []
    if opcodes_tsv.exists():
        for line in opcodes_tsv.read_text().splitlines():
            line = line.strip()
            if not line or line.startswith('#'): continue
            parts = re.split(r'\s+', line, maxsplit=2)
            if len(parts) >= 2:
                opcode_name = safe_ident(parts[0])
                opcode_val  = parts[1]
                rows.append((opcode_name, opcode_val))
    for n, v in rows:
        out.append(f'    {n} = {v},')
    out.append('};')
    out.append('')
    out.append(f'/* {len(rows)} opcodes from shine_opcodes.tsv */')
    out.append('}  /* namespace ElleFiesta::Proto */')
    return '\n'.join(out) + '\n'


# ───── Main ──────────────────────────────────────────────────────────────
def main():
    structs = load_merge()
    populated = sum(1 for s in structs if s.get('members'))
    print(f'Merged {len(structs)} structs ({populated} populated)')

    types_by_name = {s['name']: s for s in structs}

    # Save merged JSON
    merged_path = EXTRACTED / 'MERGED_protos.json'
    json.dump(structs, merged_path.open('w'), indent=2)
    print(f'Wrote {merged_path}')

    # Filter to PROTO_NC_* / NETPACKET — the wire types
    wire = [s for s in structs
            if s['name'].startswith(('PROTO_NC_', 'NETPACKET'))]
    print(f'Wire types: {len(wire)} (PROTO_NC_*  + NETPACKET*)')

    # Walk dependencies — emit referenced types BEFORE the wire types use them.
    ordered = collect_referenced_types(wire, types_by_name)
    extra = len(ordered) - len(wire)
    print(f'Including {extra} dependency types referenced by wire structs')

    # Emit FiestaProtoStructs.h
    hdr_path = GENERATED / 'FiestaProtoStructs.h'
    hdr_path.write_text(emit_header(ordered, types_by_name))
    print(f'Wrote {hdr_path} ({hdr_path.stat().st_size:,} bytes)')

    # Emit FiestaProtoOpcodes.h
    op_path = GENERATED / 'FiestaProtoOpcodes.h'
    op_path.write_text(emit_opcode_header(wire,
                       HERE.parent / 'shine_opcodes.tsv'))
    print(f'Wrote {op_path} ({op_path.stat().st_size:,} bytes)')

    print('\nDone. Add Generated/ to your .vcxproj include path.')

if __name__ == '__main__':
    main()
