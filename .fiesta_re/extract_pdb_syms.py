"""Extract (RVA, demangled name) pairs for every NC_* / pft_Store /
PacketEncrypt symbol in the server PDB. Output a TSV the disasm pass
will join against."""
import sys, re
import pdbparse
from pdbparse.symlookup import Lookup

PDB = '/app/.fiesta_re/server/5ZoneServer2.pdb'
EXE = '/app/.fiesta_re/server/5ZoneServer2.exe'

# Use Lookup-style API: build a list of (PDB, ImageBase) and ask it for names.
# But what we really want is to walk every public symbol record and dump it.
pdb = pdbparse.parse(PDB, fast_load=True)

# Public symbols
syms = []
try:
    pubsym = pdb.STREAM_GSYM if hasattr(pdb, 'STREAM_GSYM') else None
except Exception:
    pubsym = None

# Easier path: use the named global stream "Globals" / "Publics".
# pdbparse exposes them via streams[].
for name in ('STREAM_PUBSYM', 'STREAM_GSYM30', 'STREAM_GSYM'):
    s = getattr(pdb, name, None)
    if s is None:
        continue
    for sym in getattr(s, 'globals', []):
        try:
            n = sym.name
            seg = sym.segment
            off = sym.offset
            syms.append((seg, off, n))
        except Exception:
            pass

print(f'{len(syms)} symbols collected', file=sys.stderr)

# Resolve segments→RVAs using the section contributions.
# Each segment is a 1-based index into the section table.
secmap = {}
try:
    sects = pdb.STREAM_SECT_HDR.sections
except Exception:
    sects = []
for i, s in enumerate(sects, start=1):
    secmap[i] = s.VirtualAddress

# Filter to only the NC_*/pft_Store/PacketEncrypt-related symbols.
TARGET_RE = re.compile(
    r'(NC_[A-Z][A-Z0-9_]+(_REQ|_ACK|_CMD|_CMDD)|pft_Store|PacketEncrypt|EncSeedSet|so_Send|sp_NC_)',
)

with open('/tmp/pdb_target_syms.tsv', 'w') as out:
    out.write('rva\tsegment\toffset\tsymbol\n')
    for seg, off, n in syms:
        if not TARGET_RE.search(n):
            continue
        rva = secmap.get(seg, 0) + off
        out.write(f'{rva:#010x}\t{seg}\t{off:#010x}\t{n}\n')

print('wrote /tmp/pdb_target_syms.tsv', file=sys.stderr)
