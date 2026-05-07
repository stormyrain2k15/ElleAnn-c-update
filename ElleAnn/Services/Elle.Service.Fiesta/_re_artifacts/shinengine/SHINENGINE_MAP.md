# ShineEngine → Elle WorldModel Map
_Phase 6b cross-reference — written 2026-02 during ShineEngine source ingest._

## 1. What's in `_re_artifacts/shinengine/`

Two zips shipped by the user in the final 6a session and extracted into
this folder verbatim:

```
Battle_src/        — server-side damage pipeline (9 files, ~30 KB)
ProjectF2_src/     — client + server protocol skeleton (36 files, ~185 KB)
shinengine_srctree.html  — full file-index from the original repo
```

Inspection revealed the zips are a **consolidated reference skeleton**,
assembled from the same PDB symbols and packet captures we extracted
during Phase 6a (every header calls them out in its provenance comment:
_"Source: E:\ProjectF2\CSCode\…"_, _"Confirmed from Port 9110 captures"_,
etc.). In other words: same schema as our tree, cleaner headers, many
`TODO — packet capture pending` placeholders.

## 2. Header → Elle type map

| ShineEngine header (ProjectF2_src) | Elle counterpart (Services/Elle.Service.Fiesta/) | Notes |
|---|---|---|
| `FiestaCipher.h` (LCG 0x4E6D/0x5249) | `FiestaCipher.h` + `FiestaNetCrypto.h` | Ours is superset: supports LCG **and** XOR499 via `SetKind()`. LAN-bypass heuristic identical. |
| `PacketEncrypt.h` (class `CPacketEncrypt`) | Same cipher core as above | Duplicate skeleton — ours exposes the same `Init/Cipher/CipherOpCode` surface but as a C++17 class inside namespace Fiesta. |
| `NCProtocol.h` (opcode `#define`s) | `FiestaPacket.h` + `Generated/FiestaProtoTable.inc` | Ours is 2 687-entry table auto-generated from `PDB_OPCODES.json`; ShineEngine skeleton only lists the 25 we confirmed from captures. |
| `GPacket.h` (`PROTO_NC_HEADER` + `GPACKET`) | `FiestaPacket.h` `struct InPacket` / `OutPacket` | Same wire header shape: `[u16 wLen][u16 wOpCode][…]`. Ours adds cleartext-prefix variant (sizeFlag byte). |
| `protocommon.h` (`NAME_LENGTH=16`, etc.) | Same constants baked into `FiestaProtoBase.h` | Field widths identical. |
| `protouser.h` (login-chain structs) | `PROTO_NC_USER_*` in `FiestaPacket.h` | Both describe the same 0C01/0C03/0C06/0C0A/0C0B/0C0C packets. |
| `protochar.h` (WM handshake + option CMDs) | `PROTO_NC_USER_WILLLOGIN_ACK`, `PROTO_NC_CHAR_OPTION_*` | ShineEngine header confirms our inferred 200F = `NC_WM_HANDSHAKE_REQ` and 7032/33/34 option command structure. |
| `protomap.h` (`ProtoNcMapLoginAck` w/ HP/MP/Gold/Exp) | `FiestaDecoders.h` — **fields missing** | Phase 6b-Beta target: decode HP/MP/Gold/Exp from 0x1038 (`NC_MAP_LOGIN_ACK`) per the ShineEngine layout. |
| `protomisc.h` (`ProtoNcMiscGameTimeAck` HH:MM:SS) | Not yet in `FiestaDecoders.h` | Add as trivial 3-byte decoder in Phase 6b-Beta. |
| `pf_client.cpp` (Zone dispatch skeleton) | `Client::HandlePacket` in `FiestaClient.cpp` | Same switch-on-opcode pattern. ShineEngine skeleton is all `TODO /* packet capture pending */`; ours is 7 live handlers + 1 WIP. |
| `pf_worldmanager.cpp` (WM-Zone dispatch) | Routed through `Client::OnWillLoginAck` + `On*OptionCmd` | We don't run a WM peer — we **are** the client. But the field names in the skeleton confirm our struct layouts. |
| `Socket_IOCP.h`, `IOCP.cpp` | `FiestaConnection.h` | Ours is non-IOCP (simple recv thread) because we're a single-socket client, not a server accepting many. |
| `proto_stubs.h` (quest/party/friend/guild placeholders) | Not yet in our tree | Every subsystem is tagged TODO on the skeleton side too — no intel lost. |

## 3. ShineEngine → Elle WorldModel field map _(Phase 6b-Alpha scope)_

