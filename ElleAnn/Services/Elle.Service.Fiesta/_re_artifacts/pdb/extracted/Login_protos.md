# PROTO_NC_* Struct Layouts (extracted from PDB)

_Source: `Login_types.txt` -- 85 structs_

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
|     0 | `Main` | `<LF_BITFIELD:0x1f19>` |
|     0 | `Sub` | `<LF_BITFIELD:0x1f1a>` |

### `PROTO_NC_DATA_SEND1<PROTO_NC_OPTOOL_LOGIN_USER_RATABLE_GET_ACK>`  (sizeof = 16 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_OPTOOL_LOGON_PROCESS_TIME_VIEW_ACK>`  (sizeof = 27 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_USER_GER_IS_IP_BLOCK_REQ>`  (sizeof = 316 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_USER_TEENAGER_CMD>`  (sizeof = 58 bytes)

_no members_

### `PROTO_NC_DATA_SEND1<PROTO_NC_USER_TW_IS_IP_BLOCK_REQ>`  (sizeof = 37 bytes)

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

### `PROTO_NC_MISC_DELIVER_WM_LOGIN_ACDB_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `packet` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_DELIVER_WM_LOGIN_ALDB_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `packet` | `unsigned char[0]   /* 0 bytes */` |

### `PROTO_NC_MISC_HEARTBEAT_ACK_SEND`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |

### `PROTO_NC_MISC_HEARTBEAT_REQ_SEND`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |

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

### `PROTO_NC_MISC_USER_COUNT_CMD`  (sizeof = 4 bytes)

_no members_

### `PROTO_NC_OPTOOL_CLOSE_SERVER_ACK`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_OPTOOL_CONNECT_BRIF_ACK`  (sizeof = 170 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     2 | `count` | `unsigned short[2]   /* 4 bytes */[2]   /* 8 bytes */[21]   /* 168 bytes */` |

### `PROTO_NC_OPTOOL_CONNECT_BRIF_REQ`  (sizeof = 1 bytes)

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

### `PROTO_NC_USER_CH_PASSWORD_CHECK_ACK`  (sizeof = 287 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `id` | `Name256Byte` |
|   258 | `password` | `Name4` |

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

### `PROTO_NC_USER_CONNECTCUT_CMD`  (sizeof = 2 bytes)

_no members_

### `PROTO_NC_USER_CONNECTCUT_CMD_SEND`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     1 | `netcmd` | `NETCOMMAND` |
|     3 | `cmd` | `PROTO_NC_USER_CONNECTCUT_CMD` |

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

### `PROTO_NC_USER_PASSWORD_CHECK_ACK`  (sizeof = 287 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     2 | `id` | `Name256Byte` |
|   258 | `password` | `Name4` |

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

### `PROTO_NC_USER_TEENAGER_ACK`  (sizeof = 313 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sUserID` | `Name256Byte` |
|   258 | `Data` | `PROTO_NC_USER_TEENAGER_CMD` |

### `PROTO_NC_USER_TEENAGER_CMD`  (sizeof = 55 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sUserKey` | `unsigned char[50]   /* 50 bytes */` |

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

### `PROTO_NC_USER_WILLLOGIN_REQ`  (sizeof = 347 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketheader` | `NETPACKETHEADER` |
|     7 | `userid` | `Name256Byte` |
|   263 | `validate_new` | `unsigned short[32]   /* 64 bytes */` |
|   327 | `spawnapps` | `Name5` |

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

