# PROTO_NC_* Struct Layouts (extracted from PDB)

_Source: `WorldManager_types.txt` -- 1221 structs_

### `NETPACKET`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netcmd` | `NETCOMMAND` |
|     2 | `data` | `unsigned char[0]   /* 0 bytes */` |

### `NETPACKETHEADER`  (sizeof = 2 bytes)

_no members_

### `NETPACKETZONEHEADER`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ACT_CHAT_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_NOTICE_CMD_SEND`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_ACT_CHAT_REQ` |

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

### `PROTO_NC_ACT_SOMEONEWHISPER_CMD`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `talker` | `Name5` |
|    21 | `flag` | `PROTO_NC_ACT_SOMEONEWHISPER_CMD::<unnamed-type-flag>` |
|    23 | `content` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_ACT_SOMEONEWHISPER_CMD::<unnamed-type-flag>`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GMColor` | `<LF_BITFIELD:0x1526>` |

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

### `PROTO_NC_BAT_CLIENT_MOB_KILL_ANNOUNCE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTextIndex` | `MobKillAnnounceType` |

### `PROTO_NC_BAT_PKINPKFIELD_WMS_CMD`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_BAT_WORLD_MOB_KILL_ANNOUNCE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTextIndex` | `MobKillAnnounceType` |

### `PROTO_NC_BRIEFINFO_ABSTATE_CHANGE_LIST_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `infoList` | `ABSTATE_INFORMATION[?] /* 0 bytes */` |

### `PROTO_NC_CHARGED_RESETBUFF_CMD`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_CHARGED_SETBUFF_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `ChargedBuff` | `PROTO_CHARGEDBUFF_INFO` |

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
|     0 | `nPacketOrderNum` | `<LF_BITFIELD:0x32f7>` |
|     0 | `nIsLastOfInven` | `<LF_BITFIELD:0x32f8>` |

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

### `PROTO_NC_CHARSAVE_2WLDMAN_TITLE_CMD`  (sizeof = 1037 bytes)

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

### `PROTO_NC_CHARSAVE_ITEMACTIONCOOLTIME_ACK`  (sizeof = 2 bytes)

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

### `PROTO_NC_CHARSAVE_SELL_ITEM_INFO_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     7 | `SellItemList` | `PROTO_SELL_ITEM_INFO_SERVER[?] /* 0 bytes */` |

### `PROTO_NC_CHARSAVE_SET_CHAT_BLOCK_SPAMER_WM_CMD`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_CHARSAVE_SKILL_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     8 | `skill` | `PROTO_SKILLREADBLOCK[?] /* 0 bytes */` |

### `PROTO_NC_CHARSAVE_USEITEM_MINIMON_INFO_WORLD_CMD`  (sizeof = 56 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `UseItemMinimonInfo` | `USEITEM_MINIMON_INFO` |

### `PROTO_NC_CHAR_ABSTATE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `abstate` | `ABSTATEREADBLOCK[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_ARENA_CMD`  (sizeof = 1 bytes)

_no members_

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
|     0 | `skillempower_can_reset` | `<LF_BITFIELD:0x1264>` |
|     0 | `restunion` | `<LF_BITFIELD:0x35fc>` |

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

### `PROTO_NC_CHAR_CLASSCHANGE_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CHAR_COININFO_CMD`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_CHAR_DB_AUTO_PICK_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_CHAR_DB_NEWBIE_GUIDE_VIEW_SET_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_CHAR_DB_REST_EXP_LAST_EXEC_TIME_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_CHAR_EMBLEM_CMD`  (sizeof = 1 bytes)

_no members_

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

### `PROTO_NC_CHAR_FRIEND_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `friend_array` | `PROTO_NC_CHAR_FRIEND_CMD_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_FRIEND_CMD_INFO`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `charid` | `Name5` |

### `PROTO_NC_CHAR_GET_CHAT_BLOCK_SPAMER_DB_CMD`  (sizeof = 12 bytes)