| WorldModel field | Source packet | Decoder hook (current) | Status |
|---|---|---|---|
| `self.chrregnum`, `self.char_name` | `NC_CHAR_BASE_CMD` (CHAR_BASE_CMD_HEAD) | `Client::OnCharBase` → `UpdateSelfBase` | ✅ LIVE |
| `self.state` | n/a — internal state-machine | `Client::SetState` → `SetLoginState` | ✅ LIVE |
| `self.handle` | Not yet decoded (zone assigns late) | `MutableWorld().UpdateSelfHandle()` reserved | 🟡 pending 0x1038 decode |
| `self.x`, `self.y` | Out-going `NC_ACT_WALK_REQ`/`STOP_REQ` | `MoveTo`/`Stop` → `UpdateSelfPosition` | ✅ LIVE (self-authoritative) |
| `zone.map` | `NC_MAP_LOGINCOMPLETE_CMD` + `ProtoNcUserLoginWorldAck.szMap` | `OnMapLoginComplete` (Phase 6b-Beta plugs mapName) | 🟡 pending — need szMap[8] parse |
| `entities[h].name` (player) | `NC_BRIEFINFO_LOGINCHARACTER_CMD` + `NC_BRIEFINFO_CHARACTER_CMD` + `NC_BRIEFINFO_PLAYER_LIST_INFO_APPEAR_CMD` | `OnLoginCharacter`, `OnBriefCharacter`, `OnPlayerListAppear` → `UpsertPlayer` | ✅ LIVE |
| `entities[h].mob_id` | `NC_BRIEFINFO_REGENMOB_CMD` | `OnRegenMob` → `UpsertMob` | ✅ LIVE |
| removal | `NC_BRIEFINFO_BRIEFINFODELETE_CMD` + `NC_BRIEFINFO_NPC_DISAPPEAR_CMD` | `OnBriefInfoDelete`, `OnNpcDisappear` → `RemoveEntity` | ✅ LIVE |

_Explicitly out-of-scope for 6b-Alpha (moved to 6b-Beta):_ HP/MP/Gold/Exp,
equipment, inventory, skill/buff list, quest state. All of these appear
in the ShineEngine `protomap.h` `ProtoNcMapLoginAck` as an "extended
stat block follows — size TBD from captures" — meaning the engine source
confirms they're in 0x1038's payload but we still need capture-driven
offset math before we can safely cast bytes.

## 4. IPC surface (unchanged since 6a)

```
Cognitive → SVC_FIESTA  IPC_FIESTA_COMMAND  {"op":"get_world"}
SVC_FIESTA → subscribers IPC_FIESTA_EVENT   {"kind":"world_snapshot",
                                             "request_id": "...",
                                             "snapshot": { … }}
```

Snapshot envelope (see `test_fiesta_worldmodel.cpp::test_full_session_simulation`):

```json
{
  "self":     {"handle":5,"chrregnum":5,"char_name":"ElleAnn",
               "state":"in_game","x":100,"y":200,"last_update":1234567890},
  "zone":     {"map":"RouN","entered_ms":1234560000},
  "entities": [
    {"handle":4098,"kind":"player","name":"Bob",
     "first_seen":…,"last_seen":…},
    {"handle":20481,"kind":"mob","mob_id":3010,
     "first_seen":…,"last_seen":…}
  ]
}
```

## 5. Next-patch shopping list (6b-Beta)

Now that we have the reference skeleton, 6b-Beta can decode the whole
`NC_MAP_LOGIN_ACK` flood without more guesswork:

1. `FiestaDecoders.h`: add `DecodeMapLoginAck` per `ProtoNcMapLoginAck`
   — payload prefix is `{u32 charid, char[16] name, u8 level, char[8] map,
   u32 hpCur, u32 hpMax, u32 mpCur, u32 mpMax, u32 gold, u32 exp}`.
2. `FiestaClient::OnMapLoginAck` new hook, feed HP/MP/Gold/Exp into
   `WorldModel::UpdateSelfStats` (to be added).
3. `FiestaDecoders.h`: add `DecodeMiscGameTimeAck` (trivial 3B HH/MM/SS)
   per `ProtoNcMiscGameTimeAck`, surface as `{"kind":"server_time"}`.
4. Shape-match the `0x0438` 97B char-base variant (see
   `_re_artifacts/wire_captures/README.md §5`) against
   `ProtoNcMapCharInfoCmd` — skeleton says it's `dwCharID + extended
   stat block` which is exactly the trimmed shape we observed.

Phase 6b-Beta then unblocks Phase 6c (behavioural Lua bindings) because
`on_hp_changed`, `on_zone_changed`, `on_gold_changed` all have real
numeric fields to fire on.
