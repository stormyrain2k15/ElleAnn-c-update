#!/usr/bin/env python3
"""
Take Zone_raw_types.json (produced by cv_pdb_dump.py) and produce
the same Markdown / JSON outputs as parse_pdb_types.py for parity
with the other PDB extracts.

Strict NO-STUB: every UNRESOLVED type id is surfaced verbatim.
"""
import json
import sys
from pathlib import Path


BASIC = {
    '0010':'char','0011':'short','0012':'long','0013':'__int64',
    '0020':'unsigned char','0021':'unsigned short','0022':'unsigned long','0023':'unsigned __int64',
    '0030':'bool','0040':'float','0041':'double',
    '0070':'char','0071':'wchar_t','0074':'int','0075':'unsigned int','0103':'void',
}
BASIC_SIZE = {
    '0010':1,'0011':2,'0012':4,'0013':8,
    '0020':1,'0021':2,'0022':4,'0023':8,
    '0030':1,'0040':4,'0041':8,
    '0070':1,'0071':2,'0074':4,'0075':4,'0103':0,
}


def hex_id(x):
    return x.lower().lstrip('0').rjust(4, '0') if x else ''


def get_type(types, tid):
    if tid is None: return None
    return types.get(hex_id(tid))


def type_size(tid, types, depth=0):
    if depth > 6 or not tid: return None
    n = hex_id(tid)
    if n in BASIC_SIZE:
        return BASIC_SIZE[n]
    t = types.get(n)
    if not t: return None
    if t['kind'] in ('LF_STRUCTURE', 'LF_CLASS', 'LF_UNION'):
        return t.get('sizeof')
    if t['kind'] == 'LF_POINTER': return 4
    if t['kind'] == 'LF_ENUM':    return 4
    if t['kind'] == 'LF_ARRAY':   return t.get('count')
    return None


def resolve_type(tid, types, depth=0):
    if depth > 6 or not tid: return f'<deep:{tid}>'
    n = hex_id(tid)
    if n in BASIC: return BASIC[n]
    t = types.get(n)
    if not t: return f'<UNRESOLVED:0x{n}>'
    if t['kind'] in ('LF_STRUCTURE', 'LF_CLASS', 'LF_UNION'):
        if t.get('name'): return t['name']
        return f'<anon_{t["kind"][3:].lower()}:0x{n}>'
    if t['kind'] == 'LF_ENUM':
        return t.get('name') or f'enum<0x{n}>'
    if t['kind'] == 'LF_POINTER':
        return f'{resolve_type(t.get("referent"), types, depth+1)}*'
    if t['kind'] == 'LF_ARRAY':
        elem = resolve_type(t.get('element'), types, depth+1)
        total = t.get('count') or 0
        esz = type_size(t.get('element'), types, depth+1)
        if esz and total % esz == 0:
            return f'{elem}[{total // esz}]   /* {total} bytes */'
        return f'{elem}[?] /* {total} bytes */'
    return f'<{t["kind"]}:0x{n}>'


def get_fieldlist_members(struct, types):
    fl = struct.get('fieldlist')
    if not fl: return struct.get('members', [])
    fld = types.get(hex_id(fl))
    if fld and fld.get('kind') == 'LF_FIELDLIST':
        return [m for m in fld.get('members', []) if m.get('sub') == 'member']
    return []


def render_md(struct, types):
    name = struct.get('name') or '<anon>'
    sz = struct.get('sizeof')
    members = get_fieldlist_members(struct, types)
    lines = [f'### `{name}`  (sizeof = {sz} bytes)', '']
    if not members:
        lines.append('_no members_')
        return '\n'.join(lines) + '\n'
    lines.append('| Offset | Field | Type |')
    lines.append('|-------:|-------|------|')
    for m in sorted(members, key=lambda x: x.get('offset', 0)):
        ty = resolve_type(m.get('type'), types)
        lines.append(f"| {m.get('offset', 0):>5} | `{m.get('name', '')}` | `{ty}` |")
    return '\n'.join(lines) + '\n'


def render_json(struct, types):
    members = get_fieldlist_members(struct, types)
    return {
        'name': struct.get('name'),
        'sizeof': struct.get('sizeof'),
        'members': [
            {
                'offset': m.get('offset', 0),
                'name': m.get('name', ''),
                'type': resolve_type(m.get('type'), types),
                'type_id': m.get('type'),
            }
            for m in sorted(members, key=lambda x: x.get('offset', 0))
        ],
    }


def main():
    if len(sys.argv) < 3:
        print('usage: render_zone_protos.py <raw_types.json> <out_prefix>')
        sys.exit(2)
    raw = json.loads(Path(sys.argv[1]).read_text())
    prefix = sys.argv[2]

    keep = ('PROTO_NC_', 'NETPACKET', 'tagPACKET', 'NC_AVATAR_', 'CHAR_INFO',
            'CHARTITLE', 'GUILD_INFO')
    structs = [t for t in raw.values()
               if t['kind'] in ('LF_STRUCTURE', 'LF_CLASS')
               and t.get('name')
               and t['name'].startswith(keep)
               and t.get('sizeof') is not None
               and not t.get('fwdref')]

    by_name = {}
    for s in structs:
        ex = by_name.get(s['name'])
        score = (1 if get_fieldlist_members(s, raw) else 0, s.get('sizeof', 0) or 0)
        ex_score = (1 if ex and get_fieldlist_members(ex, raw) else 0,
                    (ex.get('sizeof') if ex else 0) or 0)
        if ex is None or score > ex_score:
            by_name[s['name']] = s

    final = sorted(by_name.values(), key=lambda x: x['name'])

    md_lines = [f'# Zone PROTO_NC_* Struct Layouts (custom V70 parser)\n',
                f'_Source: `5ZoneServer2.pdb` (TPI v7.0) -- {len(final)} structs_\n']
    for s in final:
        md_lines.append(render_md(s, raw))
        md_lines.append('')
    Path(f'{prefix}_protos.md').write_text('\n'.join(md_lines))

    Path(f'{prefix}_protos.json').write_text(
        json.dumps([render_json(s, raw) for s in final], indent=2)
    )

    print(f'wrote {prefix}_protos.md ({len(final)} structs)')
    print(f'wrote {prefix}_protos.json')

    # Extract NC_* enums for opcode resolution
    enums = {}
    for t in raw.values():
        if t['kind'] != 'LF_ENUM': continue
        fl = t.get('fieldlist')
        if not fl: continue
        fld = raw.get(hex_id(fl))
        if not fld: continue
        for m in fld.get('members', []):
            if m.get('sub') == 'enum' and m.get('name', '').startswith('NC_'):
                name = m['name']
                pfx = '_'.join(name.split('_')[:2])
                enums.setdefault(pfx, {})[name] = m['value']
    Path(f'{prefix}_enums.json').write_text(json.dumps(enums, indent=2, sort_keys=True))
    total = sum(len(v) for v in enums.values())
    print(f'wrote {prefix}_enums.json ({total} NC_* enumerators in {len(enums)} groups)')


if __name__ == '__main__':
    main()