_no members_

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
|     0 | `nPacketOrderNum` | `<LF_BITFIELD:0x32f7>` |
|     0 | `nIsLastOfInven` | `<LF_BITFIELD:0x32f8>` |

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

### `PROTO_NC_CHAR_MASPUP_CMD`  (sizeof = 1 bytes)

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

### `PROTO_NC_CHAR_SKILL_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `PartMark` | `PARTMARK` |
|     9 | `skill` | `PROTO_SKILLREADBLOCK[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_SKILL_PASSIVE_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `passive` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_CHAR_SUPPORT_REWARD_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `eUserType` | `USER_TYPE` |

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

### `PROTO_NC_CHAR_TUTORIAL_STEP_SAVE_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `TutorialInfo` | `PROTO_TUTORIAL_INFO` |

### `PROTO_NC_CHAR_USEITEM_MINIMON_INFO_CMD`  (sizeof = 56 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `UseItemMinimonInfo` | `USEITEM_MINIMON_INFO` |

### `PROTO_NC_CHAR_USEITEM_MINIMON_INFO_ZONE_CMD`  (sizeof = 56 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `UseItemMinimonInfo` | `USEITEM_MINIMON_INFO` |

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
|     0 | `Main` | `<LF_BITFIELD:0x1c3c>` |
|     0 | `Sub` | `<LF_BITFIELD:0x1c3d>` |

### `PROTO_NC_CHAR_WEDDING_CANCEL_WEDDING`  (sizeof = 4 bytes)

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

### `PROTO_NC_CT_ADD_FRIEND_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CT_CHARTTING_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |

### `PROTO_NC_CT_LUASCRIPT_SET_WORLD_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `CTInfo` | `CT_INFO` |

### `PROTO_NC_CT_LUASCRIPT_SET_ZONE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `CTInfo` | `CT_INFO` |

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

