# FIESTA DISASSEMBLED — Full Analysis
# Source: 5ZoneServer2.idb (CN build, NA PDB symbols), LuaScript/, Port captures
# Date: 2026-05-06

---

## 1. Architecture Discovery — The Roe_ System

The damage formula is NOT hardcoded in a single function. ZoneServer2 uses a
**dispatch table** called "Roe" (likely Korean: 로, "rule/law of engagement").

Confirmed string symbols from ZoneServer2.idb:

```
Roe_calcdamage      -- main damage calculation entry
Roe_getattack       -- compile attacker's ATK value
Roe_defendpower     -- compile defender's DEF value
Roe_hitrate         -- hit/miss check
Roe_isdamageincrease-- check if damage has a bonus multiplier
Roe_criticalrate    -- critical hit check
```

Two dispatch tables (vtable-style structs), one per physical attack type:

```
normalpyRoe   -- "normal physical" Roe table
  .calc       -- normalpyRoe_calc
  .damage     -- normalpyRoe_damage
  .hitrate    -- normalpyRoe_hitrate
  .isdmginc   -- normalpyRoe_isdamageincrease

normalmaRoe   -- "normal magic" Roe table
  .calc, .damage, .hitrate, .isdmginc  (same shape)
```

The C++ pipeline calls into the appropriate Roe table based on skill EffectType
(SE_PHYSICDAMAGE vs SE_MAGICDAMAGE from ActiveSkill.shn).

---

## 2. Key Variable: lastdmgratio

String `aLastdmgretio` = "lastdmgratio" (typo of "ratio") appears in ZoneServer2.

This variable persists a ratio value after damage calculation. A ratio only makes
sense in the formula if computation involves division:

  Candidate A: nRaw = ATK - DEF          → no ratio produced
  Candidate B: nRaw = ATK² / (ATK + DEF) → ratio = ATK / (ATK + DEF) is computed

`lastdmgratio` is consistent with Candidate B. The ratio term ATK/(ATK+DEF) would
naturally be stored as an intermediate. This supports B over A but does not prove it.

Also found: `a_attackpowercalc` = dedicated "attackpowercalc" function.
This suggests ATK is not a raw number but is itself compiled by a sub-function before
being passed to the damage formula, consistent with BuildBattleStat's role.

---

## 3. Lua 5.2 Integration

ZoneServer2 embeds Lua 5.2. References: `aLua5_2`, `aLuaopen_S`, `aLuascriptscena`.

Lua scripts are in Server/9Data/Shine/LuaScript/ — dungeon/KQ/AI scripts.
These scripts call C bindings for scripted/event damage:

  cDamaged(target, amount, attacker) -- flat scripted damage, bypasses formula
  cStaticDamage(target, amount)      -- static damage (poison floor etc)
  cSetAbstate / cResetAbstate        -- buff/debuff system
  cPermileRate(rate)                 -- per-mille (÷1000) probability check

The Roe_ functions are C++ combat functions, NOT exposed to Lua. Normal
player-vs-mob combat goes entirely through C++ Roe_ dispatch. Lua is only
for scripted boss mechanics and dungeon events.

---

## 4. Packet 0x1035 — Stat Update Tag Map

Format: [count:1] then count × [tag:1][value:4] pairs (little-endian DWORD)

Confirmed tags from ElleAnn (Lv60 Sentinel) login packet:

  0x00 = STR           307
  0x01 = CON           322
  0x02 = DEX           290
  0x03 = INT           306
  0x04 = (missing / WIS?)
  0x05 = MEN           290
  0x06 = HP current  13406
  0x07 = HP max      16552
  0x08 = SP current   7152
  0x09 = SP max      39251
  0x0A = ???           282  (MA? WC? TH?)
  0x0B = ???           292  (WC? MA?)
  0x0C = ???           292  (AC? same as 0x0B, suspicious)
  0x0D = ???          1200  (likely SP or LP pool)

Tags 0x10-0x1B appear in a secondary block — likely soul/buff related:
  0x10 = 4037, 0x11 = 4231, 0x13 = 77, 0x19 = 77, 0x1A = 77

NOTE: 0x0B and 0x0C are identical (292). This might be WC_min and WC_max for a
basic weapon, or current/max of the same stat. Cross-reference with ItemInfo
weapon MinWC/MaxWC to confirm.

---

## 5. No Combat Packets in Captures

Port 9120.txt and Port 61496.txt are login/movement sessions only.
Opcode 0x2018 (4737 occurrences) = position/HP entity update (21 bytes, very frequent).
No skill-use or damage packets identified.

To get actual combat data, need a capture session where ElleAnn attacks a mob
with known stats and observed damage numbers.

---

## 6. CalcRawDamage — Current Assessment

**STILL WIP/PENDING PROOF.** Evidence leans toward Candidate B:

  B: nRaw = ATK * ATK / (ATK + DEF)

Reasons favoring B:
- `lastdmgratio` = a ratio intermediate is stored (no ratio in A)
- Common pattern in Shine Engine variants (confirmed in some open-source forks)
- Produces diminishing returns which matches observed gameplay feel

Reasons still uncertain:
- `lastdmgratio` could also be the angle/soul/levelgap rate stored for logging
- Have not seen actual assembly code confirming the arithmetic operations
- Cannot read 5ZoneServer2.idb directly without IDA Pro

**What would confirm it:**
  Take ElleAnn (WC=292 from packet) attacking Slime (AC=3 from MobInfoServer.shn)
  Expected damage A = 292 - 3 = 289
  Expected damage B = 292² / (292 + 3) = 85264 / 295 ≈ 289

These are close at low ATK:DEF ratios! The formulas diverge at high DEF:
  ATK=292, DEF=100 → A: 192   B: 292²/392 ≈ 217
  ATK=292, DEF=200 → A:  92   B: 292²/492 ≈ 173

We need packet captures with higher-DEF targets to distinguish them.

---

## 7. Files Available But Unreadable

  5ZoneServer2.idb  (32MB) — IDA database, requires IDA Pro to open
  WorldManager.pdb  — PDB symbols for WorldManager
  Fiesta.pdb        — Client PDB
  fiesta server     — pcap with no combat captured

The .idb would contain full decompiled pseudocode for CalcRawDamage if opened.
