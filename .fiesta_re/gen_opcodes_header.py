"""Generate the final FiestaPacket.h::Op enum and a flat opcode TSV
from the recovered (arg1, arg2, name) tuples. Wire opcode encoding
confirmed: (arg2 << 8) | arg1 — arg2 is the subsystem major, arg1 is
the command minor."""
import struct, sys, re, collections
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM

EXE = '/app/.fiesta_re/server/5ZoneServer2-Port80-NoService.exe'
UDD = '/app/.fiesta_re/server/5ZoneServer2.udd'

# --- udd loader (same as before) ---
blob = open(UDD, 'rb').read()
i = 0
while i < len(blob) - 8:
    if blob[i] == 0x0a and all(0x20 <= c < 0x7F for c in blob[i+1:i+4]):
        dl = struct.unpack_from('<I', blob, i+4)[0]
        if 0 <= dl <= 0x10000 and i + 8 + dl <= len(blob):
            break
    i += 1
handler_by_rva = {}
while i < len(blob) - 8:
    if blob[i] != 0x0a: i += 1; continue
    if not all(0x20 <= c < 0x7F for c in blob[i+1:i+4]):
        i += 1; continue
    tag = blob[i+1:i+4].decode('latin1', errors='replace')
    dl  = struct.unpack_from('<I', blob, i+4)[0]
    if dl > 0x10000 or i + 8 + dl > len(blob):
        i += 1; continue
    if tag == 'Us4' and dl >= 5:
        rva  = struct.unpack_from('<I', blob, i+8)[0]
        name = blob[i+12: i+8+dl].split(b'\x00', 1)[0]
        try: n = name.decode('latin1')
        except Exception: n = ''
        if n: handler_by_rva[rva] = n
    i += 8 + dl

# --- EXE loader & pft_Store scanner ---
pe = pefile.PE(EXE, fast_load=True)
img = pe.OPTIONAL_HEADER.ImageBase
text = next(s for s in pe.sections if s.Name.startswith(b'.text'))
text_data = text.get_data()
text_va = img + text.VirtualAddress
handler_va_to_name = {img + r: n for r, n in handler_by_rva.items()}
md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = True

results = []  # (arg1, arg2, name, handler_va)
i = 0; N = len(text_data)
while i + 5 <= N:
    if text_data[i] != 0x68:
        i += 1; continue
    handler_imm = struct.unpack_from('<I', text_data, i + 1)[0]
    if handler_imm not in handler_va_to_name:
        i += 1; continue
    insns = list(md.disasm(text_data[i:i + 50], text_va + i))[:8]
    if len(insns) < 4:
        i += 1; continue
    if insns[0].mnemonic != 'push':
        i += 1; continue
    arg_pushes = []
    saw_mov_ecx = False
    saw_call = False
    for ins in insns[1:]:
        m = ins.mnemonic.lower()
        if m == 'push' and not saw_mov_ecx:
            if (len(ins.operands) == 1 and ins.operands[0].type == CS_OP_IMM):
                arg_pushes.append(ins.operands[0].imm & 0xFFFFFFFF)
            else:
                break
        elif m == 'mov' and 'ecx' in ins.op_str.split(',')[0]:
            saw_mov_ecx = True
        elif m == 'call' and saw_mov_ecx:
            saw_call = True
            break
        else:
            break
    if not saw_call or len(arg_pushes) != 2:
        i += 1; continue
    arg1, arg2 = arg_pushes
    if arg1 > 0xFFFF or arg2 > 0xFFFF:
        i += 1; continue
    results.append((arg1, arg2, handler_va_to_name[handler_imm], handler_imm))
    i += 1

print(f'Recovered {len(results)} pft_Store calls', file=sys.stderr)

# --- post-process: filter to player-facing handlers, dedup ---
# Player-facing = ShinePlayer (sp_NC_*). Server-internal sessions
# (GameDBSession, WorldManagerSession, etc.) are zone↔server traffic
# we don't speak as a client.
player = [(arg1, arg2, name) for arg1, arg2, name, _ in results
          if 'ShinePlayer' in name and 'sp_NC_' in name]
print(f'ShinePlayer-only: {len(player)}', file=sys.stderr)

# Dedup by name (some opcodes register multiple times)
seen = {}
for arg1, arg2, name in player:
    short = name.split('::sp_')[-1]
    if short.startswith('NC_'):
        wire = (arg2 << 8) | arg1
        seen[short] = (wire, arg1, arg2)
print(f'Unique NC_* names: {len(seen)}', file=sys.stderr)

# --- emit TSV ---
with open('/app/ElleAnn/Services/Elle.Service.Fiesta/_re_artifacts/shine_opcodes.tsv', 'w') as f:
    f.write('opcode\tname\tmajor\tminor\n')
    for name, (wire, arg1, arg2) in sorted(seen.items(), key=lambda x: x[1][0]):
        f.write(f'0x{wire:04X}\t{name}\t0x{arg2:02X}\t0x{arg1:02X}\n')

# --- emit C++ enum ---
# Group by subsystem for readability, sort opcodes within each.
by_sub = collections.defaultdict(list)
for name, (wire, arg1, arg2) in seen.items():
    m = re.match(r'NC_([A-Z][A-Z0-9]+)_(.+)', name)
    if m:
        by_sub[m.group(1)].append((wire, name))
for sub in by_sub:
    by_sub[sub].sort()

cpp = []
cpp.append('/* Auto-generated from 5ZoneServer2-Port80-NoService.exe + .udd */')
cpp.append('namespace Op {')
cpp.append('')
for sub in sorted(by_sub.keys()):
    cpp.append(f'    /* ── NC_{sub} ──────────────────────────────────────────── */')
    for wire, name in by_sub[sub]:
        cpp.append(f'    constexpr uint16_t {name:<55} = 0x{wire:04X};')
    cpp.append('')
cpp.append('}  /* namespace Op */')

cpp_text = '\n'.join(cpp)
with open('/app/ElleAnn/Services/Elle.Service.Fiesta/_re_artifacts/shine_opcodes_enum.cpp.inc', 'w') as f:
    f.write(cpp_text)

print('\nFirst 25 entries (sorted by wire opcode):', file=sys.stderr)
for name, (wire, arg1, arg2) in sorted(seen.items(), key=lambda x: x[1][0])[:25]:
    print(f'  0x{wire:04X}  {name}', file=sys.stderr)

print(f'\nSubsystem coverage:', file=sys.stderr)
for sub in sorted(by_sub.keys()):
    ops = by_sub[sub]
    print(f'  NC_{sub:12} {len(ops):3}  range 0x{min(o[0] for o in ops):04X}..0x{max(o[0] for o in ops):04X}',
          file=sys.stderr)
