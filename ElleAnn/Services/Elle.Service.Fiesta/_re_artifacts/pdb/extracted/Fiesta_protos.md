# PROTO_NC_* Struct Layouts (extracted from PDB)

_Source: `Fiesta_types.txt` -- 2714 structs_

### `CHARTITLE_BRIEFINFO`  (sizeof = 4 bytes)

_no members_

### `NETPACKET`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netcmd` | `NETCOMMAND` |
|     2 | `data` | `unsigned char[0]   /* 0 bytes */` |

### `NETPACKETHEADER`  (sizeof = 2 bytes)

_no members_

### `NETPACKETZONEHEADER`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ACT_ACTIONBYITEM_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_ACT_ACTIONBYITEM_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ACT_ANIMATION_LEVEL_CHANGE_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_ACT_ANIMATION_START_CMD`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sAnimationIndex` | `wchar_t[32]   /* 32 bytes */` |

### `PROTO_NC_ACT_ANIMATION_STOP_CMD`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sAnimationIndex` | `wchar_t[32]   /* 32 bytes */` |

### `PROTO_NC_ACT_AUTO_WAY_FINDING_USE_GATE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_AUTO_WAY_FINDING_USE_GATE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ACT_CHANGEMODE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ACT_CHAT_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_CREATECASTBAR`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_EFFECT_MESSAGE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ACT_EMOTICON_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ACT_EVENT_CODE_ACTION_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ACT_FOLDTENT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_GATHERCOMPLETE_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ACT_GATHERSTART_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ACT_GATHERSTART_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_MOVEFAIL_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `back` | `SHINE_XY_TYPE` |

### `PROTO_NC_ACT_MOVEFAIL_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `back` | `SHINE_XY_TYPE` |

### `PROTO_NC_ACT_MOVESPEED_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ACT_MOVEWALK_CMD`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `from` | `SHINE_XY_TYPE` |
|     8 | `to` | `SHINE_XY_TYPE` |

### `PROTO_NC_ACT_NOTICE_CMD_SEND`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_ACT_CHAT_REQ` |

### `PROTO_NC_ACT_NPCCLICK_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_NPCMENUOPEN_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ACT_NPCMENUOPEN_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_NPC_ACTION_CMD`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_ACT_NPC_MENU_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ACT_OBJECT_EFFECT_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ACT_OBJECT_SOUND_CMD`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sSoundFileName` | `unsigned char[32]   /* 32 bytes */` |

### `PROTO_NC_ACT_PARTYCHAT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_PARTYCHAT_ACK_SEND`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_ACT_PARTYCHAT_ACK` |

### `PROTO_NC_ACT_PARTYCHAT_CMD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `talker` | `Name5` |
|    20 | `chat` | `PROTO_NC_ACT_PARTYCHAT_REQ` |

### `PROTO_NC_ACT_PARTYCHAT_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_PITCHTENT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_PLAY_SOUND_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `FileName` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_PRODUCE_CAST_FAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_PRODUCE_CAST_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_PRODUCE_MAKE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ACT_REINFORCEMOVEBYPATH_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `path` | `SHINE_XY_TYPE[?] /* 0 bytes */` |

### `PROTO_NC_ACT_REINFORCE_RUN_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `xy` | `SHINE_XY_TYPE` |

### `PROTO_NC_ACT_REINFORCE_STOP_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `loc` | `SHINE_XY_TYPE` |

### `PROTO_NC_ACT_RIDE_FAIL_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_RIDE_FEEDING_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_RIDE_FEEDING_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ACT_RIDE_HUNGRY_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_RIDE_ON_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ride` | `CHARBRIEFINFO_RIDE::RideInfo` |

### `PROTO_NC_ACT_ROAR_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_ROAR_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_SCRIPT_MSG_CMD`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sScriptMsgIndex` | `unsigned char[32]   /* 32 bytes */` |
|    34 | `nlen` | `unsigned char[5]   /* 5 bytes */` |
|    39 | `sContent` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_SCRIPT_MSG_WORLD_CMD`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sScriptMsgIndex` | `unsigned char[32]   /* 32 bytes */` |
|    34 | `nlen` | `unsigned char[5]   /* 5 bytes */` |
|    39 | `sContent` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_SETITEMHEALEFFECT`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `flag` | `PROTO_NC_ACT_SETITEMHEALEFFECT::<unnamed-type-flag>` |

### `PROTO_NC_ACT_SETITEMHEALEFFECT::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isheal` | `<LF_BITFIELD:0x173b7>` |

### `PROTO_NC_ACT_SHOUT_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_SHOW_CINEMATIC_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `pText` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_SOMEEONEJUMP_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_SOMEONECHANGEMODE_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_ACT_SOMEONECHAT_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `flag` | `PROTO_NC_ACT_SOMEONECHAT_CMD::<unnamed-type-flag>` |
|     7 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_SOMEONECHAT_CMD::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GMColor` | `<LF_BITFIELD:0x173b7>` |
|     0 | `chatwin` | `<LF_BITFIELD:0x173b8>` |

### `PROTO_NC_ACT_SOMEONEEMOTICONSTOP_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `emoticon` | `STOPEMOTICON_DESCRIPT` |

### `PROTO_NC_ACT_SOMEONEEMOTICON_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_ACT_SOMEONEFOLDTENT_CMD`  (sizeof = 45 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `shape` | `CHARBRIEFINFO_NOTCAMP` |

### `PROTO_NC_ACT_SOMEONEGATHERCANCEL_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ACT_SOMEONEGATHERCOMPLETE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ACT_SOMEONEGATHERSTART_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ACT_SOMEONEMOVEWALK_CMD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `from` | `SHINE_XY_TYPE` |
|    10 | `to` | `SHINE_XY_TYPE` |
|    20 | `moveattr` | `PROTO_NC_ACT_SOMEONEMOVEWALK_CMD::<unnamed-type-moveattr>` |

### `PROTO_NC_ACT_SOMEONEMOVEWALK_CMD::<unnamed-type-moveattr>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `direct` | `<LF_BITFIELD:0x1e0c5>` |

### `PROTO_NC_ACT_SOMEONEPITCHTENT_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `camp` | `CHARBRIEFINFO_CAMP` |

### `PROTO_NC_ACT_SOMEONEPRODUCE_CASTCUT_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_SOMEONEPRODUCE_CAST_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ACT_SOMEONEPRODUCE_MAKE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ACT_SOMEONERIDE_OFF_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_SOMEONERIDE_ON_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `ride` | `CHARBRIEFINFO_RIDE::RideInfo` |

### `PROTO_NC_ACT_SOMEONESHOUT_CMD`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `speaker` | `PROTO_NC_ACT_SOMEONESHOUT_CMD::<unnamed-type-speaker>` |
|    21 | `flag` | `PROTO_NC_ACT_SOMEONESHOUT_CMD::<unnamed-type-flag>` |
|    23 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_SOMEONESHOUT_CMD::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GMColor` | `<LF_BITFIELD:0x173b7>` |
|     0 | `isMob` | `<LF_BITFIELD:0x173b8>` |

### `PROTO_NC_ACT_SOMEONESPEEDCHANGE_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_ACT_SOMEONESTOP_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `loc` | `SHINE_XY_TYPE` |

### `PROTO_NC_ACT_SOMEONEWALK_CMD`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `from` | `SHINE_XY_TYPE` |
|    10 | `to` | `SHINE_XY_TYPE` |

### `PROTO_NC_ACT_SOMEONEWHISPER_CMD`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `talker` | `Name5` |
|    21 | `flag` | `PROTO_NC_ACT_SOMEONEWHISPER_CMD::<unnamed-type-flag>` |
|    23 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_SOMEONEWHISPER_CMD::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GMColor` | `<LF_BITFIELD:0x173b7>` |

### `PROTO_NC_ACT_STOP_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `loc` | `SHINE_XY_TYPE` |

### `PROTO_NC_ACT_WALK_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `from` | `SHINE_XY_TYPE` |
|     8 | `to` | `SHINE_XY_TYPE` |

### `PROTO_NC_ACT_WEDDING_AGREEMENT_DIVORCE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_WEDDING_COMPULSORY_DIVORCE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_WEDDING_COUPLE_ENTRANCE_RNG`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |

### `PROTO_NC_ACT_WEDDING_HALL_GUEST_ENTER_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ACT_WEDDING_HALL_GUEST_ENTER_READY_ACK`  (sizeof = 174 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `GroomID` | `Name5` |
|    22 | `BrideID` | `Name5` |
|    66 | `tm_EnterStart` | `tm` |
|   102 | `tm_WeddingStart` | `tm` |
|   138 | `tm_WeddingEnd` | `tm` |

### `PROTO_NC_ACT_WEDDING_HALL_RESERV_ACK`  (sizeof = 46 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `tm_ReservedTime` | `tm` |

### `PROTO_NC_ACT_WEDDING_PROPOSEACK_ACK`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `response_word` | `wchar_t[37]   /* 37 bytes */` |

### `PROTO_NC_ACT_WEDDING_PROPOSEACK_REQ`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `propose_word` | `wchar_t[37]   /* 37 bytes */` |

### `PROTO_NC_ACT_WEDDING_PROPOSEREQ_ACK`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `response_word` | `wchar_t[37]   /* 37 bytes */` |

### `PROTO_NC_ACT_WEDDING_PROPOSEREQ_REQ`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `propose_word` | `wchar_t[37]   /* 37 bytes */` |

### `PROTO_NC_ACT_WEDDING_SOMEONE`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ACT_WHISPERFAIL_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `receiver` | `Name5` |

### `PROTO_NC_ACT_WHISPERSUCCESS_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `receiver` | `Name5` |
|    22 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_WHISPER_REQ`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `receiver` | `Name5` |
|    22 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ANNOUNCE_W2C_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `AnnounceMessage` | `ANNOUNCE_MESSAGE` |

### `PROTO_NC_ANNOUNCE_Z2W_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `AnnounceMessage` | `ANNOUNCE_MESSAGE` |

### `PROTO_NC_AVATAR_CREATEDATAFAIL_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |

### `PROTO_NC_AVATAR_CREATEDATASUC_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `avatar` | `PROTO_AVATARINFORMATION[?] /* 0 bytes */` |

### `PROTO_NC_AVATAR_CREATEDATA_REQ`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     8 | `clientdata` | `PROTO_NC_AVATAR_CREATE_REQ` |

### `PROTO_NC_AVATAR_CREATEDATA_REQ_SEND`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `req` | `PROTO_NC_AVATAR_CREATEDATA_REQ` |

### `PROTO_NC_AVATAR_CREATEFAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_AVATAR_CREATEFAIL_ACK_SEND`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_AVATAR_CREATEFAIL_ACK` |

### `PROTO_NC_AVATAR_CREATESUCC_ACK`  (sizeof = 131 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `avatar` | `PROTO_AVATARINFORMATION` |

### `PROTO_NC_AVATAR_CREATESUCC_ACK_SEND`  (sizeof = 134 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_AVATAR_CREATESUCC_ACK` |

### `PROTO_NC_AVATAR_CREATE_REQ`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `name` | `Name5` |
|    21 | `char_shape` | `PROTO_AVATAR_SHAPE_INFO` |

### `PROTO_NC_AVATAR_ERASEDATAFAIL_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_AVATAR_ERASEDATASUC_ACK`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_AVATAR_ERASEDATA_REQ`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_AVATAR_ERASEDATA_REQ_SEND`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `req` | `PROTO_NC_AVATAR_ERASEDATA_REQ` |

### `PROTO_NC_AVATAR_ERASEFAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_AVATAR_ERASEFAIL_ACK_SEND`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_AVATAR_ERASEFAIL_ACK` |

### `PROTO_NC_AVATAR_ERASESUC_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_AVATAR_ERASESUC_ACK_SEND`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_AVATAR_ERASESUC_ACK` |

### `PROTO_NC_AVATAR_ERASE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_AVATAR_GUILD_DATA_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `Guild` | `GUILD_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_AVATAR_GUILD_DATA_REQ`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_AVATAR_GUILD_DATA_REQ_SEND`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `req` | `PROTO_NC_AVATAR_GUILD_DATA_REQ` |

### `PROTO_NC_AVATAR_GUILD_MEMBER_DATA_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `MemberList` | `GUILD_MEMEBER_INFO[?] /* 0 bytes */` |

### `PROTO_NC_AVATAR_GUILD_MEMBER_DATA_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_AVATAR_RENAME_ACK`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `NewName` | `Name5` |

### `PROTO_NC_AVATAR_RENAME_DB_ACK`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `NetPacketHeader` | `NETPACKETHEADER` |
|     7 | `NewName` | `Name5` |

### `PROTO_NC_AVATAR_RENAME_DB_REQ`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `NetPacketHeader` | `NETPACKETHEADER` |
|     7 | `NewName` | `Name5` |

### `PROTO_NC_AVATAR_RENAME_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `NewName` | `Name5` |

### `PROTO_NC_BAT_ABSTATEINFORM_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `abstate` | `ABSTATEINDEX` |

### `PROTO_NC_BAT_ABSTATEINFORM_NOEFFECT_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `abstate` | `ABSTATEINDEX` |

### `PROTO_NC_BAT_ABSTATERESET_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `abstate` | `ABSTATEINDEX` |

### `PROTO_NC_BAT_ABSTATESET_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `abstate` | `ABSTATEINDEX` |

### `PROTO_NC_BAT_ABSTATE_ERASE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_ABSTATE_ERASE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_APCHANGE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_BAT_AREADOTDAMAGE_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `target` | `PROTO_NC_BAT_AREADOTDAMAGE_CMD::targetinfo[?] /* 0 bytes */` |

### `PROTO_NC_BAT_AREADOTDAMAGE_CMD::targetinfo`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_BAT_ASSIST_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_ASSIST_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_BASH_HITTED_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `damage` | `PROTO_NC_BAT_BASH_HITTED_CMD::<unnamed-type-damage>` |
|     4 | `flag` | `PROTO_NC_BAT_BASH_HITTED_CMD::<unnamed-type-flag>` |

### `PROTO_NC_BAT_BASH_HITTED_CMD::<unnamed-type-damage>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `iscritical` | `<LF_BITFIELD:0x1e0c5>` |
|     0 | `damage` | `<LF_BITFIELD:0x1f9ee>` |

### `PROTO_NC_BAT_BASH_HITTED_CMD::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `actioncode` | `<LF_BITFIELD:0x1f9eb>` |
|     0 | `isresist` | `<LF_BITFIELD:0x1c4ff>` |

### `PROTO_NC_BAT_BASH_HIT_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `damage` | `PROTO_NC_BAT_BASH_HIT_CMD::<unnamed-type-damage>` |
|     4 | `flag` | `PROTO_NC_BAT_BASH_HIT_CMD::<unnamed-type-flag>` |

### `PROTO_NC_BAT_BASH_HIT_CMD::<unnamed-type-damage>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `iscritical` | `<LF_BITFIELD:0x1e0c5>` |
|     0 | `damage` | `<LF_BITFIELD:0x1f9ee>` |

### `PROTO_NC_BAT_BASH_HIT_CMD::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `actioncode` | `<LF_BITFIELD:0x1f9eb>` |
|     0 | `isresist` | `<LF_BITFIELD:0x1c4ff>` |
|     0 | `isImmune` | `<LF_BITFIELD:0x1c500>` |

### `PROTO_NC_BAT_BASH_MISSED_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `flag` | `PROTO_NC_BAT_BASH_MISSED_CMD::<unnamed-type-flag>` |

### `PROTO_NC_BAT_BASH_MISSED_CMD::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `actioncode` | `<LF_BITFIELD:0x1f9eb>` |
|     0 | `ismissed` | `<LF_BITFIELD:0x1c4ff>` |
|     0 | `isshieldblock` | `<LF_BITFIELD:0x1c500>` |
|     0 | `isresist` | `<LF_BITFIELD:0x1c501>` |
|     0 | `isImmune` | `<LF_BITFIELD:0xd682>` |

### `PROTO_NC_BAT_BASH_MISS_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `flag` | `PROTO_NC_BAT_BASH_MISS_CMD::<unnamed-type-flag>` |

### `PROTO_NC_BAT_BASH_MISS_CMD::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `actioncode` | `<LF_BITFIELD:0x1f9eb>` |
|     0 | `ismissed` | `<LF_BITFIELD:0x1c4ff>` |
|     0 | `isshieldblock` | `<LF_BITFIELD:0x1c500>` |
|     0 | `isImmune` | `<LF_BITFIELD:0x1c501>` |

### `PROTO_NC_BAT_CEASE_FIRE_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_CLIENT_MOB_KILL_ANNOUNCE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTextIndex` | `MobKillAnnounceType` |

### `PROTO_NC_BAT_DOTDAMAGE_CMD`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_BAT_EXPGAIN_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BAT_EXPLOST_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_BAT_FAMEGAIN_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_BAT_HIT_REQ`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_BAT_HPCHANGE_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BAT_LEVELUP_CMD`  (sizeof = 235 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `newparam` | `CHAR_PARAMETER_DATA` |

### `PROTO_NC_BAT_LPCHANGE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_BAT_MOBSLAYER_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `slayers` | `unsigned long[0]   /* 0 bytes */` |

### `PROTO_NC_BAT_PKINPKFIELD_CLIENT_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_BAT_PKINPKFIELD_WMS_CMD`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_BAT_REALLYKILL_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_BAT_REFLECTIONDAMAGE_CMD`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_BAT_SKILLBASH_CASTABORT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_SKILLBASH_CAST_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_BAT_SKILLBASH_FLD_CAST_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `locate` | `SHINE_XY_TYPE` |

### `PROTO_NC_BAT_SKILLBASH_HITTED_CMD`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `targetpoint` | `SHINE_XY_TYPE` |
|    12 | `flag` | `PROTO_NC_BAT_SKILLBASH_HITTED_CMD::<unnamed-type-flag>` |
|    25 | `target` | `PROTO_NC_BAT_SKILLBASH_HITTED_CMD::<unnamed-type-target>[?] /* 0 bytes */` |

### `PROTO_NC_BAT_SKILLBASH_HITTED_CMD::<unnamed-type-flag>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isdamage` | `<LF_BITFIELD:0x173b7>` |
|     0 | `iscritical` | `<LF_BITFIELD:0x173b8>` |
|     0 | `ismissed` | `<LF_BITFIELD:0x173b9>` |
|     0 | `isshieldblock` | `<LF_BITFIELD:0x1c4fe>` |
|     0 | `isheal` | `<LF_BITFIELD:0x1c4ff>` |
|     0 | `isenchant` | `<LF_BITFIELD:0x1c500>` |
|     0 | `isresist` | `<LF_BITFIELD:0x1c501>` |
|     0 | `IsCostumWeapon` | `<LF_BITFIELD:0xd682>` |
|     1 | `isDead` | `<LF_BITFIELD:0x173b7>` |
|     1 | `IsCostumShield` | `<LF_BITFIELD:0x173b8>` |

### `PROTO_NC_BAT_SKILLBASH_HITTED_CMD::<unnamed-type-target>`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_BAT_SKILLBASH_HIT_BLAST_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BAT_SKILLBASH_HIT_CMD`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `targetpoint` | `SHINE_XY_TYPE` |
|    11 | `target` | `PROTO_NC_BAT_SKILLBASH_HIT_CMD::<unnamed-type-target>[?] /* 0 bytes */` |

### `PROTO_NC_BAT_SKILLBASH_HIT_CMD::<unnamed-type-target>`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `flag` | `PROTO_NC_BAT_SKILLBASH_HIT_CMD::<unnamed-type-target>::<unnamed-type-flag>` |

### `PROTO_NC_BAT_SKILLBASH_HIT_CMD::<unnamed-type-target>::<unnamed-type-flag>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isdamage` | `<LF_BITFIELD:0x173b7>` |
|     0 | `iscritical` | `<LF_BITFIELD:0x173b8>` |
|     0 | `ismissed` | `<LF_BITFIELD:0x173b9>` |
|     0 | `isshieldblock` | `<LF_BITFIELD:0x1c4fe>` |
|     0 | `isheal` | `<LF_BITFIELD:0x1c4ff>` |
|     0 | `isenchant` | `<LF_BITFIELD:0x1c500>` |
|     0 | `isresist` | `<LF_BITFIELD:0x1c501>` |
|     0 | `IsCostumWeapon` | `<LF_BITFIELD:0xd682>` |
|     1 | `isDead` | `<LF_BITFIELD:0x173b7>` |
|     1 | `isCostumShield` | `<LF_BITFIELD:0x173b8>` |

### `PROTO_NC_BAT_SKILLBASH_HIT_DAMAGE_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `target` | `PROTO_NC_BAT_SKILLBASH_HIT_DAMAGE_CMD::SkillDamage[?] /* 0 bytes */` |

### `PROTO_NC_BAT_SKILLBASH_HIT_DAMAGE_CMD::SkillDamage`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `flag` | `PROTO_NC_BAT_SKILLBASH_HIT_DAMAGE_CMD::SkillDamage::<unnamed-type-flag>` |

### `PROTO_NC_BAT_SKILLBASH_HIT_DAMAGE_CMD::SkillDamage::<unnamed-type-flag>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isdamage` | `<LF_BITFIELD:0x173b7>` |
|     0 | `iscritical` | `<LF_BITFIELD:0x173b8>` |
|     0 | `ismissed` | `<LF_BITFIELD:0x173b9>` |
|     0 | `isshieldblock` | `<LF_BITFIELD:0x1c4fe>` |
|     0 | `isheal` | `<LF_BITFIELD:0x1c4ff>` |
|     0 | `isenchant` | `<LF_BITFIELD:0x1c500>` |
|     0 | `isresist` | `<LF_BITFIELD:0x1c501>` |
|     0 | `IsCostumWeapon` | `<LF_BITFIELD:0xd682>` |
|     1 | `isDead` | `<LF_BITFIELD:0x173b7>` |
|     1 | `isImmune` | `<LF_BITFIELD:0x173b8>` |
|     1 | `IsCostumShield` | `<LF_BITFIELD:0x173b9>` |

### `PROTO_NC_BAT_SKILLBASH_HIT_FLD_START_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `targetloc` | `SHINE_XY_TYPE` |

### `PROTO_NC_BAT_SKILLBASH_HIT_OBJ_START_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BAT_SKILLBASH_OBJ_CAST_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_BAT_SKILLBLAST_LIGHTNINGWAVE_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_BAT_SKILLCAST_FAIL_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_BAT_SKILLCAST_REQ`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_BAT_SKILLCAST_SUC_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_BAT_SKILLENCHANT_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_BAT_SKILLSMASH_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_BAT_SKILLSMASH_ENCHANT_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_BAT_SKILLSMASH_HITTED_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `hitted` | `PROTO_NC_BAT_SKILLSMASH_HIT_CMD` |

### `PROTO_NC_BAT_SKILLSMASH_HIT_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `damage` | `PROTO_NC_BAT_SKILLSMASH_HIT_CMD::<unnamed-type-damage>` |

### `PROTO_NC_BAT_SKILLSMASH_HIT_CMD::<unnamed-type-damage>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isdead` | `<LF_BITFIELD:0x1e0c5>` |
|     0 | `iscritical` | `<LF_BITFIELD:0x1e0c6>` |
|     0 | `damage` | `<LF_BITFIELD:0x1e0c7>` |

### `PROTO_NC_BAT_SKILLSMASH_MISS_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_BAT_SMASH_HITTED_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `hitted` | `PROTO_NC_BAT_SMASH_HITTED_ACK::<unnamed-type-hitted>` |

### `PROTO_NC_BAT_SMASH_HITTED_ACK::<unnamed-type-hitted>`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `damage` | `PROTO_NC_BAT_SMASH_HITTED_ACK::<unnamed-type-hitted>::<unnamed-type-damage>` |

### `PROTO_NC_BAT_SMASH_HITTED_ACK::<unnamed-type-hitted>::<unnamed-type-damage>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isdead` | `<LF_BITFIELD:0x1e0c5>` |
|     0 | `iscritical` | `<LF_BITFIELD:0x1e0c6>` |
|     0 | `damage` | `<LF_BITFIELD:0x1e0c7>` |

### `PROTO_NC_BAT_SMASH_HIT_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `damage` | `PROTO_NC_BAT_SMASH_HIT_ACK::<unnamed-type-damage>` |

### `PROTO_NC_BAT_SMASH_HIT_ACK::<unnamed-type-damage>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isdead` | `<LF_BITFIELD:0x1e0c5>` |
|     0 | `iscritical` | `<LF_BITFIELD:0x1e0c6>` |
|     0 | `damage` | `<LF_BITFIELD:0x1e0c7>` |

### `PROTO_NC_BAT_SMASH_MISS_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_BAT_SMASH_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONEBASH_HIT_CMD`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONEBASH_MISS_CMD`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONEDAMAGED_LARGE_CMD`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONEDAMAGED_SMALL_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONESKILLBASH_CASTCUT_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONESKILLBASH_CAST_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONESKILLBASH_FLD_CAST_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `loc` | `SHINE_XY_TYPE` |

### `PROTO_NC_BAT_SOMEONESKILLBASH_HIT_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `targetpoint` | `SHINE_XY_TYPE` |
|    13 | `target` | `PROTO_NC_BAT_SOMEONESKILLBASH_HIT_CMD::<unnamed-type-target>[?] /* 0 bytes */` |

### `PROTO_NC_BAT_SOMEONESKILLBASH_HIT_CMD::<unnamed-type-target>`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `flag` | `PROTO_NC_BAT_SOMEONESKILLBASH_HIT_CMD::<unnamed-type-target>::<unnamed-type-flag>` |

### `PROTO_NC_BAT_SOMEONESKILLBASH_HIT_CMD::<unnamed-type-target>::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isdamage` | `<LF_BITFIELD:0x173b7>` |
|     0 | `ismissed` | `<LF_BITFIELD:0x173b8>` |
|     0 | `isheal` | `<LF_BITFIELD:0x173b9>` |
|     0 | `isenchant` | `<LF_BITFIELD:0x1c4fe>` |
|     0 | `damagedisplay` | `<LF_BITFIELD:0x1c4ff>` |
|     0 | `isDead` | `<LF_BITFIELD:0x1c500>` |

### `PROTO_NC_BAT_SOMEONESKILLBASH_HIT_FLD_START_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `castinfo` | `PROTO_NC_BAT_SKILLBASH_HIT_FLD_START_CMD` |

### `PROTO_NC_BAT_SOMEONESKILLBASH_HIT_OBJ_START_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `castinfo` | `PROTO_NC_BAT_SKILLBASH_HIT_OBJ_START_CMD` |

### `PROTO_NC_BAT_SOMEONESKILLBASH_OBJ_CAST_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONESKILLCASTCUT_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONESKILLCAST_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONESKILLENCHANT_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `skillenchant` | `PROTO_NC_BAT_SKILLENCHANT_REQ` |

### `PROTO_NC_BAT_SOMEONESKILLSMASH_DAMAGED_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONESKILLSMASH_DEAD_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONESKILLSMASH_ENCHANT_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONESMASH_DAMAGED_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_BAT_SOMEONESWING_DAMAGE_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `flag` | `PROTO_NC_BAT_SOMEONESWING_DAMAGE_CMD::<unnamed-type-flag>` |

### `PROTO_NC_BAT_SOMEONESWING_DAMAGE_CMD::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isMissed` | `<LF_BITFIELD:0x173b7>` |
|     0 | `isCostumCharged` | `<LF_BITFIELD:0x173b8>` |
|     0 | `isCostumShieldCharged` | `<LF_BITFIELD:0x173b9>` |

### `PROTO_NC_BAT_SOULCOLLECT_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_BAT_SPCHANGE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_BAT_SUMEONELEVELUP_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_BAT_SUMEONESKILLCUT_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_SWING_DAMAGE_CMD`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `flag` | `PROTO_NC_BAT_SWING_DAMAGE_CMD::<unnamed-type-flag>` |

### `PROTO_NC_BAT_SWING_DAMAGE_CMD::<unnamed-type-flag>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `iscritical` | `<LF_BITFIELD:0x173b7>` |
|     0 | `isresist` | `<LF_BITFIELD:0x173b8>` |
|     0 | `ismissed` | `<LF_BITFIELD:0x173b9>` |
|     0 | `isshieldblock` | `<LF_BITFIELD:0x1c4fe>` |
|     0 | `isCostumCharged` | `<LF_BITFIELD:0x1c4ff>` |
|     0 | `isDead` | `<LF_BITFIELD:0x1c500>` |
|     0 | `isDamege2Heal` | `<LF_BITFIELD:0x1c501>` |
|     0 | `isImmune` | `<LF_BITFIELD:0xd682>` |
|     1 | `isCostumShieldCharged` | `<LF_BITFIELD:0x173b7>` |

### `PROTO_NC_BAT_SWING_START_CMD`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_BAT_TARGETCHANGE_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_TARGETINFO_CMD`  (sizeof = 30 bytes)

_no members_

### `PROTO_NC_BAT_TARGET_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_TOGGLESKILL_OFF_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_TOGGLESKILL_ON_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BAT_WORLD_MOB_KILL_ANNOUNCE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTextIndex` | `MobKillAnnounceType` |

### `PROTO_NC_BOOTH_BUYREFRESH_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_BOOTH_CLOSE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BOOTH_ENTRY_BUY_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `items` | `PROTO_NC_BOOTH_ENTRY_BUY_ACK::BoothItemList[?] /* 0 bytes */` |

### `PROTO_NC_BOOTH_ENTRY_BUY_ACK::BoothItemList`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_BOOTH_ENTRY_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BOOTH_ENTRY_SELL_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `items` | `PROTO_NC_BOOTH_ENTRY_SELL_ACK::BoothItemList[?] /* 0 bytes */` |

### `PROTO_NC_BOOTH_ENTRY_SELL_ACK::BoothItemList`  (sizeof = 113 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `item` | `SHINE_ITEM_STRUCT` |

### `PROTO_NC_BOOTH_ITEMTRADE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BOOTH_ITEMTRADE_REQ`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_BOOTH_OPEN_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BOOTH_OPEN_REQ`  (sizeof = 31 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `signboard` | `STREETBOOTH_SIGNBOARD` |
|    30 | `flag` | `PROTO_NC_BOOTH_OPEN_REQ::<unnamed-type-flag>` |
|    31 | `items` | `PROTO_NC_BOOTH_OPEN_REQ::BoothItem[?] /* 0 bytes */` |

### `PROTO_NC_BOOTH_OPEN_REQ::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `issell` | `<LF_BITFIELD:0x173b7>` |
|     0 | `itemnum` | `<LF_BITFIELD:0x244c5>` |

### `PROTO_NC_BOOTH_OPEN_REQ::BoothItem`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_BOOTH_REFRESH_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BOOTH_SEARCH_BOOTH_CLOSED_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BOOTH_SEARCH_BOOTH_POSITION_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `BoothPosition` | `SHINE_XY_TYPE` |

### `PROTO_NC_BOOTH_SEARCH_BOOTH_POSITION_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BOOTH_SEARCH_ITEM_LIST_CATEGORIZED_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `ItemList` | `BoothItemInfo[?] /* 0 bytes */` |

### `PROTO_NC_BOOTH_SEARCH_ITEM_LIST_CATEGORIZED_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Category` | `MarketSearch` |

### `PROTO_NC_BOOTH_SOMEONECLOSE_CMD`  (sizeof = 45 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `shape` | `CHARBRIEFINFO_NOTCAMP` |

### `PROTO_NC_BOOTH_SOMEONEINTERIORSTART_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `signboard` | `STREETBOOTH_SIGNBOARD` |

### `PROTO_NC_BOOTH_SOMEONEOPEN_CMD`  (sizeof = 45 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `tent` | `CHARBRIEFINFO_CAMP` |
|    15 | `signboard` | `STREETBOOTH_SIGNBOARD` |

### `PROTO_NC_BRIEFINFO_ABSTATE_CHANGE_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `info` | `ABSTATE_INFORMATION` |

### `PROTO_NC_BRIEFINFO_ABSTATE_CHANGE_LIST_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `infoList` | `ABSTATE_INFORMATION[?] /* 0 bytes */` |

### `PROTO_NC_BRIEFINFO_BRIEFINFODELETE_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_BRIEFINFO_BUILDDOOR_CMD`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `coord` | `SHINE_COORD_TYPE` |
|    14 | `blockindex` | `Name8` |

### `PROTO_NC_BRIEFINFO_CHANGEDECORATE_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_BRIEFINFO_CHANGEUPGRADE_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BRIEFINFO_CHANGEWEAPON_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `upgradeinfo` | `PROTO_NC_BRIEFINFO_CHANGEUPGRADE_CMD` |

### `PROTO_NC_BRIEFINFO_CHARACTER_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `chars` | `PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD[?] /* 0 bytes */` |

### `PROTO_NC_BRIEFINFO_DOOR_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `doors` | `PROTO_NC_BRIEFINFO_BUILDDOOR_CMD[?] /* 0 bytes */` |

### `PROTO_NC_BRIEFINFO_DROPEDITEM_CMD`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `location` | `SHINE_XY_TYPE` |
|    14 | `attr` | `PROTO_NC_BRIEFINFO_DROPEDITEM_CMD::<unnamed-type-attr>` |

### `PROTO_NC_BRIEFINFO_DROPEDITEM_CMD::<unnamed-type-attr>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `rareness` | `<LF_BITFIELD:0x1e6cb>` |
|     0 | `looting` | `<LF_BITFIELD:0x1c4fe>` |

### `PROTO_NC_BRIEFINFO_EFFECTBLAST_CMD`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `effectname` | `Name8` |
|    34 | `coord` | `SHINE_COORD_TYPE` |
|    47 | `flag` | `PROTO_NC_BRIEFINFO_EFFECTBLAST_CMD::<unnamed-type-flag>` |

### `PROTO_NC_BRIEFINFO_EFFECTBLAST_CMD::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `loop` | `<LF_BITFIELD:0x173b7>` |

### `PROTO_NC_BRIEFINFO_EFFECT_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `effects` | `PROTO_NC_BRIEFINFO_EFFECTBLAST_CMD[?] /* 0 bytes */` |

### `PROTO_NC_BRIEFINFO_INFORM_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `ReceiveNetCommand` | `NETCOMMAND` |

