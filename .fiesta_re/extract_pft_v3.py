"""Find pft_Store call sites by detecting the singleton 'mov ecx, FN_TABLE'
that precedes every call. The fn-table address is reused for every
opcode registration in a session-class init, so it stands out as the
most-repeated `mov ecx, imm32 ; call rel32` immediate in the binary."""
import struct, sys, collections
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

EXE = '/app/.fiesta_re/server/5ZoneServer2.exe'
pe  = pefile.PE(EXE, fast_load=True)
img = pe.OPTIONAL_HEADER.ImageBase
text = next(s for s in pe.sections
            if s.Name.startswith(b'.text'))
text_va    = img + text.VirtualAddress
text_data  = text.get_data()

md = Cs(CS_ARCH_X86, CS_MODE_32)
md.detail = True

# Step 1 — find every (mov ecx, imm32 ; call rel32) pair, regardless
# of immediate value, and bin by (mov-imm, call-target).
print('Sweeping .text for "mov ecx, imm32 ; call rel32" pairs...',
      file=sys.stderr)

# Linear scan: at each byte where text_data[i]==0xB9, attempt to
# decode "mov ecx, imm32" (5 bytes). If the next instruction is a
# call rel32 (5 bytes), record the pair.
pairs = collections.Counter()
sites_by_pair = collections.defaultdict(list)

i = 0
N = len(text_data)
while i + 10 <= N:
    if text_data[i] == 0xB9 and text_data[i + 5] == 0xE8:
        ecx_imm = struct.unpack_from('<I', text_data, i + 1)[0]
        rel32   = struct.unpack_from('<i', text_data, i + 6)[0]
        call_rva = (text.VirtualAddress + i + 5 + 5 + rel32) & 0xFFFFFFFF
        # Only consider call targets that are inside .text.
        if 0 < call_rva < text.VirtualAddress + text.Misc_VirtualSize:
            pairs[(ecx_imm, call_rva)] += 1
            sites_by_pair[(ecx_imm, call_rva)].append(text.VirtualAddress + i)
    i += 1

print(f'  {len(pairs)} unique (ecx_imm, call_target) combos', file=sys.stderr)
print('\nTop combos (likely pft_Store registrations):', file=sys.stderr)
for (ecx, tgt), c in pairs.most_common(15):
    print(f'  ecx={ecx:#010x}  call={tgt:#010x}  hits={c}', file=sys.stderr)

# A pft_Store specialization is registered by a template, so each
# (this_ptr, target_pft_Store) combo should fire 100s of times if it's
# the player-session protocol init. Take the top combos with hits>=50.
hot = [(ecx, tgt) for (ecx, tgt), c in pairs.most_common() if c >= 50]
print(f'\n{len(hot)} hot combos with hits>=50', file=sys.stderr)

# Step 2 — for each hot combo, walk backwards from each call site to
# pull the 3 push immediates. Last-pushed = first-arg = opcode.
results = []
for ecx, tgt in hot:
    sites = sites_by_pair[(ecx, tgt)]
    decoded = 0
    for site_rva in sites:
        # site_rva is the RVA of the `mov ecx, imm32` instruction.
        # The call is 5 bytes later. We need to disassemble the bytes
        # BEFORE site_rva to find the 3 pushes.
        site_off = site_rva - text.VirtualAddress
        WIN = 24
        start = max(0, site_off - WIN)
        # Try multiple alignment offsets — instruction boundary may
        # not match start exactly.
        best = None
        for align in range(0, WIN):
            insns = list(md.disasm(
                text_data[start + align:site_off],
                text_va + start + align))
            if insns and insns[-1].address + insns[-1].size == text_va + site_off:
                best = insns
                break
        if not best:
            continue
        # Walk back collecting up to 3 immediate pushes.
        rev = list(reversed(best))
        pushes = []
        for ins in rev:
            if ins.mnemonic != 'push' or len(ins.operands) != 1:
                break
            opnd = ins.operands[0]
            if opnd.type != 2:                  # X86_OP_IMM
                break
            pushes.append(opnd.imm & 0xFFFFFFFF)
            if len(pushes) == 3:
                break
        if len(pushes) != 3:
            continue
        opcode  = pushes[0]
        arg2    = pushes[1]
        handler = pushes[2]
        # Filter: opcode must be u16 and handler must point into .text.
        if opcode > 0xFFFF:
            continue
        h_rva = handler if handler < img else handler - img
        if not (text.VirtualAddress <= h_rva
                <  text.VirtualAddress + text.Misc_VirtualSize):
            continue
        results.append((ecx, tgt, site_rva, h_rva, opcode, arg2))
        decoded += 1
    print(f'  combo ecx={ecx:#x} tgt={tgt:#x}: decoded {decoded}/{len(sites)} sites',
          file=sys.stderr)

print(f'\nTotal decoded (handler, opcode) tuples: {len(results)}',
      file=sys.stderr)

# Distribution of opcodes
op_buckets = collections.Counter()
for *_, opcode, _ in results:
    if opcode <= 0xFF:        op_buckets['<= 0xFF'] += 1
    elif opcode <= 0x7FFF:    op_buckets['0x100-0x7FFF'] += 1
    elif opcode <= 0xFFFF:    op_buckets['0x8000-0xFFFF'] += 1
print('\nOpcode bucket dist:', dict(op_buckets), file=sys.stderr)

with open('/tmp/pft_pairs.tsv', 'w') as out:
    out.write('this_ptr\tcall_target\tcall_site_rva\thandler_rva\topcode\targ2\n')
    for ecx, tgt, site, h, op, a2 in results:
        out.write(f'{ecx:#010x}\t{tgt:#010x}\t{site:#010x}\t{h:#010x}\t'
                  f'{op:#06x}\t{a2:#010x}\n')

print('\nFirst 20 results:', file=sys.stderr)
for r in results[:20]:
    print('  this={:#x} tgt={:#x}  site={:#x}  handler={:#x}  opcode={:#06x}  arg2={:#x}'
          .format(*r), file=sys.stderr)