### `PROTO_NC_DATA_SEND1<PROTO_NC_CHAR_SET_STYLE_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_CHAR_SET_STYLE_DB_REQ>`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_CHAR_SET_STYLE_GET_INFO_ACK>`  (sizeof = 29 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_CHAR_SET_STYLE_GET_INFO_DB_REQ>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_CHAR_WEDDING_CANCEL_WEDDING>`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_DATA_PRISON_GET_REQ>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_DATA_PRISON_UPDATE_MIN_CMD>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_DATA_REPORT_CANCEL_REQ>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_DATA_REPORT_GET_REQ>`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_FRIEND_DEL_ACK>`  (sizeof = 45 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_ACADEMY_CHAT_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_ACADEMY_DB_GRADUATE_JOIN_REQ>`  (sizeof = 35 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_ACADEMY_DB_LEVEL_UP_REQ>`  (sizeof = 17 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_ACADEMY_DB_MEMBER_REQ>`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_ACADEMY_MASTER_TELEPORT_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_ACADEMY_MEMBER_CHAT_BAN_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_ACADEMY_MEMBER_LEAVE_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_ACADEMY_MEMBER_VANISH_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_ACADEMY_NOTIFY_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_CHAT_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_DB_MEMBER_REQ>`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_DB_RENAME_REQ>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_DB_RETYPE_REQ>`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_DISMISS_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_MAKE_ACK>`  (sizeof = 30 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_MEMBER_GRADE_ACK>`  (sizeof = 26 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_MEMBER_LEAVE_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_MEMBER_VANISH_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_NAME_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_NOTIFY_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_TOURNAMENT_DB_FINAL_SELECTION_REQ>`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_TOURNAMENT_DB_JOIN_LIST_REQ>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_TOURNAMENT_DB_SET_MATCH_REQ>`  (sizeof = 164 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_TOURNAMENT_DB_SET_MATCH_TIME_REQ>`  (sizeof = 45 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_TOURNAMENT_DB_SET_RESULT_REQ>`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_TOURNAMENT_DB_SET_TYPE_REQ>`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_TOURNAMENT_JOIN_ACK>`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_TOURNAMENT_RESULT_CMD>`  (sizeof = 15 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_TOURNAMENT_START_CMD>`  (sizeof = 20 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_TOURNAMENT_TYPE_CMD>`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_WAR_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_WAR_SCORE_CMD>`  (sizeof = 119 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_GUILD_WORLD_USE_GUILD_TOKEN_CMD>`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_ADD_CMD>`  (sizeof = 31 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_DB_DEL_CHAR_REQ>`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_DB_DEL_DOWN_REQ>`  (sizeof = 31 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_DB_DEL_UP_REQ>`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_DB_REWARD_REQ>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_DB_SET_DATE_REQ>`  (sizeof = 15 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_DB_SET_UP_REQ>`  (sizeof = 87 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_DEL_CMD>`  (sizeof = 24 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_DEL_DOWN_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_DEL_UP_ACK>`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_LEVEL_CMD>`  (sizeof = 24 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_LOGIN_CMD>`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_LOGOUT_CMD>`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_MYUPPER_ACK>`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_MY_UP_ZONE>`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_SET_UP_ACK>`  (sizeof = 71 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_SET_UP_CONFIRM_ING>`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_HOLY_PROMISE_SET_UP_CONFIRM_REQ>`  (sizeof = 43 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_INSTANCE_DUNGEON_DELETE_DUNGEON_CMD>`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_KQ_JOIN_ACK>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_KQ_JOIN_CANCEL_ACK>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_KQ_LIST_TIME_ACK>`  (sizeof = 43 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_MISC_CONNECTFROMWHERE_ACK>`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_MISC_CONNECTFROMWHERE_DB_REQ>`  (sizeof = 15 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_MISC_EVENT_DONE_MUNSANG_WM2ACC>`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_MISC_EVENT_DONE_MUNSANG_WM2Z>`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_MISC_EVENT_L20_CMD>`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_PARTY_FINDER_ADD_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_PARTY_FINDER_DELETE_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_PARTY_FINDER_DELETE_YOUR_MSG_CMD>`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_PARTY_SET_LOOTER_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_PARTY_SET_LOOTER_BROAD_CMD>`  (sizeof = 43 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_PARTY_SET_LOOTER_CMD>`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_PARTY_ZONE_SET_LOOTER_CMD>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_PRISON_ADD_GM_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_PRISON_ALTER_GM_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_PRISON_END_ACK>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_PRISON_GET_ACK>`  (sizeof = 87 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_PRISON_OK_CMD>`  (sizeof = 85 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_CHAT_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_DEL_SUB_LEADER_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_DEL_SUB_LEADER_BROAD_CMD>`  (sizeof = 47 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_DEL_SUB_LEADER_CMD>`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_DISMISS_BROAD_CMD>`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_INVITATION_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_INVITATION_ANSWER_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_INVITATION_ANSWER_CMD>`  (sizeof = 24 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_INVITATION_CMD>`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_LEAVE_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_LEAVE_BROAD_CMD>`  (sizeof = 27 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_LOGININFO_CMD>`  (sizeof = 27 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_LOGIN_CMD>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_LOGOUTINFO_CMD>`  (sizeof = 27 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_LOGOUT_CMD>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_LOOTING_TYPE_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_LOOTING_TYPE_BROAD_CMD>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_MAKE_BROAD_CMD>`  (sizeof = 113 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_MEMBER_JOIN_BROAD_CMD>`  (sizeof = 27 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_SET_LEADER_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_SET_LEADER_BROAD_CMD>`  (sizeof = 47 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_SET_LEADER_CMD>`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_SET_LOOTER_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_SET_LOOTER_BROAD_CMD>`  (sizeof = 47 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_SET_LOOTER_CMD>`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_SET_SUB_LEADER_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_SET_SUB_LEADER_BROAD_CMD>`  (sizeof = 47 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_SET_SUB_LEADER_CMD>`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_SLOT_MOVE_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_SLOT_MOVE_BROAD_CMD>`  (sizeof = 231 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_VANISH_ACK>`  (sizeof = 25 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_VANISH_BROAD_CMD>`  (sizeof = 47 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_VANISH_CMD>`  (sizeof = 23 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_WARNING_ACK>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_ZONE_DEL_SUB_LEADER_CMD>`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_ZONE_DISMISS_CMD>`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_ZONE_LEAVE_CMD>`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_ZONE_LOOTING_TYPE_CMD>`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_ZONE_MAKE_CMD>`  (sizeof = 35 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_ZONE_MEMBER_JOIN_CMD>`  (sizeof = 45 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_ZONE_SET_LEADER_CMD>`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_ZONE_SET_LOOTER_CMD>`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_ZONE_SET_SUB_LEADER_CMD>`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_ZONE_SLOT_MOVE_CMD>`  (sizeof = 53 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_RAID_ZONE_VANISH_CMD>`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_REPORT_ADD_ACK>`  (sizeof = 29 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_REPORT_CANCEL_ACK>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_REPORT_GET_ACK>`  (sizeof = 138 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_ZONE_PRISON_END_CMD>`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_DATA_SEND2<PROTO_NC_DATA_REPORT_ADD_REQ>`  (sizeof = 2374 bytes)

