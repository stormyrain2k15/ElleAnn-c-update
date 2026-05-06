#!/usr/bin/env python3
"""
parse_capture.py — turn text packet capture logs into testable structured data.

Supports BOTH historical capture-tool formats the user has produced:

  Format A (older, decimal-byte opcode pair, '===' header):
    === Port: 56543 ===
    [5/5/2026 6:55:46 PM]  (1)  <Outbound>  ==  016 | 010   E9 FB 77 ...

  Format B (newer, hex u16 opcode + optional opcode-name annotation,
            ISO-8601 timestamp, no '===' header, port from filename):
    [2026-05-06 01:54:29.748][Outbound] [5101 | N/A]   E1 36 CB 3A 8B
    [2026-05-06 01:54:29.772][Inbound ] [0C0A | USER_LOGIN_ACK]  01 00 ...

Both formats are normalised to the same JSON record shape:

    {
      "port": 56543,
      "timestamp": "2026-05-05 18:55:46",
      "seq": 1,
      "direction": "Outbound" | "Inbound",
      "format": "A" | "B",
      "opcode_u16": 0x1038,        # logical opcode as u16
      "opcode_hex": "0x1038",
      "opcode_name": "CHAR_LOGIN_ACK" | null,  # only Format B
      "wire_dept":  0x10,          # high byte of opcode_u16
      "wire_subid": 0x38,          # low  byte
      "payload_bytes": 97,
      "payload_hex":   "06 00 00 00 ...",
      "payload_b64":   "BgAAAA...",
    }

Phase 6a finding (see wire_captures/README.md §11): Format-B "Outbound"
opcodes are RANDOMISED (every outbound packet has a unique u16) — the
Shine engine client uses an XOR cipher on the opcode bytes per-packet.
The output preserves the raw observed opcode_u16 so the dispatcher /
calibration tools can model the cipher state. Format-B "Inbound"
opcodes are stable (server→client traffic is plaintext on this build).
"""
import re, base64, json, sys
from pathlib import Path
from datetime import datetime

HERE = Path(__file__).resolve().parent

# ── Format A: legacy decimal-byte capture ─────────────────────────────────
LINE_RE_A = re.compile(
    r'^\[(?P<ts>[^\]]+)\]\s*\((?P<seq>\d+)\)\s*<(?P<dir>Inbound|Outbound)>\s*==\s*'
    r'(?P<flag>\d{3})\s*\|\s*(?P<opcode>\d{3})\s*(?P<hex>[0-9A-Fa-f ]*)\s*$'
)

# ── Format B: ISO-timestamp capture with opcode-name annotation ───────────
LINE_RE_B = re.compile(
    r'^\[(?P<ts>\d{4}-\d{2}-\d{2}\s+\d{2}:\d{2}:\d{2}\.\d+)\]'
    r'\[(?P<dir>Outbound|Inbound )\]\s*'
    r'\[(?P<opcode>[0-9A-Fa-f]{4})\s*\|\s*(?P<name>[^\]]+?)\]\s*'
    r'(?P<hex>[0-9A-Fa-f ]*)\s*$'
)


def parse_ts_a(s: str) -> str:
    """5/5/2026 6:55:46 PM  ->  2026-05-05 18:55:46"""
    try:
        return datetime.strptime(s.strip(), '%m/%d/%Y %I:%M:%S %p') \
                       .strftime('%Y-%m-%d %H:%M:%S')
    except ValueError:
        return s.strip()


def parse_ts_b(s: str) -> str:
    """2026-05-06 01:54:29.748 -> 2026-05-06 01:54:29.748 (already ISO)"""
    return s.strip()


