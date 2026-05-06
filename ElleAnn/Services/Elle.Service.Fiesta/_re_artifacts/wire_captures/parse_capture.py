#!/usr/bin/env python3
"""
parse_capture.py — turn text packet capture logs into testable structured data.

Reads the four Port_*.txt logs in this directory and emits a single
parsed_captures.json with one entry per event:

    {
      "port": 56543,
      "timestamp": "2026-05-05 18:55:46",
      "seq": 1,
      "direction": "Outbound" | "Inbound",
      "flag": "016",
      "opcode": 10,            # decimal NETCOMMAND value
      "opcode_hex": "0x000A",
      "payload": "E9 FB 77 ...",  # hex string
      "payload_bytes": 1024,
      "payload_b64": "...",       # base64 for binary use
    }

The output is the canonical fixture for Phase 6a unit tests:
- cipher round-trip validation
- opcode dispatch coverage
- world-model replay regression suite
"""
import re, base64, json, sys
from pathlib import Path
from datetime import datetime

HERE = Path(__file__).resolve().parent

# Match: [5/5/2026 6:55:46 PM]	(1)	<Outbound>	==	016 | 010	HH HH ...
LINE_RE = re.compile(
    r'^\[(?P<ts>[^\]]+)\]\s*\((?P<seq>\d+)\)\s*<(?P<dir>Inbound|Outbound)>\s*==\s*'
    r'(?P<flag>\d{3})\s*\|\s*(?P<opcode>\d{3})\s*(?P<hex>[0-9A-Fa-f ]+)\s*$'
)

def parse_ts(s: str) -> str:
    """5/5/2026 6:55:46 PM  ->  2026-05-05 18:55:46"""
    s = s.strip()
    try:
        dt = datetime.strptime(s, '%m/%d/%Y %I:%M:%S %p')
    except ValueError:
        return s
    return dt.strftime('%Y-%m-%d %H:%M:%S')

def parse_file(path: Path) -> list:
    """Each port log starts with `=== Port: NNNNN ===`. Parse out the port,
       then every subsequent line that matches LINE_RE."""
    events = []
    port = None
    for raw in path.read_text(encoding='utf-8', errors='replace').splitlines():
        if raw.startswith('=== Port:'):
            m = re.search(r'\d+', raw)
            port = int(m.group(0)) if m else None
            continue
        m = LINE_RE.match(raw)
        if not m: continue
        hex_str = m.group('hex').strip()
        # Compact whitespace + clean bytes
        hex_clean = re.sub(r'\s+', ' ', hex_str).strip()
        try:
            blob = bytes.fromhex(hex_clean.replace(' ', ''))
        except ValueError:
            continue
        events.append({
            'port':          port,
            'timestamp':     parse_ts(m.group('ts')),
            'seq':           int(m.group('seq')),
            'direction':     m.group('dir'),
            'flag':          m.group('flag'),
            'opcode':        int(m.group('opcode')),
            'opcode_hex':    f'0x{int(m.group("opcode")):04X}',
            'payload_bytes': len(blob),
            'payload_hex':   hex_clean,
            'payload_b64':   base64.b64encode(blob).decode('ascii'),
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
    print(f'Wrote {out} ({out.stat().st_size:,} bytes, {len(all_events)} total events)')

    # Quick coverage report
    from collections import Counter
    counts = Counter((e['direction'], e['opcode']) for e in all_events)
    inbound  = sorted({op for d,op in counts if d == 'Inbound'})
    outbound = sorted({op for d,op in counts if d == 'Outbound'})
    print(f'\nDistinct opcodes:')
    print(f'  Inbound  ({len(inbound):>3}): {inbound[:20]}{" ..." if len(inbound)>20 else ""}')
    print(f'  Outbound ({len(outbound):>3}): {outbound}')

if __name__ == '__main__':
    main()