_no members_

### `PROTO_NC_DATA_SEND2<PROTO_NC_GUILD_TOURNAMENT_LIST_ACK>`  (sizeof = 1019 bytes)

_no members_

### `PROTO_NC_DATA_SEND2<PROTO_NC_RAID_MEMBER_LIST_CMD>`  (sizeof = 547 bytes)

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

### `PROTO_NC_GAMBLE_EXCHANGEDCOIN_INIT_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GAMBLE_WORLD_PREVMAPNAME_CMD`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sPrevMapName` | `Name3` |

### `PROTO_NC_GAMBLE_ZONE_PREVMAPNAME_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sPrevMapName` | `Name3` |

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

### `PROTO_NC_GUILD_ACADEMY_DB_GRADUATE_JOIN_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `sName` | `Name5` |

### `PROTO_NC_GUILD_ACADEMY_DB_LEVEL_UP_ACK`  (sizeof = 16 bytes)

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

### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_GRADE_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_GRADE_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

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

### `PROTO_NC_GUILD_DB_INTRO_ACK`  (sizeof = 12 bytes)

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

### `PROTO_NC_GUILD_DB_MONEY_SUB_ACK`  (sizeof = 36 bytes)

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

### `PROTO_NC_GUILD_RENAME_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_RENAME_CMD_SEND`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_GUILD_RENAME_CMD` |

### `PROTO_NC_GUILD_RETYPE_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_GUILD_STORAGE_WITHDRAW_GRADE_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

### `PROTO_NC_GUILD_STORAGE_WITHDRAW_GRADE_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

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

### `PROTO_NC_GUILD_TOURNAMENT_DB_REWARD_CREATE_ACK`  (sizeof = 28 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_REWARD_CREATE_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `ItemOpt` | `PROTO_NC_GUILD_TOURNAMENT_DB_REWARD_CREATE_REQ::ITEM_OPT[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_DB_REWARD_CREATE_REQ::ITEM_OPT`  (sizeof = 33 bytes)

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

### `PROTO_NC_GUILD_TOURNAMENT_END_CMD`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_ENTER_LIST_DB_GET_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     5 | `EnterGuildList` | `GT_EnterList[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_ENTER_LIST_DB_GET_REQ`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_GOLD_REFUND_ZONE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `EnterGuildList` | `GT_EnterList[?] /* 0 bytes */` |

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

### `PROTO_NC_GUILD_TOURNAMENT_NOTIFY_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Type` | `GUILD_TOURNAMENT_NOTIFY_TYPE` |
|     5 | `sGuildName` | `Name4` |

### `PROTO_NC_GUILD_TOURNAMENT_REFUND_NOTICE_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_REFUND_NOTICE_DB_SET_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_RESULT_CMD`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_START_CMD`  (sizeof = 17 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_TYPE_CMD`  (sizeof = 1 bytes)

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

