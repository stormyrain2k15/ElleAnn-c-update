#!/usr/bin/env python3
"""
Minimal CodeView TPI v7.0 / v8.0 reader for older PDBs that
llvm-pdbutil refuses to parse (hash-stream key-size mismatch).

Walks Stream 2 type records sequentially and emits the same JSON
shape as our llvm-pdbutil-driven parser (`parse_pdb_types.py`).

Strict NO-STUB: every record kind we don't recognise is recorded
as `kind=UNK_<num>` with raw byte length so callers can audit gaps.
"""
import struct
import sys
import json
from pathlib import Path

# CodeView leaf kinds we care about (all u16 little-endian)
LF_POINTER     = 0x1002
LF_PROCEDURE   = 0x1008
LF_MFUNCTION   = 0x1009
LF_ARGLIST     = 0x1201
LF_FIELDLIST   = 0x1203
LF_BITFIELD    = 0x1205
LF_METHODLIST  = 0x1206
LF_ENUM        = 0x1507
LF_ARRAY       = 0x1503
LF_CLASS       = 0x1504
LF_STRUCTURE   = 0x1505
LF_UNION       = 0x1506
LF_MODIFIER    = 0x1001
LF_VTSHAPE     = 0x000A

# Sub-leaf kinds (inside LF_FIELDLIST)
LF_MEMBER      = 0x150D
LF_ENUMERATE   = 0x1502
LF_BCLASS      = 0x1400
LF_VBCLASS     = 0x1401
LF_IVBCLASS    = 0x1402
LF_NESTTYPE    = 0x1510
LF_NESTTYPE_EX = 0x1512
LF_VFUNCTAB    = 0x1409
LF_FRIENDFCN   = 0x150C
LF_FRIENDFCN_ST= 0x1403
LF_FRIENDCLS   = 0x150A
LF_ONEMETHOD   = 0x150F
LF_METHOD      = 0x150E
LF_INDEX       = 0x1404

LF_NUMERIC_BASE = 0x8000  # Numeric leaves >= 0x8000 are encoded specially


def read_msf_directory(data: bytes):
    """Return (block_size, stream_sizes, stream_block_lists)."""
    blk = struct.unpack_from('<I', data, 0x20)[0]
    dir_size = struct.unpack_from('<I', data, 0x2C)[0]
    dir_blocks_blk_addr = struct.unpack_from('<I', data, 0x34)[0]
    n_dir_blocks = (dir_size + blk - 1) // blk
    dir_blocks_data = data[dir_blocks_blk_addr*blk : dir_blocks_blk_addr*blk + n_dir_blocks*4]
    dir_block_indices = struct.unpack_from(f'<{n_dir_blocks}I', dir_blocks_data, 0)
    dir_data = b''
    for idx in dir_block_indices:
        dir_data += data[idx*blk : (idx+1)*blk]
    dir_data = dir_data[:dir_size]
    n_streams = struct.unpack_from('<I', dir_data, 0)[0]
    stream_sizes = list(struct.unpack_from(f'<{n_streams}I', dir_data, 4))
    pos = 4 + n_streams*4
    stream_blocks = []
    for i in range(n_streams):
        sz = stream_sizes[i]
        if sz == 0xFFFFFFFF:
            stream_blocks.append([])
            continue
        nb = (sz + blk - 1) // blk
        bl = list(struct.unpack_from(f'<{nb}I', dir_data, pos))
        stream_blocks.append(bl)
        pos += nb*4
    return blk, stream_sizes, stream_blocks


def read_stream(data: bytes, blk: int, stream_blocks, stream_sizes, idx: int) -> bytes:
    blocks = stream_blocks[idx]
    sz = stream_sizes[idx]
    raw = b''
    for b in blocks:
        raw += data[b*blk : (b+1)*blk]
    return raw[:sz]


def read_numeric(buf: bytes, offset: int):
    """CodeView numeric leaf — returns (value, bytes_consumed)."""
    val = struct.unpack_from('<H', buf, offset)[0]
    if val < LF_NUMERIC_BASE:
        return val, 2
    table = {
        0x8000: ('<b', 1),   # LF_CHAR
        0x8001: ('<h', 2),   # LF_SHORT
        0x8002: ('<H', 2),   # LF_USHORT
        0x8003: ('<i', 4),   # LF_LONG
        0x8004: ('<I', 4),   # LF_ULONG
        0x8005: ('<f', 4),   # LF_REAL32
        0x8006: ('<d', 8),   # LF_REAL64
        0x8009: ('<q', 8),   # LF_QUADWORD
        0x800A: ('<Q', 8),   # LF_UQUADWORD
    }
    if val in table:
        fmt, sz = table[val]
        v = struct.unpack_from(fmt, buf, offset+2)[0]
        return v, 2 + sz
    return None, 2   # unknown — skip


