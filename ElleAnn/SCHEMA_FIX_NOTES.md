> **HISTORICAL — superseded by [`AUDIT_SCOREBOARD.md`](AUDIT_SCOREBOARD.md)**.
>
> This file is kept for archaeological reference only. It documents a
> past audit wave and may describe the codebase as it was at the time
> of writing, not as it is now. For the current, line-by-line audit
> status grounded in the present tree, read `AUDIT_SCOREBOARD.md`.

---

# SCHEMA FIX — Why Elle Was Forgetting

## Root Cause

Your live `ElleCore` database was built by a **different earlier project**
(snake_case tables: `messages`, `conversations`, `users`, `CrystalProfile`,
`ElleThreads`, `UserPresence`, etc.).

Our C++ code was writing to `ElleCore.dbo.Messages` (PascalCase) and
`ElleMemory.dbo.Memories` — tables that **do not exist**.

Every `StoreMessage`, `StoreMemory`, `GetConversationHistory` call threw a
SQL error that got swallowed by `catch(...)`. Silent failure → zero memory.

## The Fix (two steps)

### 1. Run the new SQL delta on your SQL Server

File: `SQL/ElleAnn_MemoryDelta.sql`

Open in SSMS, make sure the dropdown at the top says **`ElleCore`**, press **F5**.

It creates 4 new snake_case tables (`memory`, `memory_tags`,
`world_entity`, `memory_entity_links`) without touching any of the existing
tables. Also seeds a default user(id=1) and conversation(id=1) so C++ writes
with a fallback `conversation_id` won't fail on FK.

You can re-run the script any time; everything is `IF NOT EXISTS`-guarded.

### 2. Pull + rebuild `ElleCore.Shared`

`Shared/ElleSQLConn.cpp` was rewritten so every read/write now targets the
actual live schema. Key changes:

| Function | Now writes to |
|---|---|
| `StoreMessage` | `ElleCore.dbo.messages` (maps role int → nvarchar, fills `user_id` from `conversations`, sets `Direction`, updates `conversations.total_messages`) |
| `GetConversationHistory` | `ElleCore.dbo.messages` |
| `StoreMemory` | `ElleCore.dbo.memory` + `memory_tags` + auto-links to `world_entity` when a tag matches an existing entity name |
| `RecallMemories` | 3-way union: tag exact match ∪ content/summary LIKE ∪ linked entity → ranked by `importance × 0.4 + recency_decay × 0.4 + log(access_count) × 0.2`; also bumps access_count on recall |
| `GetEntity` | `ElleCore.dbo.world_entity` (case-insensitive by name) |
| `StoreEntity` | MERGE upsert on `ElleCore.dbo.world_entity` |
| `UpdateEntityInteraction` | bumps `interaction_count` + `familiarity` |

Only `ElleCore.Shared` changed. But since any service linking to it must be
rebuilt: Rebuild Solution is safest (Visual Studio → Build → Rebuild Solution).

## Verification

After rebuild + restart:

```bat
rem Turn 1
curl -X POST http://localhost:8000/api/ai/chat -H "Content-Type: application/json" ^
  -d "{\"message\":\"Hey Elle, it's Bug. My favorite color is green.\",\"conversation_id\":99}"

rem Turn 2 (same conv_id)
curl -X POST http://localhost:8000/api/ai/chat -H "Content-Type: application/json" ^
  -d "{\"message\":\"What's my favorite color?\",\"conversation_id\":99}"
```

**Success looks like:**
- Turn 2 response says "green"
- Cognitive console line:
  `Chat reply rid=... conv=99 mode=companion memories=N entities=1 in ~1500ms`
  (N should be ≥ 1 — this is the memory from turn 1 being recalled)
- SSMS check:
  ```sql
  USE ElleCore;
  SELECT TOP 10 id, conversation_id, role, LEFT(content, 80) FROM dbo.messages ORDER BY id DESC;
  SELECT TOP 10 id, LEFT(content, 80), importance, emotional_valence FROM dbo.memory ORDER BY id DESC;
  SELECT id, name, entity_type, interaction_count FROM dbo.world_entity ORDER BY last_interaction_ms DESC;
  SELECT mt.tag, mt.memory_id FROM dbo.memory_tags mt ORDER BY mt.id DESC;
  ```
  All four should have rows after a chat turn.

## If something's off

- Turn 2 still doesn't know → **Did you run the SQL delta?** That's the single
  most common cause. The C++ code can target the new tables all day, but if
  they don't exist in SQL, writes silently fail.
- Curl returns `{"error":"Cognitive timeout"}` → Cognitive service isn't
  receiving IPC. Check `Heartbeat` summary for `HEALTHY`.
- Curl returns `{"model":..., "response":...}` only (old 2-key shape) →
  you're running the old HTTPServer binary. Rebuild `Elle.Service.HTTP` too.