### `PROTO_NC_GUILD_WAR_SCORE_CMD`  (sizeof = 116 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GuildName1` | `Name4` |
|    16 | `GuildScore1` | `SHINE_GUILD_SCORE` |
|    58 | `GuildName2` | `Name4` |
|    74 | `GuildScore2` | `SHINE_GUILD_SCORE` |

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

### `PROTO_NC_HOLY_PROMISE_DB_GET_MEMBER_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `MemberInfo` | `PROTO_HOLY_PROMISE_INFO_DB[?] /* 0 bytes */` |

### `PROTO_NC_HOLY_PROMISE_DB_GET_REMAIN_MONEY_CMD`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_HOLY_PROMISE_DB_GET_UP_ACK`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `UpMemberInfo` | `PROTO_HOLY_PROMISE_INFO_DB` |

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

### `PROTO_NC_INSTANCE_DUNGEON_DELETE_DUNGEON_CMD`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_INSTANCE_DUNGEON_EMPTY_DUNGEON_CMD`  (sizeof = 5 bytes)

_no members_

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

### `PROTO_NC_ITEMDB_BUYLOT_REQ`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `iteminfo` | `PROTO_ITEM_ATTRCHANGE` |

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

### `PROTO_NC_KQ_NOTIFY_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `Msg` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_KQ_PLAYER_DISJOIN_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_KQ_SCHEDULE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    10 | `NewQuestArray` | `PROTO_KQ_INFO_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_KQ_SCHEDULE_REQ`  (sizeof = 8 bytes)

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

### `PROTO_NC_KQ_Z2W_END_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_KQ_Z2W_MAKE_ACK`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_LOG_GAME_CREATE_AVATAR`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_LOG_GAME_CREATE_AVATAR_SEND`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `log` | `PROTO_NC_LOG_GAME_CREATE_AVATAR` |

### `PROTO_NC_LOG_GAME_DELETE_AVATAR`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_LOG_GAME_DELETE_AVATAR_SEND`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `log` | `PROTO_NC_LOG_GAME_DELETE_AVATAR` |

### `PROTO_NC_LOG_GAME_LINK`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `mappos` | `MAPPOS` |

### `PROTO_NC_LOG_USER_LOGOUT`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_LOG_USER_LOGOUT_SEND`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `log` | `PROTO_NC_LOG_USER_LOGOUT` |

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

### `PROTO_NC_MAP_REGIST_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `maparray` | `PROTO_NC_MAP_REGIST_CMD::<unnamed-type-maparray>[?] /* 0 bytes */` |

### `PROTO_NC_MAP_REGIST_CMD::<unnamed-type-maparray>`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `mapname` | `Name3` |

### `PROTO_NC_MISC_CHAR_LOGOFF_STATISTICS`  (sizeof = 21 bytes)

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

### `PROTO_NC_MISC_CONNECTFROMWHERE_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |

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

### `PROTO_NC_MISC_DB_CLIENT_LOADING_BUG_DETECT_CMD`  (sizeof = 72 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sLoadingMapName` | `Name3` |
|    24 | `sFromMapName` | `Name3` |
|    36 | `sFromZoneIP` | `Name4` |
|    52 | `sToZoneIP` | `Name4` |
|    72 | `ErrorData` | `CLBD_ErrorData[?] /* 0 bytes */` |

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

### `PROTO_NC_MISC_EVENT_DONE_MUNSANG_Z2WM`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_MISC_EVENT_L20_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MISC_EVENT_L20_DB_ACK`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_MISC_EVENT_LONG_TIME_PLAY_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_MISC_GAMETIME_ACK`  (sizeof = 3 bytes)

_no members_

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

### `PROTO_NC_MISC_HEARTBEAT_ACK_SEND`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |

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

### `PROTO_NC_MISC_PINGTEST_TOOL_WM_CLIENT_ZONE_DB`  (sizeof = 130 bytes)

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

### `PROTO_NC_MISC_RESTMINUTE_CMD_SEND`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_MISC_RESTMINUTE_CMD` |

