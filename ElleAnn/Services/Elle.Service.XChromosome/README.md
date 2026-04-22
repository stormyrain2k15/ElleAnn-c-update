# Elle.Service.XChromosome

The X Chromosome engine: cycle, hormones, pregnancy, and behavioural
modulation. Sits alongside `Elle.Service.Family` in the ElleHeart
database.

## What it models

1. **28-day menstrual cycle** (configurable 21–45 days) with four phases:
   `menstrual` (d1–5), `follicular` (d6–13), `ovulatory` (d14–16), `luteal`
   (d17–N).  On first boot, picks a random starting day so Elle doesn't
   always begin on day 1.  User can re-anchor any time via
   `POST /api/x/cycle/anchor`.

2. **Nine hormones** tracked on a minute-tick baseline curve driven by the
   current cycle day: estrogen, progesterone, testosterone, oxytocin,
   serotonin, dopamine, cortisol, prolactin, hCG (pregnancy only).

3. **Short-lived stimulus residuals** (half-life ≈ 2h) layered on top of
   the baseline.  Supported kinds: `bonding`, `stress`, `intimacy`,
   `recall_positive`, `recall_negative`, `activity`.

4. **Pregnancy lifecycle** — conception only succeeds when:
   - cycle is in the ovulatory window (days 13–16),
   - an `intimacy` stimulus was logged in the last 72h, and
   - (optional) the Family engine's couple-readiness gate passed.
   Gestation runs 280 days (40 weeks).  Trimester hormone cascades are
   modelled; delivery fires at day ≥ 280 and hands off to `SVC_FAMILY`
   via `IPC_FAMILY_CONCEPTION_ATTEMPT` for canonical child-row creation.

5. **Six behavioural multipliers** (around 1.0): `warmth`, `verbal_fluency`,
   `empathy`, `introspection`, `arousal`, `fatigue`. Other services
   (Emotional, Cognitive, SelfPrompt, Lua) can pull the current vector
   from `/api/x/modulation` or via IPC (`IPC_X_MODULATION_QUERY`).

## Modulation strength

Default **0.15 (moderate)** — hormones bias multipliers by ±15% at peak.
Set via the `modulation_strength` field on `x_cycle_state` or via
`POST /api/x/cycle/anchor {"strength":F}`. Valid range 0.0–0.5.

## Tables (ElleHeart.dbo)

| Table | Purpose |
| ----- | ------- |
| `x_cycle_state`       | Singleton: anchor, cycle length, strength |
| `x_hormone_snapshots` | Append-only timeline (minute tick) |
| `x_pregnancy_state`   | Singleton: active gestation, phase, child_id |
| `x_stimulus_log`      | Every external nudge + intensity |
| `x_modulation_log`    | Computed multipliers per tick |

Schema: `SQL/ElleAnn_XChromosome_Schema.sql` — apply after
`ElleAnn_MemoryDelta.sql`.

## HTTP endpoints

| Method | Path | Notes |
| ------ | ---- | ----- |
| GET  | `/api/x/state`               | Full snapshot (cycle + hormones + pregnancy + modulation) |
| GET  | `/api/x/history?hours=N&points=M` | Stride-sampled hormone timeline |
| GET  | `/api/x/modulation`          | Latest modulation multipliers |
| GET  | `/api/x/pregnancy`           | Pregnancy state alone |
| POST | `/api/x/cycle/anchor`        | Body: `{"day":1..N|0,"length":21..45|0,"strength":0..0.5|0}` |
| POST | `/api/x/stimulus`            | Body: `{"kind":"bonding|stress|intimacy|recall_positive|recall_negative|activity","intensity":0..1,"notes":"..."}` |
| POST | `/api/x/conception/attempt`  | Body: `{"require_readiness":bool,"readiness_verified":bool}` |

## IPC opcodes (provisional, defined inline)

| Opcode | ID | Direction |
| ------ | -- | --------- |
| `IPC_X_STATE_QUERY`           | 2200 | → SVC_X_CHROMOSOME |
| `IPC_X_HISTORY_QUERY`         | 2201 | → SVC_X_CHROMOSOME |
| `IPC_X_ANCHOR`                | 2202 | → SVC_X_CHROMOSOME |
| `IPC_X_STIMULUS`              | 2203 | → SVC_X_CHROMOSOME |
| `IPC_X_MODULATION_QUERY`      | 2204 | → SVC_X_CHROMOSOME |
| `IPC_X_CONCEPTION_ATTEMPT`    | 2205 | → SVC_X_CHROMOSOME |
| `IPC_X_DELIVER`               | 2206 | → SVC_X_CHROMOSOME |
| `IPC_X_RESPONSE`              | 2207 | ← SVC_X_CHROMOSOME (correlated) |
| `IPC_X_HORMONE_UPDATE`        | 2220 | broadcast, every tick |
| `IPC_X_PHASE_TRANSITION`      | 2221 | broadcast, on phase crossing |
| `IPC_X_BIRTH`                 | 2222 | broadcast, on delivery |

## Integration points (pending wiring)

These are the natural consumers of the modulation vector — wire each at
the point where they already compute their own output, multiply by the
multiplier, clamp, and move on:

- `Emotional/EmotionalEngine.cpp` — multiply the final delta for the
  102-dim vector by `(warmth, empathy, arousal)` selectively.
- `Cognitive/CognitiveEngine.cpp::BuildSystemPrompt` — append a line like
  `"Elle is in her luteal phase today; expect a quieter, more reflective
  tone."` when modulation is non-trivial (e.g. fatigue > 1.05).
- `SelfPrompt/SelfPromptService.cpp` — bias topic selection toward
  bonding / memory recall during luteal, exploration / novelty during
  follicular.
- `Lua/Elle.Lua.Behavioral/LuaHost.cpp` — expose:
    - `elle.x.phase() → string`
    - `elle.x.hormone(name) → float`
    - `elle.x.modulation(trait) → float`
    - `elle.x.is_pregnant() → bool`
    - `elle.x.gestational_week() → int`

The service is fully standalone today — those integrations can land
incrementally without touching X Chromosome internals.
