"""Parse OllyDbg .udd file. Record format (confirmed via hex dump):

  0a 'Us4' <u32 LE data_len> <u32 LE RVA> <NUL-term ASCII name>

The 'Us4' tag is the dominant (sometimes only) record type for symbols;
other tags ('Mod0', 'Sect', 'Br0', etc.) follow the same shell."""
import struct, re, sys

UDD = '/app/.fiesta_re/server/5ZoneServer2.udd'
blob = open(UDD, 'rb').read()
print(f'.udd size: {len(blob)}', file=sys.stderr)

# Walk record-by-record. Each record: <0a> <3-char tag ASCII> <u32 len> <len bytes>
# For 'Us4', payload = <u32 RVA> <NUL-terminated name>.
records_by_tag = {}
i = 0
# Skip header until we hit the first valid 0a-tag-len-data record.
while i < len(blob) - 8:
    if blob[i] == 0x0a and all(0x20 <= c < 0x7F for c in blob[i+1:i+4]):
        # tentatively a record start
        data_len = struct.unpack_from('<I', blob, i+4)[0]
        if 0 <= data_len <= 0x10000 and i + 8 + data_len <= len(blob):
            break
    i += 1
print(f'First record starts at offset {i:#x}', file=sys.stderr)

us4_records = []
all_tags = []
while i < len(blob) - 8:
    if blob[i] != 0x0a:
        i += 1
        continue
    tag = blob[i + 1: i + 4].decode('latin1', errors='replace')
    if not all(0x21 <= c < 0x7F for c in blob[i + 1: i + 4]):
        i += 1
        continue
    data_len = struct.unpack_from('<I', blob, i + 4)[0]
    if data_len > 0x10000 or i + 8 + data_len > len(blob):
        i += 1
        continue
    payload = blob[i + 8: i + 8 + data_len]
    all_tags.append(tag)
    if tag == 'Us4' and data_len >= 5:
        rva  = struct.unpack_from('<I', payload, 0)[0]
        # name is everything after the RVA, NUL-terminated
        name = payload[4:].split(b'\x00', 1)[0]
        try:
            name = name.decode('latin1')
        except Exception:
            name = ''
        if name:
            us4_records.append((rva, name))
    i += 8 + data_len

print(f'Total records walked: {len(all_tags)}', file=sys.stderr)
import collections
tag_dist = collections.Counter(all_tags)
print(f'Tag distribution: {tag_dist.most_common(20)}', file=sys.stderr)
print(f'Us4 records: {len(us4_records)}', file=sys.stderr)

# Distribution of RVAs
rvas = [r for r, _ in us4_records]
if rvas:
    print(f'RVA min={min(rvas):#x} max={max(rvas):#x}', file=sys.stderr)

# Save full
with open('/tmp/udd_records.tsv', 'w') as out:
    out.write('rva\tname\n')
    for rva, name in us4_records:
        out.write(f'{rva:#010x}\t{name}\n')

# NC_ filter
nc = [(rva, name) for rva, name in us4_records
      if re.search(r'NC_[A-Z][A-Z0-9_]+', name)]
print(f'NC_* labelled records: {len(nc)}', file=sys.stderr)

# pft_Store filter
pft = [(rva, name) for rva, name in us4_records
       if 'pft_Store' in name or 'pft_Setup' in name]
print(f'pft_* records: {len(pft)}', file=sys.stderr)

with open('/tmp/udd_nc.tsv', 'w') as out:
    out.write('rva\tname\n')
    for rva, name in sorted(nc):
        out.write(f'{rva:#010x}\t{name}\n')

print('\nSample NC_* records (first 30):', file=sys.stderr)
for rva, name in sorted(nc)[:30]:
    print(f'  rva={rva:#010x}  {name}', file=sys.stderr)

print('\npft_Store-related records:', file=sys.stderr)
for rva, name in sorted(pft):
    print(f'  rva={rva:#010x}  {name}', file=sys.stderr)
