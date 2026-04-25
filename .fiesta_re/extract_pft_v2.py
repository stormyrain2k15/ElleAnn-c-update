"""Recover (opcode, handler_RVA) pairs from pft_Store call sites,
using proper Capstone-based disassembly + a 3-push pattern.

Demangled signature from PDB:
  pft_Store(int H, int H, ?P8ShinePlayer::handler@AEXPATNETCOMMAND@@HG@Z)

Three args = three pushes (right-to-left in cdecl/__thiscall).

Real ShineEngine MSVC pattern:
  push offset handler          ; arg3: member fn ptr (5 bytes)
  push <imm>                   ; arg2: usually a small int (1-5 bytes)
  push <opcode_imm>            ; arg1: opcode u16 (1-5 bytes)
  mov  ecx, offset pft_table   ; this-ptr (5 bytes)
  call <pft_Store>             ; (5 bytes)

We linear-scan .text, decode every instruction, and look for the
sequence [push, push, push, mov ecx imm, call rel32] where the LAST
push (= arg1 = opcode) is < 0xFFFF and the FIRST push (= arg3 =
handler) points into .text. That removes the .rdata-string false
positives we hit on the first pass.
"""

import struct, sys
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

EXE = '/app/.fiesta_re/server/5ZoneServer2.exe'
pe  = pefile.PE(EXE, fast_load=True)
img = pe.OPTIONAL_HEADER.ImageBase

text = next(s for s in pe.sections
            if s.Name.rstrip(b'\x00\x20').decode(errors='ignore') == '.text')
text_va    = img + text.VirtualAddress
text_data  = text.get_data()
text_lo    = text_va
text_hi    = text_va + text.Misc_VirtualSize
print(f'.text  {text_lo:#010x}..{text_hi:#010x}  ({len(text_data):#x} bytes)',
      file=sys.stderr)

md = Cs(CS_ARCH_X86, CS_MODE_32)
md.detail = True

# Linear sweep — disassemble every byte offset (we'll get a lot of
# noise but we only care about valid 5-instruction windows).
# To make this tractable, walk each function start one at a time:
# we know functions begin with `push ebp; mov ebp, esp` or the like.
# But the simpler approach: keep a list of disassembled instructions
# starting at every byte we encounter as the START of a known basic
# block. As an easy first pass, just sweep linearly and disassemble
# until invalid. Capstone is fast enough.
#
# Concretely: starting at offset 0, disassemble forward. Whenever
# we hit invalid bytes, advance by 1 and try again. Cache the
# instruction list so we don't redo work.

# Faster approach: scan for the BYTE PREFIX of the trailing call to
# pft_Store. Since pft_Store has only a few specializations and each
# is a single fixed RVA, scan for `E8 rel32` where rel32 lands on one
# of the 6 hot targets we already identified (pre-filter). Then
# disassemble backwards from there.

HOT_TARGETS = {0x001d83c0, 0x00014220, 0x000b2370,
               0x001d8410, 0x0018d810, 0x001ef230}

# Find every E8 in .text whose rel32 lands on a hot target.
candidates = []
for i in range(len(text_data) - 5):
    if text_data[i] != 0xE8:
        continue
    rel = struct.unpack_from('<i', text_data, i + 1)[0]
    target = (text.VirtualAddress + i + 5 + rel) & 0xFFFFFFFF
    if target in HOT_TARGETS:
        candidates.append((i, target))
print(f'Found {len(candidates)} call-instruction candidates targeting '
      f'pft_Store specializations', file=sys.stderr)