def read_cstr(buf: bytes, offset: int):
    """Null-terminated UTF-8/ASCII string."""
    end = buf.find(b'\0', offset)
    if end < 0:
        return '', len(buf) - offset
    return buf[offset:end].decode('utf-8', errors='replace'), (end - offset + 1)


def parse_field_record(buf: bytes, offset: int, members: list):
    """Parses a single sub-leaf of LF_FIELDLIST. Returns bytes consumed."""
    kind = struct.unpack_from('<H', buf, offset)[0]
    p = offset + 2

    if kind == LF_MEMBER:
        # u16 attrs, u32 type, numeric offset, name
        attrs = struct.unpack_from('<H', buf, p)[0]; p += 2
        type_id = struct.unpack_from('<I', buf, p)[0]; p += 4
        moffset, n = read_numeric(buf, p); p += n
        name, n = read_cstr(buf, p); p += n
        members.append({
            'sub': 'member', 'name': name, 'type': f'{type_id:04x}',
            'offset': moffset if isinstance(moffset, int) else 0
        })
    elif kind == LF_ENUMERATE:
        attrs = struct.unpack_from('<H', buf, p)[0]; p += 2
        val, n = read_numeric(buf, p); p += n
        name, n = read_cstr(buf, p); p += n
        members.append({'sub': 'enum', 'name': name, 'value': val if isinstance(val, int) else 0})
    elif kind == LF_NESTTYPE:
        # padding u16, type id, name
        p += 2
        type_id = struct.unpack_from('<I', buf, p)[0]; p += 4
        name, n = read_cstr(buf, p); p += n
        members.append({'sub': 'nest', 'name': name, 'type': f'{type_id:04x}'})
    elif kind == LF_ONEMETHOD:
        attrs = struct.unpack_from('<H', buf, p)[0]; p += 2
        type_id = struct.unpack_from('<I', buf, p)[0]; p += 4
        # If introducing virtual, vtab offset (u32) follows
        if (attrs >> 2) & 0x7 in (4, 6):  # MTintro / MTpureintro
            p += 4
        name, n = read_cstr(buf, p); p += n
        members.append({'sub': 'method', 'name': name})
    elif kind == LF_METHOD:
        count = struct.unpack_from('<H', buf, p)[0]; p += 2
        ml = struct.unpack_from('<I', buf, p)[0]; p += 4
        name, n = read_cstr(buf, p); p += n
        members.append({'sub': 'methods', 'name': name, 'count': count})
    elif kind == LF_BCLASS:
        attrs = struct.unpack_from('<H', buf, p)[0]; p += 2
        type_id = struct.unpack_from('<I', buf, p)[0]; p += 4
        boff, n = read_numeric(buf, p); p += n
        members.append({'sub': 'bclass', 'type': f'{type_id:04x}'})
    elif kind == LF_VFUNCTAB:
        p += 2  # padding
        type_id = struct.unpack_from('<I', buf, p)[0]; p += 4
        members.append({'sub': 'vftab', 'type': f'{type_id:04x}'})
    elif kind == LF_INDEX:
        p += 2  # padding
        idx_type = struct.unpack_from('<I', buf, p)[0]; p += 4
        members.append({'sub': 'index', 'type': f'{idx_type:04x}'})
    else:
        # Unknown sub-leaf — bail out, return remaining chunk
        return None

    # Skip alignment padding (LF_PAD0 .. LF_PAD15: 0xF0..0xFF)
    while p < len(buf) and buf[p] >= 0xF0 and buf[p] <= 0xFF:
        p += (buf[p] & 0x0F)
        if p > len(buf):
            break
    return p - offset


def parse_struct_or_union_record(buf: bytes, kind: int):
    """Parses LF_STRUCTURE / LF_CLASS / LF_UNION header. Returns dict."""
    # u16 count, u16 props, u32 fieldlist, [u32 derived, u32 vshape] for STRUCT/CLASS,
    # numeric size, name [, unique_name]
    p = 0
    count = struct.unpack_from('<H', buf, p)[0]; p += 2
    props = struct.unpack_from('<H', buf, p)[0]; p += 2
    fieldlist = struct.unpack_from('<I', buf, p)[0]; p += 4
    if kind in (LF_STRUCTURE, LF_CLASS):
        derived = struct.unpack_from('<I', buf, p)[0]; p += 4
        vshape  = struct.unpack_from('<I', buf, p)[0]; p += 4
    sizeof, n = read_numeric(buf, p); p += n
    name, n = read_cstr(buf, p); p += n
    # If "has unique name" flag (bit 9), unique name follows
    unique = ''
    if props & 0x200:
        unique, n = read_cstr(buf, p); p += n
    is_fwdref = bool(props & 0x80)
    return {
        'kind': 'LF_UNION' if kind == LF_UNION else ('LF_CLASS' if kind == LF_CLASS else 'LF_STRUCTURE'),
        'name': name,
        'sizeof': sizeof if isinstance(sizeof, int) else None,
        'fieldlist': f'{fieldlist:04x}' if fieldlist else None,
        'fwdref': is_fwdref,
        'unique': unique,
        'count': count,
    }