### `PROTO_NC_BRIEFINFO_ITEMONFIELD_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `items` | `PROTO_NC_BRIEFINFO_DROPEDITEM_CMD[?] /* 0 bytes */` |

### `PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD`  (sizeof = 239 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `charid` | `Name5` |
|    22 | `coord` | `SHINE_COORD_TYPE` |
|    33 | `shape` | `PROTO_AVATAR_SHAPE_INFO` |
|    37 | `shapedata` | `PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD::<unnamed-type-shapedata>` |
|    84 | `emoticon` | `STOPEMOTICON_DESCRIPT` |
|    87 | `chartitle` | `CHARTITLE_BRIEFINFO` |
|    91 | `abstatebit` | `ABNORMAL_STATE_BIT` |
|   202 | `sAnimation` | `wchar_t[32]   /* 32 bytes */` |

### `PROTO_NC_BRIEFINFO_MAGICFIELDINFO_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `magicfield` | `PROTO_NC_BRIEFINFO_MAGICFIELDSPREAD_CMD[?] /* 0 bytes */` |

### `PROTO_NC_BRIEFINFO_MAGICFIELDSPREAD_CMD`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `location` | `SHINE_XY_TYPE` |

### `PROTO_NC_BRIEFINFO_MINIHOUSEBUILD_CMD`  (sizeof = 64 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `camp` | `CHARBRIEFINFO_CAMP` |
|    14 | `charid` | `Name5` |
|    34 | `coord` | `SHINE_COORD_TYPE` |
|    43 | `title` | `wchar_t[21]   /* 21 bytes */` |

### `PROTO_NC_BRIEFINFO_MINIHOUSE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `minihouse` | `PROTO_NC_BRIEFINFO_MINIHOUSEBUILD_CMD[?] /* 0 bytes */` |

### `PROTO_NC_BRIEFINFO_MOB_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `mobs` | `PROTO_NC_BRIEFINFO_REGENMOB_CMD[?] /* 0 bytes */` |

### `PROTO_NC_BRIEFINFO_MOVER_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Movers` | `PROTO_NC_BRIEFINFO_REGENMOVER_CMD[?] /* 0 bytes */` |

### `PROTO_NC_BRIEFINFO_PET_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Pets` | `PROTO_NC_BRIEFINFO_REGENPET_CMD[?] /* 0 bytes */` |

### `PROTO_NC_BRIEFINFO_PLAYER_INFO_APPEAR_CMD`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sID` | `Name5` |
|    22 | `CharTitle` | `CHARTITLE_BRIEFINFO` |

### `PROTO_NC_BRIEFINFO_PLAYER_LIST_INFO_APPEAR_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `PlayerInfo` | `PROTO_NC_BRIEFINFO_PLAYER_INFO_APPEAR_CMD[?] /* 0 bytes */` |

### `PROTO_NC_BRIEFINFO_REGENMOB_CMD`  (sizeof = 153 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `coord` | `SHINE_COORD_TYPE` |
|    15 | `flag` | `PROTO_NC_BRIEFINFO_REGENMOB_CMD::<unnamed-type-flag>` |
|   118 | `sAnimation` | `wchar_t[32]   /* 32 bytes */` |

### `PROTO_NC_BRIEFINFO_REGENMOVER_CMD`  (sizeof = 143 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `nCoord` | `SHINE_COORD_TYPE` |
|    19 | `AbstateBit` | `ABNORMAL_STATE_BIT` |
|   123 | `nSlotHandle` | `unsigned short[10]   /* 20 bytes */` |

### `PROTO_NC_BRIEFINFO_REGENPET_CMD`  (sizeof = 63 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `XYDir` | `SHINE_COORD_TYPE` |
|    15 | `sPetName` | `Name4` |
|    31 | `sAnimationIndex` | `wchar_t[32]   /* 32 bytes */` |

### `PROTO_NC_BRIEFINFO_UNEQUIP_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_CHARGED_BOOTHSLOTSIZE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHARGED_BUFFTERMINATE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHARGED_DELETEWEAPONTITLE_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |

### `PROTO_NC_CHARGED_RESETBUFF_CMD`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_CHARGED_SETBUFF_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `ChargedBuff` | `PROTO_CHARGEDBUFF_INFO` |

### `PROTO_NC_CHARGED_SKILLEMPOW_INITIALIZE_DB_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `restatitemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_CHARGED_STAT_INITIALIZE_DB_FAIL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHARGED_STAT_INITIALIZE_DB_SUC_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHARGED_STAT_INITIALIZE_FAIL_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHARGED_STAT_INITIALIZE_SUC_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHARSAVE_2WLDMAN_ABSTATE_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `abstate` | `PROTO_NC_CHARSAVE_ABSTATE_REQ` |

### `PROTO_NC_CHARSAVE_2WLDMAN_ALL_CMD`  (sizeof = 107 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `all` | `PROTO_NC_CHARSAVE_ALL_REQ` |

### `PROTO_NC_CHARSAVE_2WLDMAN_CHAT_COLOR_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHARSAVE_2WLDMAN_CHESTINFO_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `chest` | `PROTO_NC_CHAR_CHESTINFO_REQ` |

### `PROTO_NC_CHARSAVE_2WLDMAN_COININFO_CMD`  (sizeof = 22 bytes)

_no members_

### `PROTO_NC_CHARSAVE_2WLDMAN_GAME_CMD`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `game` | `PROTO_GAMEDATA_CMD` |

### `PROTO_NC_CHARSAVE_2WLDMAN_ITEMACTIONCOOLTIME_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `ActionCoolTime` | `PROTO_NC_CHARSAVE_ITEMACTIONCOOLTIME_ACK` |

### `PROTO_NC_CHARSAVE_2WLDMAN_ITEM_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `PacketOrder` | `PROTO_NC_CHARSAVE_2WLDMAN_ITEM_CMD::<unnamed-type-PacketOrder>` |
|     8 | `itemstr` | `PROTO_ITEM_CMD` |

### `PROTO_NC_CHARSAVE_2WLDMAN_ITEM_CMD::<unnamed-type-PacketOrder>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPacketOrderNum` | `<LF_BITFIELD:0x1c966>` |
|     0 | `nIsLastOfInven` | `<LF_BITFIELD:0xd682>` |

### `PROTO_NC_CHARSAVE_2WLDMAN_LINK_FROM_CMD`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `linkfrom` | `PROTO_NC_CHAR_LINKFROM_CMD` |

### `PROTO_NC_CHARSAVE_2WLDMAN_MISC_CMD`  (sizeof = 18 bytes)

_no members_

### `PROTO_NC_CHARSAVE_2WLDMAN_PET_LINK_RESUMMON_CMD`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_DOING_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `questdoing` | `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_DOING_CMD::tagQUESTDOING` |

### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_DOING_CMD::tagQUESTDOING`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `QuestDoingArray` | `PLAYER_QUEST_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_DONE_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `questdone` | `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_DONE_CMD::tagQUESTDONE` |

### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_DONE_CMD::tagQUESTDONE`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `QuestDoneArray` | `PLAYER_QUEST_DONE_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_READ_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `questread` | `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_READ_CMD::tagQUESTREAD` |

### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_READ_CMD::tagQUESTREAD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `QuestReadIDArray` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_REPEAT_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `questrepeat` | `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_REPEAT_CMD::tagQUESTREPEAT` |

### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_REPEAT_CMD::tagQUESTREPEAT`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `QuestRepeatArray` | `PLAYER_QUEST_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHARSAVE_2WLDMAN_SINGLE_OPTION_CMD`  (sizeof = 17 bytes)

_no members_

### `PROTO_NC_CHARSAVE_2WLDMAN_SKILL_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `PartMark` | `PARTMARK` |
|     5 | `skill` | `PROTO_NC_CHARSAVE_SKILL_REQ` |

### `PROTO_NC_CHARSAVE_2WLDMAN_TITLE_CMD`  (sizeof = 1069 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `CTData` | `CHARACTER_TITLE_READBLOCK` |

### `PROTO_NC_CHARSAVE_ABSTATE_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     8 | `abstate` | `ABSTATEREADBLOCK[?] /* 0 bytes */` |

### `PROTO_NC_CHARSAVE_ALL_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHARSAVE_ALL_REQ`  (sizeof = 105 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     6 | `level` | `PROTO_NC_CHARSAVE_LEVEL_CMD` |
|    23 | `location` | `PROTO_NC_CHARSAVE_LOCATION_CMD` |
|    71 | `stat` | `PROTO_NC_CHARSAVE_CHARSTAT_CMD` |

### `PROTO_NC_CHARSAVE_CHARSTAT_CMD`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    28 | `statdistribute` | `CHARSTATDISTSTR` |

### `PROTO_NC_CHARSAVE_CHAT_COLOR_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHARSAVE_DB_UI_STATE_SAVE_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHARSAVE_DB_UI_STATE_SAVE_REQ`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHARSAVE_ITEMACTIONCOOLTIME_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `group` | `ActionCooltime[?] /* 0 bytes */` |

### `PROTO_NC_CHARSAVE_ITEMACTIONCOOLTIME_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `group` | `ActionCooltime[?] /* 0 bytes */` |

### `PROTO_NC_CHARSAVE_LEVEL_CMD`  (sizeof = 17 bytes)

_no members_

### `PROTO_NC_CHARSAVE_LOCATION_CMD`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `map` | `Name3` |
|    16 | `coord` | `SHINE_XY_TYPE` |
|    28 | `map_kq` | `Name3` |
|    40 | `coord_kq` | `SHINE_XY_TYPE` |

### `PROTO_NC_CHARSAVE_PKCOUNT_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_CHARSAVE_QUEST_DOING_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     8 | `QuestDoingArray` | `PLAYER_QUEST_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHARSAVE_REST_EXP_LAST_EXEC_TIME_SAVE_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |

### `PROTO_NC_CHARSAVE_SELL_ITEM_INFO_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `SellItemList` | `PROTO_SELL_ITEM_INFO_SERVER[?] /* 0 bytes */` |

### `PROTO_NC_CHARSAVE_SET_CHAT_BLOCK_SPAMER_DB_CMD`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_CHARSAVE_SET_CHAT_BLOCK_SPAMER_WM_CMD`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_CHARSAVE_SKILL_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     8 | `skill` | `PROTO_SKILLREADBLOCK[?] /* 0 bytes */` |

### `PROTO_NC_CHARSAVE_TITLE_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     6 | `CTData` | `CHARACTER_TITLE_DB_SAVE` |

### `PROTO_NC_CHARSAVE_UI_STATE_SAVE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHARSAVE_UI_STATE_SAVE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHARSAVE_USEITEM_MINIMON_INFO_DB_CMD`  (sizeof = 78 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `UseItemMinimonInfo` | `USEITEM_MINIMON_INFO` |
|    54 | `DelSlotList_Normal` | `unsigned char[12]   /* 12 bytes */` |
|    66 | `DelSlotList_Charged` | `unsigned char[12]   /* 12 bytes */` |

### `PROTO_NC_CHARSAVE_USEITEM_MINIMON_INFO_WORLD_CMD`  (sizeof = 56 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `UseItemMinimonInfo` | `USEITEM_MINIMON_INFO` |

### `PROTO_NC_CHAR_ABSTATE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `abstate` | `ABSTATEREADBLOCK[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_ADMIN_LEVEL_INFORM_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_ANI_FILE_CHECK_CMD`  (sizeof = 124 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Checksum` | `Name8` |
|    32 | `SubDirectory` | `Name8` |
|    64 | `Filename` | `Name60Byte` |

### `PROTO_NC_CHAR_ARENA_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_BASEPARAMCHANGE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `param` | `CHAR_PARAMCHANGE_CMD[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_BASE_CMD`  (sizeof = 105 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `charid` | `Name5` |
|    66 | `logininfo` | `PROTO_NC_CHAR_BASE_CMD::LoginLocation` |
|    87 | `statdistribute` | `CHARSTATDISTSTR` |
|   101 | `flags` | `PROTO_NC_CHAR_BASE_CMD::<unnamed-type-flags>` |

### `PROTO_NC_CHAR_BASE_CMD::<unnamed-type-flags>::<unnamed-type-str>`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skillempower_can_reset` | `<LF_BITFIELD:0x170cd>` |
|     0 | `restunion` | `<LF_BITFIELD:0x170ce>` |

### `PROTO_NC_CHAR_BASE_CMD::LoginLocation`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `currentmap` | `Name3` |
|    12 | `currentcoord` | `SHINE_COORD_TYPE` |

### `PROTO_NC_CHAR_CARDCOLLECT_BOOKMARK_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `ViewList` | `PROTO_NC_CHAR_CARDCOLLECT_BOOKMARK_CMD::CardBookmarkInfo[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_CARDCOLLECT_BOOKMARK_CMD::CardBookmarkInfo`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAR_CARDCOLLECT_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `CardList` | `PROTO_NC_CHAR_CARDCOLLECT_CMD::CardInform[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_CARDCOLLECT_CMD::CardInform`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_CHAR_CARDCOLLECT_REWARD_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `RewardID` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_CHAR_CENCHANGE_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_CHAR_CHANGEBYCONDITION_PARAM_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `aParam` | `CHAR_CHANGEBYCONDITION_PARAM[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_CHARDATAFAIL_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_CHAR_CHARDATA_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_CHAR_CHARDATA_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_CHAR_CHARGEDBUFF_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `ChargedBuff` | `PROTO_CHARGEDBUFF_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_CHARGEDBUFF_ERASE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAR_CHARGEDBUFF_ERASE_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAR_CHAT_COLOR_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_CHESTINFO_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `location` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_CHAR_CHESTINFO_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     8 | `loc` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_CHAR_CLASSCHANGE_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_CLASSCHANGE_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_CLASSCHANGE_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_CLIENT_AUTO_PICK_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_CHAR_CLIENT_AUTO_PICK_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_CHAR_CLIENT_AUTO_PICK_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_CLIENT_CHARTITLE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `TitleArray` | `CT_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_CLIENT_ITEM_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `flag` | `PROTO_NC_CHAR_CLIENT_ITEM_CMD::<unnamed-type-flag>` |
|     3 | `ItemArray` | `PROTO_ITEMPACKET_INFORM[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_CLIENT_ITEM_CMD::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `invenclear` | `<LF_BITFIELD:0x173b7>` |

### `PROTO_NC_CHAR_CLIENT_SKILL_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `PartMark` | `PARTMARK` |
|     4 | `skill` | `PROTO_NC_CHAR_SKILLCLIENT_CMD` |

### `PROTO_NC_CHAR_COININFO_CMD`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_CHAR_DATATRANSMISSION_RNG`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `data` | `Proto_CharDataStruct` |

### `PROTO_NC_CHAR_DB_AUTO_PICK_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_CHAR_DB_NEWBIE_GUIDE_VIEW_SET_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_CHAR_DB_REST_EXP_LAST_EXEC_TIME_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_CHAR_DEADMENU_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `eMenuType` | `DeadMenuType` |

### `PROTO_NC_CHAR_EMBLEM_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_EMPTY_INSTANCE_DUNGEON_RNG`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |
|     7 | `Category` | `INSTANCE_DUNGEON::CATEGORY` |

### `PROTO_NC_CHAR_EVENT_ATTENDANCE_CHANGE_DAY_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_EVENT_ATTENDANCE_CHECK_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_EVENT_ATTENDANCE_CHECK_DB_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_EVENT_ATTENDANCE_CHECK_DB_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `tCheckTime` | `tm` |

