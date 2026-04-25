"""Final pft_Store extraction — pattern decoded:

  push <handler_VA>        ; 68 ?? ?? ?? ??     (5 bytes)
  push <arg2>              ; 6A ??  OR  68 ?? ?? ?? ??
  push <arg1>              ; 6A ??  OR  68 ?? ?? ?? ??
  mov  ecx, <reg>          ; 8B C? (this-ptr from register, e.g. esi)
                           ; OR mov ecx, imm32 (8B E1, B9 ...)
  call <pft_Store_rel32>   ; E8 ?? ?? ?? ??

ShineEngine pft_Store(int category, int command, MemberFnPtr handler)
— two ints encode the opcode (canonically `(arg1 << 8) | arg2` in the
NC namespace, e.g. NC_BAT_HIT_REQ = (9 << 8) | 3 = 0x903).
The full 16-bit opcode the wire uses combines both.
"""
import struct, sys, re, collections
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM, CS_OP_REG

EXE = '/app/.fiesta_re/server/5ZoneServer2-Port80-NoService.exe'
UDD = '/app/.fiesta_re/server/5ZoneServer2.udd'

# 1. Load udd records → handler_VA → name
print('Loading .udd...', file=sys.stderr)
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
    if blob[i] != 0x0a:
        i += 1; continue
    if not all(0x20 <= c < 0x7F for c in blob[i+1:i+4]):
        i += 1; continue
    tag = blob[i+1:i+4].decode('latin1', errors='replace')
    dl  = struct.unpack_from('<I', blob, i+4)[0]
    if dl > 0x10000 or i + 8 + dl > len(blob):
        i += 1; continue
    if tag == 'Us4' and dl >= 5:
        rva  = struct.unpack_from('<I', blob, i+8)[0]
        name = blob[i+12: i+8+dl].split(b'\x00', 1)[0]
        try:
            n = name.decode('latin1')
        except Exception:
            n = ''
        if n:
            handler_by_rva[rva] = n
    i += 8 + dl

print(f'Total Us4 handlers in udd: {len(handler_by_rva)}', file=sys.stderr)

# 2. Load EXE
pe   = pefile.PE(EXE, fast_load=True)
img  = pe.OPTIONAL_HEADER.ImageBase
text = next(s for s in pe.sections if s.Name.startswith(b'.text'))
text_va = img + text.VirtualAddress
text_data = text.get_data()
print(f'.text VA={text_va:#x} size={len(text_data):#x} ImageBase={img:#x}',
      file=sys.stderr)

handler_va_to_name = {img + r: n for r, n in handler_by_rva.items()}

# 3. Linear-scan .text for `push imm32` matching a handler VA, then
# walk forward.
md = Cs(CS_ARCH_X86, CS_MODE_32)
md.detail = True

results = []  # (arg1, arg2, name, handler_va, callsite_va, call_target)

# Track call_target counts so we can print the dominant pft_Store
# specialization — it's the same for every ShinePlayer pft_Store call.
call_target_count = collections.Counter()

i = 0
N = len(text_data)
while i + 5 <= N:
    if text_data[i] != 0x68:
        i += 1; continue
    handler_imm = struct.unpack_from('<I', text_data, i + 1)[0]
    if handler_imm not in handler_va_to_name:
        i += 1; continue
    # Decode forward up to ~8 instructions
    insns = list(md.disasm(text_data[i:i + 50], text_va + i))[:8]
    if len(insns) < 4:
        i += 1; continue
    # First insn is `push handler_imm` — confirm.
    if insns[0].mnemonic != 'push':
        i += 1; continue
    # Walk forward collecting up to 2 immediate pushes (arg2 and arg1),
    # then a `mov ecx, ...` (any source), then a `call rel32`.
    arg_pushes = []
    saw_mov_ecx = False
    call_target = None
    callsite = None
    for ins in insns[1:]:
        m = ins.mnemonic.lower()
        if m == 'push' and not saw_mov_ecx:
            if (len(ins.operands) == 1 and ins.operands[0].type == CS_OP_IMM):
                arg_pushes.append(ins.operands[0].imm & 0xFFFFFFFF)
                if len(arg_pushes) > 2:
                    break
            else:
                break
        elif m == 'mov' and 'ecx' in ins.op_str.split(',')[0]:
            saw_mov_ecx = True
        elif m == 'call' and saw_mov_ecx:
            if (len(ins.operands) == 1
                    and ins.operands[0].type == CS_OP_IMM):
                call_target = ins.operands[0].imm & 0xFFFFFFFF
                callsite = ins.address
            break
        else:
            break
    if call_target is None or len(arg_pushes) != 2:
        i += 1; continue
    arg1, arg2 = arg_pushes  # in code order: arg2 first then arg1 (opcode-side)
    # Filter sanity: opcode bytes are u8 each in observed pattern
    if arg1 > 0xFFFF or arg2 > 0xFFFF:
        i += 1; continue
    name = handler_va_to_name[handler_imm]
    call_target_count[call_target] += 1
    results.append((arg1, arg2, name, handler_imm, callsite, call_target))
    i += 1

