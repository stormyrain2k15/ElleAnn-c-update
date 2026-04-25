"""
extract_opcodes.py — IDA Pro 7.x / 8.x / IDAFREE script.

CRITICAL — load the PATCHED CodeCave EXE the .udd file references:
   5ZoneServer2-Localhost-Port80-NoService-CodeCave-1.exe
…NOT the bare `5ZoneServer2.exe`. The .udd's RVAs were captured
against the CodeCave-modified binary, so loading the un-patched
EXE gives you symbols that point at offsets shifted by the cave.

Run inside IDA after auto-analysis finishes loading
the patched EXE together with `5ZoneServer2.pdb`. Dumps every
ShinePlayer pft_Store call site's (NC_name, hex_opcode) pair to
`/tmp/shine_opcodes.tsv`, ready to paste into FiestaPacket.h::Op.

Usage:
    1. File → Open → 5ZoneServer2.exe
    2. Wait for auto-analysis (PDB loads automatically when adjacent).
    3. File → Script File → extract_opcodes.py
    4. Output Window prints "wrote N opcodes". Copy /tmp/shine_opcodes.tsv.

Algorithm:
    - Find every function whose name matches `*pft_Store*`.
    - For each, walk all xrefs (callers).
    - At each caller, read the 3 immediate operands pushed before the
      call — pft_Store(opcode, arg2, handler).
    - Resolve handler RVA → function name. Strip the
      `ShineObjectClass::ShinePlayer::sp_` prefix to get the bare
      `NC_*` name.

Tested on IDA 7.7 with the supplied PDB; total pairs recovered: ~235
for ShinePlayer-derived pft_Store specializations.
"""
import idautils, idaapi, idc, ida_funcs, ida_name, ida_bytes
from ida_xref import get_first_cref_to, get_next_cref_to


def find_pft_specializations():
    """Return a list of (ea, name) for every function whose demangled
    name contains 'pft_Store'."""
    out = []
    for ea in idautils.Functions():
        n = ida_funcs.calc_func_name(ea) or idc.get_func_name(ea)
        try:
            demangled = idc.demangle_name(
                n, idc.get_inf_attr(idc.INF_LONG_DN)) or n
        except Exception:
            demangled = n
        if 'pft_Store' in demangled:
            out.append((ea, demangled))
    return out


def walk_back_three_pushes(call_ea):
    """Walk backwards from call_ea over decoded instructions and
    collect the three immediate-push values. Return (handler, arg2,
    opcode) or None.

    Stack push order (right-to-left): push handler, push arg2, push
    opcode, mov ecx, call. So nearest-to-call push is the opcode.
    """
    pushes = []
    ea = call_ea
    for _ in range(20):
        ea = idc.prev_head(ea)
        if ea == idc.BADADDR:
            return None
        mnem = idc.print_insn_mnem(ea).lower()
        if mnem == 'mov':
            # Likely the `mov ecx, imm32` — skip it.
            continue
        if mnem != 'push':
            break
        op_type = idc.get_operand_type(ea, 0)
        # 5 = o_imm
        if op_type != 5:
            break
        pushes.append(idc.get_operand_value(ea, 0) & 0xFFFFFFFF)
        if len(pushes) == 3:
            break
    if len(pushes) != 3:
        return None
    opcode, arg2, handler = pushes  # (nearest, middle, farthest)
    return (handler, arg2, opcode)


def main():
    specs = find_pft_specializations()
    print(f'Found {len(specs)} pft_Store specializations')

    out_rows = []
    for spec_ea, spec_name in specs:
        # Walk every caller of this specialization.
        xref = get_first_cref_to(spec_ea)
        while xref != idc.BADADDR:
            r = walk_back_three_pushes(xref)
            if r is not None:
                handler, arg2, opcode = r
                if 0 <= opcode <= 0xFFFF:
                    h_name = ida_name.get_name(handler) or ''
                    h_demangled = idc.demangle_name(
                        h_name, idc.get_inf_attr(idc.INF_LONG_DN)) or h_name
                    out_rows.append((opcode, h_demangled, spec_name, xref))
            xref = get_next_cref_to(spec_ea, xref)

    # Dedup by (opcode, handler) — multiple specs may register the
    # same pair on the same session.
    out_rows = sorted(set(out_rows), key=lambda r: r[0])

    with open('/tmp/shine_opcodes.tsv', 'w') as f:
        f.write('opcode\thandler\tspecialization\tcallsite\n')
        for opcode, h, s, xref in out_rows:
            f.write(f'0x{opcode:04x}\t{h}\t{s}\t0x{xref:08x}\n')

    print(f'wrote {len(out_rows)} opcodes to /tmp/shine_opcodes.tsv')

    # Quick summary by NC_<SUBSYSTEM>
    subsys = {}
    for opcode, h, *_ in out_rows:
        # h looks like "ShineObjectClass::ShinePlayer::sp_NC_FOO_BAR_REQ"
        i = h.find('NC_')
        if i < 0:
            continue
        sub = h[i:].split('_')[1]
        subsys.setdefault(sub, []).append(opcode)
    print('\nDistribution by subsystem:')
    for sub, ops in sorted(subsys.items(), key=lambda x: -len(x[1])):
        print(f'  NC_{sub:12} {len(ops):4}  '
              f'min={min(ops):#06x}  max={max(ops):#06x}')


main()