def parse_file(path: Path) -> list:
    """Detect format per-line. Format A files start with '=== Port: ===';
       Format B files don't and we recover the port number from the
       filename (`Port_61496.txt` → 61496)."""
    events = []
    port = None

    # Pre-extract port from filename ("Port_61496.txt" → 61496) as a
    # fallback for Format-B files that have no '===' header.
    fname_match = re.search(r'(\d{4,5})', path.stem)
    fname_port = int(fname_match.group(1)) if fname_match else None

    seq_b = 0  # Format B has no per-line seq column; synthesise
    for raw in path.read_text(encoding='utf-8', errors='replace').splitlines():
        if raw.startswith('=== Port:'):
            m = re.search(r'\d+', raw)
            port = int(m.group(0)) if m else None
            continue

        # Try Format A first.
        m = LINE_RE_A.match(raw)
        if m:
            hex_clean = re.sub(r'\s+', ' ', m.group('hex')).strip()
            try:
                blob = bytes.fromhex(hex_clean.replace(' ', ''))
            except ValueError:
                continue
            flag = int(m.group('flag'))
            sub = int(m.group('opcode'))
            opcode_u16 = (flag << 8) | sub
            events.append({
                'port':         port,
                'timestamp':    parse_ts_a(m.group('ts')),
                'seq':          int(m.group('seq')),
                'direction':    m.group('dir'),
                'format':       'A',
                'opcode_u16':   opcode_u16,
                'opcode_hex':   f'0x{opcode_u16:04X}',
                'opcode_name':  None,
                'wire_dept':    flag,
                'wire_subid':   sub,
                'payload_bytes': len(blob),
                'payload_hex':  hex_clean,
                'payload_b64':  base64.b64encode(blob).decode('ascii'),
                # legacy field names some downstream tools still read:
                'flag':         flag,
                'opcode':       sub,
            })
            continue

        # Try Format B.
        m = LINE_RE_B.match(raw)
        if m:
            seq_b += 1
            hex_clean = re.sub(r'\s+', ' ', m.group('hex')).strip()
            try:
                blob = bytes.fromhex(hex_clean.replace(' ', ''))
            except ValueError:
                continue
            opcode_u16 = int(m.group('opcode'), 16)
            name = m.group('name').strip()
            if name == 'N/A':
                name = None
            direction = m.group('dir').strip()  # rstrip 'Inbound ' → 'Inbound'
            events.append({
                'port':         port if port is not None else fname_port,
                'timestamp':    parse_ts_b(m.group('ts')),
                'seq':          seq_b,
                'direction':    direction,
                'format':       'B',
                'opcode_u16':   opcode_u16,
                'opcode_hex':   f'0x{opcode_u16:04X}',
                'opcode_name':  name,
                'wire_dept':    (opcode_u16 >> 8) & 0xFF,
                'wire_subid':    opcode_u16 & 0xFF,
                'payload_bytes': len(blob),
                'payload_hex':  hex_clean,
                'payload_b64':  base64.b64encode(blob).decode('ascii'),
                # legacy convenience aliases
                'flag':         (opcode_u16 >> 8) & 0xFF,
                'opcode':        opcode_u16 & 0xFF,
            })

    return events


def main():
    all_events = []
    for f in sorted(HERE.glob('Port_*.txt')):
        events = parse_file(f)
        print(f'{f.name}: {len(events)} events')
        all_events.extend(events)

    out = HERE / 'parsed_captures.json'
    json.dump(all_events, out.open('w'), indent=2)
    print(f'Wrote {out} ({out.stat().st_size:,} bytes, '
          f'{len(all_events)} total events)')

    # Quick coverage report.
    from collections import Counter
    counts = Counter((e['direction'], e['opcode_u16'], e.get('format','?'))
                     for e in all_events)

    inbound  = sorted({(op,fmt) for d,op,fmt in counts if d == 'Inbound'})
    outbound = sorted({(op,fmt) for d,op,fmt in counts if d == 'Outbound'})

    # Split by format because Format-B Outbound is randomised.
    inbound_a  = [op for op,fmt in inbound  if fmt == 'A']
    inbound_b  = [op for op,fmt in inbound  if fmt == 'B']
    outbound_a = [op for op,fmt in outbound if fmt == 'A']
    outbound_b = [op for op,fmt in outbound if fmt == 'B']

    print(f'\nDistinct opcode_u16 by direction × format:')
    print(f'  Inbound  fmt A ({len(inbound_a):>4}): '
          f'{[hex(x) for x in inbound_a[:8]]}{" …" if len(inbound_a)>8 else ""}')
    print(f'  Inbound  fmt B ({len(inbound_b):>4}): '
          f'{[hex(x) for x in inbound_b[:8]]}{" …" if len(inbound_b)>8 else ""}')
    print(f'  Outbound fmt A ({len(outbound_a):>4}): '
          f'{[hex(x) for x in outbound_a[:8]]}{" …" if len(outbound_a)>8 else ""}')
    print(f'  Outbound fmt B ({len(outbound_b):>4}): '
          f'{[hex(x) for x in outbound_b[:8]]}{" …" if len(outbound_b)>8 else ""}'
          f'    ← rolling/encrypted opcodes' if outbound_b else '')


if __name__ == '__main__':
    main()
