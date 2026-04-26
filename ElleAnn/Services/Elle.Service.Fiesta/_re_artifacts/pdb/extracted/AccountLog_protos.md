# PROTO_NC_* Struct Layouts (extracted from PDB)

_Source: `AccountLog_types.txt` -- 108 structs_

### `NETPACKET`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netcmd` | `NETCOMMAND` |
|     2 | `data` | `unsigned char[0]   /* 0 bytes */` |

### `NETPACKETHEADER`  (sizeof = 2 bytes)

_no members_

### `NETPACKETZONEHEADER`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BRIEFINFO_ABSTATE_CHANGE_LIST_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `infoList` | `ABSTATE_INFORMATION[?] /* 0 bytes */` |

### `PROTO_NC_BRIEFINFO_CHANGEDECORATE_CMD`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_BRIEFINFO_CHANGEUPGRADE_CMD`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_BRIEFINFO_CHANGEWEAPON_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `upgradeinfo` | `PROTO_NC_BRIEFINFO_CHANGEUPGRADE_CMD` |

### `PROTO_NC_BRIEFINFO_UNEQUIP_CMD`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_CHARSAVE_ITEMACTIONCOOLTIME_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `group` | `ActionCooltime[?] /* 0 bytes */` |

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
|     0 | `skillempower_can_reset` | `<LF_BITFIELD:0x2925>` |
|     0 | `restunion` | `<LF_BITFIELD:0x2926>` |

### `PROTO_NC_CHAR_BASE_CMD::LoginLocation`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `currentmap` | `Name3` |
|    12 | `currentcoord` | `SHINE_COORD_TYPE` |

### `PROTO_NC_CHAR_CHARGEDBUFF_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `ChargedBuff` | `PROTO_CHARGEDBUFF_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_CHESTINFO_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `location` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_CHAR_EMBLEM_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_FRIEND_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `friend_array` | `PROTO_NC_CHAR_FRIEND_CMD_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_FRIEND_CMD_INFO`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `charid` | `Name5` |

### `PROTO_NC_CHAR_GUILD_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `Guild` | `GUILD_CLIENT[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_HOUSE_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_MASPUP_CMD`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_CHAR_NEWBIE_GUIDE_VIEW_LIST_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `nGuideViewList` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_CHAR_OPTION_GAME`  (sizeof = 64 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `unsigned char[64]   /* 64 bytes */` |

### `PROTO_NC_CHAR_OPTION_KEYMAPPING`  (sizeof = 308 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Data` | `unsigned char[308]   /* 308 bytes */` |

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

### `PROTO_NC_CHAR_QUEST_READ_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `QuestReadIDArray` | `unsigned short[0]   /* 0 bytes */` |

### `PROTO_NC_CHAR_QUEST_REPEAT_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     6 | `QuestRepeatArray` | `PLAYER_QUEST_INFO[?] /* 0 bytes */` |

### `PROTO_NC_CHAR_SEAWAR_CMD`  (sizeof = 1 bytes)

_no members_

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
|     0 | `Main` | `<LF_BITFIELD:0x10b6>` |
|     0 | `Sub` | `<LF_BITFIELD:0x10b7>` |

### `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `PartnerInfo` | `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_ACK::<unnamed-type-PartnerInfo>` |

### `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_ACK::<unnamed-type-PartnerInfo>`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `LastConnect` | `ShineDateTime` |

### `PROTO_NC_CHAT_RESTRICT_DB_LIST_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `ChatRestrictData` | `PROTO_CHAT_RESTRICT_DB_INFO[?] /* 0 bytes */` |

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

### `PROTO_NC_DATA_SEND1<PROTO_NC_DATA_REPORT_ADD_ACK>`  (sizeof = 31 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_DATA_REPORT_CANCEL_ACK>`  (sizeof = 11 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_DATA_REPORT_GET_ACK>`  (sizeof = 140 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_MISC_EVENT_L20_DB_ACK>`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_USER_GER_IS_IP_BLOCK_ACK>`  (sizeof = 293 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_USER_IS_IP_BLOCK_ACK>`  (sizeof = 278 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_USER_TW_IS_IP_BLOCK_ACK>`  (sizeof = 14 bytes)

_no members_

### `PROTO_NC_GUILD_TOURNAMENT_DB_REWARD_CREATE_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|    12 | `ItemOpt` | `PROTO_NC_GUILD_TOURNAMENT_DB_REWARD_CREATE_REQ::ITEM_OPT[?] /* 0 bytes */` |

### `PROTO_NC_GUILD_TOURNAMENT_DB_REWARD_CREATE_REQ::ITEM_OPT`  (sizeof = 33 bytes)

_no members_

### `PROTO_NC_ITEMDB_CHARGED_LIST_ACK`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|    11 | `ChargedItemInfoList` | `PROTO_CHARGED_ITEM_INFO[?] /* 0 bytes */` |

### `PROTO_NC_ITEM_BUY_REQ`  (sizeof = 6 bytes)

_no members_

### `PROTO_NC_ITEM_DROP_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `ITEM_INVEN` |
|     6 | `loc` | `SHINE_XY_TYPE` |