def parse_array_record(buf: bytes):
    p = 0
    elem_type = struct.unpack_from('<I', buf, p)[0]; p += 4
    idx_type = struct.unpack_from('<I', buf, p)[0]; p += 4
    sz, n = read_numeric(buf, p); p += n
    name, n = read_cstr(buf, p); p += n
    return {
        'kind': 'LF_ARRAY',
        'element': f'{elem_type:04x}',
        'count': sz if isinstance(sz, int) else 0,
    }


def parse_pointer_record(buf: bytes):
    if len(buf) < 4: return {'kind': 'LF_POINTER', 'referent': None}
    referent = struct.unpack_from('<I', buf, 0)[0]
    return {'kind': 'LF_POINTER', 'referent': f'{referent:04x}'}


def parse_enum_record(buf: bytes):
    p = 0
    count = struct.unpack_from('<H', buf, p)[0]; p += 2
    props = struct.unpack_from('<H', buf, p)[0]; p += 2
    underlying = struct.unpack_from('<I', buf, p)[0]; p += 4
    fieldlist = struct.unpack_from('<I', buf, p)[0]; p += 4
    name, n = read_cstr(buf, p); p += n
    return {
        'kind': 'LF_ENUM',
        'name': name,
        'fieldlist': f'{fieldlist:04x}' if fieldlist else None,
    }


def main():
    if len(sys.argv) < 3:
        print('usage: cv_pdb_dump.py <pdb> <out_prefix>')
        sys.exit(2)
    pdb_path = Path(sys.argv[1])
    prefix = sys.argv[2]
    data = pdb_path.read_bytes()

    blk, stream_sizes, stream_blocks = read_msf_directory(data)
    tpi = read_stream(data, blk, stream_blocks, stream_sizes, 2)
    print(f'TPI stream loaded: {len(tpi)} bytes', file=sys.stderr)

    # Header
    ver, hdrsz, tib, tie, recs = struct.unpack_from('<IIIII', tpi, 0)
    print(f'TPI ver={ver}, hdrsz={hdrsz}, tib={tib}, tie={tie}, recs={recs}', file=sys.stderr)
    p = hdrsz
    end = hdrsz + recs

    types = {}
    cur_id = tib
    while p + 2 <= end:
        rlen = struct.unpack_from('<H', tpi, p)[0]
        if rlen == 0:
            break
        rec_start = p + 2
        rec_end = rec_start + rlen
        if rec_end > len(tpi):
            break
        rec_kind = struct.unpack_from('<H', tpi, rec_start)[0]
        body = tpi[rec_start+2 : rec_end]

        try:
            if rec_kind in (LF_STRUCTURE, LF_CLASS, LF_UNION):
                d = parse_struct_or_union_record(body, rec_kind)
                d['id'] = f'{cur_id:04x}'
                types[d['id']] = d
            elif rec_kind == LF_FIELDLIST:
                members = []
                fp = 0
                while fp < len(body):
                    consumed = parse_field_record(body, fp, members)
                    if not consumed:
                        break
                    fp += consumed
                types[f'{cur_id:04x}'] = {
                    'id': f'{cur_id:04x}', 'kind': 'LF_FIELDLIST', 'members': members
                }
            elif rec_kind == LF_ARRAY:
                d = parse_array_record(body)
                d['id'] = f'{cur_id:04x}'
                types[d['id']] = d
            elif rec_kind == LF_POINTER:
                d = parse_pointer_record(body)
                d['id'] = f'{cur_id:04x}'
                types[d['id']] = d
            elif rec_kind == LF_ENUM:
                d = parse_enum_record(body)
                d['id'] = f'{cur_id:04x}'
                types[d['id']] = d
        except Exception as exc:
            types[f'{cur_id:04x}'] = {'id': f'{cur_id:04x}', 'kind': 'PARSE_ERROR',
                                       'err': repr(exc), 'rec_kind': rec_kind}

        cur_id += 1
        p = rec_end

    Path(f'{prefix}_raw_types.json').write_text(json.dumps(types, indent=1))
    print(f'parsed {len(types)} type records', file=sys.stderr)
    print(f'wrote {prefix}_raw_types.json')


if __name__ == '__main__':
    main()