# For each candidate call, walk backwards instruction-by-instruction
# until we have collected: a `mov ecx, imm32` and three preceding
# `push imm` instructions.
def walk_back(call_off):
    """Return (handler, arg2, opcode) or None.
    Disassemble the WINDOW before call_off and find the chain.
    """
    WINDOW = 32
    start = max(0, call_off - WINDOW)
    insns = list(md.disasm(text_data[start:call_off], text_va + start))
    if not insns:
        return None
    # Walk backwards from the last instruction in the window. The
    # very last decoded instruction should END at call_off (i.e.
    # immediately precede the call). If it doesn't, our window
    # alignment slipped — try shrinking and re-disassembling.
    # Alignment: ensure the last instruction's address+size == call_off
    if insns[-1].address + insns[-1].size != text_va + call_off:
        # Try realigning: walk start forward by 1..16 and re-dis.
        for align in range(1, 16):
            insns = list(md.disasm(
                text_data[start + align:call_off], text_va + start + align))
            if insns and insns[-1].address + insns[-1].size == text_va + call_off:
                break
        else:
            return None
    # Last → first
    rev = list(reversed(insns))
    # Expect: mov ecx, imm32  →  push imm  →  push imm  →  push imm
    if not rev:
        return None
    if rev[0].mnemonic != 'mov' or len(rev[0].operands) < 2:
        return None
    op0, op1 = rev[0].operands[0], rev[0].operands[1]
    # this-pointer mov
    if op0.type != 1 or op0.reg != 22:  # X86_OP_REG ECX -> reg #22 in capstone
        # Different capstone reg numbering — just check op_str.
        if 'ecx' not in rev[0].op_str:
            return None
    # Now expect three preceding pushes
    pushes = []
    for ins in rev[1:]:
        if ins.mnemonic != 'push':
            break
        if len(ins.operands) != 1:
            break
        opnd = ins.operands[0]
        # type 2 = X86_OP_IMM
        if opnd.type != 2:
            return None
        pushes.append(opnd.imm & 0xFFFFFFFF)
        if len(pushes) == 3:
            break
    if len(pushes) != 3:
        return None
    # Stack order — push reverses the user-visible arg list.
    # Three pushes (in code order): push arg3, push arg2, push arg1.
    # rev order is reverse-emission, so:
    #   pushes[0] = arg1 (opcode)
    #   pushes[1] = arg2
    #   pushes[2] = arg3 (handler)
    return (pushes[2], pushes[1], pushes[0])

results = []
for off, target in candidates:
    r = walk_back(off)
    if r is None:
        continue
    handler, arg2, opcode = r
    if opcode > 0xFFFF:
        continue
    if not (text_lo <= img + handler < text_hi) and not (text_lo <= handler < text_hi):
        # handler should be in .text range — accept either RVA or VA form
        continue
    # Normalize to RVA
    handler_rva = handler if handler < img else handler - img
    results.append((off, handler_rva, opcode, arg2, target))

print(f'Recovered {len(results)} (handler, opcode) pairs after filter',
      file=sys.stderr)

with open('/tmp/pft_store_pairs.tsv', 'w') as out:
    out.write('callsite_rva\thandler_rva\topcode\targ2\tpft_target_rva\n')
    for off, handler, opcode, arg2, target in results:
        out.write(f'{text.VirtualAddress + off:#010x}\t{handler:#010x}\t'
                  f'{opcode:#06x}\t{arg2:#010x}\t{target:#010x}\n')

# Quick sanity: distribution of opcode values
import collections
op_dist = collections.Counter()
for _, _, opcode, _, _ in results:
    if opcode <= 0xFF:        op_dist['<= 0xFF (u8)'] += 1
    elif opcode <= 0x7FFF:    op_dist['0x100-0x7FFF'] += 1
    else:                     op_dist['>= 0x8000'] += 1
print('\nOpcode value distribution:', file=sys.stderr)
for k, v in op_dist.most_common():
    print(f'  {k}: {v}', file=sys.stderr)

print('\nFirst 20 recovered pairs:', file=sys.stderr)
for r in results[:20]:
    print('  call={:#x}  handler={:#x}  opcode={:#06x}  arg2={:#x}  target={:#x}'.format(*r),
          file=sys.stderr)
