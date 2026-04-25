"""Disassemble the ShineEngine ZoneServer EXE and extract every
(opcode, handler_RVA) pair from `pft_Store` call sites.

ShineEngine pattern (MSVC 2008-ish, 32-bit __thiscall on the template
instantiation):

    push   offset <handler_function>     ; 68 ?? ?? ?? ??       (5 bytes)
    push   <opcode_immediate>            ; 6A ?? OR 68 ?? ?? ?? ??
    mov    ecx, offset <pft_table_obj>   ; B9 ?? ?? ?? ??       (5 bytes)
    call   <pft_Store_specialization>    ; E8 ?? ?? ?? ??       (5 bytes)

The relative `call` target is the same RVA for each instantiation of
pft_Store<T>::pft_Store. We don't know that RVA yet, but we DO know
the pattern shape: a `push imm32`/`push imm8` immediately followed by
a `mov ecx, imm32`. By scanning .text for that combination and
checking the preceding `push imm32` (the handler address), we can
recover all (opcode, handler_addr) pairs without needing PDB types.

We then cross-reference handler_addr against the PDB's public-symbol
section-relative offsets (which we extract via raw stream walk) to
turn the addresses into NC_* names.
"""

import struct, re, sys, os
import pefile

EXE = '/app/.fiesta_re/server/5ZoneServer2.exe'
PDB = '/app/.fiesta_re/server/5ZoneServer2.pdb'

pe  = pefile.PE(EXE, fast_load=True)
img = pe.OPTIONAL_HEADER.ImageBase
print(f'ImageBase: {img:#010x}', file=sys.stderr)

text = None
for sec in pe.sections:
    name = sec.Name.rstrip(b'\x00\x20').decode(errors='ignore')
    if name == '.text':
        text = sec
        break
assert text, 'no .text section'

text_va    = img + text.VirtualAddress
text_data  = text.get_data()
print(f'.text VA={text_va:#010x} size={len(text_data):#x}', file=sys.stderr)

# Quick scan for ShineEngine signature: "pft_Store" string in any
# section. Address of the ASCII literal helps us bound the data.
print('Quick string scan in .rdata...', file=sys.stderr)
for sec in pe.sections:
    name = sec.Name.rstrip(b'\x00\x20').decode(errors='ignore')
    if name not in ('.rdata', '.data'):
        continue
    d = sec.get_data()
    if b'pft_Store' in d:
        idx = d.index(b'pft_Store')
        print(f'  found "pft_Store" in {name} at section-offset {idx:#x}',
              file=sys.stderr)

# Pattern-match all candidate pft_Store call sites in .text.
# Heuristic structure (with both push variants):
#
#   68 LL LL LL LL          push imm32 (handler)
#   { 6A NN  |  68 NN NN NN NN }   push opcode (imm8 or imm32)
#   B9 ?? ?? ?? ??          mov ecx, imm32 (pft table this-ptr)
#   E8 ?? ?? ?? ??          call rel32     (pft_Store specialization)
#
# The order of the first two pushes can be either way (compiler can
# reorder), but in practice the pattern is consistent within one EXE.
# We look for both orderings.

call_targets = {}     # rel32 → count (the pft_Store specialization addresses)
hits = []             # list of (text_offset, handler_rva, opcode)

i = 0
N = len(text_data)
while i < N - 16:
    b = text_data[i]
    # try pattern A: push imm32 (handler), push imm8/imm32 (opcode), mov ecx, call
    if b == 0x68:
        # push imm32 (handler)
        handler_rva = text.VirtualAddress + i  # placeholder, we'll fix below
        handler_imm = struct.unpack_from('<I', text_data, i + 1)[0]
        j = i + 5
        opcode = None
        opc_size = 0
        if text_data[j] == 0x6A:
            opcode  = text_data[j + 1]
            opc_size = 2
        elif text_data[j] == 0x68:
            opcode  = struct.unpack_from('<I', text_data, j + 1)[0]
            opc_size = 5
        if opcode is not None:
            k = j + opc_size
            if text_data[k] == 0xB9:               # mov ecx, imm32
                this_ptr = struct.unpack_from('<I', text_data, k + 1)[0]
                m = k + 5
                if text_data[m] == 0xE8:           # call rel32
                    rel = struct.unpack_from('<i', text_data, m + 1)[0]
                    target_rva = (text.VirtualAddress + m + 5 + rel) & 0xFFFFFFFF
                    call_targets[target_rva] = call_targets.get(target_rva, 0) + 1
                    hits.append((text.VirtualAddress + i,
                                 handler_imm, opcode, target_rva, this_ptr))
                    i = m + 5
                    continue
    i += 1

print(f'\nTotal candidate (handler, opcode, target) tuples: {len(hits)}',
      file=sys.stderr)

# Bucket call_targets — the actual pft_Store specializations are the
# top hot-spots (each pft_Store specialization is called once per
# opcode it stores, so the function with the most hits is the most
# common session-class's protocol-init).
print('\nTop call targets (likely pft_Store template specializations):',
      file=sys.stderr)
for tgt, count in sorted(call_targets.items(), key=lambda x: -x[1])[:10]:
    print(f'  rva={tgt:#010x}  hits={count}', file=sys.stderr)

# Filter to only hits whose target is one of the top-N pft_Store
# specializations (cuts non-pft_Store false positives that happened
# to match the byte pattern).
hot_targets = {t for t, c in call_targets.items() if c >= 5}
print(f'\nKept {len(hot_targets)} hot targets with hits>=5',
      file=sys.stderr)
filtered = [h for h in hits if h[3] in hot_targets]
print(f'Filtered hits to those targeting hot specializations: '
      f'{len(filtered)}', file=sys.stderr)

# Group by target and print one bucket so we can eyeball the opcode
# distribution.
by_target = {}
for site, handler, opcode, target, this_ptr in filtered:
    by_target.setdefault(target, []).append((site, handler, opcode, this_ptr))

# Output: TSV with columns we can later join against PDB symbols.
with open('/tmp/pft_store_hits.tsv', 'w') as out:
    out.write('callsite_rva\thandler_rva\topcode\tpft_target_rva\tpft_this_ptr\n')
    for site, handler, opcode, target, this_ptr in filtered:
        out.write(f'{site:#010x}\t{handler:#010x}\t{opcode:#06x}\t'
                  f'{target:#010x}\t{this_ptr:#010x}\n')

print('\nWrote /tmp/pft_store_hits.tsv', file=sys.stderr)
print('\nFirst 30 filtered hits:', file=sys.stderr)
for h in filtered[:30]:
    print('  site={:#x}  handler={:#x}  opcode={:#06x}  '
          'target={:#x}  this={:#x}'.format(*h), file=sys.stderr)
