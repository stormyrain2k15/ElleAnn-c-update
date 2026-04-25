"""
extract_opcodes.py — Ghidra script (Python 2 / Jython).

CRITICAL — load the PATCHED CodeCave EXE the .udd file references:
   5ZoneServer2-Localhost-Port80-NoService-CodeCave-1.exe
…NOT the bare `5ZoneServer2.exe`. The .udd's RVAs were captured
against the CodeCave-modified binary, so loading the un-patched
EXE gives you symbols that point at offsets shifted by the cave.

Equivalent of extract_opcodes_ida.py for Ghidra users — Ghidra is
free, so this is the recommended path if you don't already own IDA.

Usage:
    1. File → Import File → 5ZoneServer2.exe
    2. When prompted to load symbols, point at 5ZoneServer2.pdb
       (or use the "Load PDB File" feature post-import).
    3. Wait for auto-analysis to finish.
    4. Window → Script Manager → New Python script
    5. Paste this file's contents, save as extract_opcodes.py
    6. Run. Output is written to /tmp/shine_opcodes.tsv (Linux) or
       %TEMP%\\shine_opcodes.tsv (Windows).

Algorithm matches the IDA version: find every pft_Store
specialization, walk callers, decode the three preceding `push imm`
instructions, pair the nearest-push (opcode) with the farthest-push
(handler).
"""
# @category    ShineEngine

import os
from ghidra.program.model.symbol import RefType
from ghidra.program.model.lang import OperandType


def is_immediate_push(instr):
    return (instr.getMnemonicString().upper() == 'PUSH' and
            (instr.getOperandType(0) & OperandType.SCALAR) != 0)


def get_imm(instr):
    return instr.getScalar(0).getUnsignedValue()


def find_pft_specializations(prog):
    fm = prog.getFunctionManager()
    out = []
    for fn in fm.getFunctions(True):
        n = fn.getName(True) or ''
        if 'pft_Store' in n:
            out.append(fn)
    return out


def walk_back_three_pushes(call_instr):
    """Walk backwards in instruction order; collect up to three
    PUSH imm values, ignoring an intervening MOV ECX, IMM32."""
    listing = currentProgram.getListing()
    pushes = []
    cur = listing.getInstructionBefore(call_instr.getAddress())
    for _ in range(20):
        if cur is None:
            return None
        mnem = cur.getMnemonicString().upper()
        if mnem == 'MOV':
            cur = listing.getInstructionBefore(cur.getAddress())
            continue
        if mnem != 'PUSH':
            break
        if (cur.getOperandType(0) & OperandType.SCALAR) == 0:
            break
        pushes.append(cur.getScalar(0).getUnsignedValue() & 0xFFFFFFFF)
        if len(pushes) == 3:
            break
        cur = listing.getInstructionBefore(cur.getAddress())
    if len(pushes) != 3:
        return None
    opcode, arg2, handler = pushes
    return (handler, arg2, opcode)


def main():
    prog = currentProgram
    listing = prog.getListing()
    specs = find_pft_specializations(prog)
    print('Found %d pft_Store specializations' % len(specs))

    rows = set()
    for fn in specs:
        for ref in fn.getSymbol().getReferences():
            if not ref.getReferenceType().isCall():
                continue
            instr = listing.getInstructionAt(ref.getFromAddress())
            if instr is None:
                continue
            r = walk_back_three_pushes(instr)
            if r is None:
                continue
            handler, arg2, opcode = r
            if not (0 <= opcode <= 0xFFFF):
                continue
            h_addr = prog.getAddressFactory().getAddress(hex(handler))
            h_sym = prog.getSymbolTable().getPrimarySymbol(h_addr)
            h_name = h_sym.getName(True) if h_sym else 'sub_%x' % handler
            rows.add((opcode, h_name, fn.getName(True),
                      ref.getFromAddress().getOffset()))

    out_path = os.path.join(
        os.environ.get('TEMP') or '/tmp', 'shine_opcodes.tsv')
    with open(out_path, 'w') as f:
        f.write('opcode\thandler\tspecialization\tcallsite\n')
        for opcode, h, s, c in sorted(rows, key=lambda r: r[0]):
            f.write('0x%04x\t%s\t%s\t0x%08x\n' % (opcode, h, s, c))

    print('wrote %d opcodes to %s' % (len(rows), out_path))


main()
