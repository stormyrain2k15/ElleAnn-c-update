#!/usr/bin/env python3
"""
Parses llvm-pdbutil-15 'dump --types' output and extracts a flat,
human-readable table of every PROTO_NC_* struct with field name,
offset, type (resolved), and total sizeof.

Output: <prefix>_protos.md  (Markdown table per struct)
        <prefix>_protos.json (JSON for automated consumption)

Strict NO-STUB: every type id encountered is resolved or flagged as
'UNRESOLVED' so callers can audit gaps.
"""
import re
import sys
import json
from pathlib import Path

# ---- Regexes for the llvm-pdbutil text output ----------------------------
RE_HEADER = re.compile(r'^\s*0x([0-9A-Fa-f]+)\s*\|\s*(LF_[A-Z_]+)\s*\[size = (\d+)\](?:\s*`([^`]+)`)?')
RE_MEMBER = re.compile(r'^\s*-\s*LF_MEMBER\s*\[name = `([^`]*)`,\s*Type = 0x([0-9A-Fa-f]+),\s*offset = (\d+)')
RE_STRUCTURE_OPTS = re.compile(r'sizeof (\d+)')
RE_FIELDLIST_REF = re.compile(r'field list:\s*0x([0-9A-Fa-f]+)')
RE_ARRAY = re.compile(r'size:\s*(\d+),\s*index type:\s*0x[0-9A-Fa-f]+[^,]*,\s*element type:\s*0x([0-9A-Fa-f]+)')
RE_POINTER = re.compile(r'referent = 0x([0-9A-Fa-f]+)')
RE_BASIC_INLINE = re.compile(r'\(([^)]+)\)')   # captures " (unsigned long) " etc.

BASIC_TYPES = {
    '0010': 'char',
    '0011': 'short',
    '0012': 'long',
    '0013': 'int64',
    '0020': 'unsigned char',
    '0021': 'unsigned short',
    '0022': 'unsigned long',
    '0023': 'unsigned __int64',
    '0030': 'bool',
    '0040': 'float',
    '0041': 'double',
    '0070': 'wchar_t',
    '0071': 'wchar_t',
    '0074': 'int',
    '0075': 'unsigned int',
    '0103': 'void',
}


def norm(tid: str) -> str:
    return tid.lower().lstrip('0').rjust(4, '0') if tid else ''


def parse_basic(tid: str):
    n = norm(tid)
    return BASIC_TYPES.get(n)


def parse_pdb_dump(path: Path):
    """Returns dict[type_id] = {kind, sizeof, name, members, element, count, referent, fieldlist}"""
    types = {}
    cur = None
    with path.open('r', encoding='utf-8', errors='replace') as fh:
        for line in fh:
            m = RE_HEADER.match(line)
            if m:
                tid = norm(m.group(1))
                kind = m.group(2)
                name = m.group(4)
                cur = {
                    'id': tid,
                    'kind': kind,
                    'name': name,
                    'sizeof': None,
                    'members': [],
                    'element': None,
                    'count': None,
                    'referent': None,
                    'fieldlist': None,
                }
                # LF_UNION metadata is on its own header line: "[size = N] `name`"
                # but llvm uses "options: ... sizeof N" for unions too -> handled below.
                types[tid] = cur
                continue
            if cur is None:
                continue
            mm = RE_MEMBER.match(line)
            if mm:
                cur['members'].append({
                    'name': mm.group(1),
                    'type': norm(mm.group(2)),
                    'offset': int(mm.group(3)),
                })
                continue
            sf = RE_STRUCTURE_OPTS.search(line)
            if sf and cur['kind'] in ('LF_STRUCTURE', 'LF_UNION'):
                cur['sizeof'] = int(sf.group(1))
            fr = RE_FIELDLIST_REF.search(line)
            if fr and cur['kind'] in ('LF_STRUCTURE', 'LF_UNION'):
                cur['fieldlist'] = norm(fr.group(1))
            ar = RE_ARRAY.search(line)
            if ar and cur['kind'] == 'LF_ARRAY':
                cur['count'] = int(ar.group(1))
                cur['element'] = norm(ar.group(2))
            pr = RE_POINTER.search(line)
            if pr and cur['kind'] == 'LF_POINTER':
                cur['referent'] = norm(pr.group(1))
    return types


def basic_size(tid: str):
    """Return byte size for a basic type id, or None."""
    n = norm(tid)
    return {
        '0010': 1, '0011': 2, '0012': 4, '0013': 8,
        '0020': 1, '0021': 2, '0022': 4, '0023': 8,
        '0030': 1, '0040': 4, '0041': 8,
        '0070': 1, '0071': 2, '0074': 4, '0075': 4,
        '0103': 0,
    }.get(n)


def type_size(tid: str, types, depth=0):
    """Recursive byte-size calculator for a type id."""
    if depth > 6:
        return None
    bs = basic_size(tid)
    if bs is not None:
        return bs
    t = types.get(tid)
    if not t:
        return None
    if t['kind'] in ('LF_STRUCTURE', 'LF_UNION', 'LF_CLASS'):
        return t.get('sizeof')
    if t['kind'] == 'LF_ENUM':
        return 4
    if t['kind'] == 'LF_POINTER':
        return 4   # ptr32
    if t['kind'] == 'LF_ARRAY':
        return t.get('count')   # already in bytes
    return None