### `PROTO_NC_ITEM_SELL_REQ`  (sizeof = 5 bytes)

_no members_

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

### `PROTO_NC_LOG_USER_LOGOUT`  (sizeof = 9 bytes)

_no members_

### `PROTO_NC_MISC_CHAR_LOGOFF_STATISTICS`  (sizeof = 21 bytes)

_no members_

### `PROTO_NC_MISC_CONNECTER_ACK`  (sizeof = 131 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     3 | `nEachClass` | `unsigned short[64]   /* 128 bytes */` |

### `PROTO_NC_MISC_CONNECTER_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_MISC_EVENT_L20_DB_ACK`  (sizeof = 13 bytes)

_no members_

### `PROTO_NC_MISC_EVENT_L20_DB_REQ`  (sizeof = 272 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `sUserID` | `Name256Byte` |

### `PROTO_NC_MISC_EVENT_LONG_TIME_PLAY_CMD`  (sizeof = 6 bytes)

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

### `PROTO_NC_MISC_ITEMSHOP_URL_DB_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     8 | `sURL` | `wchar_t[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_ITEMSHOP_URL_DB_REQ`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_MISC_S2SCONNECTION_ACK`  (sizeof = 3 bytes)

_no members_

### `PROTO_NC_MISC_S2SCONNECTION_RDY_SEND`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |

### `PROTO_NC_MISC_S2SCONNECTION_REQ`  (sizeof = 7 bytes)

_no members_

### `PROTO_NC_OPTOOL_CLOSE_SERVER_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_OPTOOL_CONNECT_BRIF_ACK`  (sizeof = 170 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `count` | `unsigned short[2]   /* 4 bytes */[2]   /* 8 bytes */[21]   /* 168 bytes */` |

### `PROTO_NC_OPTOOL_CONNECT_BRIF_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_MAP_USER_LIST_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `user_info` | `NC_OPTOOL_MAP_USER_LIST_INFO[?] /* 0 bytes */` |

### `PROTO_NC_OPTOOL_MAP_USER_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_OPTOOL_S2SCONNECT_LIST_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `connection_info` | `SERVER_CONN_INFO[?] /* 0 bytes */` |

### `PROTO_NC_OPTOOL_S2SCONNECT_LIST_REQ`  (sizeof = 1 bytes)

_no members_

### `PROTO_NC_SYSLOG_ACCOUNT_LOGIN_FAILURE`  (sizeof = 296 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `UserName` | `wchar_t[260]   /* 260 bytes */` |
|   260 | `UserIP` | `Name4` |
|   276 | `spawnapps` | `Name5` |

### `PROTO_NC_SYSLOG_ACCOUNT_LOGIN_SUCCESS`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     4 | `UserIP` | `Name4` |
|    20 | `spawnapps` | `Name5` |

### `PROTO_NC_SYSLOG_ACCOUNT_LOGOUT`  (sizeof = 8 bytes)

_no members_

### `PROTO_NC_SYSLOG_SERVER_CCU`  (sizeof = 5 bytes)

_no members_

### `PROTO_NC_USER_GER_IS_IP_BLOCK_ACK`  (sizeof = 290 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_GER_PASSWORD_CHECK_ACK` |

### `PROTO_NC_USER_GER_IS_IP_BLOCK_REQ`  (sizeof = 313 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_GER_PASSWORD_CHECK_ACK` |
|   293 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |

### `PROTO_NC_USER_GER_PASSWORD_CHECK_ACK`  (sizeof = 287 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `id` | `Name256Byte` |
|   258 | `password` | `Name4` |

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

### `PROTO_NC_USER_JP_PASSWORD_CHECK_ACK`  (sizeof = 306 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     8 | `sUserName` | `wchar_t[24]   /* 24 bytes */` |
|    32 | `sChannel` | `wchar_t[254]   /* 254 bytes */` |
|   286 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |

### `PROTO_NC_USER_LOGIN_REQ`  (sizeof = 272 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `user` | `Name256Byte` |
|   256 | `password` | `Name4` |

### `PROTO_NC_USER_LOGOUT_LAST_DAY_ACK`  (sizeof = 10 bytes)

_no members_

### `PROTO_NC_USER_LOGOUT_LAST_DAY_REQ`  (sizeof = 6 bytes)

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

### `PROTO_NC_USER_TW_PASSWORD_CHECK_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |

### `PROTO_NC_USER_US_IS_IP_BLOCK_ACK`  (sizeof = 356 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_US_PASSWORD_CHECK_ACK` |

### `PROTO_NC_USER_US_IS_IP_BLOCK_REQ`  (sizeof = 379 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LoginData` | `PROTO_NC_USER_US_PASSWORD_CHECK_ACK` |
|   359 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |

### `PROTO_NC_USER_US_PASSWORD_CHECK_ACK`  (sizeof = 353 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     8 | `sUserName` | `wchar_t[260]   /* 260 bytes */` |
|   268 | `sPassword` | `wchar_t[36]   /* 36 bytes */` |
|   304 | `sUserIP` | `wchar_t[20]   /* 20 bytes */` |
|   333 | `spawnapps` | `Name5` |