### `PROTO_NC_MISC_S2SCONNECTION_ACK`  (sizeof = 3 bytes)

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

### `PROTO_NC_MISC_XTRAP2_OPTOOL_READ_CODEMAP_REQ`  (sizeof = 1 bytes)

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

### `PROTO_NC_PARTY_MEMBERINFORM_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `Member` | `PROTO_PARTY_MEMBER[?] /* 0 bytes */` |

### `PROTO_NC_PARTY_MEMBERINFORM_REQ`  (sizeof = 2 bytes)

_no members_

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

### `PROTO_NC_PET_LINK_RESUMMON_CMD`  (sizeof = 23 bytes)

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

### `PROTO_NC_PRISON_OK_CMD`  (sizeof = 82 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sReason` | `wchar_t[16]   /* 16 bytes */` |
|    18 | `sRemark` | `wchar_t[64]   /* 64 bytes */` |

### `PROTO_NC_PROMOTION_DB_REWARD_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `RewardList` | `PROTO_NC_PROMOTION_DB_REWARD_ACK::<unnamed-type-RewardList>[?] /* 0 bytes */` |

### `PROTO_NC_PROMOTION_DB_REWARD_ACK::<unnamed-type-RewardList>`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_PROMOTION_REWARD_ITEM_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `RewardList` | `PROTO_NC_PROMOTION_REWARD_ITEM_CMD::<unnamed-type-RewardList>[?] /* 0 bytes */` |

### `PROTO_NC_PROMOTION_REWARD_ITEM_CMD::<unnamed-type-RewardList>`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_QUEST_RESET_TIME_CMD`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_QUEST_RESET_TIME_ZONE_CMD`  (sizeof = 20 bytes)

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

### `PROTO_NC_SKILL_PASSIVESKILL_LEARN_CMD`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_SYSLOG_ACCOUNT_LOGOUT`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_SYSLOG_CHAR_CREATED`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_SYSLOG_CHAR_DELETED`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_SYSLOG_CHAR_ENTER_GAME`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_SYSLOG_CHAR_LEAVE_GAME`  (sizeof = 12 bytes)

_no members_

### `PROTO_NC_SYSLOG_CHAR_LEVEL_UP`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_SYSLOG_SERVER_CCU`  (sizeof = 5 bytes)

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

### `PROTO_NC_USER_CONNECTION_CHANGE_CHAR_ID_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `sNewID` | `Name5` |

### `PROTO_NC_USER_CONNECTION_DB_CHANGE_CHAR_ID_ACK`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `sNewID` | `Name5` |

### `PROTO_NC_USER_CONNECTION_DB_CHANGE_CHAR_ID_REQ`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    14 | `sNewID` | `Name5` |

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

### `PROTO_NC_USER_KICKOFFFROMWORLD_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_USER_KICKOFFFROMWORLD_CMD_SEND`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     9 | `cmd` | `PROTO_NC_USER_KICKOFFFROMWORLD_CMD` |

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

### `PROTO_NC_USER_POSSIBLE_NEW_CONNECT_CMD`  (sizeof = 6 bytes)

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

### `PROTO_NC_USER_TUTORIAL_CAN_SKIP_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USER_WILLLOGIN_ACK`  (sizeof = 348 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `will_login_req` | `PROTO_NC_USER_WILLLOGIN_REQ` |

### `PROTO_NC_USER_WILLLOGIN_REQ`  (sizeof = 347 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     7 | `userid` | `Name256Byte` |
|   263 | `validate_new` | `unsigned short[32]   /* 64 bytes */` |
|   327 | `spawnapps` | `Name5` |

### `PROTO_NC_USER_WILL_WORLD_SELECT_ACK`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sOTP` | `Name8` |

### `PROTO_NC_USER_WILL_WORLD_SELECT_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_USP_USER_CHARACTER_DELETE`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_USP_USER_CHARACTER_INSERT`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     9 | `sCharName` | `wchar_t[40]   /* 40 bytes */` |

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