def resolve_type(tid: str, types, depth=0) -> str:
    """Returns a human-readable type expression for type id `tid`."""
    if depth > 6:
        return f'<deep:{tid}>'
    b = parse_basic(tid)
    if b:
        return b
    t = types.get(tid)
    if not t:
        return f'<UNRESOLVED:0x{tid}>'
    if t['kind'] in ('LF_STRUCTURE', 'LF_CLASS', 'LF_UNION'):
        if t['name']:
            return t['name']
        members = get_fieldlist_members(t, types)
        if members:
            inner = ', '.join(
                f"{resolve_type(m['type'], types, depth + 1)} {m['name']}@{m['offset']}"
                for m in sorted(members, key=lambda x: x['offset'])
            )
            tag = 'union' if t['kind'] == 'LF_UNION' else 'struct'
            return f'{tag}{{ {inner} }}'
        return f'<anon_{t["kind"][3:].lower()}:0x{tid}>'
    if t['kind'] == 'LF_ENUM':
        return t['name'] or f'enum<0x{tid}>'
    if t['kind'] == 'LF_ARRAY':
        # llvm-pdbutil reports `count` as TOTAL BYTE SIZE; convert to element count
        elem_id = t['element']
        elem_name = resolve_type(elem_id, types, depth + 1) if elem_id else '?'
        total = t.get('count') or 0
        esz = type_size(elem_id, types, depth + 1) if elem_id else None
        if esz and esz > 0 and total % esz == 0:
            return f'{elem_name}[{total // esz}]   /* {total} bytes */'
        return f'{elem_name}[?] /* {total} bytes */'
    if t['kind'] == 'LF_POINTER':
        ref = resolve_type(t['referent'], types, depth + 1) if t['referent'] else '?'
        return f'{ref}*'
    if t['kind'] == 'LF_PROCEDURE' or t['kind'] == 'LF_MFUNCTION':
        return f'<func:0x{tid}>'
    if t['kind'] == 'LF_MODIFIER':
        return f'<modifier:0x{tid}>'
    return f'<{t["kind"]}:0x{tid}>'


def get_fieldlist_members(struct, types):
    """Returns the member list (resolves struct->fieldlist)."""
    fl_id = struct.get('fieldlist')
    if fl_id and fl_id in types:
        fl = types[fl_id]
        if fl['kind'] == 'LF_FIELDLIST':
            return fl['members']
    # Fallback: structure may have inline members (older format) -> use its own list
    return struct.get('members', [])


def render_struct_md(struct, types):
    """Returns markdown rendering of a single struct."""
    name = struct['name'] or '<anon>'
    sz = struct.get('sizeof')
    members = get_fieldlist_members(struct, types)
    lines = [f'### `{name}`  (sizeof = {sz} bytes)', '']
    if not members:
        lines.append('_no members_')
        return '\n'.join(lines) + '\n'
    lines.append('| Offset | Field | Type |')
    lines.append('|-------:|-------|------|')
    for m in sorted(members, key=lambda x: x['offset']):
        ty = resolve_type(m['type'], types)
        lines.append(f"| {m['offset']:>5} | `{m['name']}` | `{ty}` |")
    return '\n'.join(lines) + '\n'


def render_struct_json(struct, types):
    members = get_fieldlist_members(struct, types)
    return {
        'name': struct['name'],
        'sizeof': struct.get('sizeof'),
        'members': [
            {
                'offset': m['offset'],
                'name': m['name'],
                'type': resolve_type(m['type'], types),
                'type_id': m['type'],
            }
            for m in sorted(members, key=lambda x: x['offset'])
        ],
    }


def main():
    if len(sys.argv) < 3:
        print('usage: parse_pdb_types.py <types.txt> <out_prefix>')
        sys.exit(2)
    src = Path(sys.argv[1])
    prefix = sys.argv[2]
    types = parse_pdb_dump(src)

    # Filter to PROTO_NC_*, NETPACKET-related, and a few core engine structs we care about
    keep_prefixes = ('PROTO_NC_', 'NETPACKET', 'tagPACKETHEADER', 'PACKETHEADER',
                     'NC_AVATAR_', 'CHARTITLE', 'CHAR_INFO', 'GUILD_INFO')
    structs = [t for t in types.values()
               if t['kind'] == 'LF_STRUCTURE'
               and t['name']
               and t['name'].startswith(keep_prefixes)
               and t['sizeof'] is not None]

    # De-dupe by (name, sizeof) - take the one WITH a fieldlist whenever possible
    by_name = {}
    for s in structs:
        key = s['name']
        ex = by_name.get(key)
        score = (1 if get_fieldlist_members(s, types) else 0, s.get('sizeof') or 0)
        ex_score = (1 if ex and get_fieldlist_members(ex, types) else 0,
                    (ex.get('sizeof') if ex else 0) or 0)
        if ex is None or score > ex_score:
            by_name[key] = s

    final = sorted(by_name.values(), key=lambda x: x['name'])

    # Write Markdown
    md_path = Path(f'{prefix}_protos.md')
    with md_path.open('w', encoding='utf-8') as fh:
        fh.write(f'# PROTO_NC_* Struct Layouts (extracted from PDB)\n\n')
        fh.write(f'_Source: `{src.name}` -- {len(final)} structs_\n\n')
        for s in final:
            fh.write(render_struct_md(s, types))
            fh.write('\n')

    # Write JSON
    json_path = Path(f'{prefix}_protos.json')
    with json_path.open('w', encoding='utf-8') as fh:
        json.dump([render_struct_json(s, types) for s in final], fh, indent=2)

    print(f'wrote {md_path} ({len(final)} structs)')
    print(f'wrote {json_path}')


if __name__ == '__main__':
    main()