print(f'\nRecovered {len(results)} pft_Store call sites', file=sys.stderr)
print('\nDominant call targets (pft_Store specializations):', file=sys.stderr)
for tgt, cnt in call_target_count.most_common(8):
    print(f'  {tgt:#010x}  hits={cnt}', file=sys.stderr)

# Per-session dominant target
sess_to_tgt = {}
for arg1, arg2, name, hva, cs, tgt in results:
    sess = (
        'ShinePlayer' if 'ShinePlayer' in name else
        'GameDBSession' if 'GameDBSession' in name else
        'WorldManagerSession' if 'WorldManagerSession' in name else
        'GameLogSession' if 'GameLogSession' in name else
        'ZoneListenSession' if 'ZoneListenSession' in name else
        'OPToolSession' if 'OPToolSession' in name else
        'other'
    )
    sess_to_tgt.setdefault(sess, collections.Counter())[tgt] += 1

print('\nPer-session class call targets:', file=sys.stderr)
for s, c in sess_to_tgt.items():
    print(f'  {s}: {dict(c.most_common(3))}', file=sys.stderr)

# Filter to ShinePlayer-only and emit the canonical opcode mapping
sp = [(arg1, arg2, name, hva, cs)
      for arg1, arg2, name, hva, cs, tgt in results
      if 'ShinePlayer' in name]
print(f'\nShinePlayer pairs: {len(sp)}', file=sys.stderr)

# Hypothesis: full opcode = (arg1 << 8) | arg2  OR  (arg2 << 8) | arg1.
# The user's NC_<MAJOR>_<NAME>_<TYPE> convention groups ops by major
# subsystem; arg1 should be CONSTANT inside one subsystem if our
# byte-ordering is right.
sub_to_arg1 = collections.defaultdict(set)
sub_to_arg2 = collections.defaultdict(set)
for arg1, arg2, name, *_ in sp:
    m = re.search(r'NC_([A-Z]+)_', name)
    if m:
        sub_to_arg1[m.group(1)].add(arg1)
        sub_to_arg2[m.group(1)].add(arg2)

print('\nIs arg1 constant per NC_<SUBSYS>?:', file=sys.stderr)
for s, a1s in sorted(sub_to_arg1.items()):
    print(f'  NC_{s:12} arg1 set: {sorted(hex(x) for x in a1s)}',
          file=sys.stderr)

# Save TSV
with open('/tmp/shine_opcodes.tsv', 'w') as out:
    out.write('arg1\targ2\tcombined_be\tcombined_le\tname\thandler_va\tcallsite\n')
    for arg1, arg2, name, hva, cs in sp:
        be = (arg1 << 8) | arg2
        le = (arg2 << 8) | arg1
        out.write(f'{arg1:#06x}\t{arg2:#06x}\t{be:#06x}\t{le:#06x}\t'
                  f'{name}\t{hva:#010x}\t{cs:#010x}\n')

print('\nFirst 30 ShinePlayer (arg1, arg2, name) tuples:',
      file=sys.stderr)
for arg1, arg2, name, *_ in sorted(sp)[:30]:
    short = name.replace('ShineObjectClass::ShinePlayer::sp_', '')
    print(f'  arg1={arg1:#04x} arg2={arg2:#04x}  {short}', file=sys.stderr)
