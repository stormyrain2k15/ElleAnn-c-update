"""Extract pft_Store-style opcode→handler pairs from the CLIENT exe.
The client registers its INCOMING (server-pushed) opcodes the same
way the server registers REQs — same `pft_Store(major, minor, &handler)`
template. We don't have a .udd for the client so we can't name the
handlers, but we CAN recover the (major, minor) tables — and just
counting these tells us how many distinct server-pushed opcodes the
client knows about, which directly corresponds to the missing
login-flow ACKs/CMDs we need."""
import struct, sys, collections
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM

EXE = '/app/.fiesta_re/client/Client.exe'
pe  = pefile.PE(EXE, fast_load=True)
img = pe.OPTIONAL_HEADER.ImageBase
text = next(s for s in pe.sections
            if s.Name.startswith(b'.text'))
text_va = img + text.VirtualAddress
text_data = text.get_data()

print(f'.text {text_va:#x} size={len(text_data):#x}', file=sys.stderr)

md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = True

# Server-side pft_Store had pattern:
#   push <handler_VA>
#   push <arg2>     (1-5 bytes)
#   push <arg1>     (1-5 bytes)
#   mov  ecx, <reg> or imm32
#   call rel32
# Same pattern in client; arg1=minor, arg2=major (or vice versa).

# Step 1: find the dominant (mov ecx ; call rel32) combos as before.
pairs = collections.Counter()
i = 0
N = len(text_data)
while i + 10 <= N:
    if text_data[i] == 0xB9 and text_data[i + 5] == 0xE8:
        ecx_imm = struct.unpack_from('<I', text_data, i + 1)[0]
        rel32   = struct.unpack_from('<i', text_data, i + 6)[0]
        call_rva = (text.VirtualAddress + i + 5 + 5 + rel32) & 0xFFFFFFFF
        if 0 < call_rva < text.VirtualAddress + text.Misc_VirtualSize:
            pairs[(ecx_imm, call_rva)] += 1
    i += 1

print(f'Top mov+call combos in client:', file=sys.stderr)
for (ecx, tgt), c in pairs.most_common(10):
    print(f'  ecx={ecx:#010x}  call={tgt:#010x}  hits={c}', file=sys.stderr)

# Try the OTHER pattern: push handler ; pushes ; mov ecx,REG ; call.
# The server build used `mov ecx, <reg>` (e.g. esi) — let me search
# for any push-imm32 (looks like a pointer into .text) followed by a
# call within ~16 bytes.
results = []
text_lo = text.VirtualAddress
text_hi = text.VirtualAddress + text.Misc_VirtualSize

i = 0
while i + 5 <= N:
    if text_data[i] != 0x68:
        i += 1; continue
    # candidate: push imm32 (potential handler)
    handler_imm = struct.unpack_from('<I', text_data, i + 1)[0]
    handler_rva = handler_imm if handler_imm < img else handler_imm - img
    if not (text_lo <= handler_rva < text_hi):
        i += 1; continue
    # walk forward up to 8 instructions looking for the (push, push, mov-ecx, call) tail.
    insns = list(md.disasm(text_data[i:i + 50], text_va + i))[:8]
    if len(insns) < 4:
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
    results.append((arg1, arg2, handler_imm))
    i += 1

print(f'\nTotal pft_Store-style call sites in client: {len(results)}',
      file=sys.stderr)

# Distribute by major (arg2 in our server convention)
import collections
by_major = collections.Counter()
for arg1, arg2, _ in results:
    by_major[arg2] += 1

print('\nDistribution by `major` byte:', file=sys.stderr)
for maj, c in sorted(by_major.most_common())[:30]:
    print(f'  major=0x{maj:02x}  count={c}', file=sys.stderr)

# Compute wire opcodes
opcodes = sorted(set((arg2 << 8) | arg1 for arg1, arg2, _ in results
                     if arg1 <= 0xFF and arg2 <= 0xFF))
print(f'\nUnique wire opcodes in client: {len(opcodes)}', file=sys.stderr)

# Save TSV
with open('/tmp/client_opcodes.tsv', 'w') as out:
    out.write('opcode\tmajor\tminor\thandler_rva\n')
    seen = set()
    for arg1, arg2, h in results:
        if arg1 > 0xFF or arg2 > 0xFF: continue
        wire = (arg2 << 8) | arg1
        if wire in seen: continue
        seen.add(wire)
        h_rva = h if h < img else h - img
        out.write(f'0x{wire:04X}\t0x{arg2:02X}\t0x{arg1:02X}\t{h_rva:#010x}\n')

# Cross-reference with our server-recovered ShinePlayer opcodes
# to find ACKs/CMDs the server sends back that we don't yet decode.
import os
sv_path = '/app/ElleAnn/Services/Elle.Service.Fiesta/_re_artifacts/shine_opcodes.tsv'
sv_set = set()
if os.path.exists(sv_path):
    for line in open(sv_path):
        if line.startswith('opcode'): continue
        op = line.split('\t')[0]
        sv_set.add(int(op, 16))
client_only = sorted(set(opcodes) - sv_set)
print(f'\nClient opcodes NOT in our server REQ table (likely server→client ACK/CMD): {len(client_only)}', file=sys.stderr)
print('First 30:', file=sys.stderr)
for op in client_only[:30]:
    maj, min_ = (op >> 8) & 0xFF, op & 0xFF
    print(f'  0x{op:04X}  major=0x{maj:02X}  minor=0x{min_:02X}', file=sys.stderr)

# Filter to login-flow majors: 0x02 (MISC), 0x04 (CHAR), 0x06 (MAP)
print('\nLogin-flow-relevant client opcodes (server→client ACK/CMD):',
      file=sys.stderr)
for op in client_only:
    maj = (op >> 8) & 0xFF
    if maj in (0x02, 0x04, 0x06, 0x07):
        print(f'  0x{op:04X}  major=0x{maj:02X}', file=sys.stderr)