### `PROTO_NC_CHAR_EVENT_ATTENDANCE_LIST_CLIENT_CMD`  (sizeof = 66 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `tEventStart` | `tm` |
|    37 | `AttendanceArray` | `unsigned char[28]   /* 28 bytes */` |
|    66 | `RewardList` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_CHAR_EVENT_ATTENDANCE_LIST_DB_ACK`  (sizeof = 1020 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    11 | `AttendanceArray` | `tm[?] /* 1008 bytes */` |
|  1020 | `RewardList` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_CHAR_EVENT_ATTENDANCE_LIST_DB_REQ`  (sizeof = 80 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `tmEventStartDate` | `tm` |
|    40 | `tmEventEndDate` | `tm` |

### `PROTO_NC_CHAR_EVENT_ATTENDANCE_REWARD_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAR_EVENT_ATTENDANCE_REWARD_DB_ACK`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_CHAR_EVENT_ATTENDANCE_REWARD_DB_REQ`  (sizeof = 114 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `tmEventDay` | `tm` |
|    42 | `tmCheckStartDay` | `tm` |
|    78 | `tmCheckEndDay` | `tm` |

### `PROTO_NC_CHAR_EVENT_ATTENDANCE_REWARD_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAR_EXP_CHANGED_CMD`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_CHAR_FAMECHANGE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAR_FAMESAVE_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_CHAR_FREESTAT_SET_DB_ACK`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nFreeStat` | `unsigned char[5]   /* 5 bytes */` |

### `PROTO_NC_CHAR_FREESTAT_SET_DB_REQ`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     7 | `nFreeStat` | `unsigned char[5]   /* 5 bytes */` |
|    15 | `DecItemInfo` | `PROTO_NC_CHAR_FREESTAT_SET_DB_REQ::<unnamed-type-DecItemInfo>[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_FREESTAT_SET_DB_REQ::<unnamed-type-DecItemInfo>`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `nItmeKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_CHAR_FRIEND_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `friend_array` | `PROTO_NC_CHAR_FRIEND_CMD_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_FRIEND_CMD_INFO`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `charid` | `Name5` |

### `PROTO_NC_CHAR_GET_CHAT_BLOCK_SPAMER_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_CHAR_GET_CHAT_BLOCK_SPAMER_DB_CMD`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_CHAR_GET_ITEMLIST_BY_TYPE_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    13 | `ItemCmd` | `PROTO_NC_CHAR_ITEM_CMD` |

### `PROTO_NC_CHAR_GET_ITEMLIST_BY_TYPE_NUM_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    13 | `ItemCmd` | `PROTO_NC_CHAR_ITEM_CMD` |

### `PROTO_NC_CHAR_GET_ITEMLIST_BY_TYPE_NUM_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_GET_ITEMLIST_BY_TYPE_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_GUILD_ACADEMY_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `GuildAcademy` | `GUILD_ACADEMY_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_GUILD_ACADEMY_ZONE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAR_GUILD_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `Guild` | `GUILD_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_HOUSE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_ITEM_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `PacketOrder` | `PROTO_NC_CHAR_ITEM_CMD::<unnamed-type-PacketOrder>` |
|     2 | `ItemCmd` | `PROTO_ITEM_CMD` |

### `PROTO_NC_CHAR_ITEM_CMD::<unnamed-type-PacketOrder>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPacketOrderNum` | `<LF_BITFIELD:0x1c966>` |
|     0 | `nIsLastOfInven` | `<LF_BITFIELD:0xd682>` |

### `PROTO_NC_CHAR_KICKPLAYEROUT_RNG`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     8 | `charid` | `Name5` |

### `PROTO_NC_CHAR_KQMAP_CMD`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sKQMapName` | `Name3` |
|    16 | `nKQCoord` | `SHINE_XY_TYPE` |
|    24 | `dKQDate` | `SHINE_DATETIME` |

### `PROTO_NC_CHAR_LEVEL_CHANGED_CMD`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_CHAR_LINKFROM_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `map` | `Name3` |
|    16 | `coord` | `SHINE_XY_TYPE` |

### `PROTO_NC_CHAR_LOGINFAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAR_LOGIN_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zoneip` | `Name4` |

### `PROTO_NC_CHAR_LOGIN_DB`  (sizeof = 56 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |
|    36 | `spawnapps` | `Name5` |

### `PROTO_NC_CHAR_LOGIN_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_LOGOUT_DB`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_CHAR_MAPLOGIN_ACK`  (sizeof = 242 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `param` | `CHAR_PARAMETER_DATA` |
|   234 | `logincoord` | `SHINE_XY_TYPE` |

### `PROTO_NC_CHAR_MASPUP_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_MYSTERYVAULT_UI_STATE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_NEWBIE_GUIDE_VIEW_LIST_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `nGuideViewList` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_CHAR_NEWBIE_GUIDE_VIEW_SET_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_DB_GET_GAME_ACK`  (sizeof = 67 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `Data` | `PROTO_NC_CHAR_OPTION_GAME` |

### `PROTO_NC_CHAR_OPTION_DB_GET_GAME_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_DB_GET_KEYMAPPING_ACK`  (sizeof = 311 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `Data` | `PROTO_NC_CHAR_OPTION_KEYMAPPING` |

### `PROTO_NC_CHAR_OPTION_DB_GET_KEYMAPPING_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_DB_GET_SHORTCUTDATA_ACK`  (sizeof = 1027 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `Data` | `PROTO_NC_CHAR_OPTION_SHORTCUTDATA` |

### `PROTO_NC_CHAR_OPTION_DB_GET_SHORTCUTDATA_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_DB_GET_SHORTCUTSIZE_ACK`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `Data` | `PROTO_NC_CHAR_OPTION_SHORTCUTSIZE` |

### `PROTO_NC_CHAR_OPTION_DB_GET_SHORTCUTSIZE_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_DB_GET_SOUND_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `Data` | `PROTO_NC_CHAR_OPTION_SOUND` |

### `PROTO_NC_CHAR_OPTION_DB_GET_SOUND_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_DB_GET_VIDEO_ACK`  (sizeof = 63 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `Data` | `PROTO_NC_CHAR_OPTION_VIDEO` |

### `PROTO_NC_CHAR_OPTION_DB_GET_VIDEO_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_DB_GET_WINDOWPOS_ACK`  (sizeof = 395 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `Data` | `PROTO_NC_CHAR_OPTION_WINDOWPOS` |

### `PROTO_NC_CHAR_OPTION_DB_GET_WINDOWPOS_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_DB_SET_GAME_CMD`  (sizeof = 68 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `Data` | `PROTO_NC_CHAR_OPTION_GAME` |

### `PROTO_NC_CHAR_OPTION_DB_SET_KEYMAPPING_CMD`  (sizeof = 312 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `Data` | `PROTO_NC_CHAR_OPTION_KEYMAPPING` |

### `PROTO_NC_CHAR_OPTION_DB_SET_SHORTCUTDATA_CMD`  (sizeof = 1028 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `Data` | `PROTO_NC_CHAR_OPTION_SHORTCUTDATA` |

### `PROTO_NC_CHAR_OPTION_DB_SET_SHORTCUTSIZE_CMD`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `Data` | `PROTO_NC_CHAR_OPTION_SHORTCUTSIZE` |

### `PROTO_NC_CHAR_OPTION_DB_SET_SOUND_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `Data` | `PROTO_NC_CHAR_OPTION_SOUND` |

### `PROTO_NC_CHAR_OPTION_DB_SET_VIDEO_CMD`  (sizeof = 64 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `Data` | `PROTO_NC_CHAR_OPTION_VIDEO` |

### `PROTO_NC_CHAR_OPTION_DB_SET_WINDOWPOS_CMD`  (sizeof = 396 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `Data` | `PROTO_NC_CHAR_OPTION_WINDOWPOS` |

### `PROTO_NC_CHAR_OPTION_GAME`  (sizeof = 64 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `unsigned char[64]   /* 64 bytes */` |

### `PROTO_NC_CHAR_OPTION_GET_ALL_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_GET_GAME_ACK`  (sizeof = 65 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Data` | `PROTO_NC_CHAR_OPTION_GAME` |

### `PROTO_NC_CHAR_OPTION_GET_GAME_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_GET_KEYMAPPING_ACK`  (sizeof = 309 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Data` | `PROTO_NC_CHAR_OPTION_KEYMAPPING` |

### `PROTO_NC_CHAR_OPTION_GET_KEYMAPPING_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_GET_SHORTCUTDATA_ACK`  (sizeof = 1025 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Data` | `PROTO_NC_CHAR_OPTION_SHORTCUTDATA` |

### `PROTO_NC_CHAR_OPTION_GET_SHORTCUTDATA_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_GET_SHORTCUTSIZE_ACK`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Data` | `PROTO_NC_CHAR_OPTION_SHORTCUTSIZE` |

### `PROTO_NC_CHAR_OPTION_GET_SHORTCUTSIZE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_GET_SOUND_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Data` | `PROTO_NC_CHAR_OPTION_SOUND` |

### `PROTO_NC_CHAR_OPTION_GET_SOUND_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_GET_VIDEO_ACK`  (sizeof = 61 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Data` | `PROTO_NC_CHAR_OPTION_VIDEO` |

### `PROTO_NC_CHAR_OPTION_GET_VIDEO_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_GET_WINDOWPOS_ACK`  (sizeof = 393 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Data` | `PROTO_NC_CHAR_OPTION_WINDOWPOS` |

### `PROTO_NC_CHAR_OPTION_GET_WINDOWPOS_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_GET_DATA_TYPE_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `DataType` | `CHAR_OPTION_DATA_TYPE` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_GET_ETC3_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_GET_ETC4_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_GET_GAMEOPTION_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `DBGameOptionData` | `PROTO_NC_CHAR_OPTION_IMPROVE_GET_GAMEOPTION_CMD` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_GET_KEYMAP_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `DBKeyMapData` | `PROTO_NC_CHAR_OPTION_IMPROVE_GET_KEYMAP_CMD` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_GET_SHORTCUTDATA_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `DBShortCutData` | `PROTO_NC_CHAR_OPTION_IMPROVE_GET_SHORTCUTDATA_CMD` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_INIT_ETC3_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_INIT_ETC3_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_INIT_ETC4_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_INIT_ETC4_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_INIT_GAMEOPTION_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `DBGameOptionData` | `PROTO_NC_CHAR_OPTION_IMPROVE_GET_GAMEOPTION_CMD` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_INIT_GAMEOPTION_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_INIT_KEYMAP_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `DBKeyMapData` | `PROTO_NC_CHAR_OPTION_IMPROVE_GET_KEYMAP_CMD` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_INIT_KEYMAP_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_INIT_SHORTCUTDATA_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_INIT_SHORTCUTDATA_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_SET_ETC3_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_SET_ETC3_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_SET_ETC4_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_SET_ETC4_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_SET_GAMEOPTION_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_SET_GAMEOPTION_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `DBGameOptionData` | `PROTO_NC_CHAR_OPTION_IMPROVE_GET_GAMEOPTION_CMD` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_SET_KEYMAP_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_SET_KEYMAP_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `DBKeyMapData` | `PROTO_NC_CHAR_OPTION_IMPROVE_GET_KEYMAP_CMD` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_SET_SHORTCUTDATA_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_DB_SET_SHORTCUTDATA_REQ`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `DBShortCutData` | `PROTO_NC_CHAR_OPTION_IMPROVE_SET_SHORTCUTDATA_REQ` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_GET_ETC3_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_GET_ETC4_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_GET_GAMEOPTION_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `GameOptionData` | `GAME_OPTION_DATA[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_GET_KEYMAP_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `KeyMapData` | `KEY_MAP_DATA[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_GET_SHORTCUTDATA_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `ShortCutData` | `SHORT_CUT_DATA[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_INIT_ETC3_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_INIT_ETC3_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_INIT_ETC4_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_INIT_ETC4_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_INIT_GAMEOPTION_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `DBGameOptionData` | `PROTO_NC_CHAR_OPTION_IMPROVE_GET_GAMEOPTION_CMD` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_INIT_GAMEOPTION_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_INIT_KEYMAP_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `DBKeyMapData` | `PROTO_NC_CHAR_OPTION_IMPROVE_GET_KEYMAP_CMD` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_INIT_KEYMAP_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_INIT_SHORTCUTDATA_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_INIT_SHORTCUTDATA_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_SET_ETC3_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_SET_ETC3_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_SET_ETC4_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_SET_ETC4_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_SET_GAMEOPTION_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_SET_GAMEOPTION_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `GameOptionData` | `GAME_OPTION_DATA[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_SET_KEYMAP_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_SET_KEYMAP_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `KeyMapData` | `KEY_MAP_DATA[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_OPTION_IMPROVE_SET_SHORTCUTDATA_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_IMPROVE_SET_SHORTCUTDATA_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `ShortCutData` | `SHORT_CUT_DATA[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_OPTION_KEYMAPPING`  (sizeof = 308 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `unsigned char[308]   /* 308 bytes */` |

### `PROTO_NC_CHAR_OPTION_LOGIN_BLOCKDATA_ERR_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAR_OPTION_SET_GAME_CMD`  (sizeof = 64 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `PROTO_NC_CHAR_OPTION_GAME` |

### `PROTO_NC_CHAR_OPTION_SET_KEYMAPPING_CMD`  (sizeof = 308 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `PROTO_NC_CHAR_OPTION_KEYMAPPING` |

### `PROTO_NC_CHAR_OPTION_SET_SHORTCUTDATA_CMD`  (sizeof = 1024 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `PROTO_NC_CHAR_OPTION_SHORTCUTDATA` |

### `PROTO_NC_CHAR_OPTION_SET_SHORTCUTSIZE_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `PROTO_NC_CHAR_OPTION_SHORTCUTSIZE` |

### `PROTO_NC_CHAR_OPTION_SET_SOUND_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `PROTO_NC_CHAR_OPTION_SOUND` |

### `PROTO_NC_CHAR_OPTION_SET_VIDEO_CMD`  (sizeof = 60 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `PROTO_NC_CHAR_OPTION_VIDEO` |

### `PROTO_NC_CHAR_OPTION_SET_WINDOWPOS_CMD`  (sizeof = 392 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `PROTO_NC_CHAR_OPTION_WINDOWPOS` |

### `PROTO_NC_CHAR_OPTION_SHORTCUTDATA`  (sizeof = 1024 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `unsigned char[1024]   /* 1024 bytes */` |

### `PROTO_NC_CHAR_OPTION_SHORTCUTSIZE`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `unsigned char[24]   /* 24 bytes */` |

### `PROTO_NC_CHAR_OPTION_SOUND`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `unsigned char[1]   /* 1 bytes */` |

### `PROTO_NC_CHAR_OPTION_VIDEO`  (sizeof = 60 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `unsigned char[60]   /* 60 bytes */` |

### `PROTO_NC_CHAR_OPTION_WINDOWPOS`  (sizeof = 392 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `unsigned char[392]   /* 392 bytes */` |

### `PROTO_NC_CHAR_PET_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_PLAYERBANNED_RNG`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `charid` | `Name5` |

### `PROTO_NC_CHAR_PLAYERFOUND_BY_NORMAL_USER_RNG`  (sizeof = 49 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `map` | `Name3` |
|    19 | `coord` | `SHINE_XY_TYPE` |
|    27 | `charid` | `Name5` |

### `PROTO_NC_CHAR_PLAYERFOUND_RNG`  (sizeof = 47 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `map` | `Name3` |
|    19 | `coord` | `SHINE_XY_TYPE` |
|    27 | `charid` | `Name5` |

### `PROTO_NC_CHAR_PLAYERSEARCH_BY_NORMAL_USER_RNG`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `charid` | `Name5` |

### `PROTO_NC_CHAR_PLAYERSEARCH_RNG`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `charid` | `Name5` |

### `PROTO_NC_CHAR_PLAYERSUMMON_RNG`  (sizeof = 47 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `charid` | `Name5` |
|    27 | `map` | `Name3` |
|    39 | `coord` | `SHINE_XY_TYPE` |

### `PROTO_NC_CHAR_POLYMORPH_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `AbstateIndex` | `ABSTATEINDEX` |

### `PROTO_NC_CHAR_PROMOTE_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_PROMOTE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_QUEST_DOING_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `QuestDoingArray` | `PLAYER_QUEST_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_QUEST_DONE_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `QuestDoneArray` | `PLAYER_QUEST_DONE_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_QUEST_READ_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `QuestReadIDArray` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_CHAR_QUEST_REPEAT_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `QuestRepeatArray` | `PLAYER_QUEST_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_REBIRTH_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_REBIRTH_REJECT_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_REGISTNUMBER_ACK`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `loginreq` | `PROTO_NC_CHAR_ZONE_CHARDATA_REQ` |

### `PROTO_NC_CHAR_REGISTNUMBER_ACK_SEND`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_CHAR_REGISTNUMBER_ACK` |

### `PROTO_NC_CHAR_REGISTNUMBER_REQ`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `loginreq` | `PROTO_NC_CHAR_ZONE_CHARDATA_REQ` |

### `PROTO_NC_CHAR_REGNUM_VARIFICATION_ACK`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_CHAR_REGNUM_VARIFICATION_REQ`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `zone_charid` | `Name5` |

### `PROTO_NC_CHAR_REVIVEOTHER_CMD`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `link` | `PROTO_NC_CHAR_REVIVESAME_CMD` |
|    10 | `sock` | `PROTO_NC_CHAR_LOGIN_ACK` |

### `PROTO_NC_CHAR_REVIVESAME_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `location` | `SHINE_XY_TYPE` |

### `PROTO_NC_CHAR_SEAWAR_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_SELL_ITEM_INFO_ZONE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `SellItemList` | `PROTO_SELL_ITEM_INFO_SERVER[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_SET_STYLE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAR_SET_STYLE_DB_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `Info` | `PROTO_AVATAR_SHAPE_INFO` |

### `PROTO_NC_CHAR_SET_STYLE_DB_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `Info` | `PROTO_AVATAR_SHAPE_INFO` |

### `PROTO_NC_CHAR_SET_STYLE_GET_INFO_ACK`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `Items` | `STYLE_ITEM_COUNTS[?] /* 24 bytes */` |

### `PROTO_NC_CHAR_SET_STYLE_GET_INFO_DB_ACK`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `Items` | `STYLE_ITEM_COUNTS[?] /* 24 bytes */` |

### `PROTO_NC_CHAR_SET_STYLE_GET_INFO_DB_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_SET_STYLE_GET_INFO_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_SET_STYLE_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Info` | `PROTO_AVATAR_SHAPE_INFO` |

### `PROTO_NC_CHAR_SINGLE_OPTION_CMD`  (sizeof = 15 bytes)

_no members_

### `PROTO_NC_CHAR_SKILLCLIENT_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `skill` | `PROTO_SKILLREADBLOCKCLIENT[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_SKILL_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `PartMark` | `PARTMARK` |
|     9 | `skill` | `PROTO_SKILLREADBLOCK[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_SKILL_PASSIVE_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `passive` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_CHAR_SOMEONEGUILDACADEMYCHANGE_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_SOMEONEGUILDCHANGE_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_SOMEONEPROMOTE_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_CHAR_STAT_DECPOINTFAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAR_STAT_DECPOINTFAIL_DB_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_STAT_DECPOINTSUC_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_STAT_DECPOINTSUC_DB_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_STAT_DECPOINT_DB_REQ`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     9 | `restatitemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_CHAR_STAT_DECPOINT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_STAT_INCPOINTFAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAR_STAT_INCPOINTFAIL_DB_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_STAT_INCPOINTSUC_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_STAT_INCPOINTSUC_DB_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_STAT_INCPOINT_DB_REQ`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_STAT_INCPOINT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_STAT_REMAINPOINT_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_SUPPORT_REWARD_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `eUserType` | `USER_TYPE` |

### `PROTO_NC_CHAR_TUTORIAL_DOING_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_TUTORIAL_FREESTAT_INIT_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAR_TUTORIAL_FREESTAT_INIT_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `nFreeStat` | `unsigned char[5]   /* 5 bytes */` |

### `PROTO_NC_CHAR_TUTORIAL_INFO_WORLD_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `TutorialInfo` | `PROTO_TUTORIAL_INFO` |

### `PROTO_NC_CHAR_TUTORIAL_INFO_ZONE_CMD`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `TutorialInfo` | `PROTO_TUTORIAL_INFO` |

### `PROTO_NC_CHAR_TUTORIAL_MAKE_ITEM_ACK`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_CHAR_TUTORIAL_MAKE_ITEM_REQ`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_CHAR_TUTORIAL_POPUP_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_TUTORIAL_POPUP_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_TUTORIAL_STEP_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_TUTORIAL_STEP_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_TUTORIAL_STEP_SAVE_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `TutorialInfo` | `PROTO_TUTORIAL_INFO` |

### `PROTO_NC_CHAR_TUTORIAL_STEP_SAVE_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `TutorialInfo` | `PROTO_TUTORIAL_INFO` |

### `PROTO_NC_CHAR_TUTORIAL_STEP_SAVE_REQ`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `TutorialInfo` | `PROTO_TUTORIAL_INFO` |

### `PROTO_NC_CHAR_USEITEM_MINIMON_CHARGED_ITEM_OFF_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_USEITEM_MINIMON_CHARGED_ITEM_OFF_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_USEITEM_MINIMON_CHARGED_ITEM_ON_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAR_USEITEM_MINIMON_CHARGED_ITEM_ON_REQ`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ChargedItemList` | `unsigned short[12]   /* 24 bytes */` |

### `PROTO_NC_CHAR_USEITEM_MINIMON_INFO_CLIENT_CMD`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `UseItemMinimonInfo` | `USEITEM_MINIMON_INFO` |

### `PROTO_NC_CHAR_USEITEM_MINIMON_INFO_CMD`  (sizeof = 56 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `UseItemMinimonInfo` | `USEITEM_MINIMON_INFO` |

### `PROTO_NC_CHAR_USEITEM_MINIMON_INFO_ZONE_CMD`  (sizeof = 56 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `UseItemMinimonInfo` | `USEITEM_MINIMON_INFO` |

### `PROTO_NC_CHAR_USEITEM_MINIMON_NORMAL_ITEM_OFF_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_USEITEM_MINIMON_NORMAL_ITEM_OFF_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_USEITEM_MINIMON_NORMAL_ITEM_ON_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAR_USEITEM_MINIMON_NORMAL_ITEM_ON_REQ`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `NormalItemList` | `unsigned short[12]   /* 24 bytes */` |

### `PROTO_NC_CHAR_USEITEM_MINIMON_NOTICE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAR_USEITEM_MINIMON_USE_BROAD_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_CHAR_WEDDINGDATA_ACK`  (sizeof = 43 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `Status` | `PROTO_NC_CHAR_WEDDINGDATA_ACK::<unnamed-type-Status>` |
|     9 | `When` | `ShineDateTime` |
|    13 | `SubWhen` | `ShineDateTime` |
|    13 | `ApplicationLimit` | `ShineDateTime` |
|    13 | `DelayLimit` | `ShineDateTime` |
|    13 | `DivorceLimit` | `ShineDateTime` |
|    13 | `NewlyLimit` | `ShineDateTime` |
|    17 | `Partner` | `PROTO_NC_CHAR_WEDDINGDATA_ACK::<unnamed-type-Partner>` |

### `PROTO_NC_CHAR_WEDDINGDATA_ACK::<unnamed-type-Partner>`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Name` | `Name5` |

### `PROTO_NC_CHAR_WEDDINGDATA_ACK::<unnamed-type-Status>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Main` | `<LF_BITFIELD:0x1f9eb>` |
|     0 | `Sub` | `<LF_BITFIELD:0x20bfa>` |

### `PROTO_NC_CHAR_WEDDINGDATA_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_WEDDING_CANCEL_WEDDING`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAR_WEDDING_DIVORCE_CANCEL_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_WEDDING_DIVORCE_CANCEL_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_WEDDING_DIVORCE_DO_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `PartnerInfo` | `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_ACK::<unnamed-type-PartnerInfo>` |

### `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_ACK::<unnamed-type-PartnerInfo>`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `LastConnect` | `ShineDateTime` |

### `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_RNG`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |
|     7 | `PartnerInfo` | `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_RNG::<unnamed-type-PartnerInfo>` |

### `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_RNG::<unnamed-type-PartnerInfo>`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_CHAR_WEDDING_PARTNER_SUMMON_RNG`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|    11 | `map` | `Name3` |
|    23 | `coord` | `SHINE_XY_TYPE` |

### `PROTO_NC_CHAR_WEDDING_PROPOSE_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_WEDDING_PROPOSE_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_WEDDING_REFRESH_INFO_RNG`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |

### `PROTO_NC_CHAR_WORLD_AUTO_PICK_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_WORLD_REST_EXP_LAST_EXEC_TIME_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAR_ZONE_AUTO_PICK_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_ZONE_CHARDATAFAIL_ACK_SEND`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `header` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_ERRORCODE` |

### `PROTO_NC_CHAR_ZONE_CHARDATA_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAR_ZONE_CHARDATA_ACK_SEND`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `header` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_CHAR_ZONE_CHARDATA_ACK` |

### `PROTO_NC_CHAR_ZONE_CHARDATA_REQ`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `charid` | `Name5` |

### `PROTO_NC_CHAR_ZONE_CHAT_COLOR_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_CHAR_ZONE_GUILD_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAT_RESTRICT_ADD_ACK`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `ChatRestrictData` | `PROTO_CHAT_RESTRICT_INFO` |

### `PROTO_NC_CHAT_RESTRICT_ADD_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sChatRestrictCharID` | `Name5` |

### `PROTO_NC_CHAT_RESTRICT_DB_ADD_ACK`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `ChatRestrictData` | `PROTO_CHAT_RESTRICT_INFO` |

### `PROTO_NC_CHAT_RESTRICT_DB_ADD_REQ`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sChatRestrictCharID` | `Name5` |

### `PROTO_NC_CHAT_RESTRICT_DB_DEL_ACK`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_CHAT_RESTRICT_DB_DEL_ALL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CHAT_RESTRICT_DB_DEL_ALL_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CHAT_RESTRICT_DB_DEL_REQ`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_CHAT_RESTRICT_DB_LIST_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `ChatRestrictData` | `PROTO_CHAT_RESTRICT_DB_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHAT_RESTRICT_DEL_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sChatRestrictCharID` | `Name5` |

### `PROTO_NC_CHAT_RESTRICT_DEL_ALL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_CHAT_RESTRICT_DEL_ALL_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAT_RESTRICT_DEL_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sChatRestrictCharID` | `Name5` |

### `PROTO_NC_CHAT_RESTRICT_LIST_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `ChatRestrictData` | `PROTO_CHAT_RESTRICT_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHER_EVENT_ATTENDANCE_CHANGE_START_CMD`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `tEventStart` | `tm` |

### `PROTO_NC_COLLECT_BOOKMARK_REGIST_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_COLLECT_BOOKMARK_REGIST_DB_ACK`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_COLLECT_BOOKMARK_REGIST_DB_REQ`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_COLLECT_BOOKMARK_REGIST_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_COLLECT_CARDOPEN_CMD`  (sizeof = 106 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `CardInform` | `SHINE_ITEM_STRUCT` |

### `PROTO_NC_COLLECT_CARDREGIST_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_COLLECT_CARDREGIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_COLLECT_CHARACTERDB_UP_GET_CARDCOUNT_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_COLLECT_PEEPING_COLLECT_ACK`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charname` | `Name5` |
|    22 | `collection` | `PROTO_NC_CHAR_CARDCOLLECT_CMD` |

### `PROTO_NC_COLLECT_PEEPING_COLLECT_BOOKMARK_ACK`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charname` | `Name5` |
|    22 | `collectionview` | `PROTO_NC_CHAR_CARDCOLLECT_BOOKMARK_CMD` |

### `PROTO_NC_COLLECT_PEEPING_DB_COLLECT_ACK`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `targetcharname` | `Name5` |
|    28 | `collection` | `PROTO_NC_CHAR_CARDCOLLECT_CMD` |

### `PROTO_NC_COLLECT_PEEPING_DB_COLLECT_BOOKMARK_ACK`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `targetcharname` | `Name5` |
|    28 | `collectionview` | `PROTO_NC_CHAR_CARDCOLLECT_BOOKMARK_CMD` |

### `PROTO_NC_COLLECT_PEEPING_DB_REQ`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `targetcharname` | `Name5` |

### `PROTO_NC_COLLECT_PEEPING_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charname` | `Name5` |

### `PROTO_NC_COLLECT_REGIST_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `CardInfo` | `PROTO_NC_COLLECT_CARDREGIST_ACK` |

### `PROTO_NC_COLLECT_REGIST_REQ`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `CardKey` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `CardInfo` | `PROTO_NC_COLLECT_CARDREGIST_ACK` |

### `PROTO_NC_COLLECT_REWARD_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_COLLECT_REWARD_DB_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_COLLECT_REWARD_DB_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_COLLECT_REWARD_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CT_ADD_FRIEND_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CT_CHARTTING_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CT_DB_SET_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `Info` | `CT_INFO` |

### `PROTO_NC_CT_LUASCRIPT_SET_WORLD_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `CTInfo` | `CT_INFO` |

### `PROTO_NC_CT_LUASCRIPT_SET_ZONE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `CTInfo` | `CT_INFO` |

### `PROTO_NC_CT_SET_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Info` | `CT_INFO` |

### `PROTO_NC_CT_SET_CURRENT_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_CT_SET_CURRENT_DB_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    10 | `UseTitle` | `CT_INFO` |
|    12 | `UseAbleTitle` | `CT_INFO` |

### `PROTO_NC_CT_SET_CURRENT_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_CT_SET_SOMEONECHANGE_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_DATA_PRISON_ADD_GM_ACK`  (sizeof = 110 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sImprison` | `Name5` |
|    30 | `sReason` | `wchar_t[16]   /* 16 bytes */` |
|    46 | `sRemark` | `wchar_t[64]   /* 64 bytes */` |

### `PROTO_NC_DATA_PRISON_ADD_GM_REQ`  (sizeof = 364 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sGmUserID` | `Name256Byte` |
|   262 | `sImprison` | `Name5` |
|   284 | `sReason` | `wchar_t[16]   /* 16 bytes */` |
|   300 | `sRemark` | `wchar_t[64]   /* 64 bytes */` |

### `PROTO_NC_DATA_PRISON_ALTER_GM_ACK`  (sizeof = 110 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sImprison` | `Name5` |
|    30 | `sReason` | `wchar_t[16]   /* 16 bytes */` |
|    46 | `sRemark` | `wchar_t[64]   /* 64 bytes */` |

### `PROTO_NC_DATA_PRISON_ALTER_GM_REQ`  (sizeof = 364 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sGmUserID` | `Name256Byte` |
|   262 | `sImprison` | `Name5` |
|   284 | `sReason` | `wchar_t[16]   /* 16 bytes */` |
|   300 | `sRemark` | `wchar_t[64]   /* 64 bytes */` |

### `PROTO_NC_DATA_PRISON_GET_ACK`  (sizeof = 84 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sReason` | `wchar_t[16]   /* 16 bytes */` |
|    20 | `sRemark` | `wchar_t[64]   /* 64 bytes */` |

### `PROTO_NC_DATA_PRISON_GET_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_DATA_PRISON_UPDATE_MIN_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_DATA_REPORT_ADD_ACK`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sHarmer` | `Name5` |

### `PROTO_NC_DATA_REPORT_ADD_REQ`  (sizeof = 2369 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `sReporterCharID` | `Name5` |
|    27 | `sHarmer` | `Name5` |
|    47 | `byReportType` | `unsigned char[16]   /* 16 bytes */` |
|    63 | `byReason` | `unsigned char[256]   /* 256 bytes */` |
|   321 | `byChatLog` | `unsigned char[2048]   /* 2048 bytes */` |

### `PROTO_NC_DATA_REPORT_CANCEL_ACK`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_DATA_REPORT_CANCEL_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_DATA_REPORT_GET_ACK`  (sizeof = 137 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `byRemark` | `unsigned char[128]   /* 128 bytes */` |

### `PROTO_NC_DATA_REPORT_GET_REQ`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_BETTING_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_BETTING_CANCEL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_BETTING_CANCEL_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `BettingCancelInfo` | `DiceTaiSaiBetting` |

### `PROTO_NC_DICE_TAISAI_BETTING_CANCEL_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BettingCancelInfo` | `DiceTaiSaiBetting` |

### `PROTO_NC_DICE_TAISAI_BETTING_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `BettingInfo` | `DiceTaiSaiBetting` |

### `PROTO_NC_DICE_TAISAI_BETTING_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BettingInfo` | `DiceTaiSaiBetting` |

### `PROTO_NC_DICE_TAISAI_BET_START_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bModeType` | `DICE_TAISAI_GAME_MODE` |
|     4 | `CurrentRollingDice` | `DiceTaiSaiInfo` |

### `PROTO_NC_DICE_TAISAI_BOARD_FOLD_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_BOARD_FOLD_RESERVE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_BOARD_PITCH_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_DB_DICE_ROLL_RESULT_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `header` | `NETPACKETZONEHEADER` |
|    12 | `nActiveDiceTaiSaiKey` | `SHINE_ITEM_REGISTNUMBER` |
|    22 | `DiceRollResult` | `DiceRollApplyResult[?] /* 0 bytes */` |

### `PROTO_NC_DICE_TAISAI_DB_DICE_ROLL_RESULT_LOG_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nActiveDiceTaiSaiKey` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `DiceRollResultLog` | `DiceRollApplyResultLog[?] /* 0 bytes */` |

### `PROTO_NC_DICE_TAISAI_DB_DICE_ROLL_RESULT_REQ`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nActiveDiceTaiSaiKey` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `DiceNum` | `DiceTaiSaiInfo` |
|    19 | `DiceRollResult` | `DiceRollApplyResult[?] /* 0 bytes */` |

### `PROTO_NC_DICE_TAISAI_DB_EXPECT_INCOME_MONEY_ACK`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `header` | `NETPACKETZONEHEADER` |
|     8 | `nInvenSlot` | `ITEM_INVEN` |
|    10 | `nTaiSaiItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_DICE_TAISAI_DB_EXPECT_INCOME_MONEY_REQ`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nInvenSlot` | `ITEM_INVEN` |
|     8 | `nTaiSaiItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_DICE_TAISAI_DB_ITEM_ID_CHANGE_ACK`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `header` | `NETPACKETZONEHEADER` |
|     8 | `nInvenSlot` | `ITEM_INVEN` |
|    12 | `nDiceTaiSaiKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_DICE_TAISAI_DB_ITEM_ID_CHANGE_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nInvenSlot` | `ITEM_INVEN` |
|    10 | `nDiceTaiSaiKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_DICE_TAISAI_DB_RANK_ACK`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `header` | `NETPACKETZONEHEADER` |
|    13 | `Rank` | `DiceRankInfo[?] /* 0 bytes */` |

### `PROTO_NC_DICE_TAISAI_DB_RANK_REQ`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    10 | `nTaiSaiItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_DICE_TAISAI_DB_RECEIPT_INCOME_MONEY_ACK`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `header` | `NETPACKETZONEHEADER` |
|    10 | `nInvenSlot` | `ITEM_INVEN` |
|    12 | `nTaiSaiItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_DICE_TAISAI_DB_RECEIPT_INCOME_MONEY_REQ`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `header` | `NETPACKETZONEHEADER` |
|     8 | `nInvenSlot` | `ITEM_INVEN` |
|    10 | `nTaiSaiItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_DICE_TAISAI_DICE_ROLL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_DICE_ROLL_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_DICE_ROLL_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_DICE_ROLL_RESULT_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `nDiceInfo` | `DiceTaiSaiInfo` |

### `PROTO_NC_DICE_TAISAI_DICE_ROLL_RESULT_EMOTION_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Emotion` | `EmotionInfo[?] /* 0 bytes */` |

### `PROTO_NC_DICE_TAISAI_EXPECT_INCOME_MONEY_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `nInvenSlot` | `ITEM_INVEN` |

### `PROTO_NC_DICE_TAISAI_EXPECT_INCOME_MONEY_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nInvenSlot` | `ITEM_INVEN` |

### `PROTO_NC_DICE_TAISAI_GAME_BETTING_INFO_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `Betting` | `BettingInfo[?] /* 0 bytes */` |

### `PROTO_NC_DICE_TAISAI_GAME_JOIN_ACK`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `DiceHistory` | `DiceTaiSaiInfo[?] /* 21 bytes */` |
|    23 | `nGameStatus` | `DICE_TAISAI_GAME_STATUS` |
|    27 | `bModeType` | `DICE_TAISAI_GAME_MODE` |
|    50 | `nGamerHnd` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_DICE_TAISAI_GAME_JOIN_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_GAME_JOIN_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_GAME_LEAVE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_GAME_LEAVE_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_GAME_LEAVE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_GAME_MODE_CHANGE_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `bModeType` | `DICE_TAISAI_GAME_MODE` |

### `PROTO_NC_DICE_TAISAI_GAME_MODE_CHANGE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bModeType` | `DICE_TAISAI_GAME_MODE` |

### `PROTO_NC_DICE_TAISAI_GAME_MODE_CHANGE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_LARGE_AMOUNT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_LARGE_AMOUNT_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `LargeAmount` | `LargeAmountInfo[?] /* 0 bytes */` |

### `PROTO_NC_DICE_TAISAI_LARGE_AMOUNT_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_RANK_ACK`  (sizeof = 484 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `RankHistory` | `DiceRankInfo[?] /* 480 bytes */` |
|   484 | `RankCurr` | `DiceRankInfo[?] /* 0 bytes */` |

### `PROTO_NC_DICE_TAISAI_RANK_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_DICE_TAISAI_RECEIPT_INCOME_MONEY_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `nInvenSlot` | `ITEM_INVEN` |

### `PROTO_NC_DICE_TAISAI_RECEIPT_INCOME_MONEY_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nInvenSlot` | `ITEM_INVEN` |

### `PROTO_NC_DICE_TAISAI_TIMER_CMD`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_EVENT_ADD_EVENT_ACK`  (sizeof = 1365 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `EventData` | `GM_EVENT_DATA` |

### `PROTO_NC_EVENT_ADD_EVENT_REQ`  (sizeof = 1363 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `EventData` | `GM_EVENT_DATA` |

### `PROTO_NC_EVENT_ADD_UPDATE_EVENT_CMD`  (sizeof = 1363 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `EventData` | `GM_EVENT_DATA` |

### `PROTO_NC_EVENT_DEL_EVENT_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_EVENT_DEL_EVENT_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_EVENT_DEL_EVENT_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_EVENT_GET_ALL_EVENT_INFO_ACK`  (sizeof = 1370 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `EventData` | `GM_EVENT_DATA` |

### `PROTO_NC_EVENT_GET_ALL_EVENT_INFO_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_EVENT_SET_ALL_READY_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_EVENT_SET_ALL_READY_REQ`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_EVENT_UPDATE_EVENT_ACK`  (sizeof = 1365 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `EventData` | `GM_EVENT_DATA` |

### `PROTO_NC_EVENT_UPDATE_EVENT_REQ`  (sizeof = 1363 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `EventData` | `GM_EVENT_DATA` |

### `PROTO_NC_FRIEND_ADD_CMD`  (sizeof = 72 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `friendinfo` | `PROTO_FRIEND_INFO` |

### `PROTO_NC_FRIEND_CLASS_CHANGE_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |

### `PROTO_NC_FRIEND_DB_DEL_ACK`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `charid` | `Name5` |
|    26 | `friendid` | `Name5` |

### `PROTO_NC_FRIEND_DB_DEL_REQ`  (sizeof = 46 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `charid` | `Name5` |
|    26 | `friendid` | `Name5` |

### `PROTO_NC_FRIEND_DB_GET_ACK`  (sizeof = 84 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `friendinfo` | `PROTO_FRIEND_INFO` |

### `PROTO_NC_FRIEND_DB_GET_REQ`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_FRIEND_DB_POINT`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_FRIEND_DB_POINT_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `pointinfo` | `PROTO_NC_FRIEND_DB_POINT[?] /* 0 bytes */` |

### `PROTO_NC_FRIEND_DB_SET_ACK`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_FRIEND_DB_SET_REQ`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_FRIEND_DEL_ACK`  (sizeof = 42 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |
|    20 | `friendid` | `Name5` |

### `PROTO_NC_FRIEND_DEL_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |

### `PROTO_NC_FRIEND_DEL_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |
|    20 | `friendid` | `Name5` |

### `PROTO_NC_FRIEND_FIND_FRIENDS_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `friendinfo` | `PROTO_FRIEND_INFO[?] /* 0 bytes */` |

### `PROTO_NC_FRIEND_FIND_FRIENDS_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_FRIEND_GET_DIFF_FRIEND_POINT_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_FRIEND_LEVEL_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |

### `PROTO_NC_FRIEND_LIST_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `friendinfo` | `PROTO_FRIEND_INFO[?] /* 0 bytes */` |

### `PROTO_NC_FRIEND_LOGIN_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |
|    20 | `map` | `Name3` |

### `PROTO_NC_FRIEND_LOGOUT_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |

### `PROTO_NC_FRIEND_MAP_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |
|    20 | `map` | `Name3` |

### `PROTO_NC_FRIEND_PARTY_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |

### `PROTO_NC_FRIEND_POINT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_FRIEND_POINT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_FRIEND_REFUSE_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `friendid` | `Name5` |

### `PROTO_NC_FRIEND_SET_ACK`  (sizeof = 42 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |
|    20 | `friendid` | `Name5` |

### `PROTO_NC_FRIEND_SET_CONFIRM_ACK`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |
|    20 | `friendid` | `Name5` |

### `PROTO_NC_FRIEND_SET_CONFIRM_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |
|    20 | `friendid` | `Name5` |

### `PROTO_NC_FRIEND_SET_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |
|    20 | `friendid` | `Name5` |

### `PROTO_NC_FRIEND_SOMEONE_GET_SPECIALITEM_WORLD_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |

### `PROTO_NC_FRIEND_SOMEONE_GET_SPECIALITEM_ZONE_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |

### `PROTO_NC_FRIEND_UES_FRIEND_POINT_ACK`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_FRIEND_UES_FRIEND_POINT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_COIN_BUY_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GAMBLE_COIN_BUY_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GAMBLE_COIN_CHANGE_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GAMBLE_COIN_DB_ADD_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GAMBLE_COIN_DB_ADD_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GAMBLE_COIN_DB_BUY_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GAMBLE_COIN_DB_BUY_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GAMBLE_COIN_DB_SELL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GAMBLE_COIN_DB_SELL_REQ`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GAMBLE_COIN_DB_USE_COINITEM_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GAMBLE_COIN_DB_USE_COINITEM_REQ`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nItemReg` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_GAMBLE_COIN_EXCHANGEMACHINE_UI_CLOSE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_COIN_EXCHANGEMACHINE_UI_OPEN_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_COIN_SELL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GAMBLE_COIN_SELL_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GAMBLE_COIN_USE_COINITEM_MESSAGE_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GAMBLE_COIN_VIPCARD_UI_CLOSE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_COIN_VIPCARD_UI_OPEN_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GAMBLE_COIN_VIPCARD_UI_OPEN_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_ALL_RANK_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `RankAll` | `GDT_DiceRankInfo[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_ALL_RANK_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_BETTING_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_BETTING_CANCEL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_BETTING_CANCEL_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `BettingCancelInfo` | `DiceTaiSaiBetting` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_BETTING_CANCEL_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BettingCancelInfo` | `DiceTaiSaiBetting` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_BETTING_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `BettingInfo` | `DiceTaiSaiBetting` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_BETTING_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BettingInfo` | `DiceTaiSaiBetting` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_BET_START_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bModeType` | `DICE_TAISAI_GAME_MODE` |
|     4 | `CurrentRollingDice` | `DiceTaiSaiInfo` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_CURR_RANK_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `RankCurr` | `GDT_DiceRankInfo[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_CURR_RANK_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_DB_ALL_RANK_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `header` | `NETPACKETZONEHEADER` |
|     9 | `RankAll` | `GDT_DiceRankInfo[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_DB_ALL_RANK_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_DB_DICE_ROLL_RESULT_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `header` | `NETPACKETZONEHEADER` |
|    14 | `DiceRollResult` | `GDT_DiceRollApplyResult[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_DB_DICE_ROLL_RESULT_LOG_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    12 | `DiceRollResultLog` | `GDT_DiceRollApplyResultLog[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_DB_DICE_ROLL_RESULT_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `DiceNum` | `DiceTaiSaiInfo` |
|    11 | `DiceRollResult` | `GDT_DiceRollApplyResult[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_DICE_ROLL_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_DICE_ROLL_RESULT_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `nDiceInfo` | `DiceTaiSaiInfo` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_DICE_ROLL_RESULT_EMOTION_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Emotion` | `GDT_EmotionInfo[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_BETTING_INFO_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `Betting` | `GDT_BettingInfo[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_JOIN_ACK`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `DiceHistory` | `DiceTaiSaiInfo[?] /* 21 bytes */` |
|    23 | `nGameStatus` | `DICE_TAISAI_GAME_STATUS` |
|    27 | `bModeType` | `DICE_TAISAI_GAME_MODE` |
|    50 | `nGamerHnd` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_JOIN_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_JOIN_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_LEAVE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_LEAVE_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_LEAVE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_LARGE_AMOUNT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_LARGE_AMOUNT_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `LargeAmount` | `GDT_LargeAmountInfo[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_DICE_TAISAI_LARGE_AMOUNT_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GAMBLE_DICE_TAISAI_TIMER_CMD`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_GAMBLE_ENTER_PLAYER_DIRECT_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_GAMBLE_EXCHANGECOIN_CHANGE_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GAMBLE_EXCHANGEDCOIN_DB_INFO_ACK`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GAMBLE_EXCHANGEDCOIN_DB_INFO_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GAMBLE_EXCHANGEDCOIN_INIT_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GAMBLE_GAMBLEHOUSE_EXIT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GAMBLE_GAMBLEHOUSE_EXIT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_GAMBLEHOUSE_UI_CLOSE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_GAMBLEHOUSE_UI_OPEN_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_PLAYERACT_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GAMBLE_SLOTMACHINE_DB_GAMEINFO_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `RankList` | `SLOTMACHINE_JackPotRank[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_SLOTMACHINE_DB_GAMEINFO_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `MachineIndex` | `PROTO_NC_GAMBLE_SLOTMACHINE_DB_GAMEINFO_REQ::<unnamed-type-MachineIndex>[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_SLOTMACHINE_DB_GAMEINFO_REQ::<unnamed-type-MachineIndex>`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_GAMBLE_SLOTMACHINE_DB_JACKPOT_RANK_UPDATE_CMD`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `NewJackpot` | `SLOTMACHINE_JackPotRank` |

### `PROTO_NC_GAMBLE_SLOTMACHINE_DB_RESULT_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GAMBLE_SLOTMACHINE_DB_RESULT_REQ`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    12 | `CardDisplay` | `wchar_t[10]   /* 10 bytes */` |

### `PROTO_NC_GAMBLE_SLOTMACHINE_GAME_JOIN_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GAMBLE_SLOTMACHINE_GAME_JOIN_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GAMBLE_SLOTMACHINE_GAME_LEAVE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GAMBLE_SLOTMACHINE_GAME_LEAVE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_SLOTMACHINE_JACKPOTINFO_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `JackPotList` | `PROTO_NC_GAMBLE_SLOTMACHINE_JACKPOTINFO_CMD::<unnamed-type-JackPotList>[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_SLOTMACHINE_JACKPOTINFO_CMD::<unnamed-type-JackPotList>`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GAMBLE_SLOTMACHINE_JACKPOTRANKING_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `RankList` | `SLOTMACHINE_JackPotRank[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_SLOTMACHINE_JACKPOTRANKING_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_SLOTMACHINE_SOMEONE_GET_JACKPOT_CMD`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |

### `PROTO_NC_GAMBLE_SLOTMACHINE_STAND_UP_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_SLOTMACHINE_START_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GAMBLE_SLOTMACHINE_START_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `betting` | `SLOTMACHINE_BettingLine` |

### `PROTO_NC_GAMBLE_SLOTMACHINE_STOPBUTTON_ACK`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `screen` | `SLOTMACHINE_ScreenState` |

### `PROTO_NC_GAMBLE_SLOTMACHINE_STOPBUTTON_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_SLOTMACHINE_WHEELSTOP_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `WinLine` | `SLOTMACHINE_WinState` |

### `PROTO_NC_GAMBLE_SLOTMACHINE_WHEELSTOP_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_SLOTMACHINE_WINRANKING_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `RankList` | `SLOTMACHINE_JackPotRank[?] /* 0 bytes */` |

### `PROTO_NC_GAMBLE_SLOTMACHINE_WINRANKING_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GAMBLE_TYPE_AND_WHERE_STAND_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `nGambleType` | `GAMBLE_TYPE` |

### `PROTO_NC_GAMBLE_TYPE_AND_WHERE_STAND_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GAMBLE_WORLD_PREVMAPNAME_CMD`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sPrevMapName` | `Name3` |

### `PROTO_NC_GAMBLE_ZONE_PREVMAPNAME_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sPrevMapName` | `Name3` |

### `PROTO_NC_GAMIGO_NEW_TUTORIAL_CHAR_ENTER_GAME`  (sizeof = 43 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `TutorialInfo` | `PROTO_TUTORIAL_INFO` |
|    13 | `UserID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_CHAT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_CHAT_CMD`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `talker` | `Name5` |
|    24 | `chat` | `PROTO_NC_GUILD_CHAT_REQ` |

### `PROTO_NC_GUILD_ACADEMY_CHAT_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_CLEAR_REWARD_ITEM_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_CLEAR_REWARD_ITEM_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_CLEAR_REWARD_MONEY_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_CLEAR_REWARD_MONEY_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_ACADEMY_REWARD_ACK`  (sizeof = 61 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|    28 | `WantItem` | `GUILD_ACADEMY_REWARD_ITEM` |
|    61 | `ModifyItem` | `EACH_MODIFY_ITEM[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_DB_ACADEMY_REWARD_REQ`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GUILD_ACADEMY_DB_CLEAR_REWARD_ITEM_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_CLEAR_REWARD_ITEM_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_CLEAR_REWARD_MONEY_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_CLEAR_REWARD_MONEY_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_GET_GRADUATE_ACK`  (sizeof = 69 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `GraduateData` | `GRADUATE_DATA` |

### `PROTO_NC_GUILD_ACADEMY_DB_GET_GRADUATE_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_GET_RANKING_LIST_ACK`  (sizeof = 53 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `RankData` | `GUILD_ACADEMY_RANKING_DATA` |

### `PROTO_NC_GUILD_ACADEMY_DB_GET_RANKING_LIST_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_GET_REWARD_ITEM_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `RewardItem` | `GUILD_ACADEMY_REWARD_ITEM[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_DB_GET_REWARD_ITEM_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_GET_REWARD_MONEY_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `RewardMoney` | `GUILD_ACADEMY_REWARD_MONEY[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_DB_GET_REWARD_MONEY_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_GRADUATE_JOIN_ACK`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_GRADUATE_JOIN_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_GRADUATE_JOIN_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `sName` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_DB_LEVEL_UP_ACK`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_LEVEL_UP_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_LEVEL_UP_REQ`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_MEMBER_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `AcademyMemberList` | `GUILD_ACADEMY_MEMEBER_INFO[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_DB_MEMBER_GUILD_JOIN_ACK`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sInviteCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_DB_MEMBER_GUILD_JOIN_REQ`  (sizeof = 51 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sInviteCharID` | `Name5` |
|    30 | `sTargetCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_DB_MEMBER_JOIN_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    11 | `dJoinInfo` | `SHINE_GUILD_ACADEMY_MEMBER_LOGON_INFO` |

### `PROTO_NC_GUILD_ACADEMY_DB_MEMBER_JOIN_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `sCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_DB_MEMBER_LEAVE_ACK`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_MEMBER_LEAVE_REQ`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `sCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_DB_MEMBER_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_MEMBER_VANISH_ACK`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_DB_MEMBER_VANISH_REQ`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_DB_NOTIFY_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `sNotify` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_DB_NOTIFY_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sNotify` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_DB_RANK_BALANCE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_RANK_BALANCE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_REWARD_WANT_CMD`  (sizeof = 22 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_SET_MASTER_ACK`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sName` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_DB_SET_MASTER_BY_LEAVE_ACK`  (sizeof = 54 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `sOldAcademyMasterName` | `Name5` |
|    34 | `sNewAcademyMasterName` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_DB_SET_MASTER_BY_LEAVE_REQ`  (sizeof = 52 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sOldAcademyMasterName` | `Name5` |
|    32 | `sNewAcademyMasterName` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_DB_SET_MASTER_REQ`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sName` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_DB_SET_REWARD_ITEM_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_SET_REWARD_ITEM_REQ`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `RewardItem` | `GUILD_ACADEMY_REWARD_ITEM[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_DB_SET_REWARD_MONEY_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_DB_SET_REWARD_MONEY_REQ`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `RewardMoney` | `GUILD_ACADEMY_REWARD_MONEY[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_DELETE_CMD`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Name` | `Name4` |

### `PROTO_NC_GUILD_ACADEMY_DISMISS_CMD`  (sizeof = 57 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Name` | `Name4` |
|    21 | `tm_dDismissDate` | `tm` |

### `PROTO_NC_GUILD_ACADEMY_GET_GRADUATE_ACK`  (sizeof = 67 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `GraduateData` | `GRADUATE_DATA` |

### `PROTO_NC_GUILD_ACADEMY_GET_GRADUATE_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_ACADEMY_GET_RANKING_LIST_ACK`  (sizeof = 51 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `RankData` | `GUILD_ACADEMY_RANKING_DATA` |

### `PROTO_NC_GUILD_ACADEMY_GET_RANKING_LIST_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_GET_REWARD_ITEM_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `RewardItem` | `GUILD_ACADEMY_REWARD_ITEM[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_GET_REWARD_ITEM_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_GET_REWARD_MONEY_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `RewardMoney` | `GUILD_ACADEMY_REWARD_MONEY[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_GET_REWARD_MONEY_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_HISTORY_DB_LIST_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `HistoryList` | `GUILD_HISTORY[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_HISTORY_DB_LIST_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_HISTORY_LIST_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `HistoryList` | `GUILD_HISTORY[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_HISTORY_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_LIST_ACK`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    11 | `GuildAcademyList` | `SHINE_GUILD_ACADEMY_LIST[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MASTER_TELEPORT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MASTER_TELEPORT_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sTargetCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_CHAT_BAN_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MEMBER_CHAT_BAN_CANCEL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MEMBER_CHAT_BAN_CANCEL_CMD`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sChatBanCancelSourceCharID` | `Name5` |
|    20 | `sChatBanCancelTargetCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_CHAT_BAN_CANCEL_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sChatBanCancelTargetCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_CHAT_BAN_CMD`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sChatBanSourceCharID` | `Name5` |
|    20 | `sChatBanTargetCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_CHAT_BAN_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sChatBanTargetCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_CLASS_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_DB_CHAT_BAN_ACK`  (sizeof = 18 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MEMBER_DB_CHAT_BAN_CANCEL_ACK`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MEMBER_DB_CHAT_BAN_CANCEL_REQ`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MEMBER_DB_CHAT_BAN_REQ`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MEMBER_DB_SAVE_CHAT_BAN_TIME_CMD`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MEMBER_GUILD_INVITE_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sInviteTargetCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_GUILD_INVITE_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sInviteTargetCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_GUILD_JOIN_ACK`  (sizeof = 37 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GuildName` | `Name4` |
|    16 | `sInviteSourceCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_GUILD_JOIN_CMD`  (sizeof = 130 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sInviteSourceCharID` | `Name5` |
|    20 | `Member` | `GUILD_MEMBER_CLIENT` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_GUILD_JOIN_ERR`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GuildName` | `Name4` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_GUILD_JOIN_REQ`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GuildName` | `Name4` |
|    16 | `sInviteSourceCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_INTRO_CMD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |
|    22 | `sMemberIntro` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_JOIN_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GuildName` | `Name4` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_JOIN_CMD`  (sizeof = 109 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Member` | `GUILD_ACADEMY_MEMBER_CLIENT` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_JOIN_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_LEAVE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MEMBER_LEAVE_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_LEAVE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MEMBER_LEVEL_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_LIST_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `AcademyMemberList` | `GUILD_ACADEMY_MEMBER_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MEMBER_LOGOFF_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_LOGON_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_MAP_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |
|    20 | `sMap` | `Name3` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_PARTY_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_VANISH_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_VANISH_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MEMBER_VANISH_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_MY_ACADEMY_RANK_INFO_ACK`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MY_ACADEMY_RANK_INFO_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_MY_GUILD_ACADEMY_INFO_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `GuildAcademyInfo` | `GUILD_ACADEMY_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_MY_GUILD_ACADEMY_INFO_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_NOTIFY_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_NOTIFY_CMD`  (sizeof = 62 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `tm_dNotifyDate` | `tm` |
|    40 | `sNotifyCharID` | `Name5` |
|    62 | `sNotify` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_NOTIFY_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sNotify` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_REWARDSTORAGE_GRADE_INFO_CMD`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_REWARDSTORAGE_ITEM_INFO_ZONE_RNG`  (sizeof = 65 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |
|    12 | `Charid` | `Name5` |
|    33 | `From` | `SHINE_INVEN_SLOT_INFO` |
|    49 | `To` | `SHINE_INVEN_SLOT_INFO` |

### `PROTO_NC_GUILD_ACADEMY_REWARDSTORAGE_REWARD_ZONE_RNG`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |
|    11 | `sCharID` | `Name5` |
|    48 | `Reward_ItemInfo` | `EACH_MODIFY_ITEM[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_CLOSE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_CMD`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `charid` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_RNG`  (sizeof = 53 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|    15 | `charid` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_OPEN_ACK`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    11 | `itemarray` | `PROTO_ITEMPACKET_INFORM[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_OPEN_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_CMD`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `charid` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_GRADE_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_GRADE_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_RNG`  (sizeof = 53 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|    15 | `charid` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_REWARD_WANT_CMD`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_SET_MASTER_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_SET_MASTER_BY_LEAVE_CMD`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sOldAcademyMasterName` | `Name5` |
|    20 | `sNewAcademyMasterName` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_SET_MASTER_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sName` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_SET_MASTER_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sName` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_SET_REWARD_ITEM_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_SET_REWARD_ITEM_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `RewardItem` | `GUILD_ACADEMY_REWARD_ITEM[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_SET_REWARD_MONEY_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_SET_REWARD_MONEY_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `RewardMoney` | `GUILD_ACADEMY_REWARD_MONEY[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_START_DB_ALL_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `GuildAcademyDB` | `GUILD_ACADEMY_DB[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_START_DB_ALL_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_START_DB_GET_RANKING_LIST_ACK`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `RankData` | `GUILD_ACADEMY_START_RANKING_DATA` |

### `PROTO_NC_GUILD_ACADEMY_START_DB_GET_RANKING_LIST_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_START_DB_RANK_BALANCE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_START_DB_RANK_BALANCE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_WAR_END_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_WAR_INFO_LIST_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `WarInfoList` | `PROTO_NC_GUILD_ACADEMY_WAR_INFO_LIST_CMD::WAR_INFO_LIST[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_WAR_INFO_LIST_CMD::WAR_INFO_LIST`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_WAR_START_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_ZONE_GUILD_ACADEMY_MASTER_BUFF_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `Master` | `GUILD_MASTER_BUFF[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_ZONE_GUILD_BUFF_CMD`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BuffName` | `wchar_t[32]   /* 32 bytes */` |
|    36 | `Guild` | `unsigned long[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_ACADEMY_ZONE_MASTER_TELEPORT_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sTargetCharID` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_ZONE_MEMBER_GUILD_JOIN_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_ZONE_MEMBER_JOIN_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_ACADEMY_ZONE_MEMBER_LEAVE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_CHAT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_CHAT_CMD`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `talker` | `Name5` |
|    24 | `chat` | `PROTO_NC_GUILD_CHAT_REQ` |

### `PROTO_NC_GUILD_CHAT_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_DB_ACK`  (sizeof = 1091 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `GuildClient` | `GUILD_CLIENT` |
|  1071 | `GuildServer` | `GUILD_SERVER` |

### `PROTO_NC_GUILD_DB_ALL_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `GuildDB` | `GUILD_DB[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_DB_ALL_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_DB_DELETE_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_DB_DELETE_CANCEL_ACK`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_DB_DELETE_CANCEL_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_DB_DELETE_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_DB_DISMISS_ACK`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_GUILD_DB_DISMISS_REQ`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_GUILD_DB_GRADE_GROWTH_ACK`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_GUILD_DB_GRADE_GROWTH_REQ`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_GUILD_DB_GRADE_GROWTH_REQ_SEND`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `req` | `PROTO_NC_GUILD_DB_GRADE_GROWTH_REQ` |

### `PROTO_NC_GUILD_DB_INTRO_ACK`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GUILD_DB_INTRO_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `sIntro` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_DB_LIST_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `GuildList` | `SHINE_GUILD_LIST[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_DB_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_DB_MAKE_ACK`  (sizeof = 45 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `Make` | `PROTO_NC_GUILD_MAKE_REQ` |

### `PROTO_NC_GUILD_DB_MAKE_REQ`  (sizeof = 55 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sCharID` | `Name5` |
|    26 | `Make` | `PROTO_NC_GUILD_MAKE_REQ` |

### `PROTO_NC_GUILD_DB_MEMBER_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `MemberList` | `GUILD_MEMEBER_INFO[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_DB_MEMBER_GRADE_ACK`  (sizeof = 37 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sCharID` | `Name5` |

### `PROTO_NC_GUILD_DB_MEMBER_GRADE_REQ`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sCharID` | `Name5` |

### `PROTO_NC_GUILD_DB_MEMBER_INTRO_ACK`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GUILD_DB_MEMBER_INTRO_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `sMemberIntro` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_DB_MEMBER_JOIN_ACK`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_GUILD_DB_MEMBER_JOIN_REQ`  (sizeof = 31 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `sCharID` | `Name5` |

### `PROTO_NC_GUILD_DB_MEMBER_LEAVE_ACK`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GUILD_DB_MEMBER_LEAVE_REQ`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `sCharID` | `Name5` |

### `PROTO_NC_GUILD_DB_MEMBER_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_DB_MEMBER_VANISH_ACK`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sCharID` | `Name5` |

### `PROTO_NC_GUILD_DB_MEMBER_VANISH_REQ`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sCharID` | `Name5` |

### `PROTO_NC_GUILD_DB_MONEY_ADD_ACK`  (sizeof = 36 bytes)

_no members_

### `PROTO_NC_GUILD_DB_MONEY_ADD_REQ`  (sizeof = 18 bytes)

_no members_

### `PROTO_NC_GUILD_DB_MONEY_SUB_ACK`  (sizeof = 36 bytes)

_no members_

### `PROTO_NC_GUILD_DB_MONEY_SUB_REQ`  (sizeof = 18 bytes)

_no members_

### `PROTO_NC_GUILD_DB_NOTIFY_ACK`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `sNotifyCharID` | `Name5` |
|    34 | `sNotify` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_DB_NOTIFY_REQ`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sNotifyCharID` | `Name5` |
|    28 | `sNotify` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_DB_RENAME_ACK`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_DB_RENAME_REQ`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_DB_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_DB_RESULT_WRITE_CMD`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Guild1` | `GUILD_DB_RESULT_WRITE` |
|    20 | `Guild2` | `GUILD_DB_RESULT_WRITE` |

### `PROTO_NC_GUILD_DB_RETYPE_ACK`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_GUILD_DB_RETYPE_REQ`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_GUILD_DB_TOKEN_ALL_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `GuildTokenDB` | `GUILD_TOKEN_DB[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_DB_TOKEN_ALL_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_DB_WAR_ACK`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `WarReq` | `PROTO_NC_GUILD_DB_WAR_REQ` |

### `PROTO_NC_GUILD_DB_WAR_REQ`  (sizeof = 22 bytes)

_no members_

### `PROTO_NC_GUILD_DELETE_CMD`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Name` | `Name4` |

### `PROTO_NC_GUILD_DISMISS_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_DISMISS_CMD`  (sizeof = 57 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Name` | `Name4` |
|    21 | `tm_dDismissDate` | `tm` |

### `PROTO_NC_GUILD_DISMISS_REQ`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Name` | `Name4` |
|    16 | `Password` | `Name3` |

### `PROTO_NC_GUILD_EMBLEM_CHECK_AVAILABILITY_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_EMBLEM_CHECK_AVAILABILITY_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_EMBLEM_INFO_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `EmblemInfo` | `GUILD_EMBLEM_INFO` |

### `PROTO_NC_GUILD_EMBLEM_INFO_DB_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `EmblemInfo` | `GUILD_EMBLEM_INFO_DB[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_EMBLEM_INFO_DB_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_EMBLEM_INFO_NOTICE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `EmblemInfo` | `GUILD_EMBLEM_INFO` |

### `PROTO_NC_GUILD_EMBLEM_LEVELUP_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_EMBLEM_OFF_MSG_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_EMBLEM_SAVE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_EMBLEM_SAVE_DB_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `EmblemInfo` | `GUILD_EMBLEM_INFO` |

### `PROTO_NC_GUILD_EMBLEM_SAVE_DB_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `EmblemInfo` | `GUILD_EMBLEM_INFO` |

### `PROTO_NC_GUILD_EMBLEM_SAVE_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `EmblemInfo` | `GUILD_EMBLEM_INFO` |

### `PROTO_NC_GUILD_EMBLEM_STATE_DB_ACK`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_GUILD_EMBLEM_STATE_DB_REQ`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_GUILD_GRADE_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_GUILD_GRADE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_GRADE_GROWTH_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_GRADE_GROWTH_DATA_ACK`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_GUILD_GRADE_GROWTH_DATA_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_GRADE_GROWTH_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_GRADE_GROWTH_ZONE_ACK`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_GRADE_GROWTH_ZONE_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_GRADE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_GUILDWARCONFIRM_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `guildwarA` | `PROTO_NC_GUILD_ZONE_WAR_START_CMD` |
|     8 | `guildwarB` | `PROTO_NC_GUILD_ZONE_WAR_START_CMD` |

### `PROTO_NC_GUILD_GUILDWARCONFIRM_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_GUILDWARSTATUS_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `war` | `PROTO_NC_GUILD_ZONE_WAR_START_CMD[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_GUILDWARSTATUS_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_HISTORY_DB_LIST_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `HistoryList` | `GUILD_HISTORY[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_HISTORY_DB_LIST_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_HISTORY_LIST_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `HistoryList` | `GUILD_HISTORY[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_HISTORY_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_INFO_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_INFO_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_LIST_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `GuildList` | `SHINE_GUILD_LIST_NEW[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_MAKE_ACK`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `Name` | `Name4` |

### `PROTO_NC_GUILD_MAKE_REQ`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Name` | `Name4` |
|    16 | `Password` | `Name3` |

### `PROTO_NC_GUILD_MEMBER_CLASS_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_EXP_RATIO_CMD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_FLAGS_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_GRADE_ACK`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_GRADE_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_GRADE_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_INTRO_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_MEMBER_INTRO_CMD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |
|    22 | `sMemberIntro` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_MEMBER_INTRO_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sMemberIntro` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_MEMBER_INVITE_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ToCharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_INVITE_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ToCharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_JOIN_ACK`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GuildName` | `Name4` |

### `PROTO_NC_GUILD_MEMBER_JOIN_CMD`  (sizeof = 110 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Member` | `GUILD_MEMBER_CLIENT` |

### `PROTO_NC_GUILD_MEMBER_JOIN_ERR`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GuildName` | `Name4` |

### `PROTO_NC_GUILD_MEMBER_JOIN_REQ`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GuildName` | `Name4` |
|    16 | `FromCharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_LEAVE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_MEMBER_LEAVE_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_LEAVE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_MEMBER_LEVEL_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_LIST_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `MemberList` | `GUILD_MEMBER_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_MEMBER_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_MEMBER_LOGOFF_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_LOGON_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_MAP_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |
|    20 | `sMap` | `Name3` |

### `PROTO_NC_GUILD_MEMBER_PARTY_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_VANISH_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_VANISH_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MEMBER_VANISH_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MOBGUILD_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_MONEY_ADD_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_GUILD_MONEY_ADD_CMD`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MONEY_ADD_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_MONEY_SUB_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_GUILD_MONEY_SUB_CMD`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_MONEY_SUB_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_MY_GUILD_INFO_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `GuildInfo` | `GUILD_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_MY_GUILD_INFO_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_MY_GUILD_TOKEN_INFO_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_GUILD_MY_GUILD_TOKEN_INFO_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_MY_GUILD_TOURNAMENT_MATCH_TIME_ACK`  (sizeof = 45 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `GTCondInfo` | `GT_COND_INFO` |

### `PROTO_NC_GUILD_MY_GUILD_TOURNAMENT_MATCH_TIME_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_NAME_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `Name` | `Name4` |
|    20 | `EmblemInfo` | `GUILD_EMBLEM_INFO` |

### `PROTO_NC_GUILD_NAME_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_NOTIFY_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_NOTIFY_CMD`  (sizeof = 62 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `tm_dNotifyDate` | `tm` |
|    40 | `sNotifyCharID` | `Name5` |
|    62 | `sNotify` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_NOTIFY_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sNotify` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_RENAME_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_RENAME_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_RENAME_CMD_SEND`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_GUILD_RENAME_CMD` |

### `PROTO_NC_GUILD_RENAME_REQ`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_RETYPE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_RETYPE_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_GUILD_RETYPE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_STORAGEOPEN_ACK`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    19 | `itemarray` | `PROTO_ITEMPACKET_INFORM[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_STORAGEWITHDRAW_CMD`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `charid` | `Name5` |

### `PROTO_NC_GUILD_STORAGEWITHDRAW_RNG`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|    14 | `charid` | `Name5` |

### `PROTO_NC_GUILD_STORAGE_WITHDRAW_GRADE_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GUILD_STORAGE_WITHDRAW_GRADE_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GUILD_TOURNAMENTSTART_CMD`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENTSTOPMSG_CMD`  (sizeof = 20 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENTSTOP_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_FINAL_SELECTION_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `nFSGuildNo` | `unsigned long[0]   /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_DB_FINAL_SELECTION_REQ`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_GET_ACK`  (sizeof = 204 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `MatchTime` | `long[9]   /* 36 bytes */` |
|    43 | `TournamentTree` | `GUILD_TOURNAMENT_LIST_DB[?] /* 155 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_DB_GET_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_JOIN_ACK`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_JOIN_LIST_ACK`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    11 | `JoinGuildList` | `SHINE_GUILD_LIST_NEW[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_DB_JOIN_LIST_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_JOIN_NEW_ACK`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GUILD_TOURNAMENT_DB_JOIN_NEW_REQ`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GUILD_TOURNAMENT_DB_JOIN_REQ`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_REWARD_CREATE_ACK`  (sizeof = 28 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_REWARD_CREATE_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `ItemOpt` | `PROTO_NC_GUILD_TOURNAMENT_DB_REWARD_CREATE_REQ::ITEM_OPT[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_DB_REWARD_CREATE_REQ::ITEM_OPT`  (sizeof = 33 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_SET_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_SET_MATCH_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_SET_MATCH_REQ`  (sizeof = 161 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `TournamentTree` | `GUILD_TOURNAMENT_LIST_DB[?] /* 155 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_DB_SET_MATCH_TIME_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_SET_MATCH_TIME_REQ`  (sizeof = 42 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `MatchTime` | `long[9]   /* 36 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_DB_SET_REQ`  (sizeof = 162 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `TournamentTree` | `GUILD_TOURNAMENT_LIST_DB[?] /* 155 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_DB_SET_RESULT_ACK`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_SET_RESULT_REQ`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_SET_TYPE_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `FinalSelectionGuild` | `GUILD_TOURNAMENT_LIST_DB[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_DB_SET_TYPE_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DICEGAME_BEFORE_END_TIME_MSG_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DICEGAME_START_CMD`  (sizeof = 52 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `AGuildDelegateName` | `Name5` |
|    32 | `BGuildDelegateName` | `Name5` |

### `PROTO_NC_GUILD_TOURNAMENT_DICEGAME_THROW_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DICEGAME_THROW_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DICEGAME_THROW_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_END_CMD`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_ENTER_LIST_DB_GET_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `EnterGuildList` | `GT_EnterList[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_ENTER_LIST_DB_GET_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_FLAGCAPTURE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_FLAGCAPTURE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_FLAGCAPTURE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_FLAGCAPTURE_RESULT_CMD`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CIDPlayer` | `Name5` |
|    20 | `CIDFlag` | `Name5` |

### `PROTO_NC_GUILD_TOURNAMENT_GOLD_REFUND_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_GOLD_REFUND_ZONE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `EnterGuildList` | `GT_EnterList[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_ITEM_EFFECT_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_ITEM_FLAG_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_ITEM_PICK_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_ITEM_SCAN_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `Members` | `PROTO_NC_GUILD_TOURNAMENT_ITEM_SCAN_CMD::Memberinfo[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_ITEM_SCAN_CMD::Memberinfo`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `loc` | `SHINE_XY_TYPE` |

### `PROTO_NC_GUILD_TOURNAMENT_JOIN_ACK`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_JOIN_LIST_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `JoinGuildList` | `SHINE_GUILD_LIST_NEW[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_JOIN_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_JOIN_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_LAST_WINNER_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_LEAVE_ACK`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_LEAVE_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_LIST_ACK`  (sizeof = 1014 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    38 | `tm_Time_Start` | `tm` |
|    74 | `tm_Time_Practic` | `tm` |
|   110 | `tm_Time_PracticEnd` | `tm` |
|   146 | `tm_Time_Match_161` | `tm` |
|   182 | `tm_Time_Match_162` | `tm` |
|   218 | `tm_Time_Match_8` | `tm` |
|   254 | `tm_Time_Match_4` | `tm` |
|   290 | `tm_Time_Match_2` | `tm` |
|   326 | `tm_Time_Match_End` | `tm` |
|   363 | `TournamentList` | `GUILD_TOURNAMENT_LIST[?] /* 651 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_MANAGERUSERMSG_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_GUILD_TOURNAMENT_NOTIFY_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Type` | `GUILD_TOURNAMENT_NOTIFY_TYPE` |
|     5 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_TOURNAMENT_OBSERVER_ENTER_ACK`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_OBSERVER_ENTER_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_OBSERVER_OUT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_OBSERVER_OUT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_PLAYERDIEMSG_CMD`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `VictimCharID` | `Name5` |
|    20 | `MurderCharID` | `Name5` |

### `PROTO_NC_GUILD_TOURNAMENT_PLAYERKILLMSG_CMD`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `VictimCharID` | `Name5` |
|    20 | `MurderCharID` | `Name5` |

### `PROTO_NC_GUILD_TOURNAMENT_RECALL_ENTER_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_RECALL_ENTER_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_RECALL_ENTER_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_REFUND_NOTICE_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_REFUND_NOTICE_DB_SET_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_REFUND_NOTICE_DB_SET_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_RESET_CMD`  (sizeof = 196 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `MatchTime` | `long[9]   /* 36 bytes */` |
|    41 | `TournamentTree` | `GUILD_TOURNAMENT_LIST_DB[?] /* 155 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_RESULT_CMD`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_SCORE_CMD`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    17 | `PlayerScore` | `TOURNAMENT_PLAYER_SCORE[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_SKILLPOINT_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_STARTMSG_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_START_CMD`  (sizeof = 17 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_TYPE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_USESKILL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_USESKILL_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_USESKILL_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_ZONE_FIGHTER_ENTER_CMD`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_ZONE_FIGHTER_OUT_CMD`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_ZONE_JOIN_NEW_ACK`  (sizeof = 27 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_ZONE_JOIN_NEW_REQ`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_ZONE_MEMBERGRADE_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `MemberGradeList` | `GUILD_TOURNAMENT_MEMBER_GRADE_LIST[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_ZONE_MEMBERGRADE_REQ`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_ZONE_OBSERVER_ENTER_ACK`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_ZONE_OBSERVER_ENTER_REQ`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_GUILD_WAR_ABLE_LIST_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `WarInfoList` | `SHINE_GUILD_WAR_ABLE_INFO[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_WAR_ABLE_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_WAR_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_GUILD_WAR_COOLDOWN_DONE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_WAR_END_CMD`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `EnemyGuildName` | `Name4` |

### `PROTO_NC_GUILD_WAR_LIST_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `WarInfoList` | `SHINE_GUILD_WAR_INFO[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_WAR_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_WAR_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `TargetName` | `Name4` |

### `PROTO_NC_GUILD_WAR_SCORE_ACK`  (sizeof = 102 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `EnemyGuildName` | `Name4` |
|    18 | `MyScore` | `SHINE_GUILD_SCORE` |
|    60 | `EnemyScore` | `SHINE_GUILD_SCORE` |

### `PROTO_NC_GUILD_WAR_SCORE_CMD`  (sizeof = 116 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GuildName1` | `Name4` |
|    16 | `GuildScore1` | `SHINE_GUILD_SCORE` |
|    58 | `GuildName2` | `Name4` |
|    74 | `GuildScore2` | `SHINE_GUILD_SCORE` |

### `PROTO_NC_GUILD_WAR_SCORE_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GuildNameEnemy` | `Name4` |

### `PROTO_NC_GUILD_WAR_START_CMD`  (sizeof = 60 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `WarGuildName` | `Name4` |
|    24 | `tm_EndDate` | `tm` |

### `PROTO_NC_GUILD_WAR_TARGET_CMD`  (sizeof = 234 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `WarInfo` | `SHINE_GUILD_WAR_INFO` |

### `PROTO_NC_GUILD_WORLD_RENAME_ACK`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     7 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_WORLD_RENAME_CMD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_WORLD_RENAME_REQ`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     7 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_WORLD_RETYPE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GUILD_WORLD_RETYPE_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GUILD_WORLD_RETYPE_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GUILD_WORLD_SET_GUILD_TOKEN_CMD`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_GUILD_WORLD_USE_GUILD_TOKEN_CMD`  (sizeof = 20 bytes)

_no members_

### `PROTO_NC_GUILD_ZONE_DELETE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_ZONE_LIST_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `GuildZoneList` | `GUILD_ZONE[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_ZONE_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_GUILD_ZONE_MAKE_CMD`  (sizeof = 53 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `GuildZone` | `GUILD_ZONE` |

### `PROTO_NC_GUILD_ZONE_MEMBER_JOIN_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_ZONE_MEMBER_LEAVE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_ZONE_USE_GUILD_TOKEN_CMD`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GUILD_ZONE_WAR_END_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_ZONE_WAR_KILL_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_ZONE_WAR_START_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_ADD_CMD`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `MemberInfo` | `PROTO_HOLY_PROMISE_INFO` |

### `PROTO_NC_HOLY_PROMISE_CENTRANSFER_RNG`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|    15 | `lower` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_CLIENT_GET_REMAIN_MONEY_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_DB_DEL_CHAR_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_DB_DEL_CHAR_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_DB_DEL_DOWN_ACK`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `DownCharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_DB_DEL_DOWN_REQ`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `DownCharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_DB_DEL_UP_ACK`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_DB_DEL_UP_REQ`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_DB_GET_CEN_REWARD_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_HOLY_PROMISE_DB_GET_CEN_REWARD_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_HOLY_PROMISE_DB_GET_MEMBER_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `MemberInfo` | `PROTO_HOLY_PROMISE_INFO_DB[?] /* 0 bytes */` |

### `PROTO_NC_HOLY_PROMISE_DB_GET_MEMBER_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_DB_GET_REMAIN_MONEY_CMD`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_DB_GET_UP_ACK`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `UpMemberInfo` | `PROTO_HOLY_PROMISE_INFO_DB` |

### `PROTO_NC_HOLY_PROMISE_DB_GET_UP_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_DB_REWARD_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `RewardList` | `PROTO_NC_HOLY_PROMISE_DB_REWARD_ACK::<unnamed-type-RewardList>[?] /* 0 bytes */` |

### `PROTO_NC_HOLY_PROMISE_DB_REWARD_ACK::<unnamed-type-RewardList>`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_DB_REWARD_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_DB_SET_DATE_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `RejoinableDate` | `PROTO_HOLY_PROMISE_DATE` |
|     8 | `MemberAcceptableDate` | `PROTO_HOLY_PROMISE_DATE` |

### `PROTO_NC_HOLY_PROMISE_DB_SET_DATE_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `RejoinableDate` | `PROTO_HOLY_PROMISE_DATE` |
|     8 | `MemberAcceptableDate` | `PROTO_HOLY_PROMISE_DATE` |

### `PROTO_NC_HOLY_PROMISE_DB_SET_UP_ACK`  (sizeof = 86 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ReqInfo` | `PROTO_HOLY_PROMISE_INFO_DB` |
|    42 | `UpInfo` | `PROTO_HOLY_PROMISE_INFO_DB` |

### `PROTO_NC_HOLY_PROMISE_DB_SET_UP_REQ`  (sizeof = 84 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ReqInfo` | `PROTO_HOLY_PROMISE_INFO_DB` |
|    42 | `UpInfo` | `PROTO_HOLY_PROMISE_INFO_DB` |

### `PROTO_NC_HOLY_PROMISE_DB_WITHDRAW_CEN_REWARD_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_HOLY_PROMISE_DB_WITHDRAW_CEN_REWARD_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_HOLY_PROMISE_DEL_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_DEL_DOWN_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `DownCharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_DEL_DOWN_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `DownCharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_DEL_UP_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_DEL_UP_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_GET_CEN_REWARD_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_GET_CEN_REWARD_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_LEVEL_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_LIST_CMD`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `UpInfo` | `PROTO_HOLY_PROMISE_INFO` |
|    29 | `MemberInfo` | `PROTO_HOLY_PROMISE_INFO[?] /* 0 bytes */` |

### `PROTO_NC_HOLY_PROMISE_LOGIN_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_LOGOUT_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_MYUPPER_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_HOLY_PROMISE_MYUPPER_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_HOLY_PROMISE_MY_UP_ZONE`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_REWARD_ITEM_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `RewardList` | `PROTO_NC_HOLY_PROMISE_REWARD_ITEM_CMD::<unnamed-type-RewardList>[?] /* 0 bytes */` |

### `PROTO_NC_HOLY_PROMISE_REWARD_ITEM_CMD::<unnamed-type-RewardList>`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_REWARD_MONEY_CMD`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `MemberCharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_SET_UP_ACK`  (sizeof = 68 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `UpInfo` | `PROTO_HOLY_PROMISE_INFO` |
|    32 | `tm_ResetableDateTime` | `tm` |

### `PROTO_NC_HOLY_PROMISE_SET_UP_CONFIRM_ACK`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |
|    20 | `UpCharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_SET_UP_CONFIRM_ING`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `UpCharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_SET_UP_CONFIRM_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |
|    20 | `UpCharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_SET_UP_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name5` |
|    20 | `UpCharID` | `Name5` |

### `PROTO_NC_HOLY_PROMISE_USE_MONEY_ZONE`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_WITHDRAW_CEN_REWARD_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_WITHDRAW_CEN_REWARD_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_INSTANCE_DUNGEON_DELETE_DUNGEON_CMD`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_INSTANCE_DUNGEON_EMPTY_DUNGEON_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_INSTANCE_DUNGEON_FIND_RNG`  (sizeof = 70 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |
|     7 | `InstanceCategory` | `INSTANCE_DUNGEON::CATEGORY` |
|    15 | `Argument` | `ORToken` |
|    35 | `ServerMapName` | `wchar_t[33]   /* 33 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_LEVEL_SELECT_CHECK_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `eLevelType` | `ID_LEVEL_TYPE` |

### `PROTO_NC_INSTANCE_DUNGEON_LEVEL_SELECT_CHECK_ECHO_ACK`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `eLevelType` | `ID_LEVEL_TYPE` |

### `PROTO_NC_INSTANCE_DUNGEON_LEVEL_SELECT_CHECK_ECHO_REQ`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `IndunCategory` | `INSTANCE_DUNGEON::CATEGORY` |
|    15 | `Argument` | `ORToken` |

### `PROTO_NC_INSTANCE_DUNGEON_LEVEL_SELECT_CHECK_REQ`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `IndunCategory` | `INSTANCE_DUNGEON::CATEGORY` |
|    16 | `Argument` | `ORToken` |

### `PROTO_NC_INSTANCE_DUNGEON_LEVEL_SELECT_JOIN_ACK`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `eLevelType` | `ID_LEVEL_TYPE` |
|    12 | `sMapName` | `Name3` |

### `PROTO_NC_INSTANCE_DUNGEON_LEVEL_SELECT_JOIN_ECHO_ACK`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `eLevelType` | `ID_LEVEL_TYPE` |
|    13 | `sMapName` | `Name3` |

### `PROTO_NC_INSTANCE_DUNGEON_LEVEL_SELECT_JOIN_ECHO_REQ`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `IndunCategory` | `INSTANCE_DUNGEON::CATEGORY` |
|    15 | `Argument` | `ORToken` |
|    35 | `eLevelType` | `ID_LEVEL_TYPE` |

### `PROTO_NC_INSTANCE_DUNGEON_LEVEL_SELECT_JOIN_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `IndunCategory` | `INSTANCE_DUNGEON::CATEGORY` |
|    16 | `Argument` | `ORToken` |
|    36 | `eLevelType` | `ID_LEVEL_TYPE` |

### `PROTO_NC_INSTANCE_DUNGEON_LEVEL_SELECT_MENU_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `eLevelType` | `ID_LEVEL_TYPE` |

### `PROTO_NC_INSTANCE_DUNGEON_LEVEL_SELECT_MENU_REQ`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sMapIndex` | `Name3` |
|    15 | `eLevelType` | `ID_LEVEL_TYPE[0]   /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_MAP_REGIST_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sIndunInfos` | `SHINE_INDUN_INFO[?] /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_ACK`  (sizeof = 1972 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sIndunInfo` | `SHINE_INDUN_INFO` |
|    13 | `damageDealtShapeInfo1st` | `PROTO_AVATAR_SHAPE_INFO` |
|    17 | `damageDealtShapeInfo2nd` | `PROTO_AVATAR_SHAPE_INFO` |
|    21 | `damageDealtShapeInfo3th` | `PROTO_AVATAR_SHAPE_INFO` |
|    25 | `damageTakenShapeInfo1st` | `PROTO_AVATAR_SHAPE_INFO` |
|    29 | `damageTakenShapeInfo2nd` | `PROTO_AVATAR_SHAPE_INFO` |
|    33 | `damageTakenShapeInfo3th` | `PROTO_AVATAR_SHAPE_INFO` |
|    37 | `healingDoneShapeInfo1st` | `PROTO_AVATAR_SHAPE_INFO` |
|    41 | `healingDoneShapeInfo2nd` | `PROTO_AVATAR_SHAPE_INFO` |
|    45 | `healingDoneShapeInfo3th` | `PROTO_AVATAR_SHAPE_INFO` |
|    52 | `damageDealtRanks` | `SHINE_INDUN_RANK_CLIENT_VALUE[?] /* 640 bytes */` |
|   692 | `damageTakenRanks` | `SHINE_INDUN_RANK_CLIENT_VALUE[?] /* 640 bytes */` |
|  1332 | `healingDoneRanks` | `SHINE_INDUN_RANK_CLIENT_VALUE[?] /* 640 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_LIST_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sIndunInfos` | `SHINE_INDUN_INFO[?] /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_INSTANCE_DUNGEON_RANK_MYRANK_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `MyRank` | `SHINE_INDUN_RANK_MYRANK[?] /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_NEW_RANK_MSG`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharID` | `Name5` |
|    20 | `sIndunInfo` | `SHINE_INDUN_INFO` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sIndunInfos` | `SHINE_INDUN_INFO[?] /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_RESULT_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `ranks` | `SHINE_INDUN_RANK_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_SAVE_DB_CMD`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sIndunInfo` | `SHINE_INDUN_INFO` |
|    19 | `ranks` | `SHINE_INDUN_RANK[?] /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_SAVE_WORLD_CMD`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sIndunInfo` | `SHINE_INDUN_INFO` |
|    19 | `ranks` | `SHINE_INDUN_RANK[?] /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_TAB_DAMAGEDEALT_ACK`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `shapeInfo1st` | `PROTO_AVATAR_SHAPE_INFO` |
|     4 | `shapeInfo2nd` | `PROTO_AVATAR_SHAPE_INFO` |
|     8 | `shapeInfo3th` | `PROTO_AVATAR_SHAPE_INFO` |
|    19 | `damageDealtRanks` | `SHINE_INDUN_RANK_CLIENT_VALUE[?] /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_TAB_DAMAGEDEALT_REQ`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sIndunInfo` | `SHINE_INDUN_INFO` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_TAB_DAMAGETAKEN_ACK`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `shapeInfo1st` | `PROTO_AVATAR_SHAPE_INFO` |
|     4 | `shapeInfo2nd` | `PROTO_AVATAR_SHAPE_INFO` |
|     8 | `shapeInfo3th` | `PROTO_AVATAR_SHAPE_INFO` |
|    19 | `damageTakenRanks` | `SHINE_INDUN_RANK_CLIENT_VALUE[?] /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_TAB_DAMAGETAKEN_REQ`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sIndunInfo` | `SHINE_INDUN_INFO` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_TAB_HEALINGDONE_ACK`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `shapeInfo1st` | `PROTO_AVATAR_SHAPE_INFO` |
|     4 | `shapeInfo2nd` | `PROTO_AVATAR_SHAPE_INFO` |
|     8 | `shapeInfo3th` | `PROTO_AVATAR_SHAPE_INFO` |
|    19 | `healingDoneRanks` | `SHINE_INDUN_RANK_CLIENT_VALUE[?] /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_TAB_HEALINGDONE_REQ`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sIndunInfo` | `SHINE_INDUN_INFO` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_TAB_MYRANK_ACK`  (sizeof = 62 bytes)

_no members_

### `PROTO_NC_INSTANCE_DUNGEON_RANK_TAB_MYRANK_REQ`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sIndunInfo` | `SHINE_INDUN_INFO` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_UPDATE_DB_ACK`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sIndunInfo` | `SHINE_INDUN_INFO` |
|    15 | `CharList` | `SHINE_INDUN_RANK_RANKING[?] /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RANK_UPDATE_DB_REQ`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sIndunInfo` | `SHINE_INDUN_INFO` |
|    15 | `CharList` | `NETPACKETZONEHEADER[?] /* 0 bytes */` |

### `PROTO_NC_INSTANCE_DUNGEON_RESET_COUNTDOWN_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_ITEMDB_ADMINCREATEFAIL_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ITEMDB_ADMINCREATESUC_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEMDB_ADMINCREATE_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `create` | `PROTO_ITEM_CREATE` |

### `PROTO_NC_ITEMDB_BOOTHTRADE_ACK`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `headerSeller` | `NETPACKETZONEHEADER` |
|     6 | `headerBuyer` | `NETPACKETZONEHEADER` |
|    16 | `nBoothTradeType` | `BOOTH_TRADE_TYPE` |
|    20 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_BOOTHTRADE_ALL_REQ`  (sizeof = 68 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `headerSeller` | `NETPACKETZONEHEADER` |
|     6 | `headerBuyer` | `NETPACKETZONEHEADER` |
|    25 | `itemregnum` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_BOOTHTRADE_LOT_REQ`  (sizeof = 103 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `headerSeller` | `NETPACKETZONEHEADER` |
|     6 | `headerBuyer` | `NETPACKETZONEHEADER` |
|    27 | `lotseller` | `SHINE_ITEM_REGISTNUMBER` |
|    35 | `lotcreate` | `PROTO_ITEM_CREATE` |

### `PROTO_NC_ITEMDB_BOOTHTRADE_MERGE_REQ`  (sizeof = 74 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `headerSeller` | `NETPACKETZONEHEADER` |
|     6 | `headerBuyer` | `NETPACKETZONEHEADER` |
|    27 | `lotseller` | `SHINE_ITEM_REGISTNUMBER` |
|    35 | `lotbuyer` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_BRACELET_UPGRADE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_BRACELET_UPGRADE_REQ`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `upgrade_item_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    22 | `raw_regnum` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_BUYALLFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_BUYALLSUC_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEMDB_BUYALL_REQ`  (sizeof = 96 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itembuy` | `PROTO_ITEM_CREATE` |
|    62 | `CharID` | `wchar_t[30]   /* 30 bytes */` |
|    96 | `attr` | `SHINE_ITEM_ATTRIBUTE[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_BUYCAPSULE_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_BUYCAPSULE_REQ`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|    28 | `CapsuleLocation` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_BUYLOTFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_BUYLOTSUC_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEMDB_BUYLOT_REQ`  (sizeof = 72 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `iteminfo` | `PROTO_ITEM_ATTRCHANGE` |
|    38 | `CharID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_ITEMDB_CAPSULEITEM_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     8 | `nItemReg` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `nLocation` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_CAPSULEITEM_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `nItemReg` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `nLocation` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_CHARGED_LIST_ACK`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|    11 | `ChargedItemInfoList` | `PROTO_CHARGED_ITEM_INFO[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_CHARGED_LIST_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_CHARGED_WITHDRAW_ACK`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     8 | `ChargedItemInfo` | `PROTO_CHARGED_ITEM_INFO` |

### `PROTO_NC_ITEMDB_CHARGED_WITHDRAW_REQ`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|    12 | `ChargedItemInfo` | `PROTO_CHARGED_ITEM_INFO` |
|    29 | `ItemCreate` | `PROTO_ITEM_CREATE[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_CHAT_COLOR_CHANGE_ACK`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    18 | `eChatColorItemType` | `CAHT_CHAT_COLOR_ITEM_TYPE` |

### `PROTO_NC_ITEMDB_CHAT_COLOR_CHANGE_REQ`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `eChatColorItemType` | `CAHT_CHAT_COLOR_ITEM_TYPE` |

### `PROTO_NC_ITEMDB_CHESTITEM_ACK`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `chest` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `location` | `ITEM_INVEN` |
|    18 | `item` | `PROTO_ITEM_CMD` |

### `PROTO_NC_ITEMDB_CHESTITEM_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `chest` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `location` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_CLASS_CHANGE_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEMDB_CLASS_CHANGE_REQ`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `nStuffKey` | `SHINE_ITEM_REGISTNUMBER` |
|    21 | `nFreeStat` | `unsigned char[5]   /* 5 bytes */` |
|    41 | `nDeleteSkillID` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_ITEMDB_CLOSE_GUILD_STORAGE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEMDB_CREATEITEMLISTFAIL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_CREATEITEMLISTSUC_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_CREATEITEMLIST_REQ`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    22 | `itemlist` | `PROTO_ITEM_CMD` |

### `PROTO_NC_ITEMDB_DEPOSIT_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_DEPOSIT_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_DESTROY_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_DISMANTLE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_DISMANTLE_REQ`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `raw` | `PROTO_NC_ITEMDB_DISMANTLE_REQ::<unnamed-type-raw>` |
|    26 | `produce` | `PROTO_NC_ITEMDB_DISMANTLE_REQ::<unnamed-type-produce>[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_DISMANTLE_REQ::<unnamed-type-produce>`  (sizeof = 43 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `iteminfo` | `PROTO_NC_ITEMDB_DISMANTLE_REQ::<unnamed-type-produce>::<unnamed-type-iteminfo>` |

### `PROTO_NC_ITEMDB_DISMANTLE_REQ::<unnamed-type-raw>`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `iteminfo` | `PROTO_NC_ITEMDB_DISMANTLE_REQ::<unnamed-type-raw>::<unnamed-type-iteminfo>` |

### `PROTO_NC_ITEMDB_DROPALLSUC_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEMDB_DROPALL_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `dropitem` | `PROTO_ITEM_VANISH` |

### `PROTO_NC_ITEMDB_DROPLOT_REQ`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `iteminfo` | `PROTO_ITEM_ATTRCHANGE` |
|    14 | `dropitem` | `PROTO_NC_ITEMDB_DROPLOT_REQ::<unnamed-type-dropitem>` |

### `PROTO_NC_ITEMDB_DROPLOT_REQ::<unnamed-type-dropitem>`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemregnum` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    10 | `equipment` | `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_ACK::<unnamed-type-equipment>` |
|    16 | `gem` | `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_ACK::<unnamed-type-gem>` |

### `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_ACK::<unnamed-type-equipment>`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_ACK::<unnamed-type-gem>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_REQ`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `equipment` | `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_REQ::<unnamed-type-equipment>` |
|    20 | `gem` | `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_REQ::<unnamed-type-equipment>` |

### `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_REQ::<unnamed-type-equipment>`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemRegistNumber` | `SHINE_ITEM_REGISTNUMBER` |
|    10 | `itemSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_ACK`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    10 | `equipment` | `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_ACK::<unnamed-type-equipment>` |
|    13 | `drill` | `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_ACK::<unnamed-type-drill>` |

### `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_ACK::<unnamed-type-drill>`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_ACK::<unnamed-type-equipment>`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `equipment` | `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_REQ::<unnamed-type-equipment>` |
|    20 | `drill` | `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_REQ::<unnamed-type-equipment>` |

### `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_REQ::<unnamed-type-equipment>`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemRegistNumber` | `SHINE_ITEM_REGISTNUMBER` |
|    10 | `itemSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_ACK`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    10 | `equipment` | `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_ACK::<unnamed-type-equipment>` |
|    12 | `remover` | `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_ACK::<unnamed-type-remover>` |
|    22 | `gemSlot` | `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_ACK::<unnamed-type-gemSlot>[?] /* 3 bytes */` |

### `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_ACK::<unnamed-type-equipment>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_ACK::<unnamed-type-gemSlot>`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_ACK::<unnamed-type-remover>`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_REQ`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `equipment` | `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_REQ::<unnamed-type-equipment>` |
|    20 | `remover` | `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_REQ::<unnamed-type-equipment>` |
|    32 | `gemSlot` | `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_REQ::<unnamed-type-gemSlot>[?] /* 3 bytes */` |

### `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_REQ::<unnamed-type-equipment>`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemRegistNumber` | `SHINE_ITEM_REGISTNUMBER` |
|    10 | `itemSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_REQ::<unnamed-type-gemSlot>`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEMDB_ENCHANT_SET_GEM_LOT_ACK`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `itemRegistNumber` | `SHINE_ITEM_REGISTNUMBER` |
|    17 | `sockets` | `PROTO_NC_ITEMDB_ENCHANT_SET_GEM_LOT_ACK::<unnamed-type-sockets>[?] /* 9 bytes */` |

### `PROTO_NC_ITEMDB_ENCHANT_SET_GEM_LOT_ACK::<unnamed-type-sockets>`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_ITEMDB_ENCHANT_SET_GEM_LOT_REQ`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `itemRegistNumber` | `SHINE_ITEM_REGISTNUMBER` |
|    15 | `sockets` | `PROTO_NC_ITEMDB_ENCHANT_SET_GEM_LOT_REQ::<unnamed-type-sockets>[?] /* 9 bytes */` |

### `PROTO_NC_ITEMDB_ENCHANT_SET_GEM_LOT_REQ::<unnamed-type-sockets>`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_ITEMDB_EQUIPFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_EQUIPSUC_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEMDB_EQUIP_BELONGED_CANCEL_USE_ACK`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|    10 | `nBelongedItem_ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `nCancelItem_ItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_EQUIP_BELONGED_CANCEL_USE_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     8 | `nBelongedItem_ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    18 | `nCancelItem_ItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_EQUIP_REQ`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `relocA` | `PROTO_ITEM_RELOC` |
|    12 | `relocA_IsPutOnBelonged` | `SHINE_PUT_ON_BELONGED_ITEM` |
|    16 | `relocB` | `PROTO_ITEM_RELOC` |
|    29 | `unequip` | `PROTO_NC_ITEMDB_EQUIP_REQ::<unnamed-type-unequip>[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_EQUIP_REQ::<unnamed-type-unequip>`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `item` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_EXCHANGEFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_EXCHANGEFAIL_ACK_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `zoneheader` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_EXCHANGEFAIL_ACK` |

### `PROTO_NC_ITEMDB_EXCHANGESUC_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEMDB_EXCHANGESUC_ACK_SEND`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `zoneheader` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_EXCHANGESUC_ACK` |

### `PROTO_NC_ITEMDB_EXCHANGE_REQ`  (sizeof = 42 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `relocA` | `PROTO_ITEM_RELOC` |
|    20 | `relocB` | `PROTO_ITEM_RELOC` |

### `PROTO_NC_ITEMDB_FAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEMDB_FURNITURE_ENDURE_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `dNewEndureTime` | `ShineDateTime` |

### `PROTO_NC_ITEMDB_FURNITURE_ENDURE_REQ`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    10 | `dNewEndureTime` | `ShineDateTime` |
|    14 | `CommonData` | `PROTO_NC_ITEMDB_USE_VARIATION_ITEM_REQ` |

### `PROTO_NC_ITEMDB_GETFROMCAPSULE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_GETFROMCAPSULE_REQ`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|    12 | `nItmeReg` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `nReloc` | `PROTO_ITEM_RELOC` |

### `PROTO_NC_ITEMDB_GETFROMCHESTFAIL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_GETFROMCHESTSUC_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_GETFROMCHEST_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    12 | `chest` | `SHINE_ITEM_REGISTNUMBER` |
|    21 | `reloc` | `PROTO_ITEM_RELOC[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_OPEN_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zonepackheader` | `NETPACKETZONEHEADER` |
|    16 | `itemarray` | `PROTO_ITEMPACKET_TOTAL[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_OPEN_FAIL_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_OPEN_FAIL_ACK_SEND`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_OPEN_FAIL_ACK` |

### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_OPEN_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_GUILD_STORAGE_WITHDRAW_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_GUILD_STORAGE_WITHDRAW_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_GUILD_TOKEN_BUYALLFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_GUILD_TOKEN_BUYALLSUC_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEMDB_GUILD_TOKEN_BUYALL_REQ`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `ItemBuy` | `PROTO_ITEM_CREATE` |
|    48 | `Attr` | `SHINE_ITEM_ATTRIBUTE[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_GUILD_TOKEN_BUYLOTFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_GUILD_TOKEN_BUYLOTSUC_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEMDB_GUILD_TOKEN_BUYLOT_REQ`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `ItemInfo` | `PROTO_ITEM_ATTRCHANGE` |

### `PROTO_NC_ITEMDB_GUILD_TOURNAMENT_REWARD_CREATE_ACK`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_ITEMDB_GUILD_TOURNAMENT_REWARD_CREATE_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `ItemOpt` | `PROTO_NC_ITEMDB_GUILD_TOURNAMENT_REWARD_CREATE_REQ::ITEM_OPT[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_GUILD_TOURNAMENT_REWARD_CREATE_REQ::ITEM_OPT`  (sizeof = 33 bytes)

_no members_

### `PROTO_NC_ITEMDB_INC_DEC_MONEYSUC_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEMDB_INC_DEC_MONEY_REQ`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_ITEMDB_ITEMBREAKFAIL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_ITEMBREAKSUC_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_ITEMBREAK_REQ`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     8 | `BreakItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    25 | `DeleteChestItem` | `PROTO_ITEM_VANISH[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_ITEMCHANGE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_ITEMCHANGE_ACK_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_ITEMDB_ITEMCHANGE_ACK` |

### `PROTO_NC_ITEMDB_ITEMCHANGE_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `itemstruct` | `SHINE_ITEM_STRUCT[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_ITEMINFO_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `FailProtocol` | `NETCOMMAND` |

### `PROTO_NC_ITEMDB_ITEMINFO_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `FailProtocol` | `NETCOMMAND` |

### `PROTO_NC_ITEMDB_ITEMMONEY_BUYALL_REQ`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `nItemMoneyKey` | `SHINE_ITEM_REGISTNUMBER` |
|    34 | `CreateItem` | `PROTO_NC_ITEMDB_ITEMMONEY_BUYALL_REQ::CREATE_ITEM_INFO[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_ITEMMONEY_BUYALL_REQ::CREATE_ITEM_INFO`  (sizeof = 113 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    10 | `ItemLoc` | `ITEM_INVEN` |
|    12 | `ItemAttr` | `SHINE_ITEM_ATTRIBUTE` |

### `PROTO_NC_ITEMDB_ITEMMONEY_BUYLOT_REQ`  (sizeof = 47 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    32 | `nItemMoneyKey` | `SHINE_ITEM_REGISTNUMBER` |
|    47 | `nDeleteItemInfo` | `DELETE_ITEM_INFO[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_ITEMMONEY_BUY_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEMDB_ITEMREBUILD_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEMDB_ITEMREBUILD_REQ`  (sizeof = 54 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `EraseItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `CreateItem` | `PROTO_ITEM_CREATE` |

### `PROTO_NC_ITEMDB_ITEMTOTALINFORM_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEMDB_ITEMTOTALINFORM_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemregnum` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_ITEMTRADEFAIL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_ITEMTRADEFAIL_ACK_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_ITEMDB_ITEMTRADEFAIL_ACK` |

### `PROTO_NC_ITEMDB_ITEMTRADESUC_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_ITEMTRADESUC_ACK_SEND`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_ITEMDB_ITEMTRADESUC_ACK` |

### `PROTO_NC_ITEMDB_ITEMTRADE_REQ`  (sizeof = 42 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `A` | `NETPACKETZONEHEADER` |
|     6 | `B` | `NETPACKETZONEHEADER` |
|    16 | `cen` | `PROTO_NC_ITEMDB_ITEMTRADE_REQ::<unnamed-type-cen>` |
|    42 | `tradeitem` | `PROTO_NC_ITEMDB_ITEMTRADE_REQ::<unnamed-type-tradeitem>[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_ITEMTRADE_REQ::<unnamed-type-cen>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_ITEMDB_ITEMTRADE_REQ::<unnamed-type-tradeitem>`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `invenA` | `PROTO_NC_ITEMDB_ITEMTRADE_REQ::<unnamed-type-tradeitem>::<unnamed-type-invenA>` |
|    13 | `invenB` | `PROTO_NC_ITEMDB_ITEMTRADE_REQ::<unnamed-type-tradeitem>::<unnamed-type-invenA>` |

### `PROTO_NC_ITEMDB_ITEMTRADE_REQ::<unnamed-type-tradeitem>::<unnamed-type-invenA>`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `regnum` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_MAPLINK_ITEM_CONSUME_ACK`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `mapName` | `Name3` |

### `PROTO_NC_ITEMDB_MAPLINK_ITEM_CONSUME_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `mapName` | `Name3` |
|    26 | `nStuffKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_MERGEFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_MERGEFAIL_ACK_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `zoneheader` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_MERGEFAIL_ACK` |

### `PROTO_NC_ITEMDB_MERGESUC_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `lotmain` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_MERGESUC_SEND`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `zoneheader` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_MERGESUC_ACK` |

### `PROTO_NC_ITEMDB_MERGE_REQ`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `lotmain` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `lotsub` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_MINIHOUSE_EFFECT_DEMANDGOOD_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_MINIHOUSE_EFFECT_DEMANDGOOD_ACK_SEND`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_ITEMDB_MINIHOUSE_EFFECT_DEMANDGOOD_ACK` |

### `PROTO_NC_ITEMDB_MINIHOUSE_EFFECT_DEMANDGOOD_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `nDemandItemRegNum` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_MINIHOUSE_PORTAL_EFFECT_DEMANDGOOD_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_MINIHOUSE_PORTAL_EFFECT_DEMANDGOOD_ACK_SEND`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_ITEMDB_MINIHOUSE_PORTAL_EFFECT_DEMANDGOOD_ACK` |

### `PROTO_NC_ITEMDB_MINIHOUSE_PORTAL_EFFECT_DEMANDGOOD_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `nDemandItemRegNum` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_MIX_ITEM_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEMDB_MIX_ITEM_REQ`  (sizeof = 52 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `nSubRawKey` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `nRawLeftKey` | `SHINE_ITEM_REGISTNUMBER` |
|    34 | `nRawRightKey` | `SHINE_ITEM_REGISTNUMBER` |
|    48 | `MixItemRoc` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_MOB_DROP_CMD`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `map` | `MAPPOS` |
|    24 | `createitemregnum` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_MOVER_RAREMOVER_ACK`  (sizeof = 114 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    13 | `ItemAttr` | `SHINE_ITEM_ATTRIBUTE` |

### `PROTO_NC_ITEMDB_MOVER_RAREMOVER_REQ`  (sizeof = 343 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `nMainMoverItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    18 | `MainMoverAttr` | `SHINE_ITEM_ATTRIBUTE` |
|   119 | `nSubMoverItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|   129 | `SubMoverAttr` | `SHINE_ITEM_ATTRIBUTE` |
|   230 | `nRareMoverItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|   240 | `RareMoverLoc` | `ITEM_INVEN` |
|   242 | `RareMoverAttr` | `SHINE_ITEM_ATTRIBUTE` |

### `PROTO_NC_ITEMDB_MOVER_UPGRADE_ACK`  (sizeof = 115 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `ItemAttr` | `SHINE_ITEM_ATTRIBUTE` |

### `PROTO_NC_ITEMDB_MOVER_UPGRADE_REQ`  (sizeof = 233 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `nSubMoverItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    19 | `SubMoverAttr` | `SHINE_ITEM_ATTRIBUTE` |
|   120 | `nMainMoverItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|   130 | `MainMoverLoc` | `ITEM_INVEN` |
|   132 | `MainMoverAttr` | `SHINE_ITEM_ATTRIBUTE` |

### `PROTO_NC_ITEMDB_MYSTERY_VAULT_MAKEITEM_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_MYSTERY_VAULT_MAKEITEM_REQ`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `UseVaultItemRegNo` | `SHINE_ITEM_REGISTNUMBER` |
|    19 | `itemcreate` | `PROTO_ITEM_CREATE[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_NEW_UPGRADE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_NEW_UPGRADE_ACK_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_ITEMDB_NEW_UPGRADE_ACK` |

### `PROTO_NC_ITEMDB_NEW_UPGRADE_REQ`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `upgrade_item_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    21 | `raw_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    33 | `upgrade_randomoption` | `ItemOptionStorage::Element` |

### `PROTO_NC_ITEMDB_OPENSTORAGE_ACK`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zonepackheader` | `NETPACKETZONEHEADER` |
|    17 | `itemarray` | `PROTO_ITEMPACKET_TOTAL[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_OPENSTORAGE_FAIL_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_OPENSTORAGE_FAIL_ACK_SEND`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_ITEMDB_OPENSTORAGE_FAIL_ACK` |

### `PROTO_NC_ITEMDB_OPENSTORAGE_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_OPEN_GUILD_STORAGE_ACK`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zonepackheader` | `NETPACKETZONEHEADER` |
|    23 | `itemarray` | `PROTO_ITEMPACKET_TOTAL[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_OPEN_GUILD_STORAGE_FAIL_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_OPEN_GUILD_STORAGE_FAIL_ACK_SEND`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_ITEMDB_OPENSTORAGE_FAIL_ACK` |

### `PROTO_NC_ITEMDB_OPEN_GUILD_STORAGE_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_PICKMERGE_REQ`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `itemonfield` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `itempick` | `PROTO_ITEM_ATTRCHANGE` |

### `PROTO_NC_ITEMDB_PICKMONEYSUC_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEMDB_PICKMONEY_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemonfield` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_PRODUCE_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_PRODUCE_REQ`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    10 | `produce` | `PROTO_NC_ITEMDB_PRODUCE_REQ::<unnamed-type-produce>` |
|    50 | `raw` | `PROTO_NC_ITEMDB_PRODUCE_REQ::<unnamed-type-raw>[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_PRODUCE_REQ::<unnamed-type-produce>`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `iteminfo` | `PROTO_NC_ITEMDB_PRODUCE_REQ::<unnamed-type-produce>::<unnamed-type-iteminfo>` |

### `PROTO_NC_ITEMDB_PRODUCE_REQ::<unnamed-type-raw>`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `iteminfo` | `PROTO_NC_ITEMDB_PRODUCE_REQ::<unnamed-type-raw>::<unnamed-type-iteminfo>` |

### `PROTO_NC_ITEMDB_QUESTALL_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `questitem` | `PROTO_ITEM_CREATE` |

### `PROTO_NC_ITEMDB_QUESTITEMGET_REQ`  (sizeof = 46 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `create` | `PROTO_NC_ITEMDB_ADMINCREATE_REQ` |

### `PROTO_NC_ITEMDB_QUESTLOT_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `lot` | `PROTO_ITEM_ATTRCHANGE` |

### `PROTO_NC_ITEMDB_QUESTREWARD_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_QUESTREWARD_REQ`  (sizeof = 45 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    45 | `itemcreate` | `PROTO_ITEM_CREATE[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_RANDOMOPTION_CHANGE_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEMDB_RANDOMOPTION_CHANGE_CONSUME_AND_COUNTING_ACK`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_RANDOMOPTION_CHANGE_CONSUME_AND_COUNTING_REQ`  (sizeof = 134 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `nStuffKey` | `SHINE_ITEM_REGISTNUMBER` |
|    22 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    32 | `ItemAttr` | `SHINE_ITEM_ATTRIBUTE` |

### `PROTO_NC_ITEMDB_RANDOMOPTION_CHANGE_REQ`  (sizeof = 220 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    18 | `ItemAttr` | `SHINE_ITEM_ATTRIBUTE` |
|   119 | `ItemAttrOrig` | `SHINE_ITEM_ATTRIBUTE` |

### `PROTO_NC_ITEMDB_RANDOMOPTION_RECOVER_COUNT_LIMIT_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEMDB_RANDOMOPTION_RECOVER_COUNT_LIMIT_REQ`  (sizeof = 146 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    18 | `ItemAttr` | `SHINE_ITEM_ATTRIBUTE` |
|   133 | `nStuffKey` | `SHINE_ITEM_REGISTNUMBER` |
|   146 | `nDeleteStuffKey` | `SHINE_ITEM_REGISTNUMBER[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_REINFORCEUNEQUIPFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_REINFORCEUNEQUIPSUC_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEMDB_REINFORCEUNEQUIP_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `unequ` | `PROTO_ITEM_RELOC` |
|    12 | `equloc` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_RELOCFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_RELOCFAIL_ACK_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `zoneheader` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_RELOCFAIL_ACK` |

### `PROTO_NC_ITEMDB_RELOCSUC_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEMDB_RELOCSUC_ACK_SEND`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `zoneheader` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_RELOCSUC_ACK` |

### `PROTO_NC_ITEMDB_RELOC_REQ`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `item` | `PROTO_ITEM_RELOC` |
|    20 | `inven_from` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_REPURCHASE_ALL_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEMDB_REPURCHASE_ALL_REQ`  (sizeof = 98 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `RepurchaseItem` | `PROTO_ITEM_CREATE` |
|    50 | `nSellItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    64 | `CharID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_ITEMDB_REPURCHASE_LOT_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEMDB_REPURCHASE_LOT_REQ`  (sizeof = 74 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `nItemAttrChange` | `PROTO_ITEM_ATTRCHANGE` |
|    26 | `nSellItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    40 | `CharID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_ITEMDB_RESET_SCROLL_LINK_MAP_INFO_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_SELLALLFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_SELLALLSUC_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEMDB_SELLALL_REQ`  (sizeof = 67 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `itemsell` | `PROTO_ITEM_VANISH` |
|    32 | `CharID` | `wchar_t[30]   /* 30 bytes */` |
|    67 | `DeleteChestItem` | `PROTO_ITEM_VANISH[?] /* 0 bytes */` |

### `PROTO_NC_ITEMDB_SELLLOTFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_SELLLOTSUC_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEMDB_SELLLOT_REQ`  (sizeof = 60 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `iteminfo` | `PROTO_ITEM_ATTRCHANGE` |
|    26 | `CharID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_ITEMDB_SHIELDENDURESET_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ShieldKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_SHIELDENDURE_CHARGE_ACK`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `ShieldKey` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `ShieldSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_SHIELDENDURE_CHARGE_REQ`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ShieldKey` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `ShieldSlot` | `ITEM_INVEN` |
|    14 | `CommonData` | `PROTO_NC_ITEMDB_USE_VARIATION_ITEM_REQ` |

### `PROTO_NC_ITEMDB_SOULSTONEBUYFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_SOULSTONEBUYFAIL_ACK_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `header` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_SOULSTONEBUYFAIL_ACK` |

### `PROTO_NC_ITEMDB_SOULSTONEBUYSUC_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEMDB_SOULSTONEBUYSUC_ACK_SEND`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `header` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_SOULSTONEBUYSUC_ACK` |

### `PROTO_NC_ITEMDB_SOULSTONEBUY_REQ`  (sizeof = 17 bytes)

_no members_

### `PROTO_NC_ITEMDB_SPLITFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_SPLITFAIL_ACK_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `zoneheader` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_SPLITFAIL_ACK` |

### `PROTO_NC_ITEMDB_SPLITSUC_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `lotmain` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_SPLITSUC_ACK_SEND`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `zoneheader` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_SPLITSUC_ACK` |

### `PROTO_NC_ITEMDB_SPLIT_N_MERGEFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_SPLIT_N_MERGEFAIL_ACK_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `zoneheader` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_SPLIT_N_MERGEFAIL_ACK` |

### `PROTO_NC_ITEMDB_SPLIT_N_MERGESUC_ACK`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `FromItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    10 | `ToItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_SPLIT_N_MERGESUC_ACK_SEND`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `zoneheader` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_SPLIT_N_MERGESUC_ACK` |

### `PROTO_NC_ITEMDB_SPLIT_N_MERGE_REQ`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `FromItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    22 | `ToItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_SPLIT_REQ`  (sizeof = 62 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `lotmain` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `lotcreate` | `PROTO_ITEM_CREATE` |

### `PROTO_NC_ITEMDB_TERMEXTEND_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEMDB_TERMEXTEND_REQ`  (sizeof = 141 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `nExtendItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    22 | `nTermItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    32 | `TermItemAttr` | `SHINE_ITEM_ATTRIBUTE` |
|   133 | `OrigDelDateTime` | `ShineDateTime` |

### `PROTO_NC_ITEMDB_UES_FRIEND_POINT_ACK`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_UES_FRIEND_POINT_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `itemInven` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_UNEQUIPFAIL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_UNEQUIPSUC_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEMDB_UNEQUIP_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `unequ` | `PROTO_ITEM_RELOC` |
|    12 | `equloc` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_UPGRADE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_UPGRADE_ACK_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_ITEMDB_UPGRADE_ACK` |

### `PROTO_NC_ITEMDB_UPGRADE_REQ`  (sizeof = 78 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `upgrade_item_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    22 | `raw_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    30 | `raw_left_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    38 | `raw_right_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    46 | `raw_middle_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    75 | `upgrade_randomoption` | `ItemOptionStorage::Element` |

### `PROTO_NC_ITEMDB_USEALL_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attritemregnum` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_USELOT_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attr` | `PROTO_CHANGEATTR` |

### `PROTO_NC_ITEMDB_USE_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEMDB_USE_ACK_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `header` | `NETPACKETZONEHEADER` |
|     9 | `ack` | `PROTO_NC_ITEMDB_USE_ACK` |

### `PROTO_NC_ITEMDB_USE_VARIATION_ITEM_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `attr` | `PROTO_CHANGEATTR` |

### `PROTO_NC_ITEMDB_WEAPONENDURESET_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `WeaponKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_ITEMDB_WEAPONENDURE_CHARGE_ACK`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     8 | `WeaponKey` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `WeaponSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEMDB_WEAPONENDURE_CHARGE_REQ`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `WeaponKey` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `WeaponSlot` | `ITEM_INVEN` |
|    14 | `CommonData` | `PROTO_NC_ITEMDB_USE_VARIATION_ITEM_REQ` |

### `PROTO_NC_ITEMDB_WITHDRAW_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEMDB_WITHDRAW_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_ITEM_ACCOUNT_STORAGE_CLOSE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEM_ACCOUNT_STORAGE_OPEN_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEM_AUTO_ARRANGE_INVEN_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_AUTO_ARRANGE_INVEN_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEM_BRACELET_UPGRADE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_BRACELET_UPGRADE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_BREAKFAIL_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_BUY_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ITEM_BUY_SUC_ACTION_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_CELLCHANGE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `exchange` | `ITEM_INVEN` |
|     2 | `location` | `ITEM_INVEN` |
|     4 | `item` | `SHINE_ITEM_VAR_STRUCT` |

### `PROTO_NC_ITEM_CHARGEDINVENOPEN_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `ChargedItemInfoList` | `PROTO_CHARGED_ITEM_INFO[?] /* 0 bytes */` |

### `PROTO_NC_ITEM_CHARGEDINVENOPEN_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_CHARGED_WITHDRAW_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ITEM_CHARGED_WITHDRAW_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEM_CHAT_COLOR_CHANGE_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_ITEM_CHAT_COLOR_CHANGE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEM_CLASS_CHANGE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_CLASS_CHANGE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEM_DEPOSIT_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEM_DEPOSIT_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_ITEM_DICE_GAME_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ITEM_DICE_GAME_RESULT_CMD`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `WinnerID` | `Name5` |

### `PROTO_NC_ITEM_DICE_GAME_START_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_DICE_GAME_START_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ITEM_DICE_GAME_START_REQ`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_ITEM_DISMANTLE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_DISMANTLE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEM_DROP_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `ITEM_INVEN` |
|     6 | `loc` | `SHINE_XY_TYPE` |

### `PROTO_NC_ITEM_ENCHANT_ADD_GEM_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `equipment` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_ENCHANT_ADD_GEM_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `equipment` | `ITEM_INVEN` |
|     2 | `gem` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_ENCHANT_ADD_NEW_SOCKET_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `equipment` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_ENCHANT_ADD_NEW_SOCKET_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `equipment` | `ITEM_INVEN` |
|     2 | `drill` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_ENCHANT_REMOVE_GEM_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `equipment` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_ENCHANT_REMOVE_GEM_REQ`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `equipment` | `ITEM_INVEN` |
|     2 | `remover` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_ENCHANT_SET_GEM_LOT_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `equipment` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_EQUIPCHANGE_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `exchange` | `ITEM_INVEN` |
|     3 | `item` | `SHINE_ITEM_VAR_STRUCT` |

### `PROTO_NC_ITEM_EQUIP_BELONGED_CANCEL_USE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_EQUIP_BELONGED_CANCEL_USE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_EQUIP_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEM_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEM_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_ITEM_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEM_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_ITEM_GUILD_STORAGE_WITHDRAW_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEM_GUILD_STORAGE_WITHDRAW_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_ITEM_ITEMBREAK_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_MAPLINK_SCROLL_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEM_MAPLINK_SCROLL_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `targetMap` | `WM_Link` |

### `PROTO_NC_ITEM_MH_FURNITURE_ENDURE_KIT_USE_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `dEndEndureDate` | `ShineDateTime` |

### `PROTO_NC_ITEM_MH_FURNITURE_ENDURE_KIT_USE_REQ`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_ITEM_MINIMON_EQUIP_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_MIX_ITEM_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_MIX_ITEM_REQ`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_ITEM_MOVER_UPGRADE_ACK`  (sizeof = 108 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `ItemAttr` | `SHINE_ITEM_ATTRIBUTE` |

### `PROTO_NC_ITEM_MOVER_UPGRADE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_NEW_UPGRADE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_NEW_UPGRADE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_OPENCLASSCHANGEMENU_CMD`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_ITEM_OPENSTORAGEPAGE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEM_PICKOTHER_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEM_PICK_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEM_PICK_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_RANDOMOPTION_CHANGE_ACCEPT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_RANDOMOPTION_CHANGE_ACCEPT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_ITEM_RANDOMOPTION_CHANGE_ACK`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `changeAbleOption` | `ItemOptionStorage` |

### `PROTO_NC_ITEM_RANDOMOPTION_CHANGE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_RANDOMOPTION_RECOVER_COUNT_LIMIT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_RANDOMOPTION_RECOVER_COUNT_LIMIT_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_RELOC_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `from` | `ITEM_INVEN` |
|     2 | `to` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_REPURCHASE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_REPURCHASE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_REWARDINVENOPEN_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `itemarray` | `PROTO_ITEMPACKET_INFORM[?] /* 0 bytes */` |

### `PROTO_NC_ITEM_REWARDINVENOPEN_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_RINGEQUIP_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_ROT_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_SELL_ITEM_DELETE_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_SELL_ITEM_INSERT_CMD`  (sizeof = 105 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `ItemInfo` | `SHINE_ITEM_STRUCT` |

### `PROTO_NC_ITEM_SELL_ITEM_LIST_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `SellItemList` | `PROTO_SELL_ITEM_INFO[?] /* 0 bytes */` |

### `PROTO_NC_ITEM_SELL_REQ`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_ITEM_SHIELDENDURESET_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEM_SHIELDENDURE_CHARGE_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `ShieldSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_SHIELDENDURE_CHARGE_REQ`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `ShieldSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_SOMEONEPICK_CMD`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `pickerID` | `Name5` |

### `PROTO_NC_ITEM_SOMEONEUSE_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `useloc` | `SHINE_XY_TYPE` |

### `PROTO_NC_ITEM_SPLIT_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `from` | `ITEM_INVEN` |
|     2 | `to` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_TERMEXTEND_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_TERMEXTEND_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_UNEQUIP_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_UPGRADE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_UPGRADE_REQ`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_ITEM_USE_ACK`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_ITEM_USE_ACTIVESKILL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_USE_ACTIVESKILL_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `TargetLoc` | `SHINE_XY_TYPE` |

### `PROTO_NC_ITEM_USE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_ITEM_WEAPONENDURESET_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_ITEM_WEAPONENDURE_CHARGE_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `WeaponSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_WEAPONENDURE_CHARGE_REQ`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `WeaponSlot` | `ITEM_INVEN` |

### `PROTO_NC_ITEM_WITHDRAW_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_ITEM_WITHDRAW_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_KQ_COMPLETE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_KQ_ENTRYRESPONCE_ACK`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_KQ_ENTRYRESPONCE_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_KQ_FAIL_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_KQ_JOINER`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_KQ_JOINING_ALARM_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `KQInfo` | `KQ_JOINING_ALARM_INFO` |
|     9 | `Msg` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_KQ_JOINING_ALARM_END_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_KQ_JOINING_ALARM_LIST`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `KQList` | `KQ_JOINING_ALARM_INFO[?] /* 0 bytes */` |

### `PROTO_NC_KQ_JOIN_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_KQ_JOIN_CANCEL_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_KQ_JOIN_CANCEL_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_KQ_JOIN_LIST_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `CharInfoList` | `KQ_JOIN_CHAR_INFO[?] /* 0 bytes */` |

### `PROTO_NC_KQ_JOIN_LIST_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_KQ_JOIN_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_KQ_LINK_TO_FORCE_BY_BAN_CMD`  (sizeof = 52 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sMapName` | `Name3[?] /* 48 bytes */` |

### `PROTO_NC_KQ_LIST_ACK`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `tm_ServerTime` | `tm` |
|    50 | `NewQuestArray` | `PROTO_KQ_INFO_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_KQ_LIST_ADD_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `AddQuestArray` | `PROTO_KQ_INFO_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_KQ_LIST_DELETE_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `DeleteQuestArray` | `unsigned long[0]   /* 0 bytes */` |

### `PROTO_NC_KQ_LIST_REFRESH_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_KQ_LIST_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_KQ_LIST_TIME_ACK`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `tm_ServerTime` | `tm` |

### `PROTO_NC_KQ_LIST_UPDATE_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `UpdateQuestArray` | `KQ_UPDATE_ITEMS[?] /* 0 bytes */` |

### `PROTO_NC_KQ_MOBKILLNUMBER_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_KQ_NOREWARD_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_KQ_NOTIFY_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Msg` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_KQ_PLAYER_DISJOIN_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_KQ_RESTDEADNUM_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_KQ_REWARDFAIL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_KQ_REWARDSUC_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_KQ_REWARD_REQ`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `itmlst` | `PROTO_NC_ITEMDB_CREATEITEMLIST_REQ` |

### `PROTO_NC_KQ_SCHEDULE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `NewQuestArray` | `PROTO_KQ_INFO_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_KQ_SCHEDULE_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_KQ_SCORE_BOARD_INFO_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `Red` | `TEAM_SCORE_INFO` |
|    10 | `Blue` | `TEAM_SCORE_INFO` |

### `PROTO_NC_KQ_SCORE_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `array` | `PROTO_NC_KQ_SCORE_CMD::KQScore[?] /* 0 bytes */` |

### `PROTO_NC_KQ_SCORE_CMD::KQScore`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |
|    20 | `flag` | `PROTO_NC_KQ_SCORE_CMD::KQScore::<unnamed-type-flag>` |

### `PROTO_NC_KQ_SCORE_CMD::KQScore::<unnamed-type-flag>`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `medal` | `<LF_BITFIELD:0x1c966>` |
|     0 | `gender` | `<LF_BITFIELD:0xd682>` |
|     1 | `chrcls` | `<LF_BITFIELD:0xd683>` |

### `PROTO_NC_KQ_SCORE_INFO_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nScore` | `unsigned long[2]   /* 8 bytes */` |

### `PROTO_NC_KQ_SCORE_SIMPLE_CMD`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `self` | `PROTO_NC_KQ_SCORE_SIMPLE_CMD::<unnamed-type-self>` |
|    18 | `array` | `PROTO_NC_KQ_SCORE_SIMPLE_CMD::KQScore[?] /* 0 bytes */` |

### `PROTO_NC_KQ_SCORE_SIMPLE_CMD::<unnamed-type-self>`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_KQ_SCORE_SIMPLE_CMD::KQScore`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |

### `PROTO_NC_KQ_START_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_KQ_STATUS_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `JoinerNameList` | `Name5[?] /* 0 bytes */` |

### `PROTO_NC_KQ_STATUS_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_KQ_TEAM_SELECT_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_KQ_TEAM_SELECT_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCharID` | `Name5` |

### `PROTO_NC_KQ_TEAM_SELECT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_KQ_TEAM_TYPE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_KQ_VOTE_BAN_MSG_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_KQ_VOTE_BAN_MSG_LOGOFF_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_KQ_VOTE_CANCEL_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sTarget` | `Name5` |

### `PROTO_NC_KQ_VOTE_RESULT_FAIL_CMD`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sTarget` | `Name5` |

### `PROTO_NC_KQ_VOTE_RESULT_SUC_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sTarget` | `Name5` |

### `PROTO_NC_KQ_VOTE_START_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_KQ_VOTE_START_CHECK_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_KQ_VOTE_START_CHECK_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_KQ_VOTE_START_REQ`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sTarget` | `Name5` |
|    22 | `sContents` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_KQ_VOTE_VOTING_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_KQ_VOTE_VOTING_CMD`  (sizeof = 78 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sStarter` | `Name5` |
|    20 | `sTarget` | `Name5` |
|    41 | `tEndTime` | `tm` |
|    78 | `sContents` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_KQ_VOTE_VOTING_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `eVoteType` | `KQ_VOTING_TYPE` |

### `PROTO_NC_KQ_W2Z_DESTROY_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_KQ_W2Z_MAKE_REQ`  (sizeof = 377 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `KQInfo` | `PROTO_KQ_INFO` |

### `PROTO_NC_KQ_W2Z_START_CMD`  (sizeof = 379 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `KQInfo` | `PROTO_KQ_INFO` |
|   379 | `JoinerArray` | `PROTO_NC_KQ_JOINER[?] /* 0 bytes */` |

### `PROTO_NC_KQ_WINTER_EVENT_2014_SCORE_CMD`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Red` | `TEAM_SCORE_INFO` |
|     8 | `Blue` | `TEAM_SCORE_INFO` |

### `PROTO_NC_KQ_Z2W_END_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_KQ_Z2W_MAKE_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_LOG_GAME_ADD`  (sizeof = 68 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |
|    40 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_ADD_ACK`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_LOG_GAME_ADD_REQ`  (sizeof = 72 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `logdata` | `PROTO_NC_LOG_GAME_ADD` |

### `PROTO_NC_LOG_GAME_ARENA_CNG`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_LOG_GAME_ARENA_FBZ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_LOG_GAME_ARENA_GUILD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_LOG_GAME_ARENA_PVP`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_LOG_GAME_CHANGE_CLASS`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_CHARGED_BUFF_CLR`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_LOG_GAME_CHARGED_BUFF_SET`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `buff_endtime` | `ShineDateTime` |
|    18 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_CHARGE_WITHDRAW`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_LOG_GAME_CREATE_AVATAR`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_LOG_GAME_CREATE_AVATAR_SEND`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `log` | `PROTO_NC_LOG_GAME_CREATE_AVATAR` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_0`  (sizeof = 68 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_1`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_2`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_3`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_4`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_5`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_LOG_GAME_DATA_TYPE_6`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_7`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_8`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_9`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_A`  (sizeof = 36 bytes)

_no members_

### `PROTO_NC_LOG_GAME_DATA_TYPE_B`  (sizeof = 60 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_C`  (sizeof = 56 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_D`  (sizeof = 24 bytes)

_no members_

### `PROTO_NC_LOG_GAME_DATA_TYPE_E`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_F`  (sizeof = 64 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_G`  (sizeof = 60 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_H`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_I`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_J`  (sizeof = 52 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_K`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_GAME_DATA_TYPE_L`  (sizeof = 44 bytes)

_no members_

### `PROTO_NC_LOG_GAME_DELETE_AVATAR`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_LOG_GAME_DELETE_AVATAR_SEND`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `log` | `PROTO_NC_LOG_GAME_DELETE_AVATAR` |

### `PROTO_NC_LOG_GAME_DISENCHANT`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_EMBLEM`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_LOG_GAME_ENCHANNT`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_FRIEND_ADD`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_FRIEND_DELETE`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_GUILD_4_REWARD_FAME`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_4_TOURNAMENT_RESULT`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_CREATE`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    24 | `sGuildName` | `Name4` |

### `PROTO_NC_LOG_GAME_GUILD_DELETE`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sGuildName` | `Name4` |

### `PROTO_NC_LOG_GAME_GUILD_GRADE`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_G_REWARD_EXP`  (sizeof = 20 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_G_REWARD_MONEY`  (sizeof = 20 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_G_REWARD_TOKEN`  (sizeof = 20 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_K_MONEY_WITHDRAW`  (sizeof = 20 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_M_BANISH`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_M_GRADE`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_M_JOIN`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_M_LEAVE`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_TYPE`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_WAR_ACCEPT`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_WAR_DECLARE`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_LOG_GAME_GUILD_WAR_RESULT`  (sizeof = 29 bytes)

_no members_

### `PROTO_NC_LOG_GAME_HIT`  (sizeof = 31 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_ITEM_BOOTH_BUY`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_ITEM_BOOTH_SELL`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_ITEM_BREAK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_ITEM_BUY`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    17 | `iteminven` | `ITEM_INVEN` |

### `PROTO_NC_LOG_GAME_ITEM_CREATE`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    18 | `iteminvento` | `ITEM_INVEN` |

### `PROTO_NC_LOG_GAME_ITEM_CW_BREAKATZERO`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_ITEM_DROP`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |
|    24 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    34 | `iteminven` | `ITEM_INVEN` |

### `PROTO_NC_LOG_GAME_ITEM_EQUIP`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `iteminvenfrom` | `ITEM_INVEN` |
|    14 | `iteminvento` | `ITEM_INVEN` |

### `PROTO_NC_LOG_GAME_ITEM_INVEN_MOVE`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `iteminvenfrom` | `ITEM_INVEN` |
|    14 | `iteminvento` | `ITEM_INVEN` |

### `PROTO_NC_LOG_GAME_ITEM_MERGE`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |
|    26 | `itemkeyadd` | `SHINE_ITEM_REGISTNUMBER` |
|    34 | `itemkeysub` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_ITEM_MOB_DROP`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |
|    24 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_ITEM_MOB_DROP_RATE`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_LOG_GAME_ITEM_PRODUCT`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_ITEM_PRODUCT_STUFF`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `itemkey_stuff` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_ITEM_PUT_ON_BELONGED`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `iteminvenfrom` | `ITEM_INVEN` |
|    14 | `iteminvento` | `ITEM_INVEN` |

### `PROTO_NC_LOG_GAME_ITEM_SELL`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    17 | `iteminven` | `ITEM_INVEN` |

### `PROTO_NC_LOG_GAME_ITEM_SOULSTONEBUY`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_ITEM_SPLIT`  (sizeof = 46 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |
|    26 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    34 | `itemkeynew` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_ITEM_STORE_IN`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |
|    24 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    32 | `iteminvenfrom` | `ITEM_INVEN` |
|    34 | `iteminvento` | `ITEM_INVEN` |

### `PROTO_NC_LOG_GAME_ITEM_STORE_OUT`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |
|    24 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    32 | `iteminvenfrom` | `ITEM_INVEN` |
|    34 | `iteminvento` | `ITEM_INVEN` |

### `PROTO_NC_LOG_GAME_ITEM_TAKE`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |
|    24 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    34 | `iteminven` | `ITEM_INVEN` |

### `PROTO_NC_LOG_GAME_ITEM_TITLE`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `WeaponItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `LicenseItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_ITEM_TRADE`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_ITEM_UNEQUIP`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `iteminvenfrom` | `ITEM_INVEN` |
|    14 | `iteminvento` | `ITEM_INVEN` |

### `PROTO_NC_LOG_GAME_ITEM_UPGRADE`  (sizeof = 43 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |
|    24 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    40 | `randomoption_elemental` | `ItemOptionStorage::Element` |

### `PROTO_NC_LOG_GAME_ITEM_USE`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |
|    26 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    36 | `iteminven` | `ITEM_INVEN` |

### `PROTO_NC_LOG_GAME_ITEM_USEALL`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_ITEM_USELOT`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_KQ_ENTER`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_KQ_LEAVE`  (sizeof = 31 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_LEVEL_DOWN`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_LEVEL_UP`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_LINK`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_LOGIN`  (sizeof = 56 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |
|    36 | `spawnapps` | `Name5` |

### `PROTO_NC_LOG_GAME_LOGOUT`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MAS_PUP_ADD`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MAS_PUP_DELETE`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MINIGAME`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_LOG_GAME_MINIHOUSE`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_LOG_GAME_MINIHOUSE_BUILDING`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MINIHOUSE_VISIT`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MK`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MK2_DEAD`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MK2_FAIL`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MK2_START`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MK2_SUCCESS`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MKED`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MK_DROP_ITEM`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MK_GETEXP`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_MK_GETEXPINFIELD`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_LOG_GAME_MONEY_CHANGE`  (sizeof = 28 bytes)

_no members_

### `PROTO_NC_LOG_GAME_MONEY_DEPOSIT`  (sizeof = 20 bytes)

_no members_

### `PROTO_NC_LOG_GAME_MONEY_TRADE_INCOME`  (sizeof = 24 bytes)

_no members_

### `PROTO_NC_LOG_GAME_MONEY_TRADE_OUTGO`  (sizeof = 24 bytes)

_no members_

### `PROTO_NC_LOG_GAME_MONEY_WITHDRAW`  (sizeof = 20 bytes)

_no members_

### `PROTO_NC_LOG_GAME_MOVE`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `startpos` | `MAPPOS` |
|    24 | `endpos` | `SHINE_XY_TYPE` |

### `PROTO_NC_LOG_GAME_PARTY_BANISH`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_PARTY_CHG_MAS`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_PARTY_CREATE`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_PARTY_DELETE`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_PARTY_JOIN`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_PARTY_LEAVE`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_PET`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_LOG_GAME_PK`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_PKED`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_PRISON`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_LOG_GAME_PRISON_RELEASE`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_QUEST_COMPLETE`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_QUEST_DELETE`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_QUEST_GET`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_QUEST_ITEM_GET`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |
|    28 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_QUEST_REWARD_ABSTATE`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_GAME_QUEST_REWARD_EXP`  (sizeof = 22 bytes)

_no members_

### `PROTO_NC_LOG_GAME_QUEST_REWARD_FAME`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_LOG_GAME_QUEST_REWARD_ITEM`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GAME_QUEST_REWARD_MINIHOUSE`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_LOG_GAME_QUEST_REWARD_MONEY`  (sizeof = 22 bytes)

_no members_

### `PROTO_NC_LOG_GAME_QUEST_REWARD_PET`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_LOG_GAME_QUEST_REWARD_TITLE`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_GAME_QUEST_SET_INFO`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_SEAWAR`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_LOG_GAME_SKILL_DELETE`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_SKILL_LEARN`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_SKILL_USE`  (sizeof = 31 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_STATE_CLEAR`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GAME_STATE_SET`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GMAE_LOGOUT_ZONEINFO`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_GUILD_ACADEMY_CHAT_BAN`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_LOG_GUILD_ACADEMY_CLEAR_REWARD_ITEM`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_GUILD_ACADEMY_CLEAR_REWARD_MONEY`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_GUILD_ACADEMY_GRADUATE`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_LOG_GUILD_ACADEMY_GUILD_INVITE`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_LOG_GUILD_ACADEMY_JOIN`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_GUILD_ACADEMY_LEAVE`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_GUILD_ACADEMY_LEVEL_UP`  (sizeof = 17 bytes)

_no members_

### `PROTO_NC_LOG_GUILD_ACADEMY_MASTER_TELEPORT`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `ToMapPosition` | `MAPPOS` |

### `PROTO_NC_LOG_GUILD_ACADEMY_REWARD_ITEM`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `ItemInven` | `ITEM_INVEN` |
|    13 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GUILD_ACADEMY_REWARD_ITEM_PAY`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    11 | `ItemInven` | `ITEM_INVEN` |
|    13 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_GUILD_ACADEMY_REWARD_MONEY`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_LOG_GUILD_ACADEMY_SET_MASTER`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_LOG_GUILD_ACADEMY_SET_REWARD_ITEM`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_LOG_GUILD_ACADEMY_SET_REWARD_MONEY`  (sizeof = 17 bytes)

_no members_

### `PROTO_NC_LOG_GUILD_ACADEMY_VANISH`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_LOG_ITEMMONEY_BUY`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `nItemkey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_MOVER_RAREMOVER`  (sizeof = 31 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    11 | `nRare_Key` | `SHINE_ITEM_REGISTNUMBER` |
|    23 | `nConsum_Key` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_MOVER_UPGRADE`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    11 | `nMain_Key` | `SHINE_ITEM_REGISTNUMBER` |
|    22 | `nConsum_Key` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_LOG_RANDOMOPTION_CHANGE`  (sizeof = 36 bytes)

_no members_

### `PROTO_NC_LOG_RANDOMOPTION_CHANGE_BEFORE`  (sizeof = 68 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sMap` | `Name3` |

### `PROTO_NC_LOG_REGENLOCATESAVE_CMD`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `SaveMap` | `Name3` |
|    16 | `SaveLocate` | `SHINE_XY_TYPE` |
|    28 | `InvalidLocate` | `SHINE_XY_TYPE` |

### `PROTO_NC_LOG_USER_LOGIN`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `ip` | `unsigned char[4]   /* 4 bytes */` |

### `PROTO_NC_LOG_USER_LOGINFAIL`  (sizeof = 278 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `userid` | `Name256Byte` |
|   256 | `userpassword` | `Name4` |
|   272 | `userip` | `unsigned char[4]   /* 4 bytes */` |

### `PROTO_NC_LOG_USER_LOGINFAIL_SEND`  (sizeof = 281 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `log` | `PROTO_NC_LOG_USER_LOGINFAIL` |

### `PROTO_NC_LOG_USER_LOGIN_SEND`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `log` | `PROTO_NC_LOG_USER_LOGIN` |

### `PROTO_NC_LOG_USER_LOGOUT`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_LOG_USER_LOGOUT_SEND`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `log` | `PROTO_NC_LOG_USER_LOGOUT` |

### `PROTO_NC_LOG_WEDDING_DIVORCE_CANCEL`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_WEDDING_DIVORCE_EXE`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_WEDDING_DIVORCE_REQ`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_LOG_WEDDING_HALL_CANCEL`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_WEDDING_HALL_RESERVE`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_WEDDING_HALL_START`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_WEDDING_PROPOSE_CANCEL`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_WEDDING_PROPOSE_EXE`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_LOG_WEDDING_WEDDING_EXE`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_MAP_CAN_USE_REVIVEITEM_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MAP_EXPBONUS_RNG`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |

### `PROTO_NC_MAP_FIELD_ATTRIBUTE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `eFieldMapType` | `FIELD_MAP_TYPE` |

### `PROTO_NC_MAP_INDUN_LEVEL_VIEW_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `eLevelType` | `ID_LEVEL_TYPE` |

### `PROTO_NC_MAP_ITEMBONUS_RNG`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |

### `PROTO_NC_MAP_LINKALLOW_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MAP_LINKEND_CLIENT_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MAP_LINKEND_CMD`  (sizeof = 47 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `map` | `Name3` |
|    21 | `location` | `SHINE_XY_TYPE` |

### `PROTO_NC_MAP_LINKRESERVE_ACK`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `linkother` | `PROTO_NC_CHAR_REVIVEOTHER_CMD` |

### `PROTO_NC_MAP_LINKRESERVE_ACK_SEND`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_MAP_LINKRESERVE_ACK` |

### `PROTO_NC_MAP_LINKRESERVE_REQ`  (sizeof = 59 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     9 | `linkto` | `PROTO_NC_MAP_LINKRESERVE_REQ::<unnamed-type-linkto>` |
|    29 | `linkother` | `PROTO_NC_CHAR_REVIVEOTHER_CMD` |

### `PROTO_NC_MAP_LINKRESERVE_REQ::<unnamed-type-linkto>`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mapname` | `Name3` |
|    12 | `location` | `SHINE_XY_TYPE` |

### `PROTO_NC_MAP_LINKSTART_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_MAP_LINKTO_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `link` | `PROTO_NC_CHAR_REVIVESAME_CMD[?] /* 0 bytes */` |

### `PROTO_NC_MAP_LINK_FAIL`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MAP_LOGINCOMPLETE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MAP_LOGINFAIL_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MAP_LOGIN_REQ`  (sizeof = 1622 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chardata` | `PROTO_NC_CHAR_ZONE_CHARDATA_REQ` |
|    22 | `checksum` | `Name8[?] /* 1600 bytes */` |

### `PROTO_NC_MAP_LOGOUT_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MAP_MULTY_LINK_CMD`  (sizeof = 73 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `npcPosition` | `SHINE_XY_TYPE` |
|    13 | `LinkMapName` | `Name3[?] /* 60 bytes */` |

### `PROTO_NC_MAP_MULTY_LINK_SELECT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MAP_MULTY_LINK_SELECT_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LinkMapName` | `Name3` |

### `PROTO_NC_MAP_REGIST_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `maparray` | `PROTO_NC_MAP_REGIST_CMD::<unnamed-type-maparray>[?] /* 0 bytes */` |

### `PROTO_NC_MAP_REGIST_CMD::<unnamed-type-maparray>`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `mapname` | `Name3` |

### `PROTO_NC_MAP_TOWNPORTAL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MAP_TOWNPORTAL_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MAP_WING_FLY_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MAP_WING_FLY_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MAP_WING_SAVE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MAP_WING_SAVE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MENU_INDUNRANK_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MENU_OPENSTORAGE_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `itemarray` | `PROTO_ITEMPACKET_INFORM[?] /* 0 bytes */` |

### `PROTO_NC_MENU_OPENSTORAGE_FAIL_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MENU_SERVERMENU_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MENU_SERVERMENU_CLOSE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MENU_SERVERMENU_REQ`  (sizeof = 142 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `title` | `unsigned char[128]   /* 128 bytes */` |
|   131 | `npcPosition` | `SHINE_XY_TYPE` |
|   142 | `menu` | `SERVERMENU[?] /* 0 bytes */` |

### `PROTO_NC_MENU_SHOPOPENSOULSTONE_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hp` | `PROTO_NC_MENU_SHOPOPENSOULSTONE_CMD::SOULSTONEMENU` |
|    12 | `sp` | `PROTO_NC_MENU_SHOPOPENSOULSTONE_CMD::SOULSTONEMENU` |

### `PROTO_NC_MENU_SHOPOPENSOULSTONE_CMD::SOULSTONEMENU`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_MENU_SHOPOPENTABLE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemlist` | `PROTO_NC_MENU_SHOPOPENTABLE_CMD::MENUITEM[?] /* 0 bytes */` |

### `PROTO_NC_MENU_SHOPOPENTABLE_CMD::MENUITEM`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MENU_SHOPOPEN_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `itemlist` | `PROTO_NC_MENU_SHOPOPEN_CMD::MENUITEM[?] /* 0 bytes */` |

### `PROTO_NC_MENU_SHOPOPEN_CMD::MENUITEM`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_ACTIV_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_ARRANGEMODE_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_ARRANGEMODE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_BUILDING_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_BUILDING_CMD`  (sizeof = 66 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `minihouseinfo` | `PROTO_NC_BRIEFINFO_MINIHOUSEBUILD_CMD` |

### `PROTO_NC_MINIHOUSE_BUILDING_REQ`  (sizeof = 133 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `password` | `wchar_t[9]   /* 9 bytes */` |
|    10 | `title` | `wchar_t[21]   /* 21 bytes */` |
|    32 | `sNotify` | `wchar_t[101]   /* 101 bytes */` |

### `PROTO_NC_MINIHOUSE_CHAR_ACTION_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_CHAR_ACTION_CMD`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_CHAR_ACTION_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_COMPULSIONMOVETO_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `location` | `SHINE_XY_TYPE` |

### `PROTO_NC_MINIHOUSE_CREATE_FURNITURE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `rearrange` | `PROTO_NC_MINIHOUSE_CREATE_FURNITURE_CMD::<unnamed-type-rearrange>[?] /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_CREATE_FURNITURE_CMD::<unnamed-type-rearrange>`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `location` | `SHINE_SPACE_TYPE` |

### `PROTO_NC_MINIHOUSE_CREATE_FURNITURE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `rearrange` | `PROTO_NC_MINIHOUSE_CREATE_FURNITURE_REQ::<unnamed-type-rearrange>[?] /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_CREATE_FURNITURE_REQ::<unnamed-type-rearrange>`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `location` | `SHINE_SPACE_TYPE` |

### `PROTO_NC_MINIHOUSE_DB_OWNERBLOG_GET_ACK`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    13 | `blogaddr` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_DB_OWNERBLOG_GET_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_MINIHOUSE_DB_OWNERBLOG_SET_ACK`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    13 | `blogaddr` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_DB_OWNERBLOG_SET_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    11 | `blogaddr` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_DB_PORTAL_ADD_ACK`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_MINIHOUSE_DB_PORTAL_ADD_REQ`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    15 | `Item` | `PROTO_ITEM_RELOC` |

### `PROTO_NC_MINIHOUSE_DB_PORTAL_DEL_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_MINIHOUSE_DB_PORTAL_DEL_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_MINIHOUSE_DB_PORTAL_LIST_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|    10 | `PortalInfo` | `PORTAL_INFO[?] /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_DB_PORTAL_LIST_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_MINIHOUSE_DB_VISITER_COUNT_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_MINIHOUSE_DB_VISITER_COUNT_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_MINIHOUSE_DELETE_FURNITURE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `furniturehandle` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_FUNICHER_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `array` | `PROTO_NC_MINIHOUSE_FUNICHER_CMD::<unnamed-type-array>[?] /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_FUNICHER_CMD::<unnamed-type-array>`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `location` | `SHINE_SPACE_TYPE` |

### `PROTO_NC_MINIHOUSE_FUNITUREINFOCOMPLETE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_FURNITURE_EFFECT_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_FURNITURE_EFFECT_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `nApplyPlayerHandle` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_FURNITURE_EFFECT_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_FURNITURE_EMOTION_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_FURNITURE_EMOTION_CANCEL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_FURNITURE_EMOTION_CANCEL_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `nCancelEmotion` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_FURNITURE_EMOTION_CANCEL_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_FURNITURE_EMOTION_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `Emotion` | `FURNITURE_EMOTION_INFO[?] /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_FURNITURE_EMOTION_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_FURNITURE_ENDURE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `EndureFurniture` | `ENDURE_FURNITURE_INFO[?] /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_KICKOUTCANCEL_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_KICKOUT_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_KICKOUT_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_LOGINCOMPLETE_ACK`  (sizeof = 140 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sNotify` | `wchar_t[101]   /* 101 bytes */` |
|   107 | `TargetKey` | `wchar_t[32]   /* 32 bytes */` |
|   140 | `player` | `PROTO_NC_MINIHOUSE_LOGINCOMPLETE_ACK::<unnamed-type-player>[?] /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_LOGINCOMPLETE_ACK::<unnamed-type-player>`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_LOGINCOMPLETE_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_LOGOUTCOMPLETE_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_MODIFY_ITEM_INFO_OPEN_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_MODIFY_ITEM_INFO_OPEN_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_MODIFY_ITEM_INFO_OPEN_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_MODIFY_MAXENTERNUM_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_MODIFY_NOTIFY_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_MODIFY_NOTIFY_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `sNotify` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_MODIFY_NOTIFY_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `sNotify` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_MODIFY_OUTSIDE_TITLE_CMD`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `title` | `wchar_t[21]   /* 21 bytes */` |

### `PROTO_NC_MINIHOUSE_MODIFY_OWNERBLOG_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `blogaddr` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_MODIFY_PASSWORD_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_MODIFY_PASSWORD_REQ`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `password` | `wchar_t[9]   /* 9 bytes */` |

### `PROTO_NC_MINIHOUSE_MODIFY_TITLE_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `title` | `wchar_t[21]   /* 21 bytes */` |

### `PROTO_NC_MINIHOUSE_OWNERBLOG_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `blogaddr` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_PORTAL_ADD_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `NewPortal` | `PORTAL_INFO` |

### `PROTO_NC_MINIHOUSE_PORTAL_CLOSE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_PORTAL_CLOSE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_PORTAL_CLOSE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_PORTAL_DEL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_PORTAL_DEL_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_PORTAL_DEL_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_PORTAL_EFFECT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_PORTAL_EFFECT_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `nApplyPlayerHandle` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_PORTAL_EFFECT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_PORTAL_LIST_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `PortalInfo` | `PORTAL_INFO[?] /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_PORTAL_OPEN_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_PORTAL_OPEN_CMD`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `Location` | `SHINE_SPACE_TYPE` |

### `PROTO_NC_MINIHOUSE_PORTAL_OPEN_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_REARRANGE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `rearrange` | `PROTO_NC_MINIHOUSE_REARRANGE_REQ::<unnamed-type-rearrange>[?] /* 0 bytes */` |

### `PROTO_NC_MINIHOUSE_REARRANGE_REQ::<unnamed-type-rearrange>`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `location` | `SHINE_SPACE_TYPE` |

### `PROTO_NC_MINIHOUSE_VISITREADY_ACK`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_VISITREADY_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MINIHOUSE_VISIT_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `password` | `wchar_t[9]   /* 9 bytes */` |

### `PROTO_NC_MISC_APEX_CLIENT_CHCSTART_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_MISC_APEX_CLIENT_DATA_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `Buff` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_APEX_SERVER_DATA_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `Buff` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_CHAR_LOGOFF_STATISTICS`  (sizeof = 21 bytes)

_no members_

### `PROTO_NC_MISC_CHAT_BLOCK_SPAMER_BLOCKTIME_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_MISC_CLIENT_DEBUG_MSG_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `msg` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_CLIENT_LOADING_BUG_DETECT_CMD`  (sizeof = 64 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sLoadingMapName` | `Name3` |
|    16 | `sFromMapName` | `Name3` |
|    28 | `sFromZoneIP` | `Name4` |
|    44 | `sToZoneIP` | `Name4` |
|    64 | `ErrorData` | `CLBD_ErrorData[?] /* 0 bytes */` |

### `PROTO_NC_MISC_CONNECTER_ACK`  (sizeof = 131 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `nEachClass` | `unsigned short[64]   /* 128 bytes */` |

### `PROTO_NC_MISC_CONNECTER_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MISC_CONNECTFROMWHERE_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_MISC_CONNECTFROMWHERE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MISC_CONNECTFROMWHERE_DB_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `ToZoneAck` | `PROTO_NC_MISC_CONNECTFROMWHERE_ACK` |

### `PROTO_NC_MISC_CONNECTFROMWHERE_DB_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `FromZoneReq` | `PROTO_NC_MISC_CONNECTFROMWHERE_REQ` |

### `PROTO_NC_MISC_CONNECTFROMWHERE_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `connectip` | `unsigned char[4]   /* 4 bytes */` |

### `PROTO_NC_MISC_CS_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `Data` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_CS_CLOSE`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_MISC_CS_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `Data` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_DB_CLIENT_LOADING_BUG_DETECT_CMD`  (sizeof = 72 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sLoadingMapName` | `Name3` |
|    24 | `sFromMapName` | `Name3` |
|    36 | `sFromZoneIP` | `Name4` |
|    52 | `sToZoneIP` | `Name4` |
|    72 | `ErrorData` | `CLBD_ErrorData[?] /* 0 bytes */` |

### `PROTO_NC_MISC_DELIVER_WM_LOGIN_ACDB_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `packet` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_DELIVER_WM_LOGIN_ACDB_CMD_SEND`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_MISC_DELIVER_WM_LOGIN_ACDB_CMD` |

### `PROTO_NC_MISC_DELIVER_WM_LOGIN_ALDB_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `packet` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_DELIVER_WM_LOGIN_ALDB_CMD_SEND`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_MISC_DELIVER_WM_LOGIN_ALDB_CMD` |

### `PROTO_NC_MISC_EVENTNPC_STANDEND_CLIENT_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMobIndex` | `Name8` |

### `PROTO_NC_MISC_EVENTNPC_STANDEND_ZONE_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMobIndex` | `Name8` |

### `PROTO_NC_MISC_EVENTNPC_STANDSTART_CLIENT_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMobIndex` | `Name8` |

### `PROTO_NC_MISC_EVENTNPC_STANDSTART_ZONE_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMobIndex` | `Name8` |

### `PROTO_NC_MISC_EVENT_DONE_MUNSANG_ACC2WM`  (sizeof = 15 bytes)

_no members_

### `PROTO_NC_MISC_EVENT_DONE_MUNSANG_WM2ACC`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_MISC_EVENT_DONE_MUNSANG_WM2Z`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_MISC_EVENT_DONE_MUNSANG_Z2CLI`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MISC_EVENT_DONE_MUNSANG_Z2WM`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_MISC_EVENT_HIT3_ADD_CASH`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `UserID` | `unsigned char[20]   /* 20 bytes */` |

### `PROTO_NC_MISC_EVENT_L20_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MISC_EVENT_L20_DB_ACK`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_MISC_EVENT_L20_DB_REQ`  (sizeof = 272 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sUserID` | `Name256Byte` |

### `PROTO_NC_MISC_EVENT_LONG_TIME_PLAY_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_MISC_GAMETIME_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MISC_GAMETIME_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MISC_GAMETIME_REQ_SEND`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |

### `PROTO_NC_MISC_GET_CHAT_BLOCK_SPAM_FILTER_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MISC_GET_CHAT_BLOCK_SPAM_FILTER_DB_CMD`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_MISC_GM_CHAT_COLOR_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_MISC_HACK_SCAN_STORE_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sDesc` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_HACK_SCAN_STORE_DB_CMD`  (sizeof = 51 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `sIP` | `unsigned char[20]   /* 20 bytes */` |
|    29 | `sCharID` | `Name5` |
|    49 | `ScanInfo` | `PROTO_NC_MISC_HACK_SCAN_STORE_CMD` |

### `PROTO_NC_MISC_HEARTBEAT_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MISC_HEARTBEAT_ACK_SEND`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |

### `PROTO_NC_MISC_HEARTBEAT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MISC_HEARTBEAT_REQ_SEND`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |

### `PROTO_NC_MISC_ITEMSHOP_URL_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sURL` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_ITEMSHOP_URL_DB_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sURL` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_ITEMSHOP_URL_DB_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_MISC_ITEMSHOP_URL_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MISC_PINGTEST_CLIENT_ZONE_DB`  (sizeof = 124 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    46 | `header` | `NETPACKETZONEHEADER` |
|    52 | `buylot` | `PROTO_NC_ITEMDB_BUYLOT_REQ` |

### `PROTO_NC_MISC_PINGTEST_TOOL_WM_CLIENT_ZONE_DB`  (sizeof = 164 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `TargetCharName` | `Name5` |
|    86 | `header` | `NETPACKETZONEHEADER` |
|    92 | `buylot` | `PROTO_NC_ITEMDB_BUYLOT_REQ` |

### `PROTO_NC_MISC_PINGTEST_TOOL_WM_DB`  (sizeof = 38 bytes)

_no members_

### `PROTO_NC_MISC_PINGTEST_TOOL_WM_ZONE`  (sizeof = 34 bytes)

_no members_

### `PROTO_NC_MISC_PINGTEST_TOOL_WM_ZONE_DB`  (sizeof = 58 bytes)

_no members_

### `PROTO_NC_MISC_RESTMINUTE_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MISC_S2SCONNECTION_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MISC_S2SCONNECTION_ACK_SEND`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_MISC_S2SCONNECTION_ACK` |

### `PROTO_NC_MISC_S2SCONNECTION_RDY`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MISC_S2SCONNECTION_RDY_SEND`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |

### `PROTO_NC_MISC_S2SCONNECTION_REQ`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_MISC_S2SCONNECTION_REQ_SEND`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `req` | `PROTO_NC_MISC_S2SCONNECTION_REQ` |

### `PROTO_NC_MISC_SEED_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MISC_SERVERPARAMETER_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MISC_SERVERPARAMETER_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MISC_SERVER_TIME_NOTIFY_CMD`  (sizeof = 37 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `dCurrentTM` | `tm` |

### `PROTO_NC_MISC_SET_CHAT_BLOCK_SPAM_FILTER_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MISC_SET_CHAT_BLOCK_SPAM_FILTER_DB_CMD`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_MISC_SPAMMER_CHAT_BAN_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sSpammerName` | `Name5` |

### `PROTO_NC_MISC_SPAMMER_CHAT_BAN_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sSpammerName` | `Name5` |

### `PROTO_NC_MISC_SPAMMER_RELEASE_CHAT_BAN_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MISC_SPAMMER_RELEASE_CHAT_BAN_DB_ACK`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sSpammerName` | `Name5` |

### `PROTO_NC_MISC_SPAMMER_RELEASE_CHAT_BAN_DB_REQ`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sSpammerName` | `Name5` |

### `PROTO_NC_MISC_SPAMMER_RELEASE_CHAT_BAN_REQ`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sSpammerName` | `Name5` |

### `PROTO_NC_MISC_SPAMMER_REPORT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MISC_SPAMMER_REPORT_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sSpammerName` | `Name5` |

### `PROTO_NC_MISC_SPAMMER_SET_DB_CHAT_BAN_ACK`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sSpammerName` | `Name5` |

### `PROTO_NC_MISC_SPAMMER_SET_DB_CHAT_BAN_REQ`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sSpammerName` | `Name5` |

### `PROTO_NC_MISC_SPAMMER_ZONE_CHAT_BAN_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sSpammerName` | `Name5` |

### `PROTO_NC_MISC_START_THE_BOOM_CMD`  (sizeof = 64 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sKey1` | `unsigned char[16]   /* 16 bytes */` |
|    16 | `sKey2` | `unsigned char[16]   /* 16 bytes */` |
|    32 | `sKey3` | `unsigned char[16]   /* 16 bytes */` |
|    48 | `sKey4` | `unsigned char[16]   /* 16 bytes */` |

### `PROTO_NC_MISC_TIMEFROMWORLD_CMD`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `TimeStr` | `tm` |

### `PROTO_NC_MISC_USER_COUNT_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_MISC_WEB_DB_KEY_ACK`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `WebKey` | `unsigned char[32]   /* 32 bytes */` |

### `PROTO_NC_MISC_WEB_DB_KEY_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_MISC_WEB_KEY_ACK`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `WebKey` | `unsigned char[32]   /* 32 bytes */` |

### `PROTO_NC_MISC_WEB_KEY_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MISC_WHSHANDLEFIX_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_MISC_WHSHANDLEREPAIR_CMD`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_MISC_XTRAP2_CLIENT_DATA_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `Data` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_XTRAP2_OPTOOL_READ_CODEMAP_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MISC_XTRAP2_OPTOOL_READ_CODEMAP_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MISC_XTRAP2_SERVER_DATA_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `Data` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_ZONERINGLINKTEST_RNG`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     8 | `addr` | `Name4[?] /* 0 bytes */` |

### `PROTO_NC_MOVER_FEEDING_ERROR_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MOVER_HUNGRY_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MOVER_MOVESPEED_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_MOVER_RIDE_FAIL_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MOVER_RIDE_OFF_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MOVER_RIDE_OFF_FAIL_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MOVER_RIDE_OFF_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MOVER_RIDE_ON_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_MOVER_RIDE_ON_FAIL_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MOVER_RIDE_ON_REQ`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MOVER_SKILLBASH_FLD_CAST_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `nTargetLoc` | `SHINE_XY_TYPE` |

### `PROTO_NC_MOVER_SKILLBASH_OBJ_CAST_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_MOVER_SOMEONE_RIDE_OFF_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_MOVER_SOMEONE_RIDE_ON_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_OPTOOL_ACK_CLIENT_NUM_OF_USER_LIMIT`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_OPTOOL_CHARACTER_DELETE_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_CHARACTER_DELETE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_OPTOOL_CHARACTER_DELETE_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_OPTOOL_CLOSE_SERVER_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_OPTOOL_CLOSE_SERVER_REQ`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `closekey` | `unsigned char[32]   /* 32 bytes */` |

### `PROTO_NC_OPTOOL_CONNECT_BRIF_ACK`  (sizeof = 170 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `count` | `unsigned short[2]   /* 4 bytes */[2]   /* 8 bytes */[21]   /* 168 bytes */` |

### `PROTO_NC_OPTOOL_CONNECT_BRIF_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_FIND_USER_ACK`  (sizeof = 297 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `sUserID` | `Name256Byte` |
|   265 | `sCharID` | `Name5` |
|   285 | `sMapName` | `Name3` |

### `PROTO_NC_OPTOOL_FIND_USER_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_OPTOOL_GUILD_CHANGE_MEMBER_GRADE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_OPTOOL_GUILD_CHANGE_MEMBER_GRADE_REQ`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_OPTOOL_GUILD_DATA_CHANGE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_OPTOOL_GUILD_DATA_CHANGE_REQ`  (sizeof = 739 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sName` | `Name4` |
|    20 | `sPassword` | `Name3` |
|    79 | `sNotifyCharID` | `Name5` |
|    99 | `sIntro` | `wchar_t[128]   /* 128 bytes */` |
|   227 | `sNotify` | `wchar_t[512]   /* 512 bytes */` |

### `PROTO_NC_OPTOOL_GUILD_DISMISS_CANCEL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_OPTOOL_GUILD_DISMISS_CANCEL_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_OPTOOL_GUILD_TOURNAMENT_CHANGE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_OPTOOL_GUILD_TOURNAMENT_CHANGE_CMD`  (sizeof = 688 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    37 | `TournamentTree` | `GUILD_TOURNAMENT_LIST[?] /* 651 bytes */` |

### `PROTO_NC_OPTOOL_GUILD_TOURNAMENT_SCHEDULE_RESET_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_OPTOOL_GUILD_TOURNAMENT_SCHEDULE_RESET_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_KICK_USER_ACK`  (sizeof = 297 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `sUserID` | `Name256Byte` |
|   265 | `sCharID` | `Name5` |
|   285 | `sMapName` | `Name3` |

### `PROTO_NC_OPTOOL_KICK_USER_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_OPTOOL_KQ_ALL_RESET_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_KQ_CHANGE_CMD`  (sizeof = 377 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `KQInfo` | `PROTO_KQ_INFO` |

### `PROTO_NC_OPTOOL_KQ_DELETE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_OPTOOL_KQ_MAP_ALLOC_INFO_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `MapArray` | `KQ_MAP_ALLOC_TABLE[?] /* 0 bytes */` |

### `PROTO_NC_OPTOOL_KQ_MAP_ALLOC_INFO_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_KQ_SCHEDULE_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `QuestArray` | `PROTO_KQ_INFO[?] /* 0 bytes */` |

### `PROTO_NC_OPTOOL_KQ_SCHEDULE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_LOGIN_USER_RATABLE_GET_ACK`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `RateTable` | `LOGIN_USER_RATABLE` |

### `PROTO_NC_OPTOOL_LOGIN_USER_RATABLE_GET_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_LOGIN_USER_RATABLE_SET_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `RateTable` | `LOGIN_USER_RATABLE` |

### `PROTO_NC_OPTOOL_LOGON_PROCESS_TIME_VIEW_ACK`  (sizeof = 24 bytes)

_no members_

### `PROTO_NC_OPTOOL_LOGON_PROCESS_TIME_VIEW_CLR`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_LOGON_PROCESS_TIME_VIEW_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_MAP_USER_LIST_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `user_info` | `NC_OPTOOL_MAP_USER_LIST_INFO[?] /* 0 bytes */` |

### `PROTO_NC_OPTOOL_MAP_USER_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_REQ_CLIENT_NUM_OF_USER_LIMIT`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_S2SCONNECT_LIST_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `connection_info` | `SERVER_CONN_INFO[?] /* 0 bytes */` |

### `PROTO_NC_OPTOOL_S2SCONNECT_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_SET_CLIENT_NUM_OF_USER_LIMIT`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_OPTOOL_WM_SEND_PACKET_STATISTICS_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `Data` | `PROTO_NC_OPTOOL_WM_SEND_PACKET_STATISTICS_ACK::<unnamed-type-Data>[?] /* 0 bytes */` |

### `PROTO_NC_OPTOOL_WM_SEND_PACKET_STATISTICS_ACK::<unnamed-type-Data>`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_OPTOOL_WM_SEND_PACKET_STATISTICS_CLR`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_WM_SEND_PACKET_STATISTICS_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PARTY_CHANGEMASTER_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `newmaster` | `Name5` |

### `PROTO_NC_PARTY_CHANGEMASTER_ACK_SEND`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_PARTY_CHANGEMASTER_ACK` |

### `PROTO_NC_PARTY_CHANGEMASTER_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_PARTY_CHANGEMASTER_CMD_SEND`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_PARTY_CHANGEMASTER_CMD` |

### `PROTO_NC_PARTY_CHANGEMASTER_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `newmaster` | `Name5` |

### `PROTO_NC_PARTY_DISMISS_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PARTY_DISMISS_ACK_SEND`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |

### `PROTO_NC_PARTY_DISMISS_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_PARTY_DISMISS_CMD_SEND`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_PARTY_DISMISS_CMD` |

### `PROTO_NC_PARTY_FINDER_ADD_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_PARTY_FINDER_ADD_REQ`  (sizeof = 128 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMsg` | `unsigned char[128]   /* 128 bytes */` |

### `PROTO_NC_PARTY_FINDER_DELETE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_PARTY_FINDER_DELETE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PARTY_FINDER_DELETE_YOUR_MSG_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PARTY_FINDER_INFO`  (sizeof = 154 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sCharID` | `Name5` |
|    26 | `sMsg` | `unsigned char[128]   /* 128 bytes */` |

### `PROTO_NC_PARTY_FINDER_LIST_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `InfoList` | `PROTO_NC_PARTY_FINDER_INFO[?] /* 0 bytes */` |

### `PROTO_NC_PARTY_FINDER_LIST_REQ`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_PARTY_FUNDAMENTAL_CMD`  (sizeof = 66 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `master` | `PROTO_PARTY_MEMBER_AND_ZONEMERCHINE` |
|    34 | `member` | `PROTO_PARTY_MEMBER_AND_ZONEMERCHINE` |

### `PROTO_NC_PARTY_FUNDAMENTAL_CMD_SEND`  (sizeof = 69 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_PARTY_FUNDAMENTAL_CMD` |

### `PROTO_NC_PARTY_ITEM_JOIN_LOOTING_CMD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `LooterName` | `Name5` |

### `PROTO_NC_PARTY_ITEM_LOOTING_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_PARTY_ITEM_LOOTING_SET`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_PARTY_ITEM_LOOTING_ZONE_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_PARTY_JOINPROPOSE_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mastername` | `Name5` |

### `PROTO_NC_PARTY_JOINPROPOSE_REQ_SEND`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `req` | `PROTO_NC_PARTY_JOINPROPOSE_REQ` |

### `PROTO_NC_PARTY_JOIN_ACK`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `memberid` | `Name5` |

### `PROTO_NC_PARTY_JOIN_ACK_SEND`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_PARTY_JOIN_ACK` |

### `PROTO_NC_PARTY_JOIN_CMD`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `joiner` | `PROTO_PARTY_MEMBER_AND_ZONEMERCHINE` |

### `PROTO_NC_PARTY_JOIN_CMD_SEND`  (sizeof = 37 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_PARTY_JOIN_CMD` |

### `PROTO_NC_PARTY_JOIN_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `target` | `Name5` |

### `PROTO_NC_PARTY_KICKOFF_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `memberid` | `Name5` |

### `PROTO_NC_PARTY_KICKOFF_ACK_SEND`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_PARTY_KICKOFF_ACK` |

### `PROTO_NC_PARTY_KICKOFF_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_PARTY_KICKOFF_CMD_SEND`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_PARTY_KICKOFF_CMD` |

### `PROTO_NC_PARTY_KICKOFF_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `member` | `Name5` |

### `PROTO_NC_PARTY_LEAVE_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `memberid` | `Name5` |

### `PROTO_NC_PARTY_LEAVE_ACK_SEND`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_PARTY_LEAVE_ACK` |

### `PROTO_NC_PARTY_LEAVE_CMD`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `leavemember` | `PROTO_PARTY_MEMBER` |

### `PROTO_NC_PARTY_LEAVE_CMD_SEND`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_PARTY_LEAVE_CMD` |

### `PROTO_NC_PARTY_LEAVE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PARTY_LOGININFO_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `memberid` | `Name5` |

### `PROTO_NC_PARTY_LOGININFO_CMD_SEND`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_PARTY_LOGININFO_CMD` |

### `PROTO_NC_PARTY_LOGIN_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_PARTY_LOGIN_CMD_SEND`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_PARTY_LOGIN_CMD` |

### `PROTO_NC_PARTY_LOGOUTINFO_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `memberid` | `Name5` |

### `PROTO_NC_PARTY_LOGOUTINFO_CMD_SEND`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_PARTY_LOGOUTINFO_CMD` |

### `PROTO_NC_PARTY_LOGOUT_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_PARTY_LOGOUT_CMD_SEND`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_PARTY_LOGOUT_CMD` |

### `PROTO_NC_PARTY_MEMBERCLASS_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `member` | `PROTO_PARTY_MEMBER_CLASS[?] /* 0 bytes */` |

### `PROTO_NC_PARTY_MEMBERINFORM_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `Member` | `PROTO_PARTY_MEMBER[?] /* 0 bytes */` |

### `PROTO_NC_PARTY_MEMBERINFORM_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `member` | `PROTO_PARTY_MEMBER_INFORM[?] /* 0 bytes */` |

### `PROTO_NC_PARTY_MEMBERINFORM_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_PARTY_MEMBERLOCATION_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `member` | `PROTO_NC_PARTY_MEMBERLOCATION_CMD::<unnamed-type-member>[?] /* 0 bytes */` |

### `PROTO_NC_PARTY_MEMBERLOCATION_CMD::<unnamed-type-member>`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name5` |
|    20 | `loc` | `SHINE_XY_TYPE` |

### `PROTO_NC_PARTY_MEMBERMAPOUT`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `memberid` | `Name5` |

### `PROTO_NC_PARTY_MEMBER_LIST_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `memberarray` | `PROTO_NC_PARTY_MEMBER_LIST_CMD::MEMBER_INFO[?] /* 0 bytes */` |

### `PROTO_NC_PARTY_MEMBER_LIST_CMD::MEMBER_INFO`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `memberid` | `Name5` |

### `PROTO_NC_PARTY_MEMBER_LIST_CMD_SEND`  (sizeof = 114 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     4 | `memberarray` | `PROTO_NC_PARTY_MEMBER_LIST_CMD::MEMBER_INFO[?] /* 110 bytes */` |

### `PROTO_NC_PARTY_SET_LOOTER_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sNewLooterCharID` | `Name5` |

### `PROTO_NC_PARTY_SET_LOOTER_BROAD_CMD`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCommandCharID` | `Name5` |
|    20 | `sNewLooterCharID` | `Name5` |

### `PROTO_NC_PARTY_SET_LOOTER_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCommandCharID` | `Name5` |

### `PROTO_NC_PARTY_SET_LOOTER_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sNewLooterCharID` | `Name5` |

### `PROTO_NC_PARTY_ZONE_JOIN_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_PARTY_ZONE_LEAVE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_PARTY_ZONE_SET_LOOTER_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_PATCH_CLIENT_VERSION_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ClientVer` | `PATCH_VERSION_MARK` |

### `PROTO_NC_PATCH_CLIENT_VERSION_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PATCH_CLOSE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PATCH_DATA_SERVER_READY_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_PATCH_DATA_SERVER_USER_COUNT_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_PATCH_FILE_DATA_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `Data` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_PATCH_FILE_DATA_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_PATCH_FILE_INFO_ACK`  (sizeof = 276 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    16 | `FileName` | `unsigned char[260]   /* 260 bytes */` |

### `PROTO_NC_PATCH_FILE_INFO_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_PATCH_INFO_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_PATCH_INFO_DATA_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `Data` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_PATCH_INFO_DATA_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_PATCH_INFO_REQ`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `MyClientVer` | `PATCH_VERSION_MARK` |

### `PROTO_NC_PATCH_INFO_VERIFY_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PATCH_INFO_VERIFY_REQ`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LauncherVer` | `PATCH_VERSION_MARK` |
|     5 | `LastVer` | `PATCH_VERSION_MARK` |

### `PROTO_NC_PATCH_LAUNCHER_VERSION_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LauncherVer` | `PATCH_VERSION_MARK` |

### `PROTO_NC_PATCH_LAUNCHER_VERSION_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PATCH_NOTICE_ACK`  (sizeof = 256 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Patch_Notice_URL` | `unsigned char[128]   /* 128 bytes */` |
|   128 | `Launcher_Notice_URL` | `unsigned char[128]   /* 128 bytes */` |

### `PROTO_NC_PATCH_NOTICE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PATCH_NOTICE_SET_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PATCH_SERVER_ALLOC_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `IP` | `wchar_t[16]   /* 16 bytes */` |

### `PROTO_NC_PATCH_SERVER_ALLOC_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PATCH_STATUS_SET_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PET_ASK_NEW_NAME_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_PET_ASK_NEW_NAME_REQ`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_PET_CREATE_DB_ACK`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_PET_CREATE_DB_REQ`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sPetName` | `Name4` |

### `PROTO_NC_PET_LINK_RESUMMON_CMD`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_PET_LOAD_INFO_DB_ACK`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sPetName` | `Name4` |

### `PROTO_NC_PET_LOAD_INFO_DB_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_PET_REMOVE_DB_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_PET_REMOVE_DB_REQ`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_PET_SET_NAME_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_PET_SET_NAME_CANCEL_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_PET_SET_NAME_CANCEL_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_PET_SET_NAME_CMD`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sPetName` | `Name4` |

### `PROTO_NC_PET_SET_NAME_DB_ACK`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_PET_SET_NAME_DB_REQ`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sPetName` | `Name4` |

### `PROTO_NC_PET_SET_NAME_REQ`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sPetName` | `Name4` |

### `PROTO_NC_PET_SET_TENDENCY_DB_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_PET_USE_ITEM_FAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_PRISON_ADD_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_PRISON_ADD_GM_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sImprison` | `Name5` |

### `PROTO_NC_PRISON_ADD_GM_REQ`  (sizeof = 102 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sImprison` | `Name5` |
|    22 | `sReason` | `wchar_t[16]   /* 16 bytes */` |
|    38 | `sRemark` | `wchar_t[64]   /* 64 bytes */` |

### `PROTO_NC_PRISON_ADD_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_PRISON_ALTER_GM_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sImprison` | `Name5` |

### `PROTO_NC_PRISON_ALTER_GM_REQ`  (sizeof = 102 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sImprison` | `Name5` |
|    22 | `sReason` | `wchar_t[16]   /* 16 bytes */` |
|    38 | `sRemark` | `wchar_t[64]   /* 64 bytes */` |

### `PROTO_NC_PRISON_END_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_PRISON_END_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PRISON_GET_ACK`  (sizeof = 84 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sReason` | `wchar_t[16]   /* 16 bytes */` |
|    20 | `sRemark` | `wchar_t[64]   /* 64 bytes */` |

### `PROTO_NC_PRISON_GET_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PRISON_GIVE_UP_FAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_PRISON_GIVE_UP_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PRISON_OK_CMD`  (sizeof = 82 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sReason` | `wchar_t[16]   /* 16 bytes */` |
|    18 | `sRemark` | `wchar_t[64]   /* 64 bytes */` |

### `PROTO_NC_PRISON_UNDOING_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PROMOTION_DB_REWARD_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `RewardList` | `PROTO_NC_PROMOTION_DB_REWARD_ACK::<unnamed-type-RewardList>[?] /* 0 bytes */` |

### `PROTO_NC_PROMOTION_DB_REWARD_ACK::<unnamed-type-RewardList>`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_PROMOTION_DB_REWARD_REQ`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_PROMOTION_REWARD_ITEM_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `RewardList` | `PROTO_NC_PROMOTION_REWARD_ITEM_CMD::<unnamed-type-RewardList>[?] /* 0 bytes */` |

### `PROTO_NC_PROMOTION_REWARD_ITEM_CMD::<unnamed-type-RewardList>`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_PROMOTION_USER_ACK`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_PROMOTION_USER_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_PROMOTION_USER_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_QUEST_CLIENT_SCENARIO_DONE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_QUEST_CLIENT_SCENARIO_DONE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_QUEST_DB_CLEAR_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_QUEST_DB_CLEAR_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_QUEST_DB_DONE_ACK`  (sizeof = 43 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     7 | `QuestInfo` | `PLAYER_QUEST_INFO` |

### `PROTO_NC_QUEST_DB_DONE_REQ`  (sizeof = 83 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|    13 | `QuestInfo` | `PLAYER_QUEST_INFO` |
|    83 | `ItemCreate` | `PROTO_ITEM_CREATE[?] /* 0 bytes */` |

### `PROTO_NC_QUEST_DB_GIVE_UP_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_QUEST_DB_GIVE_UP_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_QUEST_DB_READ_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_QUEST_DB_READ_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_QUEST_DB_SET_INFO_ACK`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     7 | `QuestInfo` | `PLAYER_QUEST_INFO` |

### `PROTO_NC_QUEST_DB_SET_INFO_REQ`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     7 | `QuestInfo` | `PLAYER_QUEST_INFO` |

### `PROTO_NC_QUEST_GIVE_UP_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_QUEST_GIVE_UP_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_QUEST_JOBDUNGEON_FIND_RNG`  (sizeof = 115 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |
|     9 | `ClientMapName` | `wchar_t[33]   /* 33 bytes */` |
|    42 | `ServerMapName` | `wchar_t[33]   /* 33 bytes */` |
|    75 | `ScriptName` | `wchar_t[33]   /* 33 bytes */` |
|   108 | `QuestScript` | `PROTO_NC_QUEST_SCRIPT_CMD_ACK` |

### `PROTO_NC_QUEST_JOBDUNGEON_LINK_FAIL_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_QUEST_NOTIFY_MOB_KILL_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `mobofquest` | `PROTO_NC_QUEST_NOTIFY_MOB_KILL_CMD::MobOfQuest[?] /* 0 bytes */` |

### `PROTO_NC_QUEST_NOTIFY_MOB_KILL_CMD::MobOfQuest`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_QUEST_READ_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_QUEST_READ_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_QUEST_RESET_TIME_CMD`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_QUEST_RESET_TIME_ZONE_CMD`  (sizeof = 20 bytes)

_no members_

### `PROTO_NC_QUEST_REWARD_NEED_SELECT_ITEM_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_QUEST_REWARD_SELECT_ITEM_INDEX_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_QUEST_SCENARIO_RUN_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_QUEST_SCRIPT_CMD_ACK`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_QUEST_SCRIPT_CMD_REQ`  (sizeof = 103 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `Command` | `STRUCT_QSC` |

### `PROTO_NC_QUEST_SELECT_START_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_QUEST_SELECT_START_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_QUEST_START_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_QUEST_START_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_RAID_CHAT_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_RAID_CHAT_BROAD_CMD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `sTalkerCharID` | `Name5` |
|    22 | `sChat` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_RAID_CHAT_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sChat` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_RAID_DEL_SUB_LEADER_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sDelSubLeaderCharID` | `Name5` |

### `PROTO_NC_RAID_DEL_SUB_LEADER_BROAD_CMD`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCommandCharID` | `Name5` |
|    20 | `sDelSubLeaderCharID` | `Name5` |

### `PROTO_NC_RAID_DEL_SUB_LEADER_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCommandCharID` | `Name5` |

### `PROTO_NC_RAID_DEL_SUB_LEADER_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sDelSubLeaderCharID` | `Name5` |

### `PROTO_NC_RAID_DISMISS_BROAD_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_RAID_INVITATION_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sInvitationCharID` | `Name5` |

### `PROTO_NC_RAID_INVITATION_ANSWER_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_RAID_INVITATION_ANSWER_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sInvitationCharID` | `Name5` |

### `PROTO_NC_RAID_INVITATION_ANSWER_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `sMasterCharID` | `Name5` |

### `PROTO_NC_RAID_INVITATION_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMasterCharID` | `Name5` |

### `PROTO_NC_RAID_INVITATION_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sInvitationCharID` | `Name5` |

### `PROTO_NC_RAID_LEAVE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_RAID_LEAVE_BROAD_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sCharID` | `Name5` |

### `PROTO_NC_RAID_LEAVE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_RAID_LOCATION_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `RaidMembers` | `PROTO_NC_RAID_LOCATION_CMD::Memberinfo[?] /* 0 bytes */` |

### `PROTO_NC_RAID_LOCATION_CMD::Memberinfo`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `loc` | `SHINE_XY_TYPE` |

### `PROTO_NC_RAID_LOGININFO_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sCharID` | `Name5` |

### `PROTO_NC_RAID_LOGIN_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_RAID_LOGOUTINFO_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sCharID` | `Name5` |

### `PROTO_NC_RAID_LOGOUT_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_RAID_LOOTING_TYPE_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_RAID_LOOTING_TYPE_BROAD_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_RAID_LOOTING_TYPE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_RAID_MAKE_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_RAID_MAKE_BROAD_CMD`  (sizeof = 110 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `nRaidPartyNo` | `unsigned short[4]   /* 8 bytes */` |
|    10 | `sFirstPartyCharID` | `Name5[?] /* 100 bytes */` |

### `PROTO_NC_RAID_MAKE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_RAID_MAPOUT_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCharID` | `Name5` |

### `PROTO_NC_RAID_MEMBERCLASS_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `RaidMembers` | `RAID_MEMBERCLASS[?] /* 0 bytes */` |

### `PROTO_NC_RAID_MEMBERINFORM_CMD`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_RAID_MEMBER_BROADCAST_INFO_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `RaidMembers` | `RAID_MEMBERCLASS[?] /* 0 bytes */` |

### `PROTO_NC_RAID_MEMBER_JOIN_BROAD_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sCharID` | `Name5` |

### `PROTO_NC_RAID_MEMBER_LIST_CMD`  (sizeof = 542 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Leaderid` | `Name5` |
|    20 | `SubLeaderid` | `Name5[?] /* 80 bytes */` |
|   100 | `Looterid` | `Name5` |
|   122 | `memberarray` | `PROTO_NC_RAID_MEMBER_LIST_CMD::MEMBER_INFO[?] /* 420 bytes */` |

### `PROTO_NC_RAID_MEMBER_LIST_CMD::MEMBER_INFO`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `memberid` | `Name5` |

### `PROTO_NC_RAID_SET_LEADER_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sNewLeaderCharID` | `Name5` |

### `PROTO_NC_RAID_SET_LEADER_BROAD_CMD`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCommandCharID` | `Name5` |
|    20 | `sNewLeaderCharID` | `Name5` |

### `PROTO_NC_RAID_SET_LEADER_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCommandCharID` | `Name5` |

### `PROTO_NC_RAID_SET_LEADER_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sNewLeaderCharID` | `Name5` |

### `PROTO_NC_RAID_SET_LOOTER_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sNewLooterCharID` | `Name5` |

### `PROTO_NC_RAID_SET_LOOTER_BROAD_CMD`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCommandCharID` | `Name5` |
|    20 | `sNewLooterCharID` | `Name5` |

### `PROTO_NC_RAID_SET_LOOTER_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCommandCharID` | `Name5` |

### `PROTO_NC_RAID_SET_LOOTER_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sNewLooterCharID` | `Name5` |

### `PROTO_NC_RAID_SET_SUB_LEADER_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sNewSubLeaderCharID` | `Name5` |

### `PROTO_NC_RAID_SET_SUB_LEADER_BROAD_CMD`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCommandCharID` | `Name5` |
|    20 | `sNewSubLeaderCharID` | `Name5` |

### `PROTO_NC_RAID_SET_SUB_LEADER_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCommandCharID` | `Name5` |

### `PROTO_NC_RAID_SET_SUB_LEADER_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sNewSubLeaderCharID` | `Name5` |

### `PROTO_NC_RAID_SLOT_MOVE_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sSlotMoveCharID` | `Name5` |

### `PROTO_NC_RAID_SLOT_MOVE_BROAD_CMD`  (sizeof = 228 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCommandCharID` | `Name5` |
|    20 | `PrePosition` | `PROTO_NC_RAID_SLOT_MOVE_BROAD_CMD::<unnamed-type-PrePosition>` |
|    24 | `PostPosition` | `PROTO_NC_RAID_SLOT_MOVE_BROAD_CMD::<unnamed-type-PostPosition>` |
|    28 | `sPreCharID` | `Name5[?] /* 100 bytes */` |
|   128 | `sPostCharID` | `Name5[?] /* 100 bytes */` |

### `PROTO_NC_RAID_SLOT_MOVE_BROAD_CMD::<unnamed-type-PostPosition>`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_RAID_SLOT_MOVE_BROAD_CMD::<unnamed-type-PrePosition>`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_RAID_SLOT_MOVE_REQ`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `PrePosition` | `PROTO_NC_RAID_SLOT_MOVE_REQ::<unnamed-type-PrePosition>` |
|     4 | `PostPosition` | `PROTO_NC_RAID_SLOT_MOVE_REQ::<unnamed-type-PostPosition>` |
|     8 | `sPreSlotMoveCharID` | `Name5` |
|    28 | `sPostSlotMoveCharID` | `Name5` |

### `PROTO_NC_RAID_SLOT_MOVE_REQ::<unnamed-type-PostPosition>`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_RAID_SLOT_MOVE_REQ::<unnamed-type-PrePosition>`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_RAID_VANISH_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sVanishCharID` | `Name5` |

### `PROTO_NC_RAID_VANISH_BROAD_CMD`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sCommandCharID` | `Name5` |
|    24 | `sVanishCharID` | `Name5` |

### `PROTO_NC_RAID_VANISH_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCommandCharID` | `Name5` |

### `PROTO_NC_RAID_VANISH_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sVanishCharID` | `Name5` |

### `PROTO_NC_RAID_WARNING_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_RAID_WARNING_BROAD_CMD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `sTalkerCharID` | `Name5` |
|    22 | `sChat` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_RAID_WARNING_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sChat` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_RAID_WORLD_MEMBERCLASS_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `RaidMembers` | `RAID_MEMBERCLASS[?] /* 0 bytes */` |

### `PROTO_NC_RAID_ZONE_DEL_SUB_LEADER_CMD`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_RAID_ZONE_DISMISS_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_RAID_ZONE_LEAVE_CMD`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_RAID_ZONE_LOOTING_TYPE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_RAID_ZONE_MAKE_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `nRaidPartyNo` | `unsigned short[4]   /* 8 bytes */` |
|    12 | `FirstPartyCharRegNo` | `unsigned long[5]   /* 20 bytes */` |

### `PROTO_NC_RAID_ZONE_MEMBER_JOIN_CMD`  (sizeof = 42 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `tInfo` | `PROTO_PARTY_MEMBER_AND_ZONEMERCHINE` |

### `PROTO_NC_RAID_ZONE_SET_LEADER_CMD`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_RAID_ZONE_SET_LOOTER_CMD`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_RAID_ZONE_SET_SUB_LEADER_CMD`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_RAID_ZONE_SLOT_MOVE_CMD`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `PrePosition` | `PROTO_NC_RAID_ZONE_SLOT_MOVE_CMD::<unnamed-type-PrePosition>` |
|    26 | `PostPosition` | `PROTO_NC_RAID_ZONE_SLOT_MOVE_CMD::<unnamed-type-PostPosition>` |

### `PROTO_NC_RAID_ZONE_SLOT_MOVE_CMD::<unnamed-type-PostPosition>`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `nPartySeqCharNo` | `unsigned long[5]   /* 20 bytes */` |

### `PROTO_NC_RAID_ZONE_SLOT_MOVE_CMD::<unnamed-type-PrePosition>`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `nPartySeqCharNo` | `unsigned long[5]   /* 20 bytes */` |

### `PROTO_NC_RAID_ZONE_VANISH_CMD`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_REPORT_ADD_ACK`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sHarmer` | `Name5` |

### `PROTO_NC_REPORT_ADD_REQ`  (sizeof = 2342 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sHarmer` | `Name5` |
|    20 | `byReportType` | `unsigned char[16]   /* 16 bytes */` |
|    36 | `byReason` | `unsigned char[256]   /* 256 bytes */` |
|   294 | `byChatLog` | `unsigned char[2048]   /* 2048 bytes */` |

### `PROTO_NC_REPORT_CANCEL_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_REPORT_CANCEL_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_REPORT_GET_ACK`  (sizeof = 135 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `byRemark` | `unsigned char[128]   /* 128 bytes */` |

### `PROTO_NC_REPORT_GET_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_SCENARIO_ANIMATE_CMD`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `anicode` | `Name8` |

### `PROTO_NC_SCENARIO_ANIMATION_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_SCENARIO_AREAENTRY_ACK`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `areaindex` | `Name8` |

### `PROTO_NC_SCENARIO_AREAENTRY_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `areaindex` | `Name8` |

### `PROTO_NC_SCENARIO_AREALEAVE_ACK`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `areaindex` | `Name8` |

### `PROTO_NC_SCENARIO_AREALEAVE_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `areaindex` | `Name8` |

### `PROTO_NC_SCENARIO_BRIEFEFFECT_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_SCENARIO_CAMERA_MOVE_CMD`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `ViewCoord` | `SHINE_XY_TYPE` |

### `PROTO_NC_SCENARIO_CHATWIN_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_SCENARIO_DEL_DIRECTIONAL_ARROW_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_SCENARIO_DIALOG_CMD`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sFacecutIndex` | `wchar_t[33]   /* 33 bytes */` |
|    34 | `sContents` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_SCENARIO_DIRECTIONAL_ARROW_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `DirectionalPos` | `SHINE_XY_TYPE` |

### `PROTO_NC_SCENARIO_DOORSTATE_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_SCENARIO_EFFECT_LOCATE_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `loc` | `SHINE_XY_TYPE` |

### `PROTO_NC_SCENARIO_EFFECT_OBJECT_CMD`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `effect` | `Name8` |

### `PROTO_NC_SCENARIO_EFFECT_TIMER_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_SCENARIO_FOG_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_SCENARIO_HIDE_OTHER_PLAYER_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_SCENARIO_LIGHT_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_SCENARIO_MAPMARK_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `IconList` | `PROTO_NC_SCENARIO_MAPMARK_CMD::IconInfo[?] /* 0 bytes */` |

### `PROTO_NC_SCENARIO_MAPMARK_CMD::IconInfo`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `nCoord` | `SHINE_XY_TYPE` |

### `PROTO_NC_SCENARIO_MAP_OBJECT_CONTROL_CMD`  (sizeof = 37 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sObjectIndex` | `wchar_t[32]   /* 32 bytes */` |
|    33 | `eControlType` | `MAP_OBJECT_CONTROL_TYPE` |

### `PROTO_NC_SCENARIO_MESSAGENOTICE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_SCENARIO_MESSAGENPC_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_SCENARIO_MESSAGE_CMD`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `DialogIndex` | `wchar_t[17]   /* 17 bytes */` |

### `PROTO_NC_SCENARIO_NPCACT_CMD`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_SCENARIO_NPCCHAT_CMD`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `DialogIndex` | `wchar_t[33]   /* 33 bytes */` |

### `PROTO_NC_SCENARIO_OBJTYPECHANGE_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_SCENARIO_PLAYERLIKEMOB_ANIMATE_CMD`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_SCENARIO_RANKING_LIST_CMD`  (sizeof = 1000 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `PreviousRanking` | `SCENARIO_RANKING_INFO[?] /* 500 bytes */` |
|   500 | `CurrentRanking` | `SCENARIO_RANKING_INFO[?] /* 500 bytes */` |

### `PROTO_NC_SCENARIO_RUNEFFECT_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `effecname` | `Name4` |

### `PROTO_NC_SCENARIO_SCRIPTMESSAGE_CMD`  (sizeof = 37 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `DialogIndex` | `wchar_t[32]   /* 32 bytes */` |
|    32 | `nlen` | `unsigned char[5]   /* 5 bytes */` |
|    37 | `sContent` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_SCENARIO_SOUND_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_SCENARIO_SYSTEM_MESSAGE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `sMessage` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_SCENARIO_TIMER_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_SCENARIO_TIMER_END_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_SCENARIO_TIMER_START_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_SCENARIO_TOPVIEW_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `CenterPos` | `SHINE_XY_TYPE` |

### `PROTO_NC_SKILL_COOLTIME_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_SKILL_EMPOWALLOC_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_SKILL_EMPOWALLOC_DB_ACK`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_SKILL_EMPOWALLOC_DB_ACK_SEND`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_SKILL_EMPOWALLOC_DB_ACK` |

### `PROTO_NC_SKILL_EMPOWALLOC_DB_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    11 | `empower` | `PROTO_NC_SKILL_EMPOWALLOC_REQ` |
|    17 | `redistitem` | `PROTO_ITEMDELETEREQUEST` |

### `PROTO_NC_SKILL_EMPOWALLOC_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `plus` | `SKILL_EMPOWER` |
|     4 | `minus` | `SKILL_EMPOWER` |

### `PROTO_NC_SKILL_EMPOWPOINT_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_SKILL_EMPOW_RESET_DB_FAIL_ACK_SEND`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_SKILL_EMPOW_RESET_DB_REQ` |

### `PROTO_NC_SKILL_EMPOW_RESET_DB_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_SKILL_EMPOW_RESET_DB_SUC_ACK_SEND`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_SKILL_EMPOW_RESET_DB_REQ` |

### `PROTO_NC_SKILL_EMPOW_RESET_SUC_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_SKILL_ERASE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_SKILL_ERASE_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_SKILL_JUMP_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `From` | `SHINE_XY_TYPE` |
|    10 | `To` | `SHINE_XY_TYPE` |

### `PROTO_NC_SKILL_PASSIVESKILL_LEARN_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_SKILL_PRODUCTFIELD_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_SKILL_PRODUCTFIELD_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_SKILL_REPLYREVIVE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_SKILL_RESETABSTATE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `abstateid` | `ABSTATEINDEX` |

### `PROTO_NC_SKILL_REVIVE_CMD`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_SKILL_SKILLEXP_CLIENT_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_SKILL_SKILLTEACHFAIL_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_SKILL_SKILLTEACHSUC_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_SKILL_SKILLTEACH_REQ`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_SKILL_SOMEONERESETABSTATE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `abstateid` | `ABSTATEINDEX` |

### `PROTO_NC_SKILL_SOMEONEREVAVALTOME_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_SKILL_SOMEONEREVIVE_CMD`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_SKILL_SOMEONESETABSTATE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `abstateid` | `ABSTATEINDEX` |

### `PROTO_NC_SKILL_UNLEARN_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_SKILL_UNLEARN_REQ`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_SKILL_WARP_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `from` | `SHINE_XY_TYPE` |
|    10 | `to` | `SHINE_XY_TYPE` |

### `PROTO_NC_SOULSTONE_BUYFAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_SOULSTONE_HP_BUY_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_SOULSTONE_HP_BUY_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_SOULSTONE_HP_SOMEONEUSE_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_SOULSTONE_SP_BUY_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_SOULSTONE_SP_BUY_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_SOULSTONE_SP_SOMEONEUSE_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_SYSLOG_ACCOUNT_LOGIN_FAILURE`  (sizeof = 296 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `UserName` | `wchar_t[260]   /* 260 bytes */` |
|   260 | `UserIP` | `Name4` |
|   276 | `spawnapps` | `Name5` |

### `PROTO_NC_SYSLOG_ACCOUNT_LOGIN_SUCCESS`  (sizeof = 70 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `UserIP` | `Name4` |
|    20 | `UserID` | `wchar_t[30]   /* 30 bytes */` |
|    50 | `spawnapps` | `Name5` |

### `PROTO_NC_SYSLOG_ACCOUNT_LOGOUT`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_SYSLOG_CHAR_CREATED`  (sizeof = 43 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `UserID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_SYSLOG_CHAR_DEATH`  (sizeof = 60 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `DeathCoord` | `SHINE_XY_TYPE` |
|    22 | `CharID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_SYSLOG_CHAR_DELETED`  (sizeof = 43 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `UserID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_SYSLOG_CHAR_ENTER_GAME`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `UserID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_SYSLOG_CHAR_ITEMMONEY_BUY`  (sizeof = 22 bytes)

_no members_

### `PROTO_NC_SYSLOG_CHAR_ITEM_BUY`  (sizeof = 62 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    28 | `CharID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_SYSLOG_CHAR_ITEM_REBUY`  (sizeof = 62 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    28 | `CharID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_SYSLOG_CHAR_ITEM_SELL`  (sizeof = 62 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    28 | `CharID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_SYSLOG_CHAR_LEAVE_GAME`  (sizeof = 46 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `UserID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_SYSLOG_CHAR_LEVEL_UP`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `UserID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_SYSLOG_CHAR_LOOT`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_SYSLOG_CHAR_QUEST_FINISHED`  (sizeof = 30 bytes)

_no members_

### `PROTO_NC_SYSLOG_CHAR_QUEST_STARTED`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_SYSLOG_CHAR_VICTORY`  (sizeof = 56 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `KillCoord` | `SHINE_XY_TYPE` |
|    22 | `CharID` | `wchar_t[30]   /* 30 bytes */` |

### `PROTO_NC_SYSLOG_CHAR_ZONE_TRANSITION`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_SYSLOG_SERVER_CCU`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_TRADE_CENBOARDINGFAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_TRADE_CENBOARDING_ACK`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_TRADE_CENBOARDING_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_TRADE_DOWNBOARDFAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_TRADE_DOWNBOARD_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_TRADE_DOWNBOARD_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_TRADE_OPPOSITCENBOARDING_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_TRADE_OPPOSITDOWNBOARD_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_TRADE_OPPOSITUPBOARD_CMD`  (sizeof = 104 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `iteminfo` | `SHINE_ITEM_STRUCT` |

### `PROTO_NC_TRADE_PROPOSE_ASK_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_TRADE_PROPOSE_REQ`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_TRADE_START_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_TRADE_TRADEFAIL_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_TRADE_UPBOARDFAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_TRADE_UPBOARD_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_TRADE_UPBOARD_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_AVATARINFO_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `avatar` | `PROTO_AVATARINFORMATION[?] /* 0 bytes */` |

### `PROTO_NC_USER_AVATARINFO_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_USER_AVATARINFO_REQ_SEND`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `req` | `PROTO_NC_USER_AVATARINFO_REQ` |

### `PROTO_NC_USER_AVATAR_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_CH_IS_IP_BLOCK_REQ`  (sizeof = 298 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_CH_LOGIN_REQ` |
|   278 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |

### `PROTO_NC_USER_CH_LOGIN_REQ`  (sizeof = 272 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `user` | `Name256Byte` |
|   256 | `password` | `Name4` |

### `PROTO_NC_USER_CH_PASSWORD_CHECK_ACK`  (sizeof = 287 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `id` | `Name256Byte` |
|   258 | `password` | `Name4` |

### `PROTO_NC_USER_CH_PASSWORD_CHECK_ACK_SEND`  (sizeof = 290 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_USER_CH_PASSWORD_CHECK_ACK` |

### `PROTO_NC_USER_CH_PASSWORD_CHECK_REQ`  (sizeof = 274 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `user` | `Name256Byte` |
|   258 | `password` | `Name4` |

### `PROTO_NC_USER_CH_PASSWORD_CHECK_REQ_SEND`  (sizeof = 277 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `req` | `PROTO_NC_USER_CH_PASSWORD_CHECK_REQ` |

### `PROTO_NC_USER_CLIENT_RIGHTVERSION_CHECK_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `XTrapServerKey` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_USER_CLIENT_VERSION_CHECK_REQ`  (sizeof = 64 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sVersionKey` | `wchar_t[64]   /* 64 bytes */` |

### `PROTO_NC_USER_CLIENT_WRONGVERSION_CHECK_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_CONNECTCUT2WORLDMANAGER_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_USER_CONNECTCUT2ZONE_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_USER_CONNECTCUT2ZONE_CMD_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     9 | `cmd` | `PROTO_NC_USER_CONNECTCUT2ZONE_CMD` |

### `PROTO_NC_USER_CONNECTCUT_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_USER_CONNECTCUT_CMD_SEND`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_USER_CONNECTCUT_CMD` |

### `PROTO_NC_USER_CONNECTION_CHANGE_CHAR_ID_ACADEMY_MASTER_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMasterID` | `Name5` |

### `PROTO_NC_USER_CONNECTION_CHANGE_CHAR_ID_ACK`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `sNewID` | `Name5` |

### `PROTO_NC_USER_CONNECTION_CHANGE_CHAR_ID_CMD`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `sOldID` | `Name5` |
|    21 | `sNewID` | `Name5` |

### `PROTO_NC_USER_CONNECTION_CHANGE_CHAR_ID_ITEM_USE_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_USER_CONNECTION_CHANGE_CHAR_ID_ITEM_USE_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_CONNECTION_CHANGE_CHAR_ID_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `sNewID` | `Name5` |

### `PROTO_NC_USER_CONNECTION_DB_CHANGE_CHAR_ID_ACK`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `sNewID` | `Name5` |

### `PROTO_NC_USER_CONNECTION_DB_CHANGE_CHAR_ID_ITEM_USE_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_USER_CONNECTION_DB_CHANGE_CHAR_ID_ITEM_USE_REQ`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharIDChangeItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `nCharIDChangeItemSlot` | `ITEM_INVEN` |
|    17 | `CommonData` | `PROTO_NC_ITEMDB_USE_VARIATION_ITEM_REQ` |

### `PROTO_NC_USER_CONNECTION_DB_CHANGE_CHAR_ID_REQ`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `sNewID` | `Name5` |

### `PROTO_NC_USER_CONNECTION_SET_CHANGE_CHAR_ID_FLAG_ALL_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_CONNECTION_SET_CHANGE_CHAR_ID_FLAG_CMD`  (sizeof = 54 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `sOldID` | `Name5` |
|    32 | `sNewID` | `Name5` |

### `PROTO_NC_USER_CONNECTION_ZONE_CHANGE_CHAR_ID_CMD`  (sizeof = 45 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `sOldID` | `Name5` |
|    25 | `sNewID` | `Name5` |

### `PROTO_NC_USER_CREATE_OTP_ACK`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sOTP` | `Name8` |

### `PROTO_NC_USER_CREATE_OTP_REQ`  (sizeof = 266 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sUserID` | `Name256Byte` |
|   262 | `nClientIP` | `in_addr` |

### `PROTO_NC_USER_GER_IS_IP_BLOCK_ACK`  (sizeof = 290 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_GER_PASSWORD_CHECK_ACK` |

### `PROTO_NC_USER_GER_IS_IP_BLOCK_REQ`  (sizeof = 313 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_GER_PASSWORD_CHECK_ACK` |
|   293 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |

### `PROTO_NC_USER_GER_LOGIN_REQ`  (sizeof = 272 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `user` | `Name256Byte` |
|   256 | `password` | `Name4` |

### `PROTO_NC_USER_GER_PASSWORD_CHECK_ACK`  (sizeof = 287 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `id` | `Name256Byte` |
|   258 | `password` | `Name4` |

### `PROTO_NC_USER_GER_PASSWORD_CHECK_ACK_SEND`  (sizeof = 290 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_USER_GER_PASSWORD_CHECK_ACK` |

### `PROTO_NC_USER_GER_PASSWORD_CHECK_REQ`  (sizeof = 274 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `user` | `Name256Byte` |
|   258 | `password` | `Name4` |

### `PROTO_NC_USER_GER_PASSWORD_CHECK_REQ_SEND`  (sizeof = 277 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `req` | `PROTO_NC_USER_GER_PASSWORD_CHECK_REQ` |

### `PROTO_NC_USER_IS_IP_BLOCK_ACK`  (sizeof = 275 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_LOGIN_REQ` |

### `PROTO_NC_USER_IS_IP_BLOCK_REQ`  (sizeof = 298 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_LOGIN_REQ` |
|   278 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |

### `PROTO_NC_USER_JP_IS_IP_BLOCK_ACK`  (sizeof = 309 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_JP_PASSWORD_CHECK_ACK` |

### `PROTO_NC_USER_JP_IS_IP_BLOCK_REQ`  (sizeof = 332 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_JP_PASSWORD_CHECK_ACK` |
|   312 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |

### `PROTO_NC_USER_JP_LOGIN_REQ`  (sizeof = 314 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sUserName` | `wchar_t[24]   /* 24 bytes */` |
|    24 | `sPassword` | `wchar_t[36]   /* 36 bytes */` |
|    60 | `sChannel` | `wchar_t[254]   /* 254 bytes */` |

### `PROTO_NC_USER_JP_PASSWORD_CHECK_ACK`  (sizeof = 306 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     8 | `sUserName` | `wchar_t[24]   /* 24 bytes */` |
|    32 | `sChannel` | `wchar_t[254]   /* 254 bytes */` |
|   286 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |

### `PROTO_NC_USER_JP_PASSWORD_CHECK_REQ`  (sizeof = 336 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `sUserName` | `wchar_t[24]   /* 24 bytes */` |
|    26 | `sPassword` | `wchar_t[36]   /* 36 bytes */` |
|    62 | `sChannel` | `wchar_t[254]   /* 254 bytes */` |
|   316 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |

### `PROTO_NC_USER_KICKOFFFROMWORLD_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_USER_KICKOFFFROMWORLD_CMD_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     9 | `cmd` | `PROTO_NC_USER_KICKOFFFROMWORLD_CMD` |

### `PROTO_NC_USER_LOGINFAIL_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_USER_LOGINFAIL_ACK_SEND`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_USER_LOGINFAIL_ACK` |

### `PROTO_NC_USER_LOGINWORLDFAIL_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `errorcode` | `PROTO_ERRORCODE` |

### `PROTO_NC_USER_LOGINWORLDFAIL_ACK_SEND`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `Ack` | `PROTO_NC_USER_LOGINWORLDFAIL_ACK` |

### `PROTO_NC_USER_LOGINWORLD_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `avatar` | `PROTO_AVATARINFORMATION[?] /* 0 bytes */` |

### `PROTO_NC_USER_LOGINWORLD_REQ`  (sizeof = 320 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `user` | `Name256Byte` |
|   256 | `validate_new` | `unsigned short[32]   /* 64 bytes */` |

### `PROTO_NC_USER_LOGIN_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `worldinfo` | `PROTO_NC_USER_LOGIN_ACK::WorldInfo[?] /* 0 bytes */` |

### `PROTO_NC_USER_LOGIN_ACK::WorldInfo`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `worldname` | `Name4` |

### `PROTO_NC_USER_LOGIN_DB`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `ip` | `unsigned char[4]   /* 4 bytes */` |

### `PROTO_NC_USER_LOGIN_DB_SEND`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `db` | `PROTO_NC_USER_LOGIN_DB` |

### `PROTO_NC_USER_LOGIN_NETMARBLE_DB_REQ`  (sizeof = 278 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `UniID` | `unsigned char[20]   /* 20 bytes */` |
|    22 | `UserID` | `unsigned char[256]   /* 256 bytes */` |

### `PROTO_NC_USER_LOGIN_NETMARBLE_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `CPCookie` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_USER_LOGIN_OUTSPARK_REQ`  (sizeof = 85 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LogonToken` | `unsigned char[65]   /* 65 bytes */` |
|    65 | `spawnapps` | `Name5` |

### `PROTO_NC_USER_LOGIN_REQ`  (sizeof = 272 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `user` | `Name256Byte` |
|   256 | `password` | `Name4` |

### `PROTO_NC_USER_LOGIN_WITH_OTP_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sOTP` | `Name8` |

### `PROTO_NC_USER_LOGOUT_DB`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_USER_LOGOUT_DB_SEND`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `db` | `PROTO_NC_USER_LOGOUT_DB` |

### `PROTO_NC_USER_LOGOUT_LAST_DAY_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_USER_LOGOUT_LAST_DAY_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_USER_NORMALLOGOUT_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_PASSWORD_CHECK_ACK`  (sizeof = 287 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `id` | `Name256Byte` |
|   258 | `password` | `Name4` |

### `PROTO_NC_USER_PASSWORD_CHECK_ACK_SEND`  (sizeof = 290 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_USER_PASSWORD_CHECK_ACK` |

### `PROTO_NC_USER_PASSWORD_CHECK_REQ`  (sizeof = 274 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `user` | `Name256Byte` |
|   258 | `password` | `Name4` |

### `PROTO_NC_USER_PASSWORD_CHECK_REQ_SEND`  (sizeof = 277 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `req` | `PROTO_NC_USER_PASSWORD_CHECK_REQ` |

### `PROTO_NC_USER_POSSIBLE_NEW_CONNECT_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_USER_REGISENUMBER_ACK`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_USER_RETURN_CHECK_ACK`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_USER_RETURN_CHECK_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_USER_SET_RETURN_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_USER_TEENAGER_ACK`  (sizeof = 313 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sUserID` | `Name256Byte` |
|   258 | `Data` | `PROTO_NC_USER_TEENAGER_CMD` |

### `PROTO_NC_USER_TEENAGER_CMD`  (sizeof = 55 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sUserKey` | `unsigned char[50]   /* 50 bytes */` |

### `PROTO_NC_USER_TEENAGER_REMAIN_MIN_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_USER_TEENAGER_REQ`  (sizeof = 258 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sUserID` | `Name256Byte` |

### `PROTO_NC_USER_TEENAGER_SET_CMD`  (sizeof = 54 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sUserKey` | `unsigned char[50]   /* 50 bytes */` |

### `PROTO_NC_USER_TUTORIAL_CAN_SKIP_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_TW_IS_IP_BLOCK_ACK`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_TW_PASSWORD_CHECK_ACK` |

### `PROTO_NC_USER_TW_IS_IP_BLOCK_REQ`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_TW_PASSWORD_CHECK_ACK` |
|    14 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |

### `PROTO_NC_USER_TW_LOGIN_REQ`  (sizeof = 100 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sUserName` | `wchar_t[36]   /* 36 bytes */` |
|    36 | `sSerial` | `wchar_t[36]   /* 36 bytes */` |
|    72 | `sUID` | `wchar_t[20]   /* 20 bytes */` |
|    92 | `sSID` | `wchar_t[8]   /* 8 bytes */` |

### `PROTO_NC_USER_TW_PASSWORD_CHECK_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_USER_TW_PASSWORD_CHECK_ACK_SEND`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_USER_TW_PASSWORD_CHECK_ACK` |

### `PROTO_NC_USER_TW_PASSWORD_CHECK_REQ`  (sizeof = 102 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `sUserName` | `wchar_t[36]   /* 36 bytes */` |
|    38 | `sSerial` | `wchar_t[36]   /* 36 bytes */` |
|    74 | `sUID` | `wchar_t[20]   /* 20 bytes */` |
|    94 | `sSID` | `wchar_t[8]   /* 8 bytes */` |

### `PROTO_NC_USER_TW_PASSWORD_CHECK_REQ_SEND`  (sizeof = 105 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `req` | `PROTO_NC_USER_TW_PASSWORD_CHECK_REQ` |

### `PROTO_NC_USER_USE_BEAUTY_SHOP_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_US_IS_IP_BLOCK_ACK`  (sizeof = 356 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_US_PASSWORD_CHECK_ACK` |

### `PROTO_NC_USER_US_IS_IP_BLOCK_REQ`  (sizeof = 379 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_US_PASSWORD_CHECK_ACK` |
|   359 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |

### `PROTO_NC_USER_US_LOGIN_REQ`  (sizeof = 316 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sUserName` | `wchar_t[260]   /* 260 bytes */` |
|   260 | `sPassword` | `wchar_t[36]   /* 36 bytes */` |
|   296 | `spawnapps` | `Name5` |

### `PROTO_NC_USER_US_PASSWORD_CHECK_ACK`  (sizeof = 353 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     8 | `sUserName` | `wchar_t[260]   /* 260 bytes */` |
|   268 | `sPassword` | `wchar_t[36]   /* 36 bytes */` |
|   304 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |
|   333 | `spawnapps` | `Name5` |

### `PROTO_NC_USER_US_PASSWORD_CHECK_REQ`  (sizeof = 338 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `sUserName` | `wchar_t[260]   /* 260 bytes */` |
|   262 | `sPassword` | `wchar_t[36]   /* 36 bytes */` |
|   298 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |
|   318 | `spawnapps` | `Name5` |

### `PROTO_NC_USER_WILLLOGIN_ACK`  (sizeof = 348 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `will_login_req` | `PROTO_NC_USER_WILLLOGIN_REQ` |

### `PROTO_NC_USER_WILLLOGIN_ACK_SEND`  (sizeof = 351 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_USER_WILLLOGIN_ACK` |

### `PROTO_NC_USER_WILLLOGIN_REQ`  (sizeof = 347 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     7 | `userid` | `Name256Byte` |
|   263 | `validate_new` | `unsigned short[32]   /* 64 bytes */` |
|   327 | `spawnapps` | `Name5` |

### `PROTO_NC_USER_WILLLOGIN_REQ_SEND`  (sizeof = 350 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `req` | `PROTO_NC_USER_WILLLOGIN_REQ` |

### `PROTO_NC_USER_WILL_WORLD_SELECT_ACK`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sOTP` | `Name8` |

### `PROTO_NC_USER_WILL_WORLD_SELECT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_WORLDSELECT_ACK`  (sizeof = 83 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `ip` | `Name4` |
|    19 | `validate_new` | `unsigned short[32]   /* 64 bytes */` |

### `PROTO_NC_USER_WORLDSELECT_ACK_SEND`  (sizeof = 86 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `ack` | `PROTO_NC_USER_WORLDSELECT_ACK` |

### `PROTO_NC_USER_WORLDSELECT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_WORLD_STATUS_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_XTRAP_ACK`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_XTRAP_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `XTrapClientKey` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_USP_USER_CHARACTER_DELETE`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_USP_USER_CHARACTER_INSERT`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `sCharName` | `wchar_t[40]   /* 40 bytes */` |

### `PROTO_NC_WT_GRADE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ItemInven` | `ITEM_INVEN` |

### `PROTO_NC_WT_LICENSE_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |
|     2 | `weaponitem` | `ITEM_INVEN` |

### `PROTO_NC_WT_LICENSE_CLR_ACK`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     8 | `echo_data` | `PROTO_NC_WT_LICENSE_CLR_ACK::echo_data` |
|    12 | `WeaponItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_WT_LICENSE_CLR_ACK::echo_data`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |
|     2 | `weaponitem` | `ITEM_INVEN` |

### `PROTO_NC_WT_LICENSE_CLR_REQ`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     8 | `echo_data` | `PROTO_NC_WT_LICENSE_CLR_REQ::echo_data` |
|    12 | `WeaponItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    22 | `LicenseClearItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_WT_LICENSE_CLR_REQ::echo_data`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |
|     2 | `weaponitem` | `ITEM_INVEN` |

### `PROTO_NC_WT_LICENSE_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ItemInven` | `ITEM_INVEN` |

### `PROTO_NC_WT_LICENSE_REQ`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |
|     2 | `weaponitem` | `ITEM_INVEN` |

### `PROTO_NC_WT_LICENSE_SET_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|    10 | `echo_data` | `PROTO_NC_WT_LICENSE_SET_ACK::echo_data` |

### `PROTO_NC_WT_LICENSE_SET_ACK::echo_data`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |
|     2 | `weaponitem` | `ITEM_INVEN` |

### `PROTO_NC_WT_LICENSE_SET_REQ`  (sizeof = 54 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|    12 | `chrname` | `Name5` |
|    32 | `WeaponItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    40 | `LicenItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    50 | `echo_data` | `PROTO_NC_WT_LICENSE_SET_REQ::echo_data` |

### `PROTO_NC_WT_LICENSE_SET_REQ::echo_data`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |
|     2 | `weaponitem` | `ITEM_INVEN` |

### `PROTO_NC_WT_MOBINC_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_WT_MOB_KILLCOUNT_SET_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `ItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_WT_MOB_KILLCOUNT_SET_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `ItemKey` | `SHINE_ITEM_REGISTNUMBER` |

### `PROTO_NC_WT_TITLE_CMD`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ItemInven` | `ITEM_INVEN` |
|     2 | `Title` | `wchar_t[21]   /* 21 bytes */` |

### `PROTO_NC_WT_TITLE_SET_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_WT_TITLE_SET_REQ`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    18 | `Title` | `wchar_t[21]   /* 21 bytes */` |

### `PROTO_NC_ZONE_PRISON_END_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ZONE_PRISON_GO_ACK`  (sizeof = 374 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sGmUserID` | `Name256Byte` |
|   262 | `sCharID` | `Name5` |
|   292 | `sReason` | `wchar_t[16]   /* 16 bytes */` |
|   308 | `sRemark` | `wchar_t[64]   /* 64 bytes */` |

### `PROTO_NC_ZONE_PRISON_GO_REQ`  (sizeof = 372 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `sGmUserID` | `Name256Byte` |
|   262 | `sCharID` | `Name5` |
|   292 | `sReason` | `wchar_t[16]   /* 16 bytes */` |
|   308 | `sRemark` | `wchar_t[64]   /* 64 bytes */` |

