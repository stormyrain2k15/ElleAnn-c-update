# Zone PROTO_NC_* Struct Layouts (custom V70 parser)

_Source: `5ZoneServer2.pdb` (TPI v7.0) -- 1123 structs_

### `CHARTITLE_BRIEFINFO`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Type` | `unsigned char` |
|     1 | `ElementNo` | `unsigned char` |
|     2 | `MobID` | `unsigned short` |


### `NETPACKETZONEHEADER`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `clienthandle` | `unsigned short` |
|     2 | `charregistnumber` | `unsigned long` |


### `PROTO_NC_ACT_ACTIONBYITEM_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `error` | `unsigned short` |
|     2 | `emoticonid` | `unsigned char` |


### `PROTO_NC_ACT_ACTIONBYITEM_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `unsigned char` |


### `PROTO_NC_ACT_ANIMATION_START_CMD`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHandle` | `unsigned short` |
|     2 | `sAnimationIndex` | `char[32]   /* 32 bytes */` |


### `PROTO_NC_ACT_ANIMATION_STOP_CMD`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHandle` | `unsigned short` |
|     2 | `sAnimationIndex` | `char[32]   /* 32 bytes */` |


### `PROTO_NC_ACT_AUTO_WAY_FINDING_USE_GATE_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |


### `PROTO_NC_ACT_CHANGEMODE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mode` | `unsigned char` |


### `PROTO_NC_ACT_CHAT_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `len` | `unsigned char` |
|     1 | `content` | `unsigned char[0]   /* 0 bytes */` |


### `PROTO_NC_ACT_COOKING_CAST_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Recipe` | `unsigned short` |


### `PROTO_NC_ACT_COOKING_MAKE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Err` | `unsigned short` |
|     2 | `Food` | `unsigned short` |


### `PROTO_NC_ACT_CREATECASTBAR`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `millisec` | `unsigned short` |


### `PROTO_NC_ACT_EMOTICON_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `emoticonid` | `unsigned char` |


### `PROTO_NC_ACT_GATHERCOMPLETE_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `object` | `unsigned short` |


### `PROTO_NC_ACT_GATHERSTART_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `picker` | `unsigned short` |
|     2 | `object` | `unsigned short` |
|     4 | `err` | `unsigned short` |


### `PROTO_NC_ACT_GATHERSTART_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `objhandle` | `unsigned short` |


### `PROTO_NC_ACT_MOVEFAIL_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `back` | `SHINE_XY_TYPE` |


### `PROTO_NC_ACT_MOVESPEED_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `walkspeed` | `unsigned short` |
|     2 | `runspeed` | `unsigned short` |


### `PROTO_NC_ACT_MOVEWALK_CMD`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `from` | `SHINE_XY_TYPE` |
|     8 | `to` | `SHINE_XY_TYPE` |
|    16 | `moveattr` | `PROTO_NC_ACT_MOVEWALK_CMD::__unnamed` |


### `PROTO_NC_ACT_MOVEWALK_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `movedirect` | `<UNRESOLVED:0x1281>` |


### `PROTO_NC_ACT_NPCCLICK_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `npchandle` | `unsigned short` |


### `PROTO_NC_ACT_NPCMENUOPEN_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ack` | `unsigned char` |


### `PROTO_NC_ACT_NPCMENUOPEN_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mobid` | `unsigned short` |


### `PROTO_NC_ACT_NPC_ACTION_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned char` |
|     1 | `nNPCHandle` | `unsigned short` |
|     3 | `nECode` | `unsigned long` |


### `PROTO_NC_ACT_NPC_MENU_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Ack` | `unsigned char` |


### `PROTO_NC_ACT_PRODUCE_CAST_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `produceskill` | `unsigned short` |


### `PROTO_NC_ACT_PRODUCE_MAKE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `item` | `unsigned short` |
|     2 | `err` | `unsigned short` |


### `PROTO_NC_ACT_REINFORCEMOVEBYPATH_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mover` | `unsigned short` |
|     2 | `speed` | `unsigned short` |
|     4 | `direct` | `unsigned char` |
|     5 | `pathlength` | `unsigned char` |
|     6 | `path` | `SHINE_XY_TYPE[?] /* 0 bytes */` |


### `PROTO_NC_ACT_REINFORCE_RUN_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `xy` | `SHINE_XY_TYPE` |


### `PROTO_NC_ACT_REINFORCE_STOP_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `loc` | `SHINE_XY_TYPE` |


### `PROTO_NC_ACT_RIDE_FEEDING_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `unsigned char` |


### `PROTO_NC_ACT_RIDE_HUNGRY_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hungry` | `unsigned short` |


### `PROTO_NC_ACT_RIDE_OFF_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_ACT_RIDE_ON_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ride` | `CHARBRIEFINFO_RIDE::RideInfo` |


### `PROTO_NC_ACT_ROAR_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `unsigned char` |
|     1 | `len` | `unsigned char` |
|     2 | `content` | `unsigned char[0]   /* 0 bytes */` |


### `PROTO_NC_ACT_SETITEMHEALEFFECT`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `who` | `unsigned short` |
|     2 | `flag` | `PROTO_NC_ACT_SETITEMHEALEFFECT::__unnamed` |
|     3 | `hpchange` | `unsigned short` |


### `PROTO_NC_ACT_SETITEMHEALEFFECT::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isheal` | `<UNRESOLVED:0x1281>` |


### `PROTO_NC_ACT_SHOUT_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `len` | `unsigned char` |
|     1 | `content` | `unsigned char[0]   /* 0 bytes */` |


### `PROTO_NC_ACT_SOMEEONEJUMP_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |


### `PROTO_NC_ACT_SOMEONECHANGEMODE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `mode` | `unsigned char` |


### `PROTO_NC_ACT_SOMEONECHAT_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `len` | `unsigned char` |
|     3 | `flag` | `PROTO_NC_ACT_SOMEONECHAT_CMD::__unnamed` |
|     4 | `content` | `unsigned char[0]   /* 0 bytes */` |


### `PROTO_NC_ACT_SOMEONECHAT_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GMColor` | `<UNRESOLVED:0x1281>` |
|     0 | `chatwin` | `<UNRESOLVED:0x1941>` |


### `PROTO_NC_ACT_SOMEONECOOKING_CASTCUT_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Caster` | `unsigned short` |


### `PROTO_NC_ACT_SOMEONECOOKING_CAST_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Caster` | `unsigned short` |
|     2 | `Food` | `unsigned short` |


### `PROTO_NC_ACT_SOMEONECOOKING_MAKE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Caster` | `unsigned short` |
|     2 | `Food` | `unsigned short` |


### `PROTO_NC_ACT_SOMEONEEMOTICONSTOP_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `emoticon` | `STOPEMOTICON_DESCRIPT` |


### `PROTO_NC_ACT_SOMEONEEMOTICON_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `emoticonid` | `unsigned char` |


### `PROTO_NC_ACT_SOMEONEFOLDTENT_CMD`  (sizeof = 49 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `shape` | `CHARBRIEFINFO_NOTCAMP` |


### `PROTO_NC_ACT_SOMEONEGATHERCANCEL_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `player` | `unsigned short` |
|     2 | `object` | `unsigned short` |


### `PROTO_NC_ACT_SOMEONEGATHERCOMPLETE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `player` | `unsigned short` |
|     2 | `object` | `unsigned short` |


### `PROTO_NC_ACT_SOMEONEGATHERSTART_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `player` | `unsigned short` |
|     2 | `object` | `unsigned short` |
|     4 | `picker` | `unsigned short` |


### `PROTO_NC_ACT_SOMEONEMOVEWALK_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `from` | `SHINE_XY_TYPE` |
|    10 | `to` | `SHINE_XY_TYPE` |
|    18 | `speed` | `unsigned short` |
|    20 | `moveattr` | `PROTO_NC_ACT_SOMEONEMOVEWALK_CMD::__unnamed` |


### `PROTO_NC_ACT_SOMEONEMOVEWALK_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `movedirect` | `<UNRESOLVED:0x1281>` |


### `PROTO_NC_ACT_SOMEONEPITCHTENT_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `camp` | `CHARBRIEFINFO_CAMP` |


### `PROTO_NC_ACT_SOMEONEPRODUCE_CASTCUT_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `caster` | `unsigned short` |


### `PROTO_NC_ACT_SOMEONEPRODUCE_CAST_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `caster` | `unsigned short` |
|     2 | `item` | `unsigned short` |


### `PROTO_NC_ACT_SOMEONEPRODUCE_MAKE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `caster` | `unsigned short` |
|     2 | `item` | `unsigned short` |
|     4 | `err` | `unsigned short` |


### `PROTO_NC_ACT_SOMEONERIDE_OFF_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |


### `PROTO_NC_ACT_SOMEONERIDE_ON_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `ride` | `CHARBRIEFINFO_RIDE::RideInfo` |


### `PROTO_NC_ACT_SOMEONESHOUT_CMD`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `speaker` | `PROTO_NC_ACT_SOMEONESHOUT_CMD::__unnamed` |
|    16 | `flag` | `PROTO_NC_ACT_SOMEONESHOUT_CMD::__unnamed` |
|    17 | `len` | `unsigned char` |
|    18 | `content` | `unsigned char[0]   /* 0 bytes */` |


### `PROTO_NC_ACT_SOMEONESHOUT_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GMColor` | `<UNRESOLVED:0x1281>` |
|     0 | `isMob` | `<UNRESOLVED:0x1941>` |


### `PROTO_NC_ACT_SOMEONESTOP_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `loc` | `SHINE_XY_TYPE` |


### `PROTO_NC_ACT_SOMEONEWALK_CMD`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `from` | `SHINE_XY_TYPE` |
|    10 | `to` | `SHINE_XY_TYPE` |


### `PROTO_NC_ACT_STOP_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `loc` | `SHINE_XY_TYPE` |


### `PROTO_NC_ACT_WALK_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `from` | `SHINE_XY_TYPE` |
|     8 | `to` | `SHINE_XY_TYPE` |


### `PROTO_NC_ACT_WEDDING_COUPLE_ENTRANCE_RNG`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `groom` | `unsigned long` |
|    11 | `bride` | `unsigned long` |
|    15 | `RecallCount` | `unsigned char` |


### `PROTO_NC_ACT_WEDDING_HALL_GUEST_ENTER_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_ACT_WEDDING_HALL_GUEST_ENTER_READY_ACK`  (sizeof = 166 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `GroomID` | `Name4` |
|    18 | `BrideID` | `Name4` |
|    34 | `EnterStart` | `__int64` |
|    42 | `WeddingStart` | `__int64` |
|    50 | `WeddingEnd` | `__int64` |
|    58 | `tm_EnterStart` | `tm` |
|    94 | `tm_WeddingStart` | `tm` |
|   130 | `tm_WeddingEnd` | `tm` |


### `PROTO_NC_ACT_WEDDING_HALL_RESERV_ACK`  (sizeof = 46 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `ReservedTime` | `__int64` |
|    10 | `tm_ReservedTime` | `tm` |


### `PROTO_NC_ACT_WEDDING_PROPOSEACK_ACK`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `from` | `unsigned short` |
|     2 | `reply` | `unsigned char` |
|     3 | `response_word` | `char[37]   /* 37 bytes */` |


### `PROTO_NC_ACT_WEDDING_PROPOSEACK_REQ`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `from` | `unsigned short` |
|     2 | `propose_word` | `char[37]   /* 37 bytes */` |


### `PROTO_NC_ACT_WEDDING_PROPOSEREQ_ACK`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `to` | `unsigned short` |
|     2 | `err` | `unsigned short` |
|     4 | `response_word` | `char[37]   /* 37 bytes */` |


### `PROTO_NC_ACT_WEDDING_PROPOSEREQ_REQ`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `to` | `unsigned short` |
|     2 | `propose_word` | `char[37]   /* 37 bytes */` |


### `PROTO_NC_ANNOUNCE_Z2W_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `AnnounceMessage` | `ANNOUNCE_MESSAGE` |


### `PROTO_NC_AUCTION_BUY_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nAuctionNo` | `int` |
|     6 | `nBuyPrice` | `unsigned __int64` |
|    14 | `nItemID` | `unsigned short` |


### `PROTO_NC_AUCTION_BUY_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nAuctionNo` | `int` |


### `PROTO_NC_AUCTION_DB_BUY_ACK`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nError` | `unsigned short` |
|     8 | `nAuctionNo` | `int` |
|    12 | `nSellerCharNo` | `unsigned long` |
|    16 | `nBuyPrice` | `unsigned __int64` |
|    24 | `nItemID` | `unsigned short` |


### `PROTO_NC_AUCTION_DB_BUY_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nAuctionNo` | `int` |
|    10 | `nSellPriceRate` | `unsigned short` |
|    12 | `nInventoryTerm` | `unsigned short` |


### `PROTO_NC_AUCTION_DB_MY_REGISTER_GOOD_LIST_ACK`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nError` | `unsigned short` |
|     8 | `nPartMask` | `PARTMARK` |
|     9 | `nMRGLItemNum` | `unsigned short` |
|    11 | `nMRGLItemInfoSize` | `unsigned short` |
|    13 | `MRGLItemInfo` | `char[0]   /* 0 bytes */` |


### `PROTO_NC_AUCTION_DB_MY_REGISTER_GOOD_LIST_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |


### `PROTO_NC_AUCTION_DB_MY_STORAGE_BOX_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nError` | `unsigned short` |
|     8 | `nMSBItemNum` | `unsigned short` |
|    10 | `MSBItemInfo` | `StorageBoxInfo[?] /* 0 bytes */` |


### `PROTO_NC_AUCTION_DB_MY_STORAGE_BOX_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |


### `PROTO_NC_AUCTION_DB_MY_STORAGE_ITEM_DETAIL_INFO_FAIL_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nError` | `unsigned short` |
|     8 | `nAuctionNo` | `int` |


### `PROTO_NC_AUCTION_DB_MY_STORAGE_ITEM_DETAIL_INFO_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nAuctionNo` | `int` |


### `PROTO_NC_AUCTION_DB_MY_STORAGE_ITEM_DETAIL_INFO_SUC_ACK`  (sizeof = 110 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nAuctionNo` | `int` |
|    10 | `nInventoryType` | `AuctionStorageCategory` |
|    14 | `dStartDate` | `tm` |
|    50 | `sSellerID` | `Name4` |
|    66 | `sBidderID` | `Name4` |
|    82 | `nSellPrice` | `unsigned __int64` |
|    90 | `nSellCharge` | `unsigned __int64` |
|    98 | `nMoney` | `unsigned __int64` |
|   106 | `nItemAttrSize` | `unsigned short` |
|   108 | `ItemInfoData` | `SHINE_ITEM_VAR_STRUCT` |


### `PROTO_NC_AUCTION_DB_REGISTER_CANCEL_GOOD_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nError` | `unsigned short` |
|     8 | `nAuctionNo` | `int` |


### `PROTO_NC_AUCTION_DB_REGISTER_CANCEL_GOOD_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nError` | `unsigned short` |
|     8 | `nAuctionNo` | `int` |
|    12 | `nInventoryTerm` | `unsigned short` |


### `PROTO_NC_AUCTION_DB_REGISTER_GOOD_ACK`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nLockIndex` | `unsigned short` |
|     8 | `nInvenSlot` | `unsigned char` |
|     9 | `nError` | `unsigned short` |
|    11 | `nAuctionNo` | `int` |


### `PROTO_NC_AUCTION_DB_REGISTER_GOOD_REQ`  (sizeof = 109 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nLockIndex` | `unsigned short` |
|     8 | `nInvenSlot` | `unsigned char` |
|     9 | `nEnrollLimit` | `unsigned char` |
|    10 | `nItemID` | `unsigned short` |
|    12 | `sItemName` | `char[65]   /* 65 bytes */` |
|    77 | `nItemLevel` | `unsigned short` |
|    79 | `nItemGrade` | `unsigned char` |
|    80 | `nCategoryNo` | `unsigned char` |
|    81 | `nRegistration` | `unsigned __int64` |
|    89 | `dExpirationDate` | `int` |
|    93 | `nSellPrice` | `unsigned __int64` |
|   101 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |


### `PROTO_NC_AUCTION_DB_SEARCH_ACK`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nError` | `unsigned short` |
|     8 | `nPartMask` | `PARTMARK` |
|     9 | `nSearchOptionItemCnt` | `unsigned long` |
|    13 | `nSItemNum` | `unsigned short` |
|    15 | `nSItemInfoSize` | `unsigned short` |
|    17 | `SItemInfo` | `char[0]   /* 0 bytes */` |


### `PROTO_NC_AUCTION_DB_SEARCH_REQ`  (sizeof = 99 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nCategory` | `AuctionGroup` |
|    10 | `nMinLevel` | `unsigned char` |
|    11 | `nMaxLevel` | `unsigned char` |
|    12 | `nMinGrade` | `GradeType` |
|    16 | `nMaxGrade` | `GradeType` |
|    20 | `sItemName` | `char[67]   /* 67 bytes */` |
|    87 | `nSortNo` | `AuctionSortType` |
|    91 | `bSortType` | `AuctionScendingSortType` |
|    95 | `nPageNo` | `unsigned short` |
|    97 | `nPageLimit` | `unsigned short` |


### `PROTO_NC_AUCTION_DB_STORAGE_BOX_WITHDRAW_ACK`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nLockIndex` | `unsigned short` |
|     8 | `nAuctionNo` | `int` |
|    12 | `bIsItem` | `unsigned char` |
|    13 | `nSlotNo` | `ITEM_INVEN` |
|    15 | `nError` | `unsigned short` |
|    17 | `nWithDrawMoney` | `unsigned __int64` |
|    25 | `WithdrawItemInfo` | `ItemTotalInformation[?] /* 0 bytes */` |


### `PROTO_NC_AUCTION_DB_STORAGE_BOX_WITHDRAW_REQ`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nLockIndex` | `unsigned short` |
|     8 | `nAuctionNo` | `int` |
|    12 | `bIsItem` | `unsigned char` |
|    13 | `nSlotNo` | `ITEM_INVEN` |


### `PROTO_NC_AUCTION_MY_REGISTER_GOOD_LIST_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nPartMask` | `PARTMARK` |
|     3 | `nMRGLItemNum` | `unsigned short` |
|     5 | `nMRGLItemInfoSize` | `unsigned short` |
|     7 | `MRGLItemInfo` | `char[0]   /* 0 bytes */` |


### `PROTO_NC_AUCTION_MY_REGISTER_GOOD_LIST_REQ`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_AUCTION_MY_STORAGE_BOX_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nMSBItemNum` | `unsigned short` |
|     4 | `MSBItemInfo` | `StorageBoxInfo[?] /* 0 bytes */` |


### `PROTO_NC_AUCTION_MY_STORAGE_BOX_REQ`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_AUCTION_MY_STORAGE_ITEM_DETAIL_INFO_FAIL_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nAuctionNo` | `int` |


### `PROTO_NC_AUCTION_MY_STORAGE_ITEM_DETAIL_INFO_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nAuctionNo` | `int` |


### `PROTO_NC_AUCTION_MY_STORAGE_ITEM_DETAIL_INFO_SUC_ACK`  (sizeof = 100 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nInventoryType` | `AuctionStorageCategory` |
|     4 | `dStartDate` | `tm` |
|    40 | `sSellerID` | `Name4` |
|    56 | `sBidderID` | `Name4` |
|    72 | `nSellPrice` | `unsigned __int64` |
|    80 | `nSellCharge` | `unsigned __int64` |
|    88 | `nMoney` | `unsigned __int64` |
|    96 | `nItemAttrSize` | `unsigned short` |
|    98 | `ItemInfoData` | `SHINE_ITEM_VAR_STRUCT` |


### `PROTO_NC_AUCTION_NOTIFY_STORAGE_BOX_EXIST_ITEM_CMD`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_AUCTION_NOTIFY_WORLD_SELL_ITEM_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nSellerCharNo` | `unsigned long` |
|     4 | `nAuctionNo` | `int` |
|     8 | `nItemID` | `unsigned short` |


### `PROTO_NC_AUCTION_REGISTER_CANCEL_GOOD_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nAuctionNo` | `int` |


### `PROTO_NC_AUCTION_REGISTER_CANCEL_GOOD_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nAuctionNo` | `int` |


### `PROTO_NC_AUCTION_REGISTER_GOOD_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nInvenSlot` | `unsigned char` |
|     1 | `nError` | `unsigned short` |
|     3 | `nAuctionNo` | `int` |


### `PROTO_NC_AUCTION_REGISTER_GOOD_REQ`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nInvenSlot` | `unsigned char` |
|     1 | `nEnrollPeriodType` | `AuctionEnrollPeriodType` |
|     5 | `nSellPrice` | `unsigned __int64` |


### `PROTO_NC_AUCTION_SEARCH_ACK`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nPartMask` | `PARTMARK` |
|     3 | `nSearchOptionItemCnt` | `unsigned long` |
|     7 | `nSItemNum` | `unsigned short` |
|     9 | `nSItemInfoSize` | `unsigned short` |
|    11 | `SItemInfo` | `char[0]   /* 0 bytes */` |


### `PROTO_NC_AUCTION_SEARCH_REQ`  (sizeof = 91 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCategory` | `AuctionGroup` |
|     4 | `nMinLevel` | `unsigned char` |
|     5 | `nMaxLevel` | `unsigned char` |
|     6 | `nMinGrade` | `GradeType` |
|    10 | `nMaxGrade` | `GradeType` |
|    14 | `sItemName` | `char[67]   /* 67 bytes */` |
|    81 | `nSortNo` | `AuctionSortType` |
|    85 | `bSortType` | `AuctionScendingSortType` |
|    89 | `nPageNo` | `unsigned short` |


### `PROTO_NC_AUCTION_STORAGE_BOX_WITHDRAW_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nAuctionNo` | `int` |
|     4 | `nError` | `unsigned short` |


### `PROTO_NC_AUCTION_STORAGE_BOX_WITHDRAW_REQ`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nAuctionNo` | `int` |
|     4 | `bIsItem` | `unsigned char` |


### `PROTO_NC_BAT_ABSTATEINFORM_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `abstate` | `ABSTATEINDEX` |
|     4 | `keeptime_millisec` | `unsigned long` |


### `PROTO_NC_BAT_ABSTATERESET_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `abstate` | `ABSTATEINDEX` |


### `PROTO_NC_BAT_ABSTATESET_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `abstate` | `ABSTATEINDEX` |


### `PROTO_NC_BAT_ABSTATE_ERASE_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_BAT_ABSTATE_ERASE_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `abstateIndex` | `unsigned short` |


### `PROTO_NC_BAT_AREADOTDAMAGE_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `abstate` | `unsigned short` |
|     2 | `Centerobject` | `unsigned short` |
|     4 | `num` | `unsigned char` |
|     5 | `target` | `PROTO_NC_BAT_AREADOTDAMAGE_CMD::targetinfo[?] /* 0 bytes */` |


### `PROTO_NC_BAT_AREADOTDAMAGE_CMD::targetinfo`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `object` | `unsigned short` |
|     2 | `resthp` | `unsigned long` |
|     6 | `damage` | `unsigned long` |
|    10 | `hpchangeorder` | `unsigned short` |


### `PROTO_NC_BAT_ASSIST_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `target` | `unsigned short` |


### `PROTO_NC_BAT_ASSIST_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `tanker` | `unsigned short` |


### `PROTO_NC_BAT_BASH_HITTED_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attacker` | `unsigned short` |
|     2 | `damage` | `PROTO_NC_BAT_BASH_HITTED_CMD::__unnamed` |
|     4 | `flag` | `PROTO_NC_BAT_BASH_HITTED_CMD::__unnamed` |
|     5 | `resthp` | `unsigned long` |
|     9 | `attackspeed` | `unsigned short` |
|    11 | `hpchangeorder` | `unsigned short` |


### `PROTO_NC_BAT_BASH_HITTED_CMD::__unnamed`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `iscritical` | `<UNRESOLVED:0x400c>` |
|     0 | `damage` | `<UNRESOLVED:0xe46f>` |


### `PROTO_NC_BAT_BASH_HIT_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `target` | `unsigned short` |
|     2 | `damage` | `PROTO_NC_BAT_BASH_HIT_CMD::__unnamed` |
|     4 | `flag` | `PROTO_NC_BAT_BASH_HIT_CMD::__unnamed` |
|     5 | `resthp` | `unsigned long` |
|     9 | `attackspeed` | `unsigned short` |
|    11 | `hpchangeorder` | `unsigned short` |


### `PROTO_NC_BAT_BASH_HIT_CMD::__unnamed`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `iscritical` | `<UNRESOLVED:0x400c>` |
|     0 | `damage` | `<UNRESOLVED:0xe46f>` |


### `PROTO_NC_BAT_BASH_MISSED_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attacker` | `unsigned short` |
|     2 | `flag` | `PROTO_NC_BAT_BASH_MISSED_CMD::__unnamed` |
|     3 | `attackspeed` | `unsigned short` |


### `PROTO_NC_BAT_BASH_MISSED_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `actioncode` | `<UNRESOLVED:0x11bf>` |
|     0 | `ismissed` | `<UNRESOLVED:0x38c8>` |
|     0 | `isshieldblock` | `<UNRESOLVED:0x38c9>` |
|     0 | `isresist` | `<UNRESOLVED:0x38ca>` |
|     0 | `isImmune` | `<UNRESOLVED:0x307e>` |


### `PROTO_NC_BAT_BASH_MISS_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `target` | `unsigned short` |
|     2 | `flag` | `PROTO_NC_BAT_BASH_MISS_CMD::__unnamed` |
|     3 | `attackspeed` | `unsigned short` |


### `PROTO_NC_BAT_BASH_MISS_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `actioncode` | `<UNRESOLVED:0x11bf>` |
|     0 | `ismissed` | `<UNRESOLVED:0x38c8>` |
|     0 | `isshieldblock` | `<UNRESOLVED:0x38c9>` |
|     0 | `isImmune` | `<UNRESOLVED:0x38ca>` |


### `PROTO_NC_BAT_CEASE_FIRE_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |


### `PROTO_NC_BAT_DOTDAMAGE_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `object` | `unsigned short` |
|     2 | `resthp` | `unsigned long` |
|     6 | `damage` | `unsigned short` |
|     8 | `abstate` | `unsigned short` |
|    10 | `hpchangeorder` | `unsigned short` |
|    12 | `IsMissDamage` | `unsigned char` |


### `PROTO_NC_BAT_EXPGAIN_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `expgain` | `unsigned long` |
|     4 | `mobhandle` | `unsigned short` |


### `PROTO_NC_BAT_EXPLOST_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `explost` | `unsigned long` |


### `PROTO_NC_BAT_FAMEGAIN_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `famegain` | `unsigned long` |


### `PROTO_NC_BAT_HIT_REQ`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `target` | `unsigned short` |
|     2 | `actionnumber` | `unsigned char` |


### `PROTO_NC_BAT_HPCHANGE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hp` | `unsigned long` |
|     4 | `hpchangeorder` | `unsigned short` |


### `PROTO_NC_BAT_LEVELUP_CMD`  (sizeof = 231 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `level` | `unsigned char` |
|     1 | `mobhandle` | `unsigned short` |
|     3 | `newparam` | `CHAR_PARAMETER_DATA` |


### `PROTO_NC_BAT_MOBSLAYER_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `killedmob` | `unsigned short` |
|     2 | `number` | `unsigned char` |
|     3 | `slayers` | `unsigned long[0]   /* 0 bytes */` |


### `PROTO_NC_BAT_PKINPKFIELD_CLIENT_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attacker` | `unsigned short` |
|     2 | `defender` | `unsigned short` |
|     4 | `pkcount` | `unsigned long` |


### `PROTO_NC_BAT_PKINPKFIELD_WMS_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `chrregnum` | `unsigned long` |
|     6 | `pkcount` | `unsigned long` |


### `PROTO_NC_BAT_REALLYKILL_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `dead` | `unsigned short` |
|     2 | `attacker` | `unsigned short` |


### `PROTO_NC_BAT_REFLECTIONDAMAGE_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `abstate` | `unsigned short` |
|     2 | `ReflectionObject` | `unsigned short` |
|     4 | `TargetObject` | `unsigned short` |
|     6 | `resthp` | `unsigned long` |
|    10 | `damage` | `unsigned short` |
|    12 | `hpchangeorder` | `unsigned short` |


### `PROTO_NC_BAT_SKILLBASH_FLD_CAST_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `locate` | `SHINE_XY_TYPE` |


### `PROTO_NC_BAT_SKILLBASH_HIT_BLAST_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `index` | `unsigned short` |
|     2 | `caster` | `unsigned short` |
|     4 | `nSkillID` | `unsigned short` |


### `PROTO_NC_BAT_SKILLBASH_HIT_DAMAGE_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `index` | `unsigned short` |
|     2 | `caster` | `unsigned short` |
|     4 | `targetnum` | `unsigned char` |
|     5 | `target` | `PROTO_NC_BAT_SKILLBASH_HIT_DAMAGE_CMD::SkillDamage[?] /* 0 bytes */` |


### `PROTO_NC_BAT_SKILLBASH_HIT_DAMAGE_CMD::SkillDamage`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `flag` | `PROTO_NC_BAT_SKILLBASH_HIT_DAMAGE_CMD::SkillDamage::__unnamed` |
|     4 | `hpchange` | `unsigned long` |
|     8 | `resthp` | `unsigned long` |
|    12 | `hpchangeorder` | `unsigned short` |


### `PROTO_NC_BAT_SKILLBASH_HIT_DAMAGE_CMD::SkillDamage::__unnamed`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isdamage` | `<UNRESOLVED:0x1281>` |
|     0 | `iscritical` | `<UNRESOLVED:0x1941>` |
|     0 | `ismissed` | `<UNRESOLVED:0x38c6>` |
|     0 | `isshieldblock` | `<UNRESOLVED:0x38c7>` |
|     0 | `isheal` | `<UNRESOLVED:0x38c8>` |
|     0 | `isenchant` | `<UNRESOLVED:0x38c9>` |
|     0 | `isresist` | `<UNRESOLVED:0x38ca>` |
|     0 | `IsCostumWeapon` | `<UNRESOLVED:0x307e>` |
|     1 | `isDead` | `<UNRESOLVED:0x1281>` |
|     1 | `isImmune` | `<UNRESOLVED:0x1941>` |


### `PROTO_NC_BAT_SKILLBASH_HIT_FLD_START_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `targetloc` | `SHINE_XY_TYPE` |
|    10 | `index` | `unsigned short` |


### `PROTO_NC_BAT_SKILLBASH_HIT_OBJ_START_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `targetobj` | `unsigned short` |
|     4 | `index` | `unsigned short` |


### `PROTO_NC_BAT_SKILLBASH_OBJ_CAST_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `target` | `unsigned short` |


### `PROTO_NC_BAT_SKILLBLAST_LIGHTNINGWAVE_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nBIndex` | `unsigned short` |
|     2 | `nSkillID` | `unsigned short` |
|     4 | `nShotHnd` | `unsigned short` |
|     6 | `nTargetHnd` | `unsigned short` |


### `PROTO_NC_BAT_SKILLCAST_FAIL_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attackcode` | `unsigned char` |


### `PROTO_NC_BAT_SKILLCAST_REQ`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `target` | `unsigned short` |
|     4 | `attackcode` | `unsigned char` |


### `PROTO_NC_BAT_SKILLCAST_SUC_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attackcode` | `unsigned char` |


### `PROTO_NC_BAT_SKILLENCHANT_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skillid` | `unsigned short` |
|     2 | `target` | `unsigned short` |


### `PROTO_NC_BAT_SKILLSMASH_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `target` | `unsigned short` |
|     4 | `attackcode` | `unsigned char` |


### `PROTO_NC_BAT_SKILLSMASH_HITTED_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attack` | `unsigned short` |
|     2 | `hitted` | `PROTO_NC_BAT_SKILLSMASH_HIT_CMD` |


### `PROTO_NC_BAT_SKILLSMASH_HIT_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `target` | `unsigned short` |
|     2 | `skill` | `unsigned short` |
|     4 | `damage` | `PROTO_NC_BAT_SKILLSMASH_HIT_CMD::__unnamed` |
|     6 | `attackcode` | `unsigned char` |


### `PROTO_NC_BAT_SKILLSMASH_HIT_CMD::__unnamed`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isdead` | `<UNRESOLVED:0x400c>` |
|     0 | `iscritical` | `<UNRESOLVED:0xe45f>` |
|     0 | `damage` | `<UNRESOLVED:0xe460>` |


### `PROTO_NC_BAT_SKILLSMASH_MISS_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `target` | `unsigned short` |
|     2 | `attackcode` | `unsigned char` |


### `PROTO_NC_BAT_SMASH_HITTED_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attack` | `unsigned short` |
|     2 | `hitted` | `PROTO_NC_BAT_SMASH_HITTED_ACK::__unnamed` |


### `PROTO_NC_BAT_SMASH_HITTED_ACK::__unnamed`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `damage` | `PROTO_NC_BAT_SMASH_HITTED_ACK::__unnamed::__unnamed` |
|     2 | `attackcode` | `unsigned char` |


### `PROTO_NC_BAT_SMASH_HITTED_ACK::__unnamed::__unnamed`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isdead` | `<UNRESOLVED:0x400c>` |
|     0 | `iscritical` | `<UNRESOLVED:0xe45f>` |
|     0 | `damage` | `<UNRESOLVED:0xe460>` |


### `PROTO_NC_BAT_SMASH_HIT_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `target` | `unsigned short` |
|     2 | `damage` | `PROTO_NC_BAT_SMASH_HIT_ACK::__unnamed` |
|     4 | `attackcode` | `unsigned char` |


### `PROTO_NC_BAT_SMASH_HIT_ACK::__unnamed`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isdead` | `<UNRESOLVED:0x400c>` |
|     0 | `iscritical` | `<UNRESOLVED:0xe45f>` |
|     0 | `damage` | `<UNRESOLVED:0xe460>` |


### `PROTO_NC_BAT_SMASH_MISS_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `target` | `unsigned short` |
|     2 | `attackcode` | `unsigned char` |


### `PROTO_NC_BAT_SMASH_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attackcode` | `unsigned char` |
|     1 | `actionnumber` | `unsigned char` |


### `PROTO_NC_BAT_SOMEONEBASH_HIT_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attacker` | `unsigned short` |
|     2 | `target` | `unsigned short` |
|     4 | `actioncode` | `unsigned char` |
|     5 | `resthp` | `unsigned long` |
|     9 | `attackspeed` | `unsigned short` |
|    11 | `hpchangeorder` | `unsigned short` |


### `PROTO_NC_BAT_SOMEONEBASH_MISS_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attacker` | `unsigned short` |
|     2 | `target` | `unsigned short` |
|     4 | `actioncode` | `unsigned char` |
|     5 | `attackspeed` | `unsigned short` |


### `PROTO_NC_BAT_SOMEONEDAMAGED_LARGE_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attack` | `unsigned short` |
|     2 | `target` | `unsigned short` |
|     4 | `damage` | `unsigned short` |
|     6 | `actionnumber` | `unsigned char` |


### `PROTO_NC_BAT_SOMEONEDAMAGED_SMALL_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attack` | `unsigned short` |
|     2 | `target` | `unsigned short` |
|     4 | `damage` | `unsigned char` |
|     5 | `actionnumber` | `unsigned char` |


### `PROTO_NC_BAT_SOMEONESKILLBASH_CASTCUT_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `caster` | `unsigned short` |


### `PROTO_NC_BAT_SOMEONESKILLBASH_FLD_CAST_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `caster` | `unsigned short` |
|     4 | `loc` | `SHINE_XY_TYPE` |


### `PROTO_NC_BAT_SOMEONESKILLBASH_HIT_FLD_START_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `caster` | `unsigned short` |
|     2 | `castinfo` | `PROTO_NC_BAT_SKILLBASH_HIT_FLD_START_CMD` |


### `PROTO_NC_BAT_SOMEONESKILLBASH_HIT_OBJ_START_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `caster` | `unsigned short` |
|     2 | `castinfo` | `PROTO_NC_BAT_SKILLBASH_HIT_OBJ_START_CMD` |


### `PROTO_NC_BAT_SOMEONESKILLBASH_OBJ_CAST_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `caster` | `unsigned short` |
|     4 | `target` | `unsigned short` |


### `PROTO_NC_BAT_SOMEONESKILLCAST_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `caster` | `unsigned short` |
|     4 | `target` | `unsigned short` |


### `PROTO_NC_BAT_SOMEONESKILLENCHANT_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attacker` | `unsigned short` |
|     2 | `skillenchant` | `PROTO_NC_BAT_SKILLENCHANT_REQ` |
|     6 | `damage` | `unsigned short` |


### `PROTO_NC_BAT_SOMEONESKILLSMASH_DEAD_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `attack` | `unsigned short` |
|     4 | `target` | `unsigned short` |


### `PROTO_NC_BAT_SOMEONESMASH_DAMAGED_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attack` | `unsigned short` |
|     2 | `target` | `unsigned short` |
|     4 | `actionnumber` | `unsigned char` |


### `PROTO_NC_BAT_SOMEONESWING_DAMAGE_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attacker` | `unsigned short` |
|     2 | `defender` | `unsigned short` |
|     4 | `flag` | `PROTO_NC_BAT_SOMEONESWING_DAMAGE_CMD::__unnamed` |
|     5 | `resthp` | `unsigned long` |
|     9 | `hpchangeorder` | `unsigned short` |
|    11 | `damageindex` | `unsigned char` |
|    12 | `attacksequence` | `unsigned char` |


### `PROTO_NC_BAT_SOMEONESWING_DAMAGE_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isMissed` | `<UNRESOLVED:0x1281>` |
|     0 | `isCostumCharged` | `<UNRESOLVED:0x1941>` |


### `PROTO_NC_BAT_SOULCOLLECT_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `target` | `unsigned short` |
|     2 | `soulnumber` | `unsigned char` |


### `PROTO_NC_BAT_SPCHANGE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sp` | `unsigned long` |


### `PROTO_NC_BAT_SUMEONELEVELUP_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ply` | `unsigned short` |
|     2 | `mobhandle` | `unsigned short` |


### `PROTO_NC_BAT_SUMEONESKILLCUT_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `caster` | `unsigned short` |


### `PROTO_NC_BAT_SWING_DAMAGE_CMD`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attacker` | `unsigned short` |
|     2 | `defender` | `unsigned short` |
|     4 | `flag` | `PROTO_NC_BAT_SWING_DAMAGE_CMD::__unnamed` |
|     5 | `damage` | `unsigned short` |
|     7 | `resthp` | `unsigned long` |
|    11 | `hpchangeorder` | `unsigned short` |
|    13 | `damageindex` | `unsigned char` |
|    14 | `attacksequence` | `unsigned char` |


### `PROTO_NC_BAT_SWING_DAMAGE_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `iscritical` | `<UNRESOLVED:0x1281>` |
|     0 | `isresist` | `<UNRESOLVED:0x1941>` |
|     0 | `ismissed` | `<UNRESOLVED:0x38c6>` |
|     0 | `isshieldblock` | `<UNRESOLVED:0x38c7>` |
|     0 | `isCostumCharged` | `<UNRESOLVED:0x38c8>` |
|     0 | `isDead` | `<UNRESOLVED:0x38c9>` |
|     0 | `isDamege2Heal` | `<UNRESOLVED:0x38ca>` |
|     0 | `isImmune` | `<UNRESOLVED:0x307e>` |


### `PROTO_NC_BAT_SWING_START_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attacker` | `unsigned short` |
|     2 | `defender` | `unsigned short` |
|     4 | `actioncode` | `unsigned char` |
|     5 | `attackspeed` | `unsigned short` |
|     7 | `damageindex` | `unsigned char` |
|     8 | `attacksequence` | `unsigned char` |


### `PROTO_NC_BAT_TARGETCHANGE_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `target` | `unsigned short` |


### `PROTO_NC_BAT_TARGETINFO_CMD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `flag` | `PROTO_NC_BAT_TARGETINFO_CMD::__unnamed` |
|     1 | `targethandle` | `unsigned short` |
|     3 | `targethp` | `unsigned long` |
|     7 | `targetmaxhp` | `unsigned long` |
|    11 | `targetsp` | `unsigned long` |
|    15 | `targetmaxsp` | `unsigned long` |
|    19 | `targetlevel` | `unsigned char` |
|    20 | `hpchangeorder` | `unsigned short` |


### `PROTO_NC_BAT_TARGETINFO_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `order` | `<UNRESOLVED:0x11bf>` |
|     0 | `inguildwar` | `<UNRESOLVED:0x38c8>` |


### `PROTO_NC_BAT_TARGET_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `target` | `unsigned short` |


### `PROTO_NC_BAT_TOGGLESKILL_OFF_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nSkillID` | `unsigned short` |


### `PROTO_NC_BAT_TOGGLESKILL_ON_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nSkillID` | `unsigned short` |


### `PROTO_NC_BAT_WORLD_MOB_KILL_ANNOUNCE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTextIndex` | `MobKillAnnounceType` |


### `PROTO_NC_BOOTH_BUYREFRESH_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `boothslot` | `unsigned char` |
|     1 | `restlot` | `unsigned short` |


### `PROTO_NC_BOOTH_ENTRY_BUY_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `boothhandle` | `unsigned short` |
|     4 | `numofitem` | `unsigned char` |
|     5 | `items` | `PROTO_NC_BOOTH_ENTRY_BUY_ACK::BoothItemList[?] /* 0 bytes */` |


### `PROTO_NC_BOOTH_ENTRY_BUY_ACK::BoothItemList`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `boothslot` | `unsigned char` |
|     1 | `item` | `unsigned short` |
|     3 | `unitcost` | `unsigned __int64` |
|    11 | `restquantity` | `unsigned short` |


### `PROTO_NC_BOOTH_ENTRY_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `booth` | `unsigned short` |


### `PROTO_NC_BOOTH_ENTRY_SELL_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `boothhandle` | `unsigned short` |
|     4 | `numofitem` | `unsigned char` |
|     5 | `items` | `PROTO_NC_BOOTH_ENTRY_SELL_ACK::BoothItemList[?] /* 0 bytes */` |


### `PROTO_NC_BOOTH_ENTRY_SELL_ACK::BoothItemList`  (sizeof = 113 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `datasize` | `unsigned char` |
|     1 | `boothslot` | `unsigned char` |
|     2 | `unitcost` | `unsigned __int64` |
|    10 | `item` | `SHINE_ITEM_STRUCT` |


### `PROTO_NC_BOOTH_ITEMTRADE_REQ`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `booth` | `unsigned short` |
|     2 | `slot` | `unsigned char` |
|     3 | `itemnum` | `unsigned short` |


### `PROTO_NC_BOOTH_OPEN_REQ`  (sizeof = 31 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `signboard` | `STREETBOOTH_SIGNBOARD` |
|    30 | `flag` | `PROTO_NC_BOOTH_OPEN_REQ::__unnamed` |
|    31 | `items` | `PROTO_NC_BOOTH_OPEN_REQ::BoothItem[?] /* 0 bytes */` |


### `PROTO_NC_BOOTH_OPEN_REQ::BoothItem`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `invenslot` | `unsigned char` |
|     1 | `boardslot` | `unsigned char` |
|     2 | `unitcost` | `unsigned __int64` |
|    10 | `totallot` | `unsigned short` |


### `PROTO_NC_BOOTH_OPEN_REQ::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `issell` | `<UNRESOLVED:0x1281>` |
|     0 | `itemnum` | `<UNRESOLVED:0x1282>` |


### `PROTO_NC_BOOTH_REFRESH_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `booth` | `unsigned short` |


### `PROTO_NC_BOOTH_SOMEONECLOSE_CMD`  (sizeof = 49 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `shape` | `CHARBRIEFINFO_NOTCAMP` |


### `PROTO_NC_BOOTH_SOMEONEINTERIORSTART_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `boothowner` | `unsigned short` |
|     2 | `signboard` | `STREETBOOTH_SIGNBOARD` |


### `PROTO_NC_BOOTH_SOMEONEOPEN_CMD`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `tent` | `CHARBRIEFINFO_CAMP` |
|    13 | `issell` | `unsigned char` |
|    14 | `signboard` | `STREETBOOTH_SIGNBOARD` |


### `PROTO_NC_BRIEFINFO_ABSTATE_CHANGE_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `info` | `ABSTATE_INFORMATION` |


### `PROTO_NC_BRIEFINFO_ABSTATE_CHANGE_LIST_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `count` | `unsigned char` |
|     3 | `infoList` | `ABSTATE_INFORMATION[?] /* 0 bytes */` |


### `PROTO_NC_BRIEFINFO_BRIEFINFODELETE_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hnd` | `unsigned short` |


### `PROTO_NC_BRIEFINFO_BUILDDOOR_CMD`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `mobid` | `unsigned short` |
|     4 | `coord` | `SHINE_COORD_TYPE` |
|    13 | `doorstate` | `unsigned char` |
|    14 | `blockindex` | `Name8` |
|    46 | `scale` | `unsigned short` |


### `PROTO_NC_BRIEFINFO_CHANGEDECORATE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `item` | `unsigned short` |


### `PROTO_NC_BRIEFINFO_CHANGEUPGRADE_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `item` | `unsigned short` |
|     4 | `upgrade` | `unsigned char` |


### `PROTO_NC_BRIEFINFO_CHANGEWEAPON_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `upgradeinfo` | `PROTO_NC_BRIEFINFO_CHANGEUPGRADE_CMD` |
|     5 | `currentmobid` | `unsigned short` |
|     7 | `currentkilllevel` | `unsigned char` |


### `PROTO_NC_BRIEFINFO_CHANGE_DISPOSITION_CMD`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `DispositionInfo` | `DISPOSITION_BRIEFINFO` |


### `PROTO_NC_BRIEFINFO_CHARACTER_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charnum` | `unsigned char` |
|     1 | `chars` | `PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD[?] /* 0 bytes */` |


### `PROTO_NC_BRIEFINFO_DOOR_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `doornum` | `unsigned char` |
|     1 | `doors` | `PROTO_NC_BRIEFINFO_BUILDDOOR_CMD[?] /* 0 bytes */` |


### `PROTO_NC_BRIEFINFO_DROPEDITEM_CMD`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `itemid` | `unsigned short` |
|     4 | `location` | `SHINE_XY_TYPE` |
|    12 | `dropmobhandle` | `unsigned short` |
|    14 | `attr` | `PROTO_NC_BRIEFINFO_DROPEDITEM_CMD::__unnamed` |


### `PROTO_NC_BRIEFINFO_DROPEDITEM_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `rareness` | `<UNRESOLVED:0x5723>` |
|     0 | `looting` | `<UNRESOLVED:0x38c7>` |


### `PROTO_NC_BRIEFINFO_EFFECTBLAST_CMD`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `effectname` | `Name8` |
|    34 | `coord` | `SHINE_COORD_TYPE` |
|    43 | `detach` | `unsigned short` |
|    45 | `scale` | `unsigned short` |
|    47 | `flag` | `PROTO_NC_BRIEFINFO_EFFECTBLAST_CMD::__unnamed` |


### `PROTO_NC_BRIEFINFO_EFFECTBLAST_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `loop` | `<UNRESOLVED:0x1281>` |


### `PROTO_NC_BRIEFINFO_EFFECT_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `effectnum` | `unsigned char` |
|     1 | `effects` | `PROTO_NC_BRIEFINFO_EFFECTBLAST_CMD[?] /* 0 bytes */` |


### `PROTO_NC_BRIEFINFO_INFORM_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nMyHnd` | `unsigned short` |
|     2 | `ReceiveNetCommand` | `NETCOMMAND` |
|     4 | `hnd` | `unsigned short` |


### `PROTO_NC_BRIEFINFO_ITEMONFIELD_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemnum` | `unsigned char` |
|     1 | `items` | `PROTO_NC_BRIEFINFO_DROPEDITEM_CMD[?] /* 0 bytes */` |


### `PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD`  (sizeof = 221 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `charid` | `Name4` |
|    18 | `coord` | `SHINE_COORD_TYPE` |
|    27 | `mode` | `unsigned char` |
|    28 | `chrclass` | `unsigned char` |
|    29 | `shapedata` | `PROTO_NC_BRIEFINFO_LOGINCHARACTER_CMD::__unnamed` |
|    78 | `polymorph` | `unsigned short` |
|    80 | `emoticon` | `STOPEMOTICON_DESCRIPT` |
|    83 | `chartitle` | `CHARTITLE_BRIEFINFO` |
|    87 | `abstatebit` | `ABNORMAL_STATE_BIT` |
|   164 | `myguild` | `unsigned long` |
|   168 | `type` | `unsigned char` |
|   169 | `isGuildAcademyMember` | `unsigned char` |
|   170 | `IsAutoPick` | `unsigned char` |
|   171 | `DispositionInfo` | `DISPOSITION_BRIEFINFO` |
|   188 | `Level` | `unsigned char` |
|   189 | `sAnimation` | `char[32]   /* 32 bytes */` |


### `PROTO_NC_BRIEFINFO_MAGICFIELDINFO_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `magicfields` | `unsigned char` |
|     1 | `magicfield` | `PROTO_NC_BRIEFINFO_MAGICFIELDSPREAD_CMD[?] /* 0 bytes */` |


### `PROTO_NC_BRIEFINFO_MAGICFIELDSPREAD_CMD`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `caster` | `unsigned short` |
|     4 | `skillid` | `unsigned short` |
|     6 | `location` | `SHINE_XY_TYPE` |
|    14 | `radious` | `unsigned short` |


### `PROTO_NC_BRIEFINFO_MINIHOUSEBUILD_CMD`  (sizeof = 59 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `camp` | `CHARBRIEFINFO_CAMP` |
|    13 | `charid` | `Name4` |
|    29 | `coord` | `SHINE_COORD_TYPE` |
|    38 | `title` | `char[21]   /* 21 bytes */` |


### `PROTO_NC_BRIEFINFO_MINIHOUSE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `minihouses` | `unsigned char` |
|     1 | `minihouse` | `PROTO_NC_BRIEFINFO_MINIHOUSEBUILD_CMD[?] /* 0 bytes */` |


### `PROTO_NC_BRIEFINFO_MOB_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mobnum` | `unsigned char` |
|     1 | `mobs` | `PROTO_NC_BRIEFINFO_REGENMOB_CMD[?] /* 0 bytes */` |


### `PROTO_NC_BRIEFINFO_PLAYER_INFO_APPEAR_CMD`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHnd` | `unsigned short` |
|     2 | `sID` | `Name4` |
|    18 | `CharTitle` | `CHARTITLE_BRIEFINFO` |
|    22 | `nGuildID` | `unsigned long` |


### `PROTO_NC_BRIEFINFO_PLAYER_LIST_INFO_APPEAR_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPlayerCnt` | `unsigned char` |
|     1 | `PlayerInfo` | `PROTO_NC_BRIEFINFO_PLAYER_INFO_APPEAR_CMD[?] /* 0 bytes */` |


### `PROTO_NC_BRIEFINFO_REGENMOB_CMD`  (sizeof = 124 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `mode` | `unsigned char` |
|     3 | `mobid` | `unsigned short` |
|     5 | `coord` | `SHINE_COORD_TYPE` |
|    14 | `flagstate` | `unsigned char` |
|    15 | `flag` | `PROTO_NC_BRIEFINFO_REGENMOB_CMD::__unnamed` |
|    92 | `sAnimation` | `char[32]   /* 32 bytes */` |


### `PROTO_NC_BRIEFINFO_UNEQUIP_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `slot` | `unsigned char` |


### `PROTO_NC_CHARGED_BOOTHSLOTSIZE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `boothsize` | `unsigned char` |


### `PROTO_NC_CHARGED_BUFFTERMINATE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ChargedBuffKey` | `unsigned long` |


### `PROTO_NC_CHARGED_DELETEWEAPONTITLE_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |


### `PROTO_NC_CHARGED_RESETBUFF_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharRegistNumber` | `unsigned long` |
|     4 | `ChargedBuffKey` | `unsigned long` |
|     8 | `ChargedBuffID` | `unsigned short` |
|    10 | `nUserNo` | `unsigned long` |


### `PROTO_NC_CHARGED_SETBUFF_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharRegistNumber` | `unsigned long` |
|     4 | `ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `ItemId` | `unsigned short` |
|    14 | `ChargedBuff` | `PROTO_CHARGEDBUFF_INFO` |
|    28 | `nUserNo` | `unsigned long` |


### `PROTO_NC_CHARGED_SKILLEMPOW_INITIALIZE_DB_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `restatitemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `restatitemid` | `unsigned short` |
|    18 | `isdelete` | `unsigned char` |
|    19 | `attr` | `unsigned short` |


### `PROTO_NC_CHARGED_STAT_INITIALIZE_DB_FAIL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_CHARGED_STAT_INITIALIZE_DB_SUC_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |


### `PROTO_NC_CHARGED_STAT_INITIALIZE_SUC_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `remain` | `unsigned char` |


### `PROTO_NC_CHARSAVE_2WLDMAN_ABSTATE_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `abstate` | `PROTO_NC_CHARSAVE_ABSTATE_REQ` |


### `PROTO_NC_CHARSAVE_2WLDMAN_ALL_CMD`  (sizeof = 107 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `all` | `PROTO_NC_CHARSAVE_ALL_REQ` |


### `PROTO_NC_CHARSAVE_2WLDMAN_CHESTINFO_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `chest` | `PROTO_NC_CHAR_CHESTINFO_REQ` |


### `PROTO_NC_CHARSAVE_2WLDMAN_COININFO_CMD`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `nCoin` | `unsigned __int64` |
|    10 | `nExchangedCoin` | `unsigned __int64` |


### `PROTO_NC_CHARSAVE_2WLDMAN_GAME_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `game` | `PROTO_GAMEDATA_CMD` |


### `PROTO_NC_CHARSAVE_2WLDMAN_ITEM_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `PacketOrder` | `PROTO_NC_CHARSAVE_2WLDMAN_ITEM_CMD::__unnamed` |
|     3 | `nPartMark` | `unsigned char` |
|     4 | `itemstr` | `PROTO_ITEM_CMD` |


### `PROTO_NC_CHARSAVE_2WLDMAN_ITEM_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPacketOrderNum` | `<UNRESOLVED:0x307d>` |
|     0 | `nIsLastOfInven` | `<UNRESOLVED:0x307e>` |


### `PROTO_NC_CHARSAVE_2WLDMAN_LINK_FROM_CMD`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `linkfrom` | `PROTO_NC_CHAR_LINKFROM_CMD` |


### `PROTO_NC_CHARSAVE_2WLDMAN_MISC_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `cen` | `unsigned __int64` |
|    10 | `fame` | `unsigned long` |


### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_DOING_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `questdoing` | `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_DOING_CMD::tagQUESTDOING` |


### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_DOING_CMD::tagQUESTDOING`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `bNeedClear` | `unsigned char` |
|     5 | `nNumOfDoingQuest` | `unsigned char` |
|     6 | `QuestDoingArray` | `PLAYER_QUEST_INFO[?] /* 0 bytes */` |


### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_DONE_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `questdone` | `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_DONE_CMD::tagQUESTDONE` |


### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_DONE_CMD::tagQUESTDONE`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `nNumOfDoneQuest` | `unsigned short` |
|     6 | `QuestDoneIDArray` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_READ_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `questread` | `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_READ_CMD::tagQUESTREAD` |


### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_READ_CMD::tagQUESTREAD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `nNumOfReadQuest` | `unsigned short` |
|     6 | `QuestReadIDArray` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_REPEAT_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `questrepeat` | `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_REPEAT_CMD::tagQUESTREPEAT` |


### `PROTO_NC_CHARSAVE_2WLDMAN_QUEST_REPEAT_CMD::tagQUESTREPEAT`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `nNumOfRepeatQuest` | `unsigned short` |
|     6 | `QuestRepeatArray` | `PLAYER_QUEST_INFO[?] /* 0 bytes */` |


### `PROTO_NC_CHARSAVE_2WLDMAN_SINGLE_OPTION_CMD`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `WmHandle` | `unsigned short` |
|     2 | `chrregnum` | `unsigned long` |
|     6 | `resttime` | `long` |
|    10 | `autopick` | `unsigned char` |
|    11 | `mystery_vault` | `unsigned char` |
|    12 | `nChatBan` | `unsigned long` |
|    16 | `bIsAuctionNewItem` | `unsigned char` |
|    17 | `bFirstJoin` | `unsigned char` |


### `PROTO_NC_CHARSAVE_2WLDMAN_SKILL_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `PartMark` | `PARTMARK` |
|     3 | `nMaxNum` | `unsigned short` |
|     5 | `skill` | `PROTO_NC_CHARSAVE_SKILL_REQ` |


### `PROTO_NC_CHARSAVE_2WLDMAN_TITLE_CMD`  (sizeof = 497 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `CTData` | `CHARACTER_TITLE_READBLOCK` |


### `PROTO_NC_CHARSAVE_ABSTATE_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     6 | `abstatenum` | `unsigned short` |
|     8 | `abstate` | `ABSTATEREADBLOCK[?] /* 0 bytes */` |


### `PROTO_NC_CHARSAVE_ALL_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bufferlocation` | `unsigned short` |
|     2 | `chrregnum` | `unsigned long` |


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
|     0 | `chrregnum` | `unsigned long` |
|     4 | `CurPwrStone` | `unsigned long` |
|     8 | `CurGrdStone` | `unsigned long` |
|    12 | `CurHPStone` | `unsigned long` |
|    16 | `CurSPStone` | `unsigned long` |
|    20 | `CurHP` | `unsigned long` |
|    24 | `CurSP` | `unsigned long` |
|    28 | `statdistribute` | `CHARSTATDISTSTR` |


### `PROTO_NC_CHARSAVE_DB_UI_STATE_SAVE_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `err` | `unsigned short` |


### `PROTO_NC_CHARSAVE_DB_UI_STATE_SAVE_REQ`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `btMysteryType` | `unsigned char` |


### `PROTO_NC_CHARSAVE_DISPOSITION_SAVE_CMD`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wmHandle` | `unsigned short` |
|     2 | `charRegNum` | `unsigned long` |
|     6 | `updateTime` | `unsigned long` |
|    10 | `data` | `DISPOSITION_BRIEFINFO` |


### `PROTO_NC_CHARSAVE_DISPOSITION_SAVE_REQ`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     6 | `data` | `DISPOSITION_BRIEFINFO` |


### `PROTO_NC_CHARSAVE_ITEMACTIONCOOLTIME_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `num` | `unsigned short` |
|     2 | `group` | `ActionCooltime[?] /* 0 bytes */` |


### `PROTO_NC_CHARSAVE_ITEMACTIONCOOLTIME_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `num` | `unsigned short` |
|     2 | `group` | `ActionCooltime[?] /* 0 bytes */` |


### `PROTO_NC_CHARSAVE_LEVEL_CMD`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `level` | `unsigned char` |
|     5 | `exp` | `unsigned __int64` |
|    13 | `fame` | `unsigned long` |


### `PROTO_NC_CHARSAVE_LOCATION_CMD`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `map` | `Name3` |
|    16 | `coord` | `SHINE_XY_TYPE` |
|    24 | `kqhandle` | `unsigned long` |
|    28 | `map_kq` | `Name3` |
|    40 | `coord_kq` | `SHINE_XY_TYPE` |


### `PROTO_NC_CHARSAVE_PKCOUNT_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `pkcount` | `unsigned long` |


### `PROTO_NC_CHARSAVE_QUEST_DOING_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     6 | `bNeedClear` | `unsigned char` |
|     7 | `nNumOfDoingQuest` | `unsigned char` |
|     8 | `QuestDoingArray` | `PLAYER_QUEST_INFO[?] /* 0 bytes */` |


### `PROTO_NC_CHARSAVE_REST_EXP_LAST_EXEC_TIME_SAVE_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |


### `PROTO_NC_CHARSAVE_SET_CHAT_BLOCK_SPAMER_DB_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharNo` | `unsigned long` |
|     4 | `BlockTime` | `unsigned long` |
|     8 | `DelayTime` | `unsigned long` |


### `PROTO_NC_CHARSAVE_SET_CHAT_BLOCK_SPAMER_WM_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `WmHandle` | `unsigned short` |
|     2 | `CharNo` | `unsigned long` |
|     6 | `BlockTime` | `unsigned long` |
|    10 | `DelayTime` | `unsigned long` |


### `PROTO_NC_CHARSAVE_SKILL_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     6 | `number` | `unsigned short` |
|     8 | `skill` | `PROTO_SKILLREADBLOCK[?] /* 0 bytes */` |


### `PROTO_NC_CHARSAVE_TITLE_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     6 | `CTData` | `CHARACTER_TITLE_DB_SAVE` |


### `PROTO_NC_CHARSAVE_UI_STATE_SAVE_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_CHARSAVE_UI_STATE_SAVE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `btMysteryType` | `unsigned char` |


### `PROTO_NC_CHAR_ABSTATE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `number` | `unsigned short` |
|     6 | `abstate` | `ABSTATEREADBLOCK[?] /* 0 bytes */` |


### `PROTO_NC_CHAR_ADMIN_LEVEL_INFORM_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nAdminLevel` | `unsigned char` |


### `PROTO_NC_CHAR_BASEPARAMCHANGE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `changenum` | `unsigned char` |
|     1 | `param` | `CHAR_PARAMCHANGE_CMD[?] /* 0 bytes */` |


### `PROTO_NC_CHAR_BASE_CMD`  (sizeof = 97 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `charid` | `Name4` |
|    20 | `slotno` | `unsigned char` |
|    21 | `Level` | `unsigned char` |
|    22 | `Experience` | `unsigned __int64` |
|    30 | `CurPwrStone` | `unsigned short` |
|    32 | `CurGrdStone` | `unsigned short` |
|    34 | `CurHPStone` | `unsigned short` |
|    36 | `CurSPStone` | `unsigned short` |
|    38 | `CurHP` | `unsigned long` |
|    42 | `CurSP` | `unsigned long` |
|    46 | `fame` | `unsigned long` |
|    50 | `Cen` | `unsigned __int64` |
|    58 | `logininfo` | `PROTO_NC_CHAR_BASE_CMD::LoginLocation` |
|    79 | `statdistribute` | `CHARSTATDISTSTR` |
|    85 | `pkyellowtime` | `unsigned char` |
|    86 | `pkcount` | `unsigned long` |
|    90 | `prisonmin` | `unsigned short` |
|    92 | `adminlevel` | `unsigned char` |
|    93 | `flags` | `PROTO_NC_CHAR_BASE_CMD::__unnamed` |


### `PROTO_NC_CHAR_BASE_CMD::LoginLocation`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `currentmap` | `Name3` |
|    12 | `currentcoord` | `SHINE_COORD_TYPE` |


### `PROTO_NC_CHAR_BASE_CMD::__unnamed::__unnamed`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skillempower_can_reset` | `<UNRESOLVED:0x3755>` |
|     0 | `restunion` | `<UNRESOLVED:0x3756>` |


### `PROTO_NC_CHAR_CENCHANGE_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |


### `PROTO_NC_CHAR_CHANGEBYCONDITION_PARAM_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nSkillID` | `unsigned short` |
|     2 | `nChangeRate` | `unsigned short` |
|     4 | `nParamNum` | `unsigned short` |
|     6 | `aParam` | `CHAR_CHANGEBYCONDITION_PARAM[?] /* 0 bytes */` |


### `PROTO_NC_CHAR_CHARGEDBUFF_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `NumOfChargedBuff` | `unsigned short` |
|     2 | `ChargedBuff` | `PROTO_CHARGEDBUFF_INFO[?] /* 0 bytes */` |


### `PROTO_NC_CHAR_CHESTINFO_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemnum` | `unsigned short` |
|     2 | `location` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_CHAR_CHESTINFO_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `PROTO_NC_CHARSAVE_ALL_ACK` |
|     6 | `number` | `unsigned short` |
|     8 | `loc` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_CHAR_CLASSCHANGE_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `newclass` | `unsigned char` |


### `PROTO_NC_CHAR_CLASSCHANGE_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `wldmanhandle` | `unsigned short` |
|     8 | `newclass` | `unsigned char` |


### `PROTO_NC_CHAR_CLASSCHANGE_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `newclass` | `unsigned char` |
|     7 | `level` | `unsigned char` |


### `PROTO_NC_CHAR_CLIENT_AUTO_PICK_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `bEnable` | `unsigned char` |


### `PROTO_NC_CHAR_CLIENT_AUTO_PICK_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `player` | `unsigned short` |
|     2 | `bEnable` | `unsigned char` |


### `PROTO_NC_CHAR_CLIENT_AUTO_PICK_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bEnable` | `unsigned char` |


### `PROTO_NC_CHAR_CLIENT_CHARTITLE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CurrentTitle` | `unsigned char` |
|     1 | `CurrentTitleElement` | `unsigned char` |
|     2 | `CurrentTitleMobID` | `unsigned short` |
|     4 | `NumOfTitle` | `unsigned short` |
|     6 | `TitleArray` | `CT_INFO[?] /* 0 bytes */` |


### `PROTO_NC_CHAR_CLIENT_FREESTAT_APPLICATION_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_CHAR_CLIENT_FREESTAT_APPLICATION_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bIsApplication` | `unsigned char` |


### `PROTO_NC_CHAR_CLIENT_ITEM_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `numofitem` | `unsigned char` |
|     1 | `box` | `unsigned char` |
|     2 | `flag` | `PROTO_NC_CHAR_CLIENT_ITEM_CMD::__unnamed` |
|     3 | `ItemArray` | `PROTO_ITEMPACKET_INFORM[?] /* 0 bytes */` |


### `PROTO_NC_CHAR_CLIENT_ITEM_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `invenclear` | `<UNRESOLVED:0x1281>` |


### `PROTO_NC_CHAR_CLIENT_SKILL_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `restempow` | `unsigned char` |
|     1 | `PartMark` | `PARTMARK` |
|     2 | `nMaxNum` | `unsigned short` |
|     4 | `skill` | `PROTO_NC_CHAR_SKILLCLIENT_CMD` |


### `PROTO_NC_CHAR_COININFO_CMD`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCoin` | `unsigned __int64` |
|     8 | `nExchangedCoin` | `unsigned __int64` |


### `PROTO_NC_CHAR_DEADMENU_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `minute` | `unsigned char` |
|     1 | `priority` | `unsigned char` |
|     2 | `eMenuType` | `DeadMenuType` |


### `PROTO_NC_CHAR_EMPTY_INSTANCE_DUNGEON_RNG`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |
|     7 | `Category` | `INSTANCE_DUNGEON::CATEGORY` |


### `PROTO_NC_CHAR_EXP_CHANGED_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wmhandle` | `unsigned short` |
|     2 | `CharNo` | `unsigned long` |
|     6 | `CurrentExp` | `unsigned __int64` |


### `PROTO_NC_CHAR_FAMECHANGE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `fame` | `unsigned long` |


### `PROTO_NC_CHAR_FAMESAVE_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charregnum` | `unsigned long` |
|     4 | `fame` | `unsigned long` |


### `PROTO_NC_CHAR_FREESTAT_SET_DB_ACK`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nFreeStat` | `unsigned char[5]   /* 5 bytes */` |
|    11 | `nLockIndex` | `unsigned short` |
|    13 | `nErrorType` | `unsigned short` |


### `PROTO_NC_CHAR_FREESTAT_SET_DB_REQ`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nMaxStat` | `unsigned char` |
|     7 | `nFreeStat` | `unsigned char[5]   /* 5 bytes */` |
|    12 | `nLockIndex` | `unsigned short` |
|    14 | `nItemNum` | `unsigned char` |
|    15 | `DecItemInfo` | `PROTO_NC_CHAR_FREESTAT_SET_DB_REQ::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_CHAR_FREESTAT_SET_DB_REQ::__unnamed`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nIsDeleteItem` | `unsigned char` |
|     1 | `nItmeKey` | `SHINE_ITEM_REGISTNUMBER` |
|     9 | `nItemID` | `unsigned short` |
|    11 | `nItemLot` | `unsigned char` |


### `PROTO_NC_CHAR_GET_CHAT_BLOCK_SPAMER_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BlockTime` | `unsigned long` |
|     4 | `DelayTime` | `unsigned long` |


### `PROTO_NC_CHAR_GET_ITEMLIST_BY_TYPE_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nInvenType` | `unsigned char` |
|     7 | `nOwner` | `unsigned long` |
|    11 | `Error` | `unsigned short` |
|    13 | `ItemCmd` | `PROTO_NC_CHAR_ITEM_CMD` |


### `PROTO_NC_CHAR_GET_ITEMLIST_BY_TYPE_NUM_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nInvenType` | `unsigned char` |
|     7 | `nOwner` | `unsigned long` |
|    11 | `Error` | `unsigned short` |
|    13 | `ItemCmd` | `PROTO_NC_CHAR_ITEM_CMD` |


### `PROTO_NC_CHAR_GET_ITEMLIST_BY_TYPE_NUM_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nInvenType` | `unsigned char` |
|     7 | `nOwner` | `unsigned long` |
|    11 | `slotORnumber` | `unsigned char` |
|    12 | `from` | `unsigned short` |
|    14 | `to` | `unsigned short` |


### `PROTO_NC_CHAR_GET_ITEMLIST_BY_TYPE_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nInvenType` | `unsigned char` |
|     7 | `nOwner` | `unsigned long` |


### `PROTO_NC_CHAR_ITEM_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `PacketOrder` | `PROTO_NC_CHAR_ITEM_CMD::__unnamed` |
|     1 | `nPartMark` | `unsigned char` |
|     2 | `ItemCmd` | `PROTO_ITEM_CMD` |


### `PROTO_NC_CHAR_ITEM_CMD::__unnamed`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPacketOrderNum` | `<UNRESOLVED:0x307d>` |
|     0 | `nIsLastOfInven` | `<UNRESOLVED:0x307e>` |


### `PROTO_NC_CHAR_KICKPLAYEROUT_RNG`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `tmp` | `unsigned char` |
|     8 | `charid` | `Name4` |


### `PROTO_NC_CHAR_LEVEL_CHANGED_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wmhandle` | `unsigned short` |
|     2 | `CharNo` | `unsigned long` |
|     6 | `CharNewLevel` | `unsigned char` |


### `PROTO_NC_CHAR_LINKFROM_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `map` | `Name3` |
|    16 | `coord` | `SHINE_XY_TYPE` |


### `PROTO_NC_CHAR_LOGIN_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zoneip` | `Name4` |
|    16 | `zoneport` | `unsigned short` |


### `PROTO_NC_CHAR_MAPLOGIN_ACK`  (sizeof = 238 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charhandle` | `unsigned short` |
|     2 | `param` | `CHAR_PARAMETER_DATA` |
|   230 | `logincoord` | `SHINE_XY_TYPE` |


### `PROTO_NC_CHAR_MYSTERYVAULT_UI_STATE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mystery_vault` | `unsigned char` |


### `PROTO_NC_CHAR_PK_MODE_TOGGLE_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isPKMode` | `unsigned char` |
|     1 | `errorCode` | `unsigned short` |


### `PROTO_NC_CHAR_PLAYERBANNED_RNG`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `charid` | `Name4` |


### `PROTO_NC_CHAR_PLAYERFOUND_BY_NORMAL_USER_RNG`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `map` | `Name3` |
|    19 | `coord` | `SHINE_XY_TYPE` |
|    27 | `charid` | `Name4` |
|    43 | `canRecall` | `unsigned char` |


### `PROTO_NC_CHAR_PLAYERFOUND_RNG`  (sizeof = 43 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `map` | `Name3` |
|    19 | `coord` | `SHINE_XY_TYPE` |
|    27 | `charid` | `Name4` |


### `PROTO_NC_CHAR_PLAYERSEARCH_BY_NORMAL_USER_RNG`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `charid` | `Name4` |


### `PROTO_NC_CHAR_PLAYERSEARCH_RNG`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `charid` | `Name4` |


### `PROTO_NC_CHAR_PLAYERSUMMON_RNG`  (sizeof = 43 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `charid` | `Name4` |
|    23 | `map` | `Name3` |
|    35 | `coord` | `SHINE_XY_TYPE` |


### `PROTO_NC_CHAR_POLYMORPH_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `player` | `unsigned short` |
|     2 | `polymorph` | `unsigned short` |
|     4 | `AbstateIndex` | `unsigned short` |


### `PROTO_NC_CHAR_PROMOTE_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `newclass` | `unsigned char` |


### `PROTO_NC_CHAR_QUEST_DOING_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `bNeedClear` | `unsigned char` |
|     5 | `nNumOfDoingQuest` | `unsigned char` |
|     6 | `QuestDoingArray` | `PLAYER_QUEST_INFO[?] /* 0 bytes */` |


### `PROTO_NC_CHAR_QUEST_DONE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `nNumOfDoneQuest` | `unsigned short` |
|     6 | `QuestDoneIDArray` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_CHAR_QUEST_READ_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `nNumOfReadQuest` | `unsigned short` |
|     6 | `QuestReadIDArray` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_CHAR_QUEST_REPEAT_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `nNumOfRepeatQuest` | `unsigned short` |
|     6 | `QuestRepeatArray` | `PLAYER_QUEST_INFO[?] /* 0 bytes */` |


### `PROTO_NC_CHAR_REGISTNUMBER_ACK`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `usrregnum` | `unsigned long` |
|     4 | `chrregnum` | `unsigned long` |
|     8 | `loginreq` | `PROTO_NC_CHAR_ZONE_CHARDATA_REQ` |


### `PROTO_NC_CHAR_REGISTNUMBER_REQ`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `loginreq` | `PROTO_NC_CHAR_ZONE_CHARDATA_REQ` |


### `PROTO_NC_CHAR_REGNUM_VARIFICATION_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zonehandle` | `unsigned short` |
|     2 | `charregnum` | `unsigned long` |
|     6 | `iscorrect` | `unsigned char` |


### `PROTO_NC_CHAR_REGNUM_VARIFICATION_REQ`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zonehandle` | `unsigned short` |
|     2 | `zone_charid` | `Name4` |
|    18 | `charregnum` | `unsigned long` |


### `PROTO_NC_CHAR_REVIVEOTHER_CMD`  (sizeof = 31 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `link` | `PROTO_NC_CHAR_REVIVESAME_CMD` |
|    10 | `sock` | `PROTO_NC_CHAR_LOGIN_ACK` |
|    28 | `wldmanhandle` | `unsigned short` |
|    30 | `isPKModeOn` | `unsigned char` |


### `PROTO_NC_CHAR_REVIVESAME_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mapid` | `unsigned short` |
|     2 | `location` | `SHINE_XY_TYPE` |


### `PROTO_NC_CHAR_SINGLE_OPTION_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `resttime` | `long` |
|     8 | `autopick` | `unsigned char` |
|     9 | `mystery_vault` | `unsigned char` |
|    10 | `nChatBan` | `unsigned long` |
|    14 | `bIsAuctionNewItem` | `unsigned char` |
|    15 | `dMidNoManner` | `long` |
|    19 | `bFirstJoin` | `unsigned char` |


### `PROTO_NC_CHAR_SKILLCLIENT_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `number` | `unsigned short` |
|     6 | `skill` | `PROTO_SKILLREADBLOCKCLIENT[?] /* 0 bytes */` |


### `PROTO_NC_CHAR_SKILL_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `PartMark` | `PARTMARK` |
|     5 | `nMaxNum` | `unsigned short` |
|     7 | `number` | `unsigned short` |
|     9 | `skill` | `PROTO_SKILLREADBLOCK[?] /* 0 bytes */` |


### `PROTO_NC_CHAR_SKILL_PASSIVE_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `number` | `unsigned short` |
|     2 | `passive` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_CHAR_SOMEONEGUILDACADEMYCHANGE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `guild` | `unsigned long` |


### `PROTO_NC_CHAR_SOMEONEGUILDCHANGE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `guild` | `unsigned long` |


### `PROTO_NC_CHAR_SOMEONEPROMOTE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `newclass` | `unsigned char` |


### `PROTO_NC_CHAR_STAT_DECPOINTFAIL_DB_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_CHAR_STAT_DECPOINTSUC_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `stat` | `unsigned char` |


### `PROTO_NC_CHAR_STAT_DECPOINTSUC_DB_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `stat` | `unsigned char` |
|     7 | `lockindex` | `unsigned short` |


### `PROTO_NC_CHAR_STAT_DECPOINT_DB_REQ`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `stat` | `unsigned char` |
|     7 | `lockindex` | `unsigned short` |
|     9 | `restatitemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    17 | `restatitemid` | `unsigned short` |
|    19 | `isdelete` | `unsigned char` |
|    20 | `attr` | `unsigned short` |


### `PROTO_NC_CHAR_STAT_DECPOINT_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `stat` | `unsigned char` |


### `PROTO_NC_CHAR_STAT_INCPOINTFAIL_DB_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `err` | `unsigned short` |


### `PROTO_NC_CHAR_STAT_INCPOINTSUC_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `stat` | `unsigned char` |


### `PROTO_NC_CHAR_STAT_INCPOINTSUC_DB_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `stat` | `unsigned char` |


### `PROTO_NC_CHAR_STAT_INCPOINT_DB_REQ`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `stat` | `unsigned char` |


### `PROTO_NC_CHAR_STAT_INCPOINT_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `stat` | `unsigned char` |


### `PROTO_NC_CHAR_STAT_REMAINPOINT_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `remain` | `unsigned char` |


### `PROTO_NC_CHAR_WEDDINGDATA_ACK`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `Error` | `unsigned short` |
|     8 | `Status` | `PROTO_NC_CHAR_WEDDINGDATA_ACK::__unnamed` |
|     9 | `When` | `ShineDateTime` |
|    13 | `SubWhen` | `ShineDateTime` |
|    13 | `ApplicationLimit` | `ShineDateTime` |
|    13 | `DelayLimit` | `ShineDateTime` |
|    13 | `DivorceLimit` | `ShineDateTime` |
|    13 | `NewlyLimit` | `ShineDateTime` |
|    17 | `Partner` | `PROTO_NC_CHAR_WEDDINGDATA_ACK::__unnamed` |
|    37 | `WeddingCount` | `unsigned char` |
|    38 | `RefusalCount` | `unsigned char` |


### `PROTO_NC_CHAR_WEDDINGDATA_ACK::__unnamed`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Name` | `Name4` |
|    16 | `RegisterNumber` | `unsigned long` |


### `PROTO_NC_CHAR_WEDDINGDATA_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |


### `PROTO_NC_CHAR_WEDDING_CANCEL_WEDDING`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `RegisterNumber` | `unsigned long` |


### `PROTO_NC_CHAR_WEDDING_DIVORCE_CANCEL_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `Success` | `int` |
|    10 | `MySubStatus` | `unsigned char` |
|    11 | `Partner` | `unsigned long` |
|    15 | `PartnerSubStatus` | `unsigned char` |


### `PROTO_NC_CHAR_WEDDING_DIVORCE_CANCEL_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `Partner` | `unsigned long` |
|    10 | `IncreseCount` | `unsigned char` |


### `PROTO_NC_CHAR_WEDDING_DIVORCE_DO_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `Partner` | `unsigned long` |
|    10 | `Reason` | `unsigned char` |


### `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `PartnerInfo` | `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_ACK::__unnamed` |


### `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_ACK::__unnamed`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Level` | `unsigned char` |
|     1 | `Connected` | `unsigned char` |
|     2 | `LastConnect` | `ShineDateTime` |
|     6 | `CurrentMap` | `unsigned short` |


### `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `Partner` | `unsigned long` |


### `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_RNG`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |
|     7 | `PartnerInfo` | `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_RNG::__unnamed` |


### `PROTO_NC_CHAR_WEDDING_PARTNER_INFO_RNG::__unnamed`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Level` | `unsigned char` |
|     1 | `Connected` | `unsigned char` |
|     2 | `CurrentMap` | `unsigned short` |
|     4 | `RegisterNumber` | `unsigned long` |


### `PROTO_NC_CHAR_WEDDING_PARTNER_SUMMON_RNG`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `RegisterNumber` | `unsigned long` |
|    11 | `map` | `Name3` |
|    23 | `coord` | `SHINE_XY_TYPE` |
|    31 | `from` | `unsigned char` |
|    32 | `to` | `unsigned char` |
|    33 | `err` | `unsigned short` |


### `PROTO_NC_CHAR_WEDDING_PROPOSE_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `Success` | `int` |
|    10 | `Partner` | `unsigned long` |


### `PROTO_NC_CHAR_WEDDING_PROPOSE_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `Partner` | `unsigned long` |


### `PROTO_NC_CHAR_WEDDING_REFRESH_INFO_RNG`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |


### `PROTO_NC_CHAR_ZONE_CHARDATA_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |


### `PROTO_NC_CHAR_ZONE_CHARDATA_REQ`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wldmanhandle` | `unsigned short` |
|     2 | `charid` | `Name4` |


### `PROTO_NC_CT_ADD_FRIEND_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `NumberOfFriends` | `unsigned short` |


### `PROTO_NC_CT_CHARTTING_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |


### `PROTO_NC_CT_DB_SET_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `Info` | `CT_INFO` |


### `PROTO_NC_CT_SET_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Info` | `CT_INFO` |


### `PROTO_NC_CT_SET_CURRENT_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Type` | `unsigned char` |
|     1 | `ElementNo` | `unsigned char` |
|     2 | `MobID` | `unsigned short` |
|     4 | `Error` | `unsigned short` |


### `PROTO_NC_CT_SET_CURRENT_DB_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `CurrentTitle` | `unsigned char` |
|     7 | `CurrentTitleElement` | `unsigned char` |
|     8 | `CurrentTitleMobID` | `unsigned short` |
|    10 | `UseTitle` | `CT_INFO` |
|    12 | `UseAbleTitle` | `CT_INFO` |


### `PROTO_NC_CT_SET_CURRENT_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Type` | `unsigned char` |
|     1 | `ElementNo` | `unsigned char` |
|     2 | `MobID` | `unsigned short` |


### `PROTO_NC_CT_SET_SOMEONECHANGE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hnd` | `unsigned short` |
|     2 | `Type` | `unsigned char` |
|     3 | `ElementNo` | `unsigned char` |
|     4 | `MobID` | `unsigned short` |


### `PROTO_NC_DICE_TAISAI_BETTING_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nBettingIndex` | `unsigned short` |


### `PROTO_NC_DICE_TAISAI_BETTING_CANCEL_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nBettingCancelIndex` | `unsigned short` |


### `PROTO_NC_DICE_TAISAI_BETTING_CANCEL_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPlayerHnd` | `unsigned short` |
|     2 | `BettingCancelInfo` | `DiceTaiSaiBetting` |


### `PROTO_NC_DICE_TAISAI_BETTING_CANCEL_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BettingCancelInfo` | `DiceTaiSaiBetting` |
|     8 | `nBettingCancelIndex` | `unsigned short` |


### `PROTO_NC_DICE_TAISAI_BETTING_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPlayerHnd` | `unsigned short` |
|     2 | `BettingInfo` | `DiceTaiSaiBetting` |


### `PROTO_NC_DICE_TAISAI_BETTING_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BettingInfo` | `DiceTaiSaiBetting` |
|     8 | `nBettingIndex` | `unsigned short` |


### `PROTO_NC_DICE_TAISAI_BET_START_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bModeType` | `DICE_TAISAI_GAME_MODE` |
|     4 | `CurrentRollingDice` | `DiceTaiSaiInfo` |


### `PROTO_NC_DICE_TAISAI_BOARD_FOLD_CMD`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_DICE_TAISAI_DB_DICE_ROLL_RESULT_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nGameNo` | `int` |
|     6 | `header` | `NETPACKETZONEHEADER` |
|    12 | `nActiveDiceTaiSaiKey` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `nResultCnt` | `unsigned short` |
|    22 | `DiceRollResult` | `DiceRollApplyResult[?] /* 0 bytes */` |


### `PROTO_NC_DICE_TAISAI_DB_DICE_ROLL_RESULT_LOG_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nActiveDiceTaiSaiKey` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `nGameNo` | `int` |
|    18 | `nResultLogCnt` | `unsigned short` |
|    20 | `DiceRollResultLog` | `DiceRollApplyResultLog[?] /* 0 bytes */` |


### `PROTO_NC_DICE_TAISAI_DB_DICE_ROLL_RESULT_REQ`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nActiveDiceTaiSaiKey` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `DiceNum` | `DiceTaiSaiInfo` |
|    17 | `nResultCnt` | `unsigned short` |
|    19 | `DiceRollResult` | `DiceRollApplyResult[?] /* 0 bytes */` |


### `PROTO_NC_DICE_TAISAI_DB_EXPECT_INCOME_MONEY_ACK`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `header` | `NETPACKETZONEHEADER` |
|     8 | `nInvenSlot` | `ITEM_INVEN` |
|    10 | `nTaiSaiItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    18 | `nGameCnt` | `unsigned long` |
|    22 | `nExpectIncomeMoney` | `__int64` |


### `PROTO_NC_DICE_TAISAI_DB_EXPECT_INCOME_MONEY_REQ`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nInvenSlot` | `ITEM_INVEN` |
|     8 | `nTaiSaiItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `nRewardMoney` | `unsigned __int64` |
|    24 | `GetMasterRate` | `unsigned short` |


### `PROTO_NC_DICE_TAISAI_DB_ITEM_ID_CHANGE_ACK`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `header` | `NETPACKETZONEHEADER` |
|     8 | `nInvenSlot` | `ITEM_INVEN` |
|    10 | `lockindex` | `unsigned short` |
|    12 | `nDiceTaiSaiKey` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `nGameItemID` | `unsigned short` |
|    22 | `nMoneyCardItemID` | `unsigned short` |
|    24 | `nRewardMoney` | `unsigned __int64` |


### `PROTO_NC_DICE_TAISAI_DB_ITEM_ID_CHANGE_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nInvenSlot` | `ITEM_INVEN` |
|     8 | `lockindex` | `unsigned short` |
|    10 | `nDiceTaiSaiKey` | `SHINE_ITEM_REGISTNUMBER` |
|    18 | `nGameItemID` | `unsigned short` |
|    20 | `nMoneyCardItemID` | `unsigned short` |
|    22 | `nRewardMoney` | `unsigned __int64` |
|    30 | `GetMasterRate` | `unsigned short` |


### `PROTO_NC_DICE_TAISAI_DB_RANK_ACK`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `header` | `NETPACKETZONEHEADER` |
|     8 | `nReqCharNo` | `unsigned long` |
|    12 | `nRankCnt` | `unsigned char` |
|    13 | `Rank` | `DiceRankInfo[?] /* 0 bytes */` |


### `PROTO_NC_DICE_TAISAI_DB_RANK_REQ`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nReqCharNo` | `unsigned long` |
|    10 | `nTaiSaiItemKey` | `SHINE_ITEM_REGISTNUMBER` |


### `PROTO_NC_DICE_TAISAI_DB_RECEIPT_INCOME_MONEY_ACK`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `lockindex` | `unsigned short` |
|     4 | `header` | `NETPACKETZONEHEADER` |
|    10 | `nInvenSlot` | `ITEM_INVEN` |
|    12 | `nTaiSaiItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `nReceiptIncomeMoney` | `unsigned __int64` |


### `PROTO_NC_DICE_TAISAI_DB_RECEIPT_INCOME_MONEY_REQ`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `header` | `NETPACKETZONEHEADER` |
|     8 | `nInvenSlot` | `ITEM_INVEN` |
|    10 | `nTaiSaiItemKey` | `SHINE_ITEM_REGISTNUMBER` |


### `PROTO_NC_DICE_TAISAI_DICE_ROLL_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |


### `PROTO_NC_DICE_TAISAI_DICE_ROLL_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |


### `PROTO_NC_DICE_TAISAI_DICE_ROLL_RESULT_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nDiceInfo` | `DiceTaiSaiInfo` |


### `PROTO_NC_DICE_TAISAI_DICE_ROLL_RESULT_EMOTION_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nGamerCnt` | `unsigned char` |
|     1 | `Emotion` | `EmotionInfo[?] /* 0 bytes */` |


### `PROTO_NC_DICE_TAISAI_EXPECT_INCOME_MONEY_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nInvenSlot` | `ITEM_INVEN` |
|     4 | `nGameCnt` | `unsigned long` |
|     8 | `nExpectIncomeMoney` | `unsigned __int64` |


### `PROTO_NC_DICE_TAISAI_EXPECT_INCOME_MONEY_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nInvenSlot` | `ITEM_INVEN` |


### `PROTO_NC_DICE_TAISAI_GAME_BETTING_INFO_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `PartMask` | `unsigned char` |
|     1 | `nBettingCnt` | `unsigned short` |
|     3 | `Betting` | `BettingInfo[?] /* 0 bytes */` |


### `PROTO_NC_DICE_TAISAI_GAME_JOIN_ACK`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `DiceHistory` | `DiceTaiSaiInfo[?] /* 21 bytes */` |
|    23 | `nGameStatus` | `DICE_TAISAI_GAME_STATUS` |
|    27 | `bModeType` | `DICE_TAISAI_GAME_MODE` |
|    31 | `nTimer` | `unsigned short` |
|    33 | `nInterval` | `unsigned long` |
|    37 | `nGetSysRate` | `unsigned short` |
|    39 | `nMaxBetMoney` | `unsigned __int64` |
|    47 | `nUseMinLv` | `unsigned short` |
|    49 | `nGamerCnt` | `unsigned char` |
|    50 | `nGamerHnd` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_DICE_TAISAI_GAME_JOIN_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPlayerHnd` | `unsigned short` |


### `PROTO_NC_DICE_TAISAI_GAME_LEAVE_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |


### `PROTO_NC_DICE_TAISAI_GAME_LEAVE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bReserve` | `unsigned char` |
|     1 | `nPlayerHnd` | `unsigned short` |


### `PROTO_NC_DICE_TAISAI_GAME_MODE_CHANGE_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `bModeType` | `DICE_TAISAI_GAME_MODE` |


### `PROTO_NC_DICE_TAISAI_GAME_MODE_CHANGE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bModeType` | `DICE_TAISAI_GAME_MODE` |


### `PROTO_NC_DICE_TAISAI_LARGE_AMOUNT_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nGamerCnt` | `unsigned char` |
|     1 | `LargeAmount` | `LargeAmountInfo[?] /* 0 bytes */` |


### `PROTO_NC_DICE_TAISAI_RANK_ACK`  (sizeof = 404 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nRankHistoryCnt` | `unsigned char` |
|     3 | `RankHistory` | `DiceRankInfo[?] /* 400 bytes */` |
|   403 | `nRankCurrCnt` | `unsigned char` |
|   404 | `RankCurr` | `DiceRankInfo[?] /* 0 bytes */` |


### `PROTO_NC_DICE_TAISAI_RECEIPT_INCOME_MONEY_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nInvenSlot` | `ITEM_INVEN` |
|     4 | `ReceiptIncomeMoney` | `unsigned __int64` |


### `PROTO_NC_DICE_TAISAI_RECEIPT_INCOME_MONEY_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nInvenSlot` | `ITEM_INVEN` |


### `PROTO_NC_DICE_TAISAI_TIMER_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bIsStart` | `unsigned char` |
|     1 | `nTimer` | `unsigned short` |
|     3 | `nInterval` | `unsigned long` |


### `PROTO_NC_FRIEND_ADD_ZONE_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |
|     4 | `nFriendNo` | `unsigned long` |


### `PROTO_NC_FRIEND_DEL_ZONE_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |
|     4 | `nFriendNo` | `unsigned long` |


### `PROTO_NC_FRIEND_LIST_ZONE_CMD`  (sizeof = 205 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |
|     4 | `FriendNumber` | `unsigned char` |
|     5 | `FriendList` | `unsigned long[50]   /* 200 bytes */` |


### `PROTO_NC_GAMBLE_COIN_BUY_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCoin` | `unsigned __int64` |


### `PROTO_NC_GAMBLE_COIN_CHANGE_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCoin` | `unsigned __int64` |


### `PROTO_NC_GAMBLE_COIN_DB_BUY_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nLockIndex` | `unsigned short` |
|     8 | `nError` | `unsigned short` |


### `PROTO_NC_GAMBLE_COIN_DB_BUY_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nLockIndex` | `unsigned short` |
|     8 | `nCoin` | `unsigned __int64` |
|    16 | `nMoney` | `unsigned __int64` |
|    24 | `nCanExchangeMaxCoin` | `unsigned long` |
|    28 | `tExchangedCoinDate` | `long` |


### `PROTO_NC_GAMBLE_COIN_DB_SELL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nLockIndex` | `unsigned short` |
|     8 | `nError` | `unsigned short` |


### `PROTO_NC_GAMBLE_COIN_DB_SELL_REQ`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nLockIndex` | `unsigned short` |
|     8 | `nCoin` | `unsigned __int64` |
|    16 | `nMoney` | `unsigned __int64` |


### `PROTO_NC_GAMBLE_COIN_DB_USE_COINITEM_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nCoin` | `unsigned __int64` |
|    14 | `nLockIndex` | `unsigned short` |
|    16 | `nError` | `unsigned short` |


### `PROTO_NC_GAMBLE_COIN_DB_USE_COINITEM_REQ`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHeader` | `NETPACKETZONEHEADER` |
|     6 | `nItemReg` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `nCoin` | `unsigned __int64` |
|    22 | `nLockIndex` | `unsigned short` |


### `PROTO_NC_GAMBLE_COIN_USE_COINITEM_MESSAGE_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nAddCoin` | `unsigned __int64` |


### `PROTO_NC_GAMBLE_COIN_VIPCARD_UI_OPEN_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nInvenSlot` | `unsigned char` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_ALL_RANK_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nRankAllCnt` | `unsigned char` |
|     3 | `RankAll` | `GDT_DiceRankInfo[?] /* 0 bytes */` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_ALL_RANK_REQ`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_GAMBLE_DICE_TAISAI_BETTING_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nBettingIndex` | `unsigned short` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_BETTING_CANCEL_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nBettingCancelIndex` | `unsigned short` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_BETTING_CANCEL_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPlayerHnd` | `unsigned short` |
|     2 | `BettingCancelInfo` | `DiceTaiSaiBetting` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_BETTING_CANCEL_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BettingCancelInfo` | `DiceTaiSaiBetting` |
|     8 | `nBettingCancelIndex` | `unsigned short` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_BETTING_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPlayerHnd` | `unsigned short` |
|     2 | `BettingInfo` | `DiceTaiSaiBetting` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_BETTING_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BettingInfo` | `DiceTaiSaiBetting` |
|     8 | `nBettingIndex` | `unsigned short` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_BET_START_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bModeType` | `DICE_TAISAI_GAME_MODE` |
|     4 | `CurrentRollingDice` | `DiceTaiSaiInfo` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_CURR_RANK_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nRankCurrCnt` | `unsigned char` |
|     3 | `RankCurr` | `GDT_DiceRankInfo[?] /* 0 bytes */` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_CURR_RANK_REQ`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_GAMBLE_DICE_TAISAI_DB_ALL_RANK_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `header` | `NETPACKETZONEHEADER` |
|     8 | `nRankAllCnt` | `unsigned char` |
|     9 | `RankAll` | `GDT_DiceRankInfo[?] /* 0 bytes */` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_DB_ALL_RANK_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_DB_DICE_ROLL_RESULT_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nGameNo` | `int` |
|     6 | `header` | `NETPACKETZONEHEADER` |
|    12 | `nResultCnt` | `unsigned short` |
|    14 | `DiceRollResult` | `GDT_DiceRollApplyResult[?] /* 0 bytes */` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_DB_DICE_ROLL_RESULT_LOG_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nGameNo` | `int` |
|    10 | `nResultLogCnt` | `unsigned short` |
|    12 | `DiceRollResultLog` | `GDT_DiceRollApplyResultLog[?] /* 0 bytes */` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_DB_DICE_ROLL_RESULT_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `DiceNum` | `DiceTaiSaiInfo` |
|     9 | `nResultCnt` | `unsigned short` |
|    11 | `DiceRollResult` | `GDT_DiceRollApplyResult[?] /* 0 bytes */` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_DICE_ROLL_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_DICE_ROLL_RESULT_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nDiceInfo` | `DiceTaiSaiInfo` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_DICE_ROLL_RESULT_EMOTION_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nGamerCnt` | `unsigned char` |
|     1 | `Emotion` | `GDT_EmotionInfo[?] /* 0 bytes */` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_BETTING_INFO_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `PartMask` | `unsigned char` |
|     1 | `nBettingCnt` | `unsigned short` |
|     3 | `Betting` | `GDT_BettingInfo[?] /* 0 bytes */` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_JOIN_ACK`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `DiceHistory` | `DiceTaiSaiInfo[?] /* 21 bytes */` |
|    23 | `nGameStatus` | `DICE_TAISAI_GAME_STATUS` |
|    27 | `bModeType` | `DICE_TAISAI_GAME_MODE` |
|    31 | `nTimer` | `unsigned short` |
|    33 | `nInterval` | `unsigned long` |
|    37 | `nGetSysRate` | `unsigned short` |
|    39 | `nMaxBetMoney` | `unsigned __int64` |
|    47 | `nUseMinLv` | `unsigned short` |
|    49 | `nGamerCnt` | `unsigned char` |
|    50 | `nGamerHnd` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_JOIN_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPlayerHnd` | `unsigned short` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_JOIN_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNpcHandle` | `unsigned short` |
|     2 | `nWhereStand` | `unsigned char` |
|     3 | `nDirect` | `unsigned char` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_LEAVE_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_GAME_LEAVE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bReserve` | `unsigned char` |
|     1 | `nPlayerHnd` | `unsigned short` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_LARGE_AMOUNT_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nGamerCnt` | `unsigned char` |
|     1 | `LargeAmount` | `GDT_LargeAmountInfo[?] /* 0 bytes */` |


### `PROTO_NC_GAMBLE_DICE_TAISAI_TIMER_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bIsStart` | `unsigned char` |
|     1 | `nTimer` | `unsigned short` |
|     3 | `nInterval` | `unsigned long` |


### `PROTO_NC_GAMBLE_ENTER_PLAYER_DIRECT_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nHandle` | `unsigned short` |
|     2 | `nDirect` | `unsigned char` |


### `PROTO_NC_GAMBLE_EXCHANGECOIN_CHANGE_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nExchangeCoin` | `unsigned __int64` |


### `PROTO_NC_GAMBLE_EXCHANGEDCOIN_DB_INFO_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |
|     4 | `nExchangedCoin` | `unsigned __int64` |


### `PROTO_NC_GAMBLE_EXCHANGEDCOIN_DB_INFO_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |
|     4 | `dChackDate` | `long` |


### `PROTO_NC_GAMBLE_EXCHANGEDCOIN_INIT_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `dChackDate` | `long` |


### `PROTO_NC_GAMBLE_PLAYERACT_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `playerhandle` | `unsigned short` |
|     2 | `effect_body` | `unsigned long` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_DB_GAMEINFO_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Machine` | `unsigned char` |
|     1 | `nError` | `unsigned short` |
|     3 | `JackPotPrice` | `unsigned long` |
|     7 | `RankNumber` | `unsigned char` |
|     8 | `RankList` | `SLOTMACHINE_JackPotRank[?] /* 0 bytes */` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_DB_GAMEINFO_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nSlotMachineNum` | `unsigned char` |
|     1 | `MachineIndex` | `PROTO_NC_GAMBLE_SLOTMACHINE_DB_GAMEINFO_REQ::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_DB_GAMEINFO_REQ::__unnamed`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Machine` | `unsigned char` |
|     1 | `BetUnits` | `unsigned short` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_DB_RESULT_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `nError` | `unsigned short` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_DB_RESULT_REQ`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `PlayerRegistNumber` | `unsigned long` |
|    12 | `CardDisplay` | `char[10]   /* 10 bytes */` |
|    22 | `IsJackPot` | `unsigned char` |
|    23 | `JackPotAccumulate` | `unsigned long` |
|    27 | `BetUnit` | `unsigned short` |
|    29 | `BetLine` | `unsigned char` |
|    30 | `nJackpotRate` | `unsigned short` |
|    32 | `WinMoney` | `unsigned long` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_GAME_JOIN_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BettingUnit` | `unsigned short` |
|     2 | `nError` | `unsigned short` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_GAME_JOIN_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNpcHandle` | `unsigned short` |
|     2 | `nWhereStand` | `unsigned char` |
|     3 | `nDirect` | `unsigned char` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_GAME_LEAVE_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_JACKPOTINFO_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `JackPotNum` | `unsigned char` |
|     1 | `JackPotList` | `PROTO_NC_GAMBLE_SLOTMACHINE_JACKPOTINFO_CMD::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_JACKPOTINFO_CMD::__unnamed`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BettingUnit` | `unsigned short` |
|     2 | `JackpotPrice` | `unsigned long` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_JACKPOTRANKING_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ListLen` | `unsigned char` |
|     1 | `RankList` | `SLOTMACHINE_JackPotRank[?] /* 0 bytes */` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_SOMEONE_GET_JACKPOT_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name4` |
|    16 | `price` | `unsigned __int64` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_START_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_START_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `betting` | `SLOTMACHINE_BettingLine` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_STOPBUTTON_ACK`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `screen` | `SLOTMACHINE_ScreenState` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_WHEELSTOP_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `bet` | `unsigned short` |
|     4 | `win` | `unsigned long` |
|     8 | `WinLine` | `SLOTMACHINE_WinState` |


### `PROTO_NC_GAMBLE_SLOTMACHINE_WINRANKING_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ListLen` | `unsigned char` |
|     1 | `RankList` | `SLOTMACHINE_JackPotRank[?] /* 0 bytes */` |


### `PROTO_NC_GAMBLE_TYPE_AND_WHERE_STAND_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nNpcHandle` | `unsigned short` |
|     4 | `nGambleType` | `GAMBLE_TYPE` |
|     8 | `nWhereStand` | `unsigned char` |


### `PROTO_NC_GAMBLE_TYPE_AND_WHERE_STAND_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNpcHandle` | `unsigned short` |


### `PROTO_NC_GAMBLE_WORLD_PREVMAPNAME_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nWldHandle` | `unsigned short` |
|     2 | `sPrevMapName` | `Name3` |


### `PROTO_NC_GAMBLE_ZONE_PREVMAPNAME_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sPrevMapName` | `Name3` |


### `PROTO_NC_GUILD_ACADEMY_DB_ACADEMY_REWARD_ACK`  (sizeof = 61 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `Error` | `unsigned short` |
|     8 | `nCharNo` | `unsigned long` |
|    12 | `nGuildNo` | `unsigned long` |
|    16 | `nWantMoney` | `unsigned __int64` |
|    24 | `nWantItemCnt` | `unsigned long` |
|    28 | `WantItem` | `GUILD_ACADEMY_REWARD_ITEM` |
|    44 | `nSubAcademyMoney` | `unsigned __int64` |
|    52 | `nStorageMoney` | `unsigned __int64` |
|    60 | `nModifyItemCnt` | `unsigned char` |
|    61 | `ModifyItem` | `EACH_MODIFY_ITEM[?] /* 0 bytes */` |


### `PROTO_NC_GUILD_ACADEMY_DB_ACADEMY_REWARD_REQ`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `nCharNo` | `unsigned long` |
|    10 | `nGuildNo` | `unsigned long` |
|    14 | `nLevel` | `unsigned char` |


### `PROTO_NC_GUILD_ACADEMY_REWARDSTORAGE_GRADE_INFO_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |
|     4 | `nGuildNo` | `unsigned long` |
|     8 | `Grade` | `unsigned char` |


### `PROTO_NC_GUILD_ACADEMY_REWARDSTORAGE_ITEM_INFO_ZONE_RNG`  (sizeof = 172 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |
|     7 | `nGldregnum` | `unsigned long` |
|    11 | `Who` | `unsigned char` |
|    12 | `Charid` | `Name4` |
|    28 | `Type` | `unsigned char` |
|    29 | `From` | `SHINE_INVEN_SLOT_INFO` |
|    45 | `To` | `SHINE_INVEN_SLOT_INFO` |
|    61 | `ChangeIteminfo` | `ItemTotalInformation` |


### `PROTO_NC_GUILD_ACADEMY_REWARDSTORAGE_REWARD_ZONE_RNG`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |
|     7 | `nGuildNo` | `unsigned long` |
|    11 | `sCharID` | `Name4` |
|    27 | `Reward_Cen` | `unsigned __int64` |
|    35 | `Storage_Cen` | `unsigned __int64` |
|    43 | `Reward_ItemNum` | `unsigned char` |
|    44 | `Reward_ItemInfo` | `EACH_MODIFY_ITEM[?] /* 0 bytes */` |


### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_CLOSE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCloseType` | `unsigned char` |


### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_CMD`  (sizeof = 37 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `who` | `unsigned char` |
|     1 | `type` | `unsigned char` |
|     2 | `charid` | `Name4` |
|    18 | `item` | `unsigned short` |
|    20 | `slot` | `unsigned char` |
|    21 | `lot` | `unsigned __int64` |
|    21 | `cen` | `unsigned __int64` |
|    29 | `storage_cen` | `unsigned __int64` |


### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_RNG`  (sizeof = 49 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `gldregnum` | `unsigned long` |
|    11 | `why` | `unsigned char` |
|    12 | `who` | `unsigned char` |
|    13 | `type` | `unsigned char` |
|    14 | `slot` | `unsigned char` |
|    15 | `charid` | `Name4` |
|    31 | `item` | `unsigned short` |
|    33 | `lot` | `unsigned __int64` |
|    33 | `cen` | `unsigned __int64` |
|    41 | `storage_cen` | `unsigned __int64` |


### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_OPEN_ACK`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `cen` | `unsigned __int64` |
|    10 | `itemcounter` | `unsigned char` |
|    11 | `itemarray` | `PROTO_ITEMPACKET_INFORM[?] /* 0 bytes */` |


### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_OPEN_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nOpenType` | `unsigned char` |


### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_CMD`  (sizeof = 37 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `who` | `unsigned char` |
|     1 | `type` | `unsigned char` |
|     2 | `charid` | `Name4` |
|    18 | `item` | `unsigned short` |
|    20 | `slot` | `unsigned char` |
|    21 | `lot` | `unsigned __int64` |
|    21 | `cen` | `unsigned __int64` |
|    29 | `storage_cen` | `unsigned __int64` |


### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_GRADE_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `err` | `unsigned short` |
|     8 | `Grade` | `unsigned char` |


### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_GRADE_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |


### `PROTO_NC_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_RNG`  (sizeof = 49 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `gldregnum` | `unsigned long` |
|    11 | `why` | `unsigned char` |
|    12 | `who` | `unsigned char` |
|    13 | `type` | `unsigned char` |
|    14 | `slot` | `unsigned char` |
|    15 | `charid` | `Name4` |
|    31 | `item` | `unsigned short` |
|    33 | `lot` | `unsigned __int64` |
|    33 | `cen` | `unsigned __int64` |
|    41 | `storage_cen` | `unsigned __int64` |


### `PROTO_NC_GUILD_ACADEMY_ZONE_GUILD_ACADEMY_MASTER_BUFF_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `NumOfMaster` | `unsigned long` |
|     4 | `Master` | `GUILD_MASTER_BUFF[?] /* 0 bytes */` |


### `PROTO_NC_GUILD_ACADEMY_ZONE_GUILD_BUFF_CMD`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `BuffName` | `char[32]   /* 32 bytes */` |
|    32 | `NumOfGuild` | `unsigned long` |
|    36 | `Guild` | `unsigned long[0]   /* 0 bytes */` |


### `PROTO_NC_GUILD_ACADEMY_ZONE_MASTER_TELEPORT_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nMasterCharNo` | `unsigned long` |
|     4 | `sTargetCharID` | `Name4` |


### `PROTO_NC_GUILD_ACADEMY_ZONE_MEMBER_GUILD_JOIN_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNo` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |


### `PROTO_NC_GUILD_ACADEMY_ZONE_MEMBER_JOIN_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNo` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |


### `PROTO_NC_GUILD_ACADEMY_ZONE_MEMBER_LEAVE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |


### `PROTO_NC_GUILD_GUILDWARCONFIRM_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `guildwarA` | `PROTO_NC_GUILD_ZONE_WAR_START_CMD` |
|     8 | `guildwarB` | `PROTO_NC_GUILD_ZONE_WAR_START_CMD` |


### `PROTO_NC_GUILD_GUILDWARCONFIRM_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `guildA` | `unsigned long` |
|     4 | `guildB` | `unsigned long` |


### `PROTO_NC_GUILD_GUILDWARSTATUS_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `warnumber` | `unsigned short` |
|     2 | `war` | `PROTO_NC_GUILD_ZONE_WAR_START_CMD[?] /* 0 bytes */` |


### `PROTO_NC_GUILD_MOBGUILD_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mobhandle` | `unsigned short` |
|     2 | `mobguild` | `unsigned long` |


### `PROTO_NC_GUILD_RENAME_REQ`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nItemSlot` | `unsigned char` |
|     1 | `sGuildName` | `Name4` |


### `PROTO_NC_GUILD_RETYPE_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nItemSlot` | `unsigned char` |
|     1 | `nType` | `unsigned char` |


### `PROTO_NC_GUILD_STORAGEOPEN_ACK`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `cen` | `unsigned __int64` |
|    10 | `nToken` | `unsigned __int64` |
|    18 | `itemcounter` | `unsigned char` |
|    19 | `itemarray` | `PROTO_ITEMPACKET_INFORM[?] /* 0 bytes */` |


### `PROTO_NC_GUILD_STORAGEWITHDRAW_CMD`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `type` | `unsigned char` |
|     1 | `isBuy` | `unsigned char` |
|     2 | `charid` | `Name4` |
|    18 | `item` | `unsigned short` |
|    20 | `slot` | `unsigned char` |
|    21 | `lot` | `unsigned __int64` |
|    21 | `cen` | `unsigned __int64` |


### `PROTO_NC_GUILD_STORAGEWITHDRAW_RNG`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `gldregnum` | `unsigned long` |
|    11 | `type` | `unsigned char` |
|    12 | `slot` | `unsigned char` |
|    13 | `isBuy` | `unsigned char` |
|    14 | `charid` | `Name4` |
|    30 | `item` | `unsigned short` |
|    32 | `lot` | `unsigned __int64` |
|    32 | `cen` | `unsigned __int64` |


### `PROTO_NC_GUILD_STORAGE_WITHDRAW_GRADE_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `err` | `unsigned short` |
|     8 | `Grade` | `unsigned char` |


### `PROTO_NC_GUILD_STORAGE_WITHDRAW_GRADE_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |


### `PROTO_NC_GUILD_TOURNAMENTSTART_CMD`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nGuildA` | `unsigned long` |
|     4 | `nGuildB` | `unsigned long` |
|     8 | `nGuildTourNamentType` | `unsigned short` |
|    10 | `nGuildMyRegNumber` | `unsigned long` |


### `PROTO_NC_GUILD_TOURNAMENTSTOPMSG_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `WinnerGuild` | `unsigned long` |
|     4 | `LoseGuild` | `unsigned long` |
|     8 | `AGuildRegNumber` | `unsigned long` |
|    12 | `AGuildScore` | `unsigned short` |
|    14 | `BGuildRegNumber` | `unsigned long` |
|    18 | `BGuildScore` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_DB_JOIN_NEW_ACK`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `Error` | `unsigned short` |
|    10 | `hWMChar` | `unsigned short` |
|    12 | `nCharNo` | `unsigned long` |
|    16 | `nGTNo` | `unsigned long` |
|    20 | `nGuildNo` | `unsigned long` |
|    24 | `nMinLevel` | `unsigned char` |
|    25 | `nMinMember` | `unsigned short` |
|    27 | `nProposalMoney` | `unsigned __int64` |


### `PROTO_NC_GUILD_TOURNAMENT_DB_JOIN_NEW_REQ`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `hWMChar` | `unsigned short` |
|    10 | `nCharNo` | `unsigned long` |
|    14 | `nGTNo` | `unsigned long` |
|    18 | `nGuildNo` | `unsigned long` |
|    22 | `nMinLevel` | `unsigned char` |
|    23 | `nMinMember` | `unsigned short` |
|    25 | `nProposalMoney` | `unsigned __int64` |


### `PROTO_NC_GUILD_TOURNAMENT_DICEGAME_BEFORE_END_TIME_MSG_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `unEndTime` | `unsigned int` |


### `PROTO_NC_GUILD_TOURNAMENT_DICEGAME_START_CMD`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `AGuildRegNumber` | `unsigned long` |
|     4 | `AGuildDelegate` | `unsigned short` |
|     6 | `AGuildDelegateName` | `Name4` |
|    22 | `BGuildRegNumber` | `unsigned long` |
|    26 | `BGuildDelegate` | `unsigned short` |
|    28 | `BGuildDelegateName` | `Name4` |


### `PROTO_NC_GUILD_TOURNAMENT_DICEGAME_THROW_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_DICEGAME_THROW_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GuildRegNumber` | `unsigned long` |
|     4 | `unDiceGameResult` | `unsigned int` |


### `PROTO_NC_GUILD_TOURNAMENT_DICEGAME_THROW_REQ`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_GUILD_TOURNAMENT_END_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTournamentSeed` | `unsigned char` |
|     1 | `nTournamentMapNo` | `unsigned short` |
|     3 | `nStatus` | `unsigned char` |
|     4 | `nWinnerGuildNo` | `unsigned long` |
|     8 | `nLoseGuildNo` | `unsigned long` |


### `PROTO_NC_GUILD_TOURNAMENT_FLAGCAPTURE_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_FLAGCAPTURE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `hPlayer` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_FLAGCAPTURE_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hTargetFlag` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_FLAGCAPTURE_RESULT_CMD`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CIDPlayer` | `Name4` |
|    16 | `CIDFlag` | `Name4` |
|    32 | `gRegGuildNo` | `unsigned long` |
|    36 | `gRegFlagCaptureGuild` | `unsigned long` |


### `PROTO_NC_GUILD_TOURNAMENT_GOLD_REFUND_ZONE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `PartMask` | `unsigned char` |
|     1 | `nEnterGuildNum` | `unsigned short` |
|     3 | `EnterGuildList` | `GT_EnterList[?] /* 0 bytes */` |


### `PROTO_NC_GUILD_TOURNAMENT_ITEM_EFFECT_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GTIiemID` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_ITEM_FLAG_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `FlagScoreMultiple` | `unsigned char` |
|     1 | `nGuildRegNo` | `unsigned long` |


### `PROTO_NC_GUILD_TOURNAMENT_ITEM_PICK_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `GTIiemID` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_ITEM_SCAN_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNum` | `unsigned short` |
|     2 | `Members` | `PROTO_NC_GUILD_TOURNAMENT_ITEM_SCAN_CMD::Memberinfo[?] /* 0 bytes */` |


### `PROTO_NC_GUILD_TOURNAMENT_ITEM_SCAN_CMD::Memberinfo`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `loc` | `SHINE_XY_TYPE` |


### `PROTO_NC_GUILD_TOURNAMENT_MANAGERUSERMSG_CMD`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharID` | `Name4` |


### `PROTO_NC_GUILD_TOURNAMENT_OBSERVER_ENTER_ACK`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `nTournamentSeed` | `unsigned char` |
|     3 | `nGuildRed` | `unsigned long` |
|     7 | `nGuildBlue` | `unsigned long` |


### `PROTO_NC_GUILD_TOURNAMENT_OBSERVER_ENTER_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTournamentSeed` | `unsigned char` |


### `PROTO_NC_GUILD_TOURNAMENT_OBSERVER_OUT_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTournamentSeed` | `unsigned char` |


### `PROTO_NC_GUILD_TOURNAMENT_PLAYERDIEMSG_CMD`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `VictimCharID` | `Name4` |
|    16 | `MurderCharID` | `Name4` |
|    32 | `VictimGuildGrade` | `int` |


### `PROTO_NC_GUILD_TOURNAMENT_PLAYERKILLMSG_CMD`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `VictimCharID` | `Name4` |
|    16 | `MurderCharID` | `Name4` |
|    32 | `VictimGuildGrade` | `int` |


### `PROTO_NC_GUILD_TOURNAMENT_RECALL_ENTER_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_RECALL_ENTER_CMD`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_GUILD_TOURNAMENT_RECALL_ENTER_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ack` | `unsigned char` |


### `PROTO_NC_GUILD_TOURNAMENT_SCORE_CMD`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `PartMask` | `unsigned char` |
|     1 | `regNumber` | `unsigned long` |
|     5 | `unAllScore` | `unsigned int` |
|     9 | `unCaptureFlagCount` | `unsigned int` |
|    13 | `unPlayerNum` | `unsigned int` |
|    17 | `PlayerScore` | `TOURNAMENT_PLAYER_SCORE[?] /* 0 bytes */` |


### `PROTO_NC_GUILD_TOURNAMENT_SKILLPOINT_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wSkillPoint` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_STARTMSG_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nStartBeforeTime` | `int` |


### `PROTO_NC_GUILD_TOURNAMENT_START_CMD`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTournamentSeed` | `unsigned char` |
|     1 | `nTournamentMapNo` | `unsigned short` |
|     3 | `nExploerTimeMin` | `unsigned short` |
|     5 | `nWaitPlayTimeSec` | `unsigned short` |
|     7 | `nPlayTimeMin` | `unsigned short` |
|     9 | `nNo1` | `unsigned long` |
|    13 | `nNo2` | `unsigned long` |


### `PROTO_NC_GUILD_TOURNAMENT_USESKILL_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `wUseSkillNumber` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_USESKILL_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `regGuildNumber` | `unsigned long` |
|     4 | `wUseSkillNumber` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_USESKILL_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wUseSkillNumber` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_ZONE_FIGHTER_ENTER_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTournamentSeed` | `unsigned char` |
|     1 | `nGuildNo` | `unsigned long` |
|     5 | `nCharNo` | `unsigned long` |


### `PROTO_NC_GUILD_TOURNAMENT_ZONE_FIGHTER_OUT_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTournamentSeed` | `unsigned char` |
|     1 | `nGuildNo` | `unsigned long` |
|     5 | `nCharNo` | `unsigned long` |


### `PROTO_NC_GUILD_TOURNAMENT_ZONE_JOIN_NEW_ACK`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Error` | `unsigned short` |
|     2 | `hWMChar` | `unsigned short` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `nGTNo` | `unsigned long` |
|    12 | `nGuildNo` | `unsigned long` |
|    16 | `nMinLevel` | `unsigned char` |
|    17 | `nMinMember` | `unsigned short` |
|    19 | `nProposalMoney` | `unsigned __int64` |


### `PROTO_NC_GUILD_TOURNAMENT_ZONE_JOIN_NEW_REQ`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hWMChar` | `unsigned short` |
|     2 | `nCharNo` | `unsigned long` |
|     6 | `nGTNo` | `unsigned long` |
|    10 | `nGuildNo` | `unsigned long` |
|    14 | `nMinLevel` | `unsigned char` |
|    15 | `nMinMember` | `unsigned short` |
|    17 | `nProposalMoney` | `unsigned __int64` |


### `PROTO_NC_GUILD_TOURNAMENT_ZONE_MEMBERGRADE_ACK`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Error` | `unsigned short` |
|     2 | `nTournamentSeed` | `unsigned char` |
|     3 | `nGuildNo` | `unsigned long` |
|     7 | `nGuildMemberCnt` | `unsigned short` |
|     9 | `MemberGradeList` | `GUILD_TOURNAMENT_MEMBER_GRADE_LIST[?] /* 0 bytes */` |


### `PROTO_NC_GUILD_TOURNAMENT_ZONE_MEMBERGRADE_REQ`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTournamentSeed` | `unsigned char` |
|     1 | `nGuildNo` | `unsigned long` |


### `PROTO_NC_GUILD_TOURNAMENT_ZONE_OBSERVER_ENTER_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `nTournamentSeed` | `unsigned char` |
|     3 | `nPlayerHnd` | `unsigned short` |


### `PROTO_NC_GUILD_TOURNAMENT_ZONE_OBSERVER_ENTER_REQ`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nTournamentSeed` | `unsigned char` |
|     1 | `nPlayerHnd` | `unsigned short` |


### `PROTO_NC_GUILD_WORLD_RENAME_ACK`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `nItemSlot` | `unsigned char` |
|     7 | `sGuildName` | `Name4` |
|    23 | `err` | `unsigned short` |


### `PROTO_NC_GUILD_WORLD_RENAME_CMD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `sGuildName` | `Name4` |


### `PROTO_NC_GUILD_WORLD_RENAME_REQ`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `nItemSlot` | `unsigned char` |
|     7 | `sGuildName` | `Name4` |


### `PROTO_NC_GUILD_WORLD_RETYPE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `nItemSlot` | `unsigned char` |
|     7 | `nType` | `unsigned char` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_GUILD_WORLD_RETYPE_CMD`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `nType` | `unsigned char` |


### `PROTO_NC_GUILD_WORLD_RETYPE_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `nItemSlot` | `unsigned char` |
|     7 | `nType` | `unsigned char` |


### `PROTO_NC_GUILD_WORLD_USE_GUILD_TOKEN_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nGuildNo` | `unsigned long` |
|     4 | `nDecreaseGuildToken` | `unsigned __int64` |
|    12 | `nRestGuildToken` | `unsigned __int64` |


### `PROTO_NC_GUILD_ZONE_DELETE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNo` | `unsigned long` |


### `PROTO_NC_GUILD_ZONE_LIST_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `PartMask` | `unsigned char` |
|     1 | `NumOfList` | `unsigned short` |
|     3 | `GuildZoneList` | `GUILD_ZONE[?] /* 0 bytes */` |


### `PROTO_NC_GUILD_ZONE_LIST_REQ`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_GUILD_ZONE_MAKE_CMD`  (sizeof = 53 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nGuildMasterCharNo` | `unsigned long` |
|     4 | `GuildZone` | `GUILD_ZONE` |


### `PROTO_NC_GUILD_ZONE_MEMBER_JOIN_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNo` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |


### `PROTO_NC_GUILD_ZONE_MEMBER_LEAVE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |


### `PROTO_NC_GUILD_ZONE_USE_GUILD_TOKEN_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nGuildNo` | `unsigned long` |
|     4 | `nDecreaseGuildToken` | `unsigned __int64` |


### `PROTO_NC_GUILD_ZONE_WAR_END_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNo` | `unsigned long` |
|     4 | `nEnemyNo` | `unsigned long` |


### `PROTO_NC_GUILD_ZONE_WAR_KILL_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hWMKiller` | `unsigned short` |
|     2 | `hWMTarget` | `unsigned short` |


### `PROTO_NC_GUILD_ZONE_WAR_START_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNo` | `unsigned long` |
|     4 | `nEnemyNo` | `unsigned long` |


### `PROTO_NC_HOLY_PROMISE_CENTRANSFER_RNG`  (sizeof = 31 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `upper` | `unsigned long` |
|    11 | `cen` | `unsigned long` |
|    15 | `lower` | `Name4` |


### `PROTO_NC_HOLY_PROMISE_DB_GET_CEN_REWARD_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nError` | `unsigned short` |
|     8 | `nMoney` | `unsigned __int64` |


### `PROTO_NC_HOLY_PROMISE_DB_GET_CEN_REWARD_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |


### `PROTO_NC_HOLY_PROMISE_DB_WITHDRAW_CEN_REWARD_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nError` | `unsigned short` |
|     8 | `nMoney` | `unsigned __int64` |


### `PROTO_NC_HOLY_PROMISE_DB_WITHDRAW_CEN_REWARD_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |


### `PROTO_NC_HOLY_PROMISE_GET_CEN_REWARD_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nMoney` | `unsigned __int64` |


### `PROTO_NC_HOLY_PROMISE_MYUPPER_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nUpCharNo` | `unsigned long` |


### `PROTO_NC_HOLY_PROMISE_MYUPPER_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |


### `PROTO_NC_HOLY_PROMISE_MY_UP_ZONE`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |
|     4 | `nUpCharNo` | `unsigned long` |


### `PROTO_NC_HOLY_PROMISE_REWARD_MONEY_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `MemberCharID` | `Name4` |
|    16 | `nMoney` | `unsigned __int64` |


### `PROTO_NC_HOLY_PROMISE_WITHDRAW_CEN_REWARD_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nMoney` | `unsigned __int64` |


### `PROTO_NC_INSTANCE_DUNGEON_CURRENT_DUNGEON_NAME_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `currDungeonMapName` | `Name3` |


### `PROTO_NC_INSTANCE_DUNGEON_DELETE_DUNGEON_CMD`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Category` | `unsigned char` |
|     1 | `RegisterNumber` | `unsigned long` |
|     5 | `currDungeonMapName` | `Name3` |


### `PROTO_NC_INSTANCE_DUNGEON_EMPTY_DUNGEON_CMD`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Category` | `unsigned char` |
|     1 | `RegisterNumber` | `unsigned long` |
|     5 | `currDungeonMapName` | `Name3` |


### `PROTO_NC_INSTANCE_DUNGEON_ERR_OTHERS_IN_ANOTHER`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `otherDungeonMapName` | `Name3` |


### `PROTO_NC_INSTANCE_DUNGEON_FIND_RNG`  (sizeof = 70 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Start` | `ZONERINGLINKAGESTART` |
|     7 | `InstanceCategory` | `INSTANCE_DUNGEON::CATEGORY` |
|    11 | `IDRegisterNumber` | `unsigned long` |
|    15 | `Argument` | `ORToken` |
|    35 | `ServerMapName` | `char[33]   /* 33 bytes */` |
|    68 | `Error` | `unsigned short` |


### `PROTO_NC_INSTANCE_DUNGEON_RESET_COUNTDOWN_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Category` | `unsigned char` |
|     1 | `RegisterNumber` | `unsigned long` |


### `PROTO_NC_ITEMDB_ADMINCREATEFAIL_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `owner` | `unsigned long` |


### `PROTO_NC_ITEMDB_ADMINCREATESUC_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |


### `PROTO_NC_ITEMDB_ADMINCREATE_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `create` | `PROTO_ITEM_CREATE` |


### `PROTO_NC_ITEMDB_BOOTHTRADE_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `headerSeller` | `NETPACKETZONEHEADER` |
|     6 | `headerBuyer` | `NETPACKETZONEHEADER` |
|    12 | `boothlockindexSeller` | `unsigned short` |
|    14 | `boothlockindexBuyer` | `unsigned short` |
|    16 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_BOOTHTRADE_ALL_REQ`  (sizeof = 67 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `headerSeller` | `NETPACKETZONEHEADER` |
|     6 | `headerBuyer` | `NETPACKETZONEHEADER` |
|    12 | `boothSeller` | `unsigned long` |
|    16 | `boothBuyer` | `unsigned long` |
|    20 | `boothlockindexSeller` | `unsigned short` |
|    22 | `boothlockindexBuyer` | `unsigned short` |
|    24 | `itemregnum` | `SHINE_ITEM_REGISTNUMBER` |
|    32 | `itemid` | `unsigned short` |
|    34 | `invenslotBuyer` | `unsigned char` |
|    35 | `nLot` | `unsigned __int64` |
|    43 | `price` | `unsigned __int64` |
|    51 | `boothSellerResultMoney` | `unsigned __int64` |
|    59 | `boothBuyerResultMoney` | `unsigned __int64` |


### `PROTO_NC_ITEMDB_BOOTHTRADE_LOT_REQ`  (sizeof = 98 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `headerSeller` | `NETPACKETZONEHEADER` |
|     6 | `headerBuyer` | `NETPACKETZONEHEADER` |
|    12 | `boothSeller` | `unsigned long` |
|    16 | `boothBuyer` | `unsigned long` |
|    20 | `boothlockindexSeller` | `unsigned short` |
|    22 | `boothlockindexBuyer` | `unsigned short` |
|    24 | `itemid` | `unsigned short` |
|    26 | `lotseller` | `SHINE_ITEM_REGISTNUMBER` |
|    34 | `lotcreate` | `PROTO_ITEM_CREATE` |
|    72 | `createlot` | `unsigned short` |
|    74 | `price` | `unsigned __int64` |
|    82 | `boothSellerResultMoney` | `unsigned __int64` |
|    90 | `boothBuyerResultMoney` | `unsigned __int64` |


### `PROTO_NC_ITEMDB_BOOTHTRADE_MERGE_REQ`  (sizeof = 69 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `headerSeller` | `NETPACKETZONEHEADER` |
|     6 | `headerBuyer` | `NETPACKETZONEHEADER` |
|    12 | `boothSeller` | `unsigned long` |
|    16 | `boothBuyer` | `unsigned long` |
|    20 | `boothlockindexSeller` | `unsigned short` |
|    22 | `boothlockindexBuyer` | `unsigned short` |
|    24 | `itemid` | `unsigned short` |
|    26 | `lotseller` | `SHINE_ITEM_REGISTNUMBER` |
|    34 | `lotbuyer` | `SHINE_ITEM_REGISTNUMBER` |
|    42 | `mergelot` | `unsigned short` |
|    44 | `price` | `unsigned __int64` |
|    52 | `allmove` | `unsigned char` |
|    53 | `boothSellerResultMoney` | `unsigned __int64` |
|    61 | `boothBuyerResultMoney` | `unsigned __int64` |


### `PROTO_NC_ITEMDB_BUYALL_REQ`  (sizeof = 52 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itembuy` | `PROTO_ITEM_CREATE` |
|    38 | `lockindex` | `unsigned short` |
|    40 | `price_cen` | `unsigned long` |
|    44 | `price_fame` | `unsigned long` |
|    48 | `price_gamblecoin` | `unsigned long` |
|    52 | `attr` | `SHINE_ITEM_ATTRIBUTE[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_BUYCAPSULE_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `nBuyNpcHandle` | `unsigned short` |
|     8 | `nLockIndex` | `unsigned short` |
|    10 | `nError` | `unsigned short` |


### `PROTO_NC_ITEMDB_BUYCAPSULE_REQ`  (sizeof = 252 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `nBuyNpcHandle` | `unsigned short` |
|     8 | `nLockIndex` | `unsigned short` |
|    10 | `nMap` | `unsigned short` |
|    12 | `nPrice` | `unsigned __int64` |
|    20 | `nFame` | `unsigned long` |
|    24 | `nGbCoin` | `unsigned long` |
|    28 | `CapsuleLocation` | `ITEM_INVEN` |
|    30 | `CapsuleInfo` | `ItemTotalInformation` |
|   141 | `ContentInfo` | `ItemTotalInformation` |


### `PROTO_NC_ITEMDB_BUYLOT_REQ`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `iteminfo` | `PROTO_ITEM_ATTRCHANGE` |
|    14 | `itemid` | `unsigned short` |
|    16 | `price_cen` | `unsigned long` |
|    20 | `price_fame` | `unsigned long` |
|    24 | `price_gamblecoin` | `unsigned long` |


### `PROTO_NC_ITEMDB_CAPSULEITEM_ACK`  (sizeof = 129 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `nError` | `unsigned short` |
|     8 | `nItemReg` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `nLocation` | `ITEM_INVEN` |
|    18 | `ContentItemInfo` | `ItemTotalInformation` |


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
|     6 | `ErrorCode` | `unsigned short` |
|     8 | `nPartMark` | `unsigned char` |
|     9 | `NumOfChargedItem` | `unsigned short` |
|    11 | `ChargedItemInfoList` | `PROTO_CHARGED_ITEM_INFO[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_CHARGED_LIST_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `nUserNo` | `unsigned long` |
|    10 | `from` | `unsigned short` |
|    12 | `to` | `unsigned short` |


### `PROTO_NC_ITEMDB_CHARGED_WITHDRAW_ACK`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `ChargedItemInfo` | `PROTO_CHARGED_ITEM_INFO` |
|    24 | `ErrorCode` | `unsigned short` |


### `PROTO_NC_ITEMDB_CHARGED_WITHDRAW_REQ`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `nUserNo` | `unsigned long` |
|    12 | `ChargedItemInfo` | `PROTO_CHARGED_ITEM_INFO` |
|    28 | `NumOfItemCreate` | `unsigned char` |
|    29 | `ItemCreate` | `PROTO_ITEM_CREATE[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_CHESTITEM_ACK`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `chest` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `location` | `ITEM_INVEN` |
|    16 | `error` | `unsigned short` |
|    18 | `item` | `PROTO_ITEM_CMD` |


### `PROTO_NC_ITEMDB_CHESTITEM_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `chest` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `location` | `ITEM_INVEN` |


### `PROTO_NC_ITEMDB_CREATEITEMLISTFAIL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `error` | `unsigned short` |


### `PROTO_NC_ITEMDB_CREATEITEMLISTSUC_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |


### `PROTO_NC_ITEMDB_CREATEITEMLIST_REQ`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `owner` | `unsigned long` |
|    10 | `lockindex` | `unsigned short` |
|    12 | `map` | `unsigned short` |
|    14 | `price` | `unsigned __int64` |
|    22 | `itemlist` | `PROTO_ITEM_CMD` |


### `PROTO_NC_ITEMDB_DEPOSIT_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `cen` | `unsigned __int64` |
|    16 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_DEPOSIT_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `usrregnum` | `unsigned long` |
|    12 | `cen` | `unsigned __int64` |


### `PROTO_NC_ITEMDB_DESTROY_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_DISMANTLE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_DISMANTLE_REQ`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `raw` | `PROTO_NC_ITEMDB_DISMANTLE_REQ::__unnamed` |
|    25 | `producenumber` | `unsigned char` |
|    26 | `produce` | `PROTO_NC_ITEMDB_DISMANTLE_REQ::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_ITEMDB_DISMANTLE_REQ::__unnamed`  (sizeof = 43 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `create` | `unsigned char` |
|     1 | `product` | `unsigned short` |
|     3 | `productuselot_log` | `unsigned short` |
|     5 | `iteminfo` | `PROTO_NC_ITEMDB_DISMANTLE_REQ::__unnamed::__unnamed` |


### `PROTO_NC_ITEMDB_DROPALLSUC_ACK`  (sizeof = 111 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `dropitemstr` | `ItemTotalInformation` |


### `PROTO_NC_ITEMDB_DROPALL_REQ`  (sizeof = 123 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemid` | `unsigned short` |
|     2 | `dropitem` | `PROTO_ITEM_VANISH` |
|    12 | `dropitemstr` | `ItemTotalInformation` |


### `PROTO_NC_ITEMDB_DROPLOT_REQ`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemid` | `unsigned short` |
|     2 | `iteminfo` | `PROTO_ITEM_ATTRCHANGE` |
|    14 | `dropitem` | `PROTO_NC_ITEMDB_DROPLOT_REQ::__unnamed` |
|    26 | `nCurLot` | `unsigned long` |


### `PROTO_NC_ITEMDB_DROPLOT_REQ::__unnamed`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemregnum` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `lot` | `unsigned long` |


### `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `err` | `unsigned short` |
|     8 | `itemLockIndex` | `unsigned short` |
|    10 | `equipment` | `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_ACK::__unnamed` |
|    16 | `gem` | `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_ACK::__unnamed` |


### `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_ACK::__unnamed`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemSlot` | `ITEM_INVEN` |
|     2 | `enchantedSocketIndex` | `unsigned char` |
|     3 | `gemItemIndex` | `unsigned short` |
|     5 | `restGemCount` | `unsigned char` |


### `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_REQ`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `itemLockIndex` | `unsigned short` |
|     8 | `equipment` | `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_REQ::__unnamed` |
|    20 | `gem` | `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_REQ::__unnamed` |
|    32 | `gemSocketIndex` | `unsigned char` |
|    33 | `gemLot` | `unsigned char` |


### `PROTO_NC_ITEMDB_ENCHANT_ADD_GEM_REQ::__unnamed`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemRegistNumber` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `itemID` | `unsigned short` |
|    10 | `itemSlot` | `ITEM_INVEN` |


### `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_ACK`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `err` | `unsigned short` |
|     8 | `itemLockIndex` | `unsigned short` |
|    10 | `equipment` | `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_ACK::__unnamed` |
|    13 | `drill` | `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_ACK::__unnamed` |


### `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_ACK::__unnamed`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemSlot` | `ITEM_INVEN` |
|     2 | `itemLot` | `unsigned __int64` |


### `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `itemLockIndex` | `unsigned short` |
|     8 | `equipment` | `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_REQ::__unnamed` |
|    20 | `drill` | `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_REQ::__unnamed` |


### `PROTO_NC_ITEMDB_ENCHANT_ADD_NEW_SOCKET_REQ::__unnamed`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemRegistNumber` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `itemID` | `unsigned short` |
|    10 | `itemSlot` | `ITEM_INVEN` |


### `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_ACK`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `err` | `unsigned short` |
|     8 | `itemLockIndex` | `unsigned short` |
|    10 | `equipment` | `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_ACK::__unnamed` |
|    12 | `remover` | `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_ACK::__unnamed` |
|    22 | `gemSlot` | `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_ACK::__unnamed[3]   /* 3 bytes */` |
|    25 | `needItem` | `unsigned char` |


### `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_ACK::__unnamed`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemSlot` | `ITEM_INVEN` |
|     2 | `itemLot` | `unsigned __int64` |


### `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_REQ`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `itemLockIndex` | `unsigned short` |
|     8 | `equipment` | `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_REQ::__unnamed` |
|    20 | `remover` | `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_REQ::__unnamed` |
|    32 | `gemSlot` | `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_REQ::__unnamed[3]   /* 3 bytes */` |
|    35 | `needItem` | `unsigned char` |


### `PROTO_NC_ITEMDB_ENCHANT_REMOVE_GEM_REQ::__unnamed`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemRegistNumber` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `itemID` | `unsigned short` |
|    10 | `itemSlot` | `ITEM_INVEN` |


### `PROTO_NC_ITEMDB_ENCHANT_SET_GEM_LOT_REQ`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `itemRegistNumber` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `socketCount` | `unsigned char` |
|    15 | `sockets` | `PROTO_NC_ITEMDB_ENCHANT_SET_GEM_LOT_REQ::__unnamed[3]   /* 9 bytes */` |


### `PROTO_NC_ITEMDB_ENCHANT_SET_GEM_LOT_REQ::__unnamed`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `gemItemIndex` | `unsigned short` |
|     2 | `restGemCount` | `unsigned char` |


### `PROTO_NC_ITEMDB_EQUIP_BELONGED_CANCEL_USE_ACK`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `nLockIndex` | `unsigned short` |
|     8 | `nError` | `unsigned short` |
|    10 | `nBelongedItem_ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    18 | `nBelongedItem_ItemID` | `unsigned short` |
|    20 | `nCancelItem_ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    28 | `nCancelItem_ItemID` | `unsigned short` |
|    30 | `nCancelItem_LotCnt` | `unsigned long` |


### `PROTO_NC_ITEMDB_EQUIP_BELONGED_CANCEL_USE_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `nLockIndex` | `unsigned short` |
|     8 | `nBelongedItem_ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `nBelongedItem_ItemID` | `unsigned short` |
|    18 | `nCancelItem_ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    26 | `nCancelItem_ItemID` | `unsigned short` |
|    28 | `nCancelItem_LotCnt` | `unsigned long` |


### `PROTO_NC_ITEMDB_EQUIP_REQ`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `relocA` | `PROTO_ITEM_RELOC` |
|    10 | `relocA_ItemID` | `unsigned short` |
|    12 | `relocA_IsPutOnBelonged` | `SHINE_PUT_ON_BELONGED_ITEM` |
|    16 | `relocB` | `PROTO_ITEM_RELOC` |
|    26 | `lockindex` | `unsigned short` |
|    28 | `unequipnum` | `unsigned char` |
|    29 | `unequip` | `PROTO_NC_ITEMDB_EQUIP_REQ::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_ITEMDB_EQUIP_REQ::__unnamed`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `item` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `invenslot` | `unsigned char` |


### `PROTO_NC_ITEMDB_EXCHANGE_REQ`  (sizeof = 42 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ownerA` | `unsigned long` |
|     4 | `ownerB` | `unsigned long` |
|     8 | `lockindex` | `unsigned short` |
|    10 | `relocA` | `PROTO_ITEM_RELOC` |
|    20 | `relocB` | `PROTO_ITEM_RELOC` |
|    30 | `relocA_ID` | `unsigned short` |
|    32 | `relocB_ID` | `unsigned short` |
|    34 | `relocA_Lot` | `unsigned long` |
|    38 | `relocB_Lot` | `unsigned long` |


### `PROTO_NC_ITEMDB_FURNITURE_ENDURE_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `nFurnitureHnd` | `unsigned short` |
|     4 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `dNewEndureTime` | `ShineDateTime` |
|    16 | `Error` | `unsigned short` |


### `PROTO_NC_ITEMDB_FURNITURE_ENDURE_REQ`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nFurnitureHnd` | `unsigned short` |
|     2 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    10 | `dNewEndureTime` | `ShineDateTime` |
|    14 | `CommonData` | `PROTO_NC_ITEMDB_USE_VARIATION_ITEM_REQ` |


### `PROTO_NC_ITEMDB_GETFROMCAPSULE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `nLockIndex` | `unsigned short` |
|     8 | `nError` | `unsigned short` |


### `PROTO_NC_ITEMDB_GETFROMCAPSULE_REQ`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `nOwner` | `unsigned long` |
|    10 | `nLockIndex` | `unsigned short` |
|    12 | `nItmeReg` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `nReloc` | `PROTO_ITEM_RELOC` |


### `PROTO_NC_ITEMDB_GETFROMCHESTFAIL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_GETFROMCHESTSUC_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |


### `PROTO_NC_ITEMDB_GETFROMCHEST_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `owner` | `unsigned long` |
|    10 | `lockindex` | `unsigned short` |
|    12 | `chest` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `relocnum` | `unsigned char` |
|    21 | `reloc` | `PROTO_ITEM_RELOC[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `cen` | `unsigned __int64` |
|    16 | `Error` | `unsigned short` |


### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `guildregnum` | `unsigned long` |
|    12 | `cen` | `unsigned __int64` |


### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_OPEN_ACK`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zonepackheader` | `NETPACKETZONEHEADER` |
|     6 | `cen` | `unsigned __int64` |
|    14 | `nOpenType` | `unsigned char` |
|    15 | `itemcounter` | `unsigned char` |
|    16 | `itemarray` | `PROTO_ITEMPACKET_TOTAL[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_OPEN_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `guildregnum` | `unsigned long` |
|    10 | `nOpenType` | `unsigned char` |


### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `cen` | `unsigned __int64` |
|    16 | `Error` | `unsigned short` |


### `PROTO_NC_ITEMDB_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `guildregnum` | `unsigned long` |
|    12 | `cen` | `unsigned __int64` |


### `PROTO_NC_ITEMDB_GUILD_STORAGE_WITHDRAW_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `cen` | `unsigned __int64` |
|    16 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_GUILD_STORAGE_WITHDRAW_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `guildregnum` | `unsigned long` |
|    12 | `cen` | `unsigned __int64` |


### `PROTO_NC_ITEMDB_GUILD_TOKEN_BUYALLSUC_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `nGuildNo` | `unsigned long` |
|     6 | `nPriceGuildToken` | `unsigned long` |


### `PROTO_NC_ITEMDB_GUILD_TOKEN_BUYALL_REQ`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNo` | `unsigned long` |
|     4 | `ItemBuy` | `PROTO_ITEM_CREATE` |
|    42 | `lockindex` | `unsigned short` |
|    44 | `nPriceGuildToken` | `unsigned long` |
|    48 | `Attr` | `SHINE_ITEM_ATTRIBUTE[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_GUILD_TOKEN_BUYLOTSUC_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `nGuildNo` | `unsigned long` |
|     6 | `nPriceGuildToken` | `unsigned long` |


### `PROTO_NC_ITEMDB_GUILD_TOKEN_BUYLOT_REQ`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `nNo` | `unsigned long` |
|     6 | `ItemInfo` | `PROTO_ITEM_ATTRCHANGE` |
|    18 | `nItemID` | `unsigned short` |
|    20 | `nPriceGuildToken` | `unsigned long` |


### `PROTO_NC_ITEMDB_INC_DEC_MONEY_REQ`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `owner` | `unsigned long` |
|     4 | `cen` | `unsigned long` |
|     8 | `Positive` | `unsigned char` |


### `PROTO_NC_ITEMDB_ITEMBREAKFAIL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `ErrorCode` | `unsigned short` |


### `PROTO_NC_ITEMDB_ITEMBREAKSUC_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |


### `PROTO_NC_ITEMDB_ITEMBREAK_REQ`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `BreakItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `Owner` | `unsigned long` |
|    20 | `nID` | `unsigned short` |
|    22 | `nLot` | `unsigned short` |
|    24 | `nDeleteChestItem` | `unsigned char` |
|    25 | `DeleteChestItem` | `PROTO_ITEM_VANISH[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_ITEMCHANGE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_ITEMCHANGE_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `itemstruct` | `SHINE_ITEM_STRUCT[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_ITEMTOTALINFORM_ACK`  (sizeof = 111 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemtotalinfo` | `ItemTotalInformation` |


### `PROTO_NC_ITEMDB_ITEMTOTALINFORM_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemregnum` | `SHINE_ITEM_REGISTNUMBER` |


### `PROTO_NC_ITEMDB_ITEMTRADEFAIL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_ITEMTRADESUC_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |


### `PROTO_NC_ITEMDB_ITEMTRADE_REQ`  (sizeof = 42 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `A` | `NETPACKETZONEHEADER` |
|     6 | `B` | `NETPACKETZONEHEADER` |
|    12 | `lockindexA` | `unsigned short` |
|    14 | `lockindexB` | `unsigned short` |
|    16 | `cen` | `PROTO_NC_ITEMDB_ITEMTRADE_REQ::__unnamed` |
|    41 | `tradenumber` | `unsigned char` |
|    42 | `tradeitem` | `PROTO_NC_ITEMDB_ITEMTRADE_REQ::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_ITEMDB_ITEMTRADE_REQ::__unnamed`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `invenA` | `PROTO_NC_ITEMDB_ITEMTRADE_REQ::__unnamed::__unnamed` |
|    13 | `invenB` | `PROTO_NC_ITEMDB_ITEMTRADE_REQ::__unnamed::__unnamed` |


### `PROTO_NC_ITEMDB_ITEMTRADE_REQ::__unnamed::__unnamed`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `regnum` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `id` | `unsigned short` |
|    10 | `lot` | `unsigned short` |
|    12 | `invenloc` | `unsigned char` |


### `PROTO_NC_ITEMDB_MERGESUC_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `lotmain` | `SHINE_ITEM_REGISTNUMBER` |
|    10 | `MergeResultLot` | `unsigned short` |


### `PROTO_NC_ITEMDB_MERGE_REQ`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ownerA` | `unsigned long` |
|     4 | `ownerB` | `unsigned long` |
|     8 | `lockindex` | `unsigned short` |
|    10 | `itemid` | `unsigned short` |
|    12 | `lotmain` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `lotsub` | `SHINE_ITEM_REGISTNUMBER` |
|    28 | `nMainLot` | `unsigned long` |
|    32 | `nSubLot` | `unsigned long` |


### `PROTO_NC_ITEMDB_MINIHOUSE_EFFECT_DEMANDGOOD_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `nEffectHandle` | `unsigned short` |
|    10 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_MINIHOUSE_EFFECT_DEMANDGOOD_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `nDemandItemRegNum` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `nDemandItemID` | `unsigned short` |
|    18 | `nNeedItemLot` | `unsigned long` |
|    22 | `nDemandMoney` | `unsigned __int64` |
|    30 | `nEffectHandle` | `unsigned short` |


### `PROTO_NC_ITEMDB_MINIHOUSE_PORTAL_EFFECT_DEMANDGOOD_REQ`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `nDemandItemRegNum` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `nDemandItemID` | `unsigned short` |
|    18 | `nNeedItemLot` | `unsigned long` |
|    22 | `nDemandMoney` | `unsigned __int64` |
|    30 | `nEffectHandle` | `unsigned short` |


### `PROTO_NC_ITEMDB_MYSTERY_VAULT_MAKEITEM_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `itemLockIndex` | `unsigned short` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_MYSTERY_VAULT_MAKEITEM_REQ`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `itemLockIndex` | `unsigned short` |
|     8 | `UseVaultItemRegNo` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `UseVaultItemID` | `unsigned short` |
|    18 | `itemcreatecount` | `unsigned char` |
|    19 | `itemcreate` | `PROTO_ITEM_CREATE[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_OPENSTORAGE_ACK`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zonepackheader` | `NETPACKETZONEHEADER` |
|     6 | `curpage` | `unsigned char` |
|     7 | `nOpenType` | `unsigned char` |
|     8 | `cen` | `unsigned __int64` |
|    16 | `itemcounter` | `unsigned char` |
|    17 | `itemarray` | `PROTO_ITEMPACKET_TOTAL[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_OPENSTORAGE_FAIL_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_OPENSTORAGE_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `page` | `unsigned char` |
|     7 | `usrregnum` | `unsigned long` |
|    11 | `nOpenType` | `unsigned char` |


### `PROTO_NC_ITEMDB_OPEN_GUILD_STORAGE_ACK`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zonepackheader` | `NETPACKETZONEHEADER` |
|     6 | `cen` | `unsigned __int64` |
|    14 | `nToken` | `unsigned __int64` |
|    22 | `itemcounter` | `unsigned char` |
|    23 | `itemarray` | `PROTO_ITEMPACKET_TOTAL[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_OPEN_GUILD_STORAGE_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `guildregnum` | `unsigned long` |


### `PROTO_NC_ITEMDB_PICKMERGE_REQ`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemid` | `unsigned short` |
|     2 | `itemonfield` | `SHINE_ITEM_REGISTNUMBER` |
|    10 | `nCellItemLot` | `unsigned long` |
|    14 | `itempick` | `PROTO_ITEM_ATTRCHANGE` |


### `PROTO_NC_ITEMDB_PICKMONEY_REQ`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `owner` | `unsigned long` |
|     4 | `itemonfield` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `cen` | `unsigned long` |


### `PROTO_NC_ITEMDB_PRODUCE_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `product` | `unsigned short` |
|    10 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_PRODUCE_REQ`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `product` | `unsigned short` |
|    10 | `produce` | `PROTO_NC_ITEMDB_PRODUCE_REQ::__unnamed` |
|    49 | `rawnumber` | `unsigned char` |
|    50 | `raw` | `PROTO_NC_ITEMDB_PRODUCE_REQ::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_ITEMDB_PRODUCE_REQ::__unnamed`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `create` | `unsigned char` |
|     1 | `iteminfo` | `PROTO_NC_ITEMDB_PRODUCE_REQ::__unnamed::__unnamed` |


### `PROTO_NC_ITEMDB_QUESTREWARD_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `nQuestID` | `unsigned short` |
|    10 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_QUESTREWARD_REQ`  (sizeof = 45 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `nQuestID` | `unsigned short` |
|    10 | `money` | `unsigned long` |
|    14 | `exp` | `unsigned long` |
|    18 | `fame` | `unsigned long` |
|    22 | `current_money` | `unsigned __int64` |
|    30 | `current_exp` | `unsigned __int64` |
|    38 | `current_fame` | `unsigned long` |
|    42 | `ct_type` | `unsigned char` |
|    43 | `ct_elementno` | `unsigned char` |
|    44 | `itemcreatecount` | `unsigned char` |
|    45 | `itemcreate` | `PROTO_ITEM_CREATE[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_REINFORCEUNEQUIP_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `unequ` | `PROTO_ITEM_RELOC` |
|    10 | `lockindex` | `unsigned short` |
|    12 | `equloc` | `ITEM_INVEN` |


### `PROTO_NC_ITEMDB_RELOC_REQ`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ownerA` | `unsigned long` |
|     4 | `ownerB` | `unsigned long` |
|     8 | `lockindex` | `unsigned short` |
|    10 | `item` | `PROTO_ITEM_RELOC` |
|    20 | `inven_from` | `ITEM_INVEN` |
|    22 | `nFromItemID` | `unsigned short` |
|    24 | `nFromItemLot` | `unsigned long` |


### `PROTO_NC_ITEMDB_SELLALL_REQ`  (sizeof = 27 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `itemsell` | `PROTO_ITEM_VANISH` |
|    12 | `price` | `unsigned long` |
|    16 | `itemid` | `unsigned short` |
|    18 | `nLot` | `unsigned __int64` |
|    26 | `nDeleteChestItem` | `unsigned char` |
|    27 | `DeleteChestItem` | `PROTO_ITEM_VANISH[?] /* 0 bytes */` |


### `PROTO_NC_ITEMDB_SELLLOT_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `iteminfo` | `PROTO_ITEM_ATTRCHANGE` |
|    14 | `itemid` | `unsigned short` |
|    16 | `price` | `unsigned long` |


### `PROTO_NC_ITEMDB_SOULSTONEBUY_REQ`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `flag` | `unsigned char` |
|     5 | `number` | `unsigned short` |
|     7 | `price` | `unsigned __int64` |
|    15 | `lockindex` | `unsigned short` |


### `PROTO_NC_ITEMDB_SPLITSUC_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `lotmain` | `SHINE_ITEM_REGISTNUMBER` |
|    10 | `RemainLot` | `unsigned short` |


### `PROTO_NC_ITEMDB_SPLIT_N_MERGESUC_ACK`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `FromItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    10 | `ToItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    18 | `MoveLot` | `unsigned short` |
|    20 | `ResultLotFrom` | `unsigned short` |
|    22 | `ResultLotTo` | `unsigned short` |


### `PROTO_NC_ITEMDB_SPLIT_N_MERGE_REQ`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `lockindex` | `unsigned short` |
|     2 | `Itemid` | `unsigned short` |
|     4 | `MoveLot` | `unsigned short` |
|     6 | `FromOwner` | `unsigned long` |
|    10 | `FromItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    18 | `ToOwner` | `unsigned long` |
|    22 | `ToItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    30 | `nFromItemLot` | `unsigned long` |
|    34 | `nToItemLot` | `unsigned long` |


### `PROTO_NC_ITEMDB_SPLIT_REQ`  (sizeof = 62 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ownerA` | `unsigned long` |
|     4 | `ownerB` | `unsigned long` |
|     8 | `lockindex` | `unsigned short` |
|    10 | `itemid` | `unsigned short` |
|    12 | `lotmain` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `lotcreate` | `PROTO_ITEM_CREATE` |
|    58 | `splitlot` | `unsigned short` |
|    60 | `MainLot` | `unsigned short` |


### `PROTO_NC_ITEMDB_UNEQUIP_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `unequ` | `PROTO_ITEM_RELOC` |
|    10 | `lockindex` | `unsigned short` |
|    12 | `equloc` | `ITEM_INVEN` |


### `PROTO_NC_ITEMDB_UPGRADE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_UPGRADE_REQ`  (sizeof = 78 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `upgrade_item_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `upgrade_item_id` | `unsigned short` |
|    18 | `upgrade_item_adj_level` | `unsigned char` |
|    19 | `upgrade_item_destroy` | `unsigned char` |
|    20 | `upgrade_item_new_level` | `unsigned char` |
|    21 | `upgrade_item_fail_count` | `unsigned char` |
|    22 | `raw_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    30 | `raw_left_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    38 | `raw_right_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    46 | `raw_middle_regnum` | `SHINE_ITEM_REGISTNUMBER` |
|    54 | `gift_money` | `unsigned __int64` |
|    62 | `raw_item_id` | `unsigned short` |
|    64 | `raw_left_item_id` | `unsigned short` |
|    66 | `raw_right_item_id` | `unsigned short` |
|    68 | `raw_lot` | `unsigned char` |
|    69 | `raw_left_lot` | `unsigned char` |
|    70 | `raw_right_lot` | `unsigned char` |
|    71 | `raw_middle_item_id` | `unsigned short` |
|    73 | `raw_middle_lot` | `unsigned char` |
|    74 | `is_randomoption_enable` | `unsigned char` |
|    75 | `upgrade_randomoption` | `ItemOptionStorage::Element` |


### `PROTO_NC_ITEMDB_USEALL_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attritemregnum` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `lockindex` | `unsigned short` |


### `PROTO_NC_ITEMDB_USELOT_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `attr` | `PROTO_CHANGEATTR` |
|    12 | `lockindex` | `unsigned short` |


### `PROTO_NC_ITEMDB_USE_VARIATION_ITEM_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `attr` | `PROTO_CHANGEATTR` |
|    18 | `lockindex` | `unsigned short` |
|    20 | `isClear` | `unsigned char` |


### `PROTO_NC_ITEMDB_WEAPONENDURESET_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `WeaponKey` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `ChargedNumber` | `unsigned long` |


### `PROTO_NC_ITEMDB_WEAPONENDURE_CHARGE_ACK`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `WeaponKey` | `SHINE_ITEM_REGISTNUMBER` |
|    16 | `WeaponSlot` | `ITEM_INVEN` |
|    18 | `ChargedNumber` | `unsigned long` |
|    22 | `Error` | `unsigned short` |


### `PROTO_NC_ITEMDB_WEAPONENDURE_CHARGE_REQ`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `WeaponKey` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `WeaponSlot` | `ITEM_INVEN` |
|    10 | `ChargedNumber` | `unsigned long` |
|    14 | `CommonData` | `PROTO_NC_ITEMDB_USE_VARIATION_ITEM_REQ` |


### `PROTO_NC_ITEMDB_WITHDRAW_ACK`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `cen` | `unsigned __int64` |
|    16 | `err` | `unsigned short` |


### `PROTO_NC_ITEMDB_WITHDRAW_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `usrregnum` | `unsigned long` |
|    12 | `cen` | `unsigned __int64` |


### `PROTO_NC_ITEM_AUTO_ARRANGE_INVEN_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_ITEM_BUY_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemid` | `unsigned short` |
|     2 | `lot` | `unsigned long` |


### `PROTO_NC_ITEM_CELLCHANGE_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `exchange` | `ITEM_INVEN` |
|     2 | `location` | `ITEM_INVEN` |
|     4 | `item` | `SHINE_ITEM_VAR_STRUCT` |


### `PROTO_NC_ITEM_CHARGEDINVENOPEN_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ErrorCode` | `unsigned short` |
|     2 | `nPartMark` | `unsigned char` |
|     3 | `NumOfChargedItem` | `unsigned short` |
|     5 | `ChargedItemInfoList` | `PROTO_CHARGED_ITEM_INFO[?] /* 0 bytes */` |


### `PROTO_NC_ITEM_CHARGEDINVENOPEN_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `page` | `unsigned short` |


### `PROTO_NC_ITEM_CHARGED_WITHDRAW_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ItemOrderNo` | `unsigned long` |
|     4 | `err` | `unsigned short` |


### `PROTO_NC_ITEM_CHARGED_WITHDRAW_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ItemOrderNo` | `unsigned long` |


### `PROTO_NC_ITEM_DEPOSIT_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_ITEM_DEPOSIT_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |


### `PROTO_NC_ITEM_DICE_GAME_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemhandle` | `unsigned short` |
|     2 | `nGameTimesec` | `int` |


### `PROTO_NC_ITEM_DICE_GAME_RESULT_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `error` | `unsigned short` |
|     2 | `WinnerID` | `Name4` |
|    18 | `diceresult` | `int` |
|    22 | `PickItemID` | `unsigned short` |


### `PROTO_NC_ITEM_DICE_GAME_START_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `error` | `unsigned short` |


### `PROTO_NC_ITEM_DICE_GAME_START_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charhandle` | `unsigned short` |
|     2 | `diceresult` | `int` |


### `PROTO_NC_ITEM_DICE_GAME_START_REQ`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemhandle` | `unsigned short` |
|     2 | `IsYes` | `unsigned char` |


### `PROTO_NC_ITEM_DISMANTLE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `item` | `unsigned char` |


### `PROTO_NC_ITEM_DROP_REQ`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `ITEM_INVEN` |
|     2 | `lot` | `unsigned long` |
|     6 | `loc` | `SHINE_XY_TYPE` |


### `PROTO_NC_ITEM_ENCHANT_ADD_GEM_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `equipment` | `ITEM_INVEN` |
|     4 | `GemID` | `unsigned short` |
|     6 | `gemSocketIndex` | `unsigned char` |


### `PROTO_NC_ITEM_ENCHANT_ADD_GEM_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `equipment` | `ITEM_INVEN` |
|     2 | `gem` | `ITEM_INVEN` |


### `PROTO_NC_ITEM_ENCHANT_ADD_NEW_SOCKET_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `equipment` | `ITEM_INVEN` |
|     4 | `socketCount` | `unsigned char` |


### `PROTO_NC_ITEM_ENCHANT_ADD_NEW_SOCKET_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `equipment` | `ITEM_INVEN` |
|     2 | `drill` | `ITEM_INVEN` |


### `PROTO_NC_ITEM_ENCHANT_REMOVE_GEM_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `equipment` | `ITEM_INVEN` |
|     4 | `gemSocketIndex` | `unsigned char` |


### `PROTO_NC_ITEM_ENCHANT_REMOVE_GEM_REQ`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `equipment` | `ITEM_INVEN` |
|     2 | `remover` | `ITEM_INVEN` |
|     4 | `gemSocketIndex` | `unsigned char` |


### `PROTO_NC_ITEM_ENCHANT_SET_GEM_LOT_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `equipment` | `ITEM_INVEN` |
|     2 | `gemSocketIndex` | `unsigned char` |
|     3 | `restCount` | `unsigned char` |


### `PROTO_NC_ITEM_EQUIPCHANGE_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `exchange` | `ITEM_INVEN` |
|     2 | `location` | `unsigned char` |
|     3 | `item` | `SHINE_ITEM_VAR_STRUCT` |


### `PROTO_NC_ITEM_EQUIP_BELONGED_CANCEL_USE_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |


### `PROTO_NC_ITEM_EQUIP_BELONGED_CANCEL_USE_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCancelItemInvenSlot` | `unsigned char` |
|     1 | `nBelongedItemInvenSlot` | `unsigned char` |


### `PROTO_NC_ITEM_EQUIP_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `unsigned char` |


### `PROTO_NC_ITEM_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |
|     8 | `Error` | `unsigned short` |


### `PROTO_NC_ITEM_GUILD_ACADEMY_REWARD_STORAGE_DEPOSIT_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |


### `PROTO_NC_ITEM_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |
|     8 | `Error` | `unsigned short` |


### `PROTO_NC_ITEM_GUILD_ACADEMY_REWARD_STORAGE_WITHDRAW_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |


### `PROTO_NC_ITEM_GUILD_STORAGE_WITHDRAW_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_ITEM_GUILD_STORAGE_WITHDRAW_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |


### `PROTO_NC_ITEM_ITEMBREAK_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `ITEM_INVEN` |


### `PROTO_NC_ITEM_MH_FURNITURE_ENDURE_KIT_USE_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `dEndEndureDate` | `ShineDateTime` |
|     4 | `Error` | `unsigned short` |


### `PROTO_NC_ITEM_MH_FURNITURE_ENDURE_KIT_USE_REQ`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nInvenSlot` | `unsigned char` |
|     1 | `nFurnitureHnd` | `unsigned short` |


### `PROTO_NC_ITEM_OPENSTORAGEPAGE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `page` | `unsigned char` |


### `PROTO_NC_ITEM_PICKOTHER_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemid` | `unsigned short` |
|     2 | `towho` | `unsigned short` |


### `PROTO_NC_ITEM_PICK_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemid` | `unsigned short` |
|     2 | `lot` | `unsigned long` |
|     6 | `error` | `unsigned short` |
|     8 | `itemhandle` | `unsigned short` |


### `PROTO_NC_ITEM_PICK_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemhandle` | `unsigned short` |


### `PROTO_NC_ITEM_RELOC_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `from` | `ITEM_INVEN` |
|     2 | `to` | `ITEM_INVEN` |


### `PROTO_NC_ITEM_REWARDINVENOPEN_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemcounter` | `unsigned char` |
|     1 | `itemarray` | `PROTO_ITEMPACKET_INFORM[?] /* 0 bytes */` |


### `PROTO_NC_ITEM_REWARDINVENOPEN_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `page` | `unsigned short` |


### `PROTO_NC_ITEM_RINGEQUIP_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `unsigned char` |
|     1 | `equip` | `unsigned char` |


### `PROTO_NC_ITEM_SELL_REQ`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `unsigned char` |
|     1 | `lot` | `unsigned long` |


### `PROTO_NC_ITEM_SOMEONEPICK_CMD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `pickerID` | `Name4` |
|    16 | `itemID` | `unsigned short` |
|    18 | `nItemLot` | `unsigned long` |


### `PROTO_NC_ITEM_SOMEONEUSE_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `user` | `unsigned short` |
|     2 | `useloc` | `SHINE_XY_TYPE` |
|    10 | `useditem` | `unsigned short` |


### `PROTO_NC_ITEM_SPLIT_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `from` | `ITEM_INVEN` |
|     2 | `to` | `ITEM_INVEN` |
|     4 | `lot` | `unsigned long` |


### `PROTO_NC_ITEM_UNEQUIP_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slotequip` | `unsigned char` |
|     1 | `slotinven` | `unsigned char` |


### `PROTO_NC_ITEM_UPGRADE_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_ITEM_UPGRADE_REQ`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `equip` | `unsigned char` |
|     1 | `raw` | `unsigned char` |
|     2 | `raw_left` | `unsigned char` |
|     3 | `raw_right` | `unsigned char` |
|     4 | `gift_money` | `unsigned long` |
|     8 | `raw_middle` | `unsigned char` |


### `PROTO_NC_ITEM_USE_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `error` | `unsigned short` |
|     2 | `useditem` | `unsigned short` |
|     4 | `invenType` | `unsigned char` |


### `PROTO_NC_ITEM_USE_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `invenslot` | `unsigned char` |
|     1 | `invenType` | `unsigned char` |


### `PROTO_NC_ITEM_WEAPONENDURESET_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ChargedNumber` | `unsigned long` |


### `PROTO_NC_ITEM_WEAPONENDURE_CHARGE_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ChargedNumber` | `unsigned long` |
|     4 | `WeaponSlot` | `ITEM_INVEN` |
|     6 | `Error` | `unsigned short` |


### `PROTO_NC_ITEM_WEAPONENDURE_CHARGE_REQ`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ChargingSlot` | `unsigned char` |
|     1 | `WeaponSlot` | `ITEM_INVEN` |


### `PROTO_NC_ITEM_WITHDRAW_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_ITEM_WITHDRAW_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |


### `PROTO_NC_KQ_ENTRYRESPONCE_ACK`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Reply` | `unsigned char` |
|     1 | `EncHandle` | `unsigned long` |


### `PROTO_NC_KQ_JOINER`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |


### `PROTO_NC_KQ_MOBKILLNUMBER_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CurrentMobKill` | `unsigned short` |
|     2 | `DemandMobKill` | `unsigned short` |


### `PROTO_NC_KQ_RESTDEADNUM_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `number` | `unsigned char` |


### `PROTO_NC_KQ_REWARDFAIL_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `error` | `unsigned short` |


### `PROTO_NC_KQ_REWARDSUC_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |


### `PROTO_NC_KQ_REWARD_REQ`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `fame` | `unsigned long` |
|     4 | `cen` | `unsigned __int64` |
|    12 | `itmlst` | `PROTO_NC_ITEMDB_CREATEITEMLIST_REQ` |


### `PROTO_NC_KQ_SCORE_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `num` | `unsigned char` |
|     1 | `base` | `unsigned long` |
|     5 | `array` | `PROTO_NC_KQ_SCORE_CMD::KQScore[?] /* 0 bytes */` |


### `PROTO_NC_KQ_SCORE_CMD::KQScore`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name4` |
|    16 | `flag` | `PROTO_NC_KQ_SCORE_CMD::KQScore::__unnamed` |
|    18 | `killpoint` | `unsigned long` |
|    22 | `downpoint` | `unsigned long` |
|    26 | `contribute` | `unsigned long` |
|    30 | `score` | `unsigned long` |


### `PROTO_NC_KQ_SCORE_CMD::KQScore::__unnamed`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `medal` | `<UNRESOLVED:0x307d>` |
|     0 | `gender` | `<UNRESOLVED:0x307e>` |
|     1 | `chrcls` | `<UNRESOLVED:0x31aa>` |


### `PROTO_NC_KQ_SCORE_SIMPLE_CMD`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `num` | `unsigned char` |
|     1 | `base` | `unsigned long` |
|     5 | `self` | `PROTO_NC_KQ_SCORE_SIMPLE_CMD::__unnamed` |
|    18 | `array` | `PROTO_NC_KQ_SCORE_SIMPLE_CMD::KQScore[?] /* 0 bytes */` |


### `PROTO_NC_KQ_SCORE_SIMPLE_CMD::KQScore`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name4` |
|    16 | `kill` | `unsigned long` |
|    20 | `down` | `unsigned long` |
|    24 | `score` | `unsigned long` |


### `PROTO_NC_KQ_SCORE_SIMPLE_CMD::__unnamed`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `rank` | `unsigned char` |
|     1 | `kill` | `unsigned long` |
|     5 | `down` | `unsigned long` |
|     9 | `score` | `unsigned long` |


### `PROTO_NC_KQ_W2Z_DESTROY_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hquest` | `unsigned long` |


### `PROTO_NC_KQ_W2Z_MAKE_REQ`  (sizeof = 324 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `KQInfo` | `PROTO_KQ_INFO` |


### `PROTO_NC_KQ_W2Z_START_CMD`  (sizeof = 326 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `KQInfo` | `PROTO_KQ_INFO` |
|   324 | `NumOfJoiner` | `unsigned short` |
|   326 | `JoinerArray` | `PROTO_NC_KQ_JOINER[?] /* 0 bytes */` |


### `PROTO_NC_KQ_Z2W_END_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Handle` | `unsigned long` |


### `PROTO_NC_KQ_Z2W_MAKE_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hquest` | `unsigned long` |
|     4 | `err` | `unsigned short` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_0`  (sizeof = 68 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nTargetCharNo` | `unsigned long` |
|    36 | `nTargetID` | `unsigned long` |
|    40 | `nItemKey` | `unsigned __int64` |
|    48 | `nInt1` | `unsigned long` |
|    52 | `nInt2` | `unsigned long` |
|    56 | `nInt3` | `unsigned long` |
|    60 | `nBigint1` | `unsigned __int64` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_1`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_2`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nInt1` | `unsigned long` |
|    36 | `nInt2` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_3`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nTargetCharNo` | `unsigned long` |
|    36 | `nTargetID` | `unsigned long` |
|    40 | `nInt1` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_4`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nInt1` | `unsigned long` |
|    36 | `nInt2` | `unsigned long` |
|    40 | `nInt3` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_5`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `nInt1` | `unsigned long` |
|    12 | `nInt2` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_6`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nInt1` | `unsigned long` |
|    36 | `nBigint1` | `unsigned __int64` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_7`  (sizeof = 44 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nTargetCharNo` | `unsigned long` |
|    36 | `nInt1` | `unsigned long` |
|    40 | `nInt2` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_8`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nInt1` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_9`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nItemKey` | `unsigned __int64` |
|    40 | `nInt1` | `unsigned long` |
|    44 | `nInt2` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_A`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `nTargetCharNo` | `unsigned long` |
|    12 | `nTargetID` | `unsigned long` |
|    16 | `nItemKey` | `unsigned __int64` |
|    24 | `nInt1` | `unsigned long` |
|    28 | `nInt2` | `unsigned long` |
|    32 | `nInt3` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_B`  (sizeof = 60 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nTargetCharNo` | `unsigned long` |
|    36 | `nTargetID` | `unsigned long` |
|    40 | `nItemKey` | `unsigned __int64` |
|    48 | `nInt1` | `unsigned long` |
|    52 | `nInt2` | `unsigned long` |
|    56 | `nInt3` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_C`  (sizeof = 56 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nTargetID` | `unsigned long` |
|    36 | `nItemKey` | `unsigned __int64` |
|    44 | `nInt1` | `unsigned long` |
|    48 | `nInt2` | `unsigned long` |
|    52 | `nInt3` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_D`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `nItemKey` | `unsigned __int64` |
|    16 | `nInt1` | `unsigned long` |
|    20 | `nInt2` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_E`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `sMap` | `Name3` |
|    16 | `nMapX` | `unsigned long` |
|    20 | `nMapY` | `unsigned long` |
|    24 | `nMapZ` | `unsigned long` |
|    28 | `nTargetID` | `unsigned long` |
|    32 | `nItemKey` | `unsigned __int64` |
|    40 | `nInt1` | `unsigned long` |
|    44 | `nInt2` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_F`  (sizeof = 64 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nTargetID` | `unsigned long` |
|    36 | `nItemKey` | `unsigned __int64` |
|    44 | `nInt1` | `unsigned long` |
|    48 | `nInt2` | `unsigned long` |
|    52 | `nInt3` | `unsigned long` |
|    56 | `nBigint1` | `unsigned __int64` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_G`  (sizeof = 60 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nTargetID` | `unsigned long` |
|    36 | `nItemKey` | `unsigned __int64` |
|    44 | `nInt1` | `unsigned long` |
|    48 | `nInt2` | `unsigned long` |
|    52 | `nBigint1` | `unsigned __int64` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_H`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nTargetCharNo` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_I`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nTargetCharNo` | `unsigned long` |
|    36 | `nTargetID` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_J`  (sizeof = 52 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nTargetID` | `unsigned long` |
|    36 | `nItemKey` | `unsigned __int64` |
|    44 | `nInt1` | `unsigned long` |
|    48 | `nInt2` | `unsigned long` |


### `PROTO_NC_LOG_GAME_DATA_TYPE_K`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nType` | `unsigned long` |
|     4 | `nCharNo` | `unsigned long` |
|     8 | `sMap` | `Name3` |
|    20 | `nMapX` | `unsigned long` |
|    24 | `nMapY` | `unsigned long` |
|    28 | `nMapZ` | `unsigned long` |
|    32 | `nInt1` | `unsigned long` |
|    36 | `nInt2` | `unsigned long` |
|    40 | `nBigint1` | `unsigned __int64` |


### `PROTO_NC_LOG_GAME_ITEM_CW_BREAKATZERO`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `mappos` | `MAPPOS` |


### `PROTO_NC_LOG_GAME_ITEM_PUT_ON_BELONGED`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `iteminvenfrom` | `ITEM_INVEN` |
|    14 | `iteminvento` | `ITEM_INVEN` |


### `PROTO_NC_LOG_GAME_ITEM_UPGRADE`  (sizeof = 43 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `mappos` | `MAPPOS` |
|    24 | `itemkey` | `SHINE_ITEM_REGISTNUMBER` |
|    32 | `itemoldlevel` | `unsigned char` |
|    33 | `itemnewlevel` | `unsigned char` |
|    34 | `left_item_id` | `unsigned short` |
|    36 | `right_item_id` | `unsigned short` |
|    38 | `middle_item_id` | `unsigned short` |
|    40 | `randomoption_elemental` | `ItemOptionStorage::Element` |


### `PROTO_NC_LOG_GAME_KQ_ENTER`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `mappos` | `MAPPOS` |
|    24 | `KQ_ID` | `unsigned short` |
|    26 | `KQ_Handle` | `unsigned long` |


### `PROTO_NC_LOG_GAME_KQ_LEAVE`  (sizeof = 31 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `mappos` | `MAPPOS` |
|    24 | `KQ_ID` | `unsigned short` |
|    26 | `KQ_Handle` | `unsigned long` |
|    30 | `Leave_Status` | `unsigned char` |


### `PROTO_NC_LOG_GAME_LEVEL_UP`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `mappos` | `MAPPOS` |
|    24 | `level` | `unsigned char` |
|    25 | `currentexp` | `unsigned __int64` |


### `PROTO_NC_LOG_GAME_MINIHOUSE_BUILDING`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `mappos` | `MAPPOS` |


### `PROTO_NC_LOG_GAME_MINIHOUSE_VISIT`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `owner_chrregnum` | `unsigned long` |
|     4 | `visitor_chrregnum` | `unsigned long` |
|     8 | `mappos` | `MAPPOS` |


### `PROTO_NC_LOG_GAME_MK2_DEAD`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mobid` | `unsigned short` |
|     2 | `mappos` | `MAPPOS` |


### `PROTO_NC_LOG_GAME_MK2_START`  (sizeof = 28 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `nLevel` | `unsigned char` |
|     5 | `nClass` | `unsigned char` |
|     6 | `mobid` | `unsigned short` |
|     8 | `mappos` | `MAPPOS` |


### `PROTO_NC_LOG_GAME_MKED`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `mappos` | `MAPPOS` |
|    24 | `mobid` | `unsigned short` |
|    26 | `exp` | `unsigned __int64` |


### `PROTO_NC_LOG_GAME_MK_DROP_ITEM`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nItemID` | `unsigned short` |
|     2 | `nLot` | `unsigned short` |
|     4 | `nItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    12 | `mobid` | `unsigned short` |
|    14 | `mappos` | `MAPPOS` |


### `PROTO_NC_LOG_GAME_MK_GETEXPINFIELD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `mobid` | `unsigned short` |
|     6 | `number` | `unsigned short` |
|     8 | `exp` | `unsigned long` |


### `PROTO_NC_LOG_GAME_PKED`  (sizeof = 33 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `mappos` | `MAPPOS` |
|    24 | `killerdiechrrgnum` | `unsigned long` |
|    28 | `pkcount` | `unsigned long` |
|    32 | `pkyellowtime` | `unsigned char` |


### `PROTO_NC_LOG_GMAE_LOGOUT_ZONEINFO`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `mappos` | `MAPPOS` |
|    24 | `StoreType` | `unsigned char` |
|    25 | `CharExp` | `unsigned __int64` |
|    33 | `CharMoney` | `unsigned __int64` |


### `PROTO_NC_LOG_MAP_NOBASE_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `map` | `Name3` |
|    12 | `x` | `unsigned long` |
|    16 | `y` | `unsigned long` |


### `PROTO_NC_LOG_REGENLOCATESAVE_CMD`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ChrRegNum` | `unsigned long` |
|     4 | `SaveMap` | `Name3` |
|    16 | `SaveLocate` | `SHINE_XY_TYPE` |
|    24 | `KQHandle` | `unsigned long` |
|    28 | `InvalidLocate` | `SHINE_XY_TYPE` |
|    36 | `Flag` | `unsigned short` |


### `PROTO_NC_LOG_WEDDING_HALL_CANCEL`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharRegisterNumber` | `unsigned long` |
|     4 | `PartnerRegisterNumber` | `unsigned long` |


### `PROTO_NC_LOG_WEDDING_HALL_RESERVE`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharRegisterNumber` | `unsigned long` |
|     4 | `PartnerRegisterNumber` | `unsigned long` |


### `PROTO_NC_LOG_WEDDING_HALL_START`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `CharRegisterNumber` | `unsigned long` |
|     4 | `PartnerRegisterNumber` | `unsigned long` |


### `PROTO_NC_MAP_ADELIA_MAP_MOVETO_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nErr` | `unsigned short` |
|     2 | `nLocation` | `SHINE_XY_TYPE` |


### `PROTO_NC_MAP_EXPBONUS_RNG`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `bonus` | `unsigned short` |


### `PROTO_NC_MAP_FIELD_PK_STATUS_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `status` | `unsigned char` |


### `PROTO_NC_MAP_ITEMBONUS_RNG`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `bonus` | `unsigned short` |


### `PROTO_NC_MAP_LINKEND_CMD`  (sizeof = 47 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wmshandle` | `unsigned short` |
|     2 | `zonehandle` | `unsigned short` |
|     4 | `chrregnum` | `unsigned long` |
|     8 | `map` | `Name3` |
|    20 | `zoneid` | `unsigned char` |
|    21 | `location` | `SHINE_XY_TYPE` |
|    29 | `StoreType` | `unsigned char` |
|    30 | `CharExp` | `unsigned __int64` |
|    38 | `CharMoney` | `unsigned __int64` |
|    46 | `IsPartyRelease` | `unsigned char` |


### `PROTO_NC_MAP_LINKRESERVE_ACK`  (sizeof = 37 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `linkother` | `PROTO_NC_CHAR_REVIVEOTHER_CMD` |


### `PROTO_NC_MAP_LINKRESERVE_REQ`  (sizeof = 60 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `wldmanhandle` | `unsigned short` |
|     8 | `newmechineid` | `unsigned char` |
|     9 | `linkto` | `PROTO_NC_MAP_LINKRESERVE_REQ::__unnamed` |
|    29 | `linkother` | `PROTO_NC_CHAR_REVIVEOTHER_CMD` |


### `PROTO_NC_MAP_LINKRESERVE_REQ::__unnamed`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mapname` | `Name3` |
|    12 | `location` | `SHINE_XY_TYPE` |


### `PROTO_NC_MAP_LINKSTART_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wmshandle` | `unsigned short` |
|     2 | `chrregnum` | `unsigned long` |


### `PROTO_NC_MAP_LOGINCOMPLETE_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `checksumDoorBlock` | `Name8` |


### `PROTO_NC_MAP_LOGINFAIL_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `nWrongDataFileIndex` | `unsigned char` |


### `PROTO_NC_MAP_LOGIN_REQ`  (sizeof = 978 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chardata` | `PROTO_NC_CHAR_ZONE_CHARDATA_REQ` |
|    18 | `checksum` | `Name8[?] /* 960 bytes */` |


### `PROTO_NC_MAP_LOGOUT_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |


### `PROTO_NC_MAP_MULTY_LINK_CMD`  (sizeof = 73 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `npcHandle` | `unsigned short` |
|     2 | `npcPosition` | `SHINE_XY_TYPE` |
|    10 | `limitRange` | `unsigned short` |
|    12 | `num` | `unsigned char` |
|    13 | `LinkMapName` | `Name3[?] /* 60 bytes */` |


### `PROTO_NC_MAP_MULTY_LINK_SELECT_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_MAP_MULTY_LINK_SELECT_REQ`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LinkMapName` | `Name3` |


### `PROTO_NC_MAP_REGIST_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `numofmap` | `unsigned short` |
|     2 | `maparray` | `PROTO_NC_MAP_REGIST_CMD::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_MAP_REGIST_CMD::__unnamed`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `canparty` | `unsigned char` |
|     1 | `mapname` | `Name3` |
|    13 | `IsKQ` | `unsigned char` |
|    14 | `IsPartyReleaseMap` | `unsigned char` |


### `PROTO_NC_MAP_TOWNPORTAL_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `portalindex` | `unsigned char` |


### `PROTO_NC_MAP_WING_FLY_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `where` | `unsigned char` |


### `PROTO_NC_MAP_WING_SAVE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `where` | `unsigned char` |


### `PROTO_NC_MENU_OPENSTORAGE_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |
|     8 | `maxpage` | `unsigned char` |
|     9 | `curpage` | `unsigned char` |
|    10 | `nOpenType` | `unsigned char` |
|    11 | `itemcounter` | `unsigned char` |
|    12 | `itemarray` | `PROTO_ITEMPACKET_INFORM[?] /* 0 bytes */` |


### `PROTO_NC_MENU_SERVERMENU_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `reply` | `unsigned char` |


### `PROTO_NC_MENU_SERVERMENU_REQ`  (sizeof = 142 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `title` | `unsigned char[128]   /* 128 bytes */` |
|   128 | `priority` | `unsigned char` |
|   129 | `npcHandle` | `unsigned short` |
|   131 | `npcPosition` | `SHINE_XY_TYPE` |
|   139 | `limitRange` | `unsigned short` |
|   141 | `menunum` | `unsigned char` |
|   142 | `menu` | `SERVERMENU[?] /* 0 bytes */` |


### `PROTO_NC_MENU_SHOPOPENSOULSTONE_CMD`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hp` | `PROTO_NC_MENU_SHOPOPENSOULSTONE_CMD::SOULSTONEMENU` |
|    12 | `sp` | `PROTO_NC_MENU_SHOPOPENSOULSTONE_CMD::SOULSTONEMENU` |


### `PROTO_NC_MENU_SHOPOPENSOULSTONE_CMD::SOULSTONEMENU`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `recover` | `unsigned long` |
|     4 | `maxnum` | `unsigned long` |
|     8 | `price` | `unsigned long` |


### `PROTO_NC_MENU_SHOPOPENTABLE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemnum` | `unsigned short` |
|     2 | `npc` | `unsigned short` |
|     4 | `itemlist` | `PROTO_NC_MENU_SHOPOPENTABLE_CMD::MENUITEM[?] /* 0 bytes */` |


### `PROTO_NC_MENU_SHOPOPENTABLE_CMD::MENUITEM`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `unsigned char` |
|     1 | `item` | `unsigned short` |


### `PROTO_NC_MENU_SHOPOPEN_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `itemnum` | `unsigned short` |
|     2 | `npc` | `unsigned short` |
|     4 | `itemlist` | `PROTO_NC_MENU_SHOPOPEN_CMD::MENUITEM[?] /* 0 bytes */` |


### `PROTO_NC_MENU_SHOPOPEN_CMD::MENUITEM`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `item` | `unsigned short` |


### `PROTO_NC_MID_CHAR_MAPPOS_INFO_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |
|     4 | `LastMapXY` | `SHINE_XY_TYPE` |


### `PROTO_NC_MID_COMPLETE_WORLD_CMD`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMapName` | `Name3` |
|    12 | `bIsOnline` | `unsigned char` |


### `PROTO_NC_MID_DESTROY_SCRIPT_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMapName` | `Name3` |


### `PROTO_NC_MID_END_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMapName` | `Name3` |


### `PROTO_NC_MID_GET_MAP_INFO_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nMapCnt` | `unsigned short` |
|     2 | `MapInfo` | `MID_MAP_INFO[?] /* 0 bytes */` |


### `PROTO_NC_MID_GET_MAP_INFO_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nMapCnt` | `unsigned short` |
|     2 | `sMapName` | `Name3[?] /* 0 bytes */` |


### `PROTO_NC_MID_LINKTO_MID_IN_ACK`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |
|     4 | `FromMap` | `MAPPOS` |
|    24 | `LastMapXY` | `SHINE_XY_TYPE` |


### `PROTO_NC_MID_LINKTO_MID_IN_REQ`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMapName` | `Name3` |
|    12 | `sMapBase` | `Name3` |
|    24 | `bIsUseMapXY` | `int` |
|    28 | `MapXY` | `SHINE_XY_TYPE` |
|    36 | `nCharCnt` | `unsigned short` |
|    38 | `nCharNo` | `unsigned long[0]   /* 0 bytes */` |


### `PROTO_NC_MID_LINKTO_MID_OUT_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |


### `PROTO_NC_MID_LINKTO_MID_OUT_REQ`  (sizeof = 36 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMapName` | `Name3` |
|    12 | `sMapBase` | `Name3` |
|    24 | `MapXY` | `SHINE_XY_TYPE` |
|    32 | `nCharNo` | `unsigned long` |


### `PROTO_NC_MID_MID_MAP_NAME_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nMapCnt` | `unsigned short` |
|     2 | `MapInfo` | `PROTO_NC_MID_MID_MAP_NAME_CMD::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_MID_MID_MAP_NAME_CMD::__unnamed`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nMaxUser` | `unsigned short` |
|     2 | `sMapName` | `Name3` |


### `PROTO_NC_MID_MID_MAP_REWARD_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nMapCnt` | `unsigned short` |
|     2 | `MapReward` | `MID_MAP_REWARD[?] /* 0 bytes */` |


### `PROTO_NC_MID_START_SCRIPT_CMD`  (sizeof = 48 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMapName` | `Name3` |
|    12 | `sScriptLanguage` | `char[32]   /* 32 bytes */` |
|    44 | `nRepeatCount` | `unsigned short` |
|    46 | `nRewardIndex` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_ACTIV_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_ARRANGEMODE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ArrangeMode` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_BUILDING_CMD`  (sizeof = 61 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `player` | `unsigned short` |
|     2 | `minihouseinfo` | `PROTO_NC_BRIEFINFO_MINIHOUSEBUILD_CMD` |


### `PROTO_NC_MINIHOUSE_BUILDING_REQ`  (sizeof = 133 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `maxvisiter` | `unsigned char` |
|     1 | `password` | `char[9]   /* 9 bytes */` |
|    10 | `title` | `char[21]   /* 21 bytes */` |
|    31 | `bItemInfoClose` | `unsigned char` |
|    32 | `sNotify` | `char[101]   /* 101 bytes */` |


### `PROTO_NC_MINIHOUSE_CHAR_ACTION_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_CHAR_ACTION_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `PreHandle` | `unsigned short` |
|     2 | `NextHandle` | `unsigned short` |
|     4 | `nFurnitureHnd` | `unsigned short` |
|     6 | `ActorHnd` | `unsigned short` |
|     8 | `ActorIdx` | `unsigned char` |
|     9 | `ActeeIdx` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_CHAR_ACTION_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Handle` | `unsigned short` |
|     2 | `nFurnitureHnd` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_COMPULSIONMOVETO_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hnd` | `unsigned short` |
|     2 | `location` | `SHINE_XY_TYPE` |


### `PROTO_NC_MINIHOUSE_CREATE_FURNITURE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `furniturecount` | `unsigned char` |
|     1 | `rearrange` | `PROTO_NC_MINIHOUSE_CREATE_FURNITURE_CMD::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_CREATE_FURNITURE_CMD::__unnamed`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `furniturehandle` | `unsigned short` |
|     2 | `furnitureid` | `unsigned short` |
|     4 | `location` | `SHINE_SPACE_TYPE` |
|    20 | `nEndureGrade` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_CREATE_FURNITURE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `furniturecount` | `unsigned char` |
|     1 | `rearrange` | `PROTO_NC_MINIHOUSE_CREATE_FURNITURE_REQ::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_CREATE_FURNITURE_REQ::__unnamed`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `furniturehandle` | `unsigned short` |
|     2 | `location` | `SHINE_SPACE_TYPE` |


### `PROTO_NC_MINIHOUSE_DB_PORTAL_ADD_ACK`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nPortalItemID` | `unsigned short` |
|     8 | `bType` | `unsigned char` |
|     9 | `dEndDate` | `long` |
|    13 | `lockindex` | `unsigned short` |
|    15 | `Error` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_DB_PORTAL_ADD_REQ`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nPortalItemID` | `unsigned short` |
|     8 | `bType` | `unsigned char` |
|     9 | `dEndDate` | `long` |
|    13 | `lockindex` | `unsigned short` |
|    15 | `Item` | `PROTO_ITEM_RELOC` |


### `PROTO_NC_MINIHOUSE_DB_PORTAL_DEL_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `Error` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_DB_PORTAL_LIST_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `Error` | `unsigned short` |
|     8 | `nPortalCnt` | `unsigned short` |
|    10 | `PortalInfo` | `PORTAL_INFO[?] /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_DB_PORTAL_LIST_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |


### `PROTO_NC_MINIHOUSE_DB_VISITER_COUNT_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `Error` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_DB_VISITER_COUNT_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `nVisiterCharNo` | `unsigned long` |


### `PROTO_NC_MINIHOUSE_DELETE_FURNITURE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `furniturecount` | `unsigned char` |
|     1 | `furniturehandle` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_FUNICHER_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `furniturecount` | `unsigned char` |
|     1 | `next` | `char` |
|     2 | `array` | `PROTO_NC_MINIHOUSE_FUNICHER_CMD::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_FUNICHER_CMD::__unnamed`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `furniturehandle` | `unsigned short` |
|     2 | `furnitureid` | `unsigned short` |
|     4 | `location` | `SHINE_SPACE_TYPE` |
|    20 | `nEndureGrade` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_FUNITUREINFOCOMPLETE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `funitureinfocomplete` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_FURNITURE_EFFECT_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nEffectHandle` | `unsigned short` |
|     2 | `err` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_FURNITURE_EFFECT_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nEffectHandle` | `unsigned short` |
|     2 | `nApplyPlayerCnt` | `unsigned short` |
|     4 | `nApplyPlayerHandle` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_FURNITURE_EFFECT_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nFurnitureHnd` | `unsigned short` |
|     2 | `nEffectHandle` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_FURNITURE_EMOTION_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nFurnitureHnd` | `unsigned short` |
|     2 | `nPosition` | `unsigned char` |
|     3 | `nEmotionNo` | `unsigned char` |
|     4 | `nError` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_FURNITURE_EMOTION_CANCEL_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_FURNITURE_EMOTION_CANCEL_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCancelEmotionCnt` | `unsigned short` |
|     2 | `nCancelEmotion` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_FURNITURE_EMOTION_CANCEL_REQ`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_MINIHOUSE_FURNITURE_EMOTION_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nEmotionCnt` | `unsigned short` |
|     2 | `Emotion` | `FURNITURE_EMOTION_INFO[?] /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_FURNITURE_EMOTION_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nFurnitureHnd` | `unsigned short` |
|     2 | `nPosition` | `unsigned char` |
|     3 | `nEmotionNo` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_FURNITURE_ENDURE_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nEndureFurnitureCnt` | `unsigned char` |
|     1 | `EndureFurniture` | `ENDURE_FURNITURE_INFO[?] /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_KICKOUT_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `kick` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_KICKOUT_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hnd` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_LOGINCOMPLETE_ACK`  (sizeof = 140 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `error` | `int` |
|     4 | `maxvisiter` | `unsigned char` |
|     5 | `bItemInfoClose` | `unsigned char` |
|     6 | `sNotify` | `char[101]   /* 101 bytes */` |
|   107 | `TargetKey` | `char[32]   /* 32 bytes */` |
|   139 | `playernum` | `unsigned char` |
|   140 | `player` | `PROTO_NC_MINIHOUSE_LOGINCOMPLETE_ACK::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_LOGINCOMPLETE_ACK::__unnamed`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `level` | `unsigned char` |
|     1 | `hnd` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_LOGINCOMPLETE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `level` | `unsigned char` |
|     1 | `hnd` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_LOGOUTCOMPLETE_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hnd` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_MODIFY_ITEM_INFO_OPEN_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bItemInfoClose` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_MODIFY_ITEM_INFO_OPEN_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `bItemInfoClose` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_MODIFY_MAXENTERNUM_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `maxvisiter` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_MODIFY_NOTIFY_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNotifyLen` | `unsigned char` |
|     1 | `sNotify` | `char[0]   /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_MODIFY_NOTIFY_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNotifyLen` | `unsigned char` |
|     1 | `sNotify` | `char[0]   /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_MODIFY_OUTSIDE_TITLE_CMD`  (sizeof = 23 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mh_hnd` | `unsigned short` |
|     2 | `title` | `char[21]   /* 21 bytes */` |


### `PROTO_NC_MINIHOUSE_MODIFY_PASSWORD_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `isPublic` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_MODIFY_PASSWORD_REQ`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `password` | `char[9]   /* 9 bytes */` |


### `PROTO_NC_MINIHOUSE_MODIFY_TITLE_REQ`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `title` | `char[21]   /* 21 bytes */` |


### `PROTO_NC_MINIHOUSE_PORTAL_ADD_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `NewPortal` | `PORTAL_INFO` |


### `PROTO_NC_MINIHOUSE_PORTAL_CLOSE_REQ`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_MINIHOUSE_PORTAL_DEL_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPortalItemID` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_PORTAL_EFFECT_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Error` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_PORTAL_EFFECT_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nEffectHandle` | `unsigned short` |
|     2 | `nApplyPlayerCnt` | `unsigned short` |
|     4 | `nApplyPlayerHandle` | `unsigned short[0]   /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_PORTAL_EFFECT_REQ`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_MINIHOUSE_PORTAL_LIST_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPortalCnt` | `unsigned short` |
|     2 | `PortalInfo` | `PORTAL_INFO[?] /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_PORTAL_OPEN_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPortalItemID` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_REARRANGE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `furniturecount` | `unsigned char` |
|     1 | `rearrange` | `PROTO_NC_MINIHOUSE_REARRANGE_REQ::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_MINIHOUSE_REARRANGE_REQ::__unnamed`  (sizeof = 18 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `furniturehandle` | `unsigned short` |
|     2 | `location` | `SHINE_SPACE_TYPE` |


### `PROTO_NC_MINIHOUSE_VISITREADY_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `error` | `int` |
|     4 | `isPublic` | `unsigned char` |
|     5 | `visitercnt` | `unsigned char` |
|     6 | `maxvisiter` | `unsigned char` |


### `PROTO_NC_MINIHOUSE_VISITREADY_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mh_hnd` | `unsigned short` |


### `PROTO_NC_MINIHOUSE_VISIT_REQ`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mh_hnd` | `unsigned short` |
|     2 | `password` | `char[9]   /* 9 bytes */` |


### `PROTO_NC_MISC_CHAT_BLOCK_SPAMER_BLOCKTIME_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nBlockTime` | `unsigned long` |


### `PROTO_NC_MISC_CONNECTFROMWHERE_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `connectfrom` | `unsigned char` |


### `PROTO_NC_MISC_CONNECTFROMWHERE_REQ`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `netpacketzoneheader` | `NETPACKETZONEHEADER` |
|     6 | `connectip` | `unsigned char[4]   /* 4 bytes */` |


### `PROTO_NC_MISC_EVENTNPC_STANDEND_ZONE_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMobIndex` | `Name8` |


### `PROTO_NC_MISC_EVENTNPC_STANDSTART_ZONE_CMD`  (sizeof = 32 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sMobIndex` | `Name8` |


### `PROTO_NC_MISC_GM_CHAT_COLOR_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |


### `PROTO_NC_MISC_HEARTBEAT_REQ_SEND`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `size` | `unsigned char` |
|     1 | `netcmd` | `NETCOMMAND` |


### `PROTO_NC_MISC_PINGTEST_CLIENT_ZONE_DB`  (sizeof = 80 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ID` | `unsigned long` |
|     4 | `ClientSendZone` | `unsigned long` |
|     8 | `ClientRecvZone` | `unsigned long` |
|    12 | `hZoneClient` | `unsigned short` |
|    14 | `ZoneSendToDB` | `unsigned long` |
|    18 | `ZoneRecvFromDB` | `unsigned long` |
|    22 | `ZoneSendToClient` | `unsigned long` |
|    26 | `ZoneRecvFromClient` | `unsigned long` |
|    30 | `DBRecvFromZone` | `unsigned long` |
|    34 | `DBSendToZone` | `unsigned long` |
|    38 | `DBSendToODBC` | `unsigned long` |
|    42 | `DBRecvFromODBC` | `unsigned long` |
|    46 | `header` | `NETPACKETZONEHEADER` |
|    52 | `buylot` | `PROTO_NC_ITEMDB_BUYLOT_REQ` |


### `PROTO_NC_MISC_PINGTEST_TOOL_WM_CLIENT_ZONE_DB`  (sizeof = 116 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ID` | `unsigned long` |
|     4 | `hTool` | `unsigned short` |
|     6 | `ToolSendToWM` | `unsigned long` |
|    10 | `ToolRecvFromWM` | `unsigned long` |
|    14 | `TargetCharName` | `Name4` |
|    30 | `hWMClient` | `unsigned short` |
|    32 | `WMSendClient` | `unsigned long` |
|    36 | `WMRecvFromClient` | `unsigned long` |
|    40 | `ClientSendZone` | `unsigned long` |
|    44 | `ClientRecvZone` | `unsigned long` |
|    48 | `hZoneClient` | `unsigned short` |
|    50 | `ZoneSendToDB` | `unsigned long` |
|    54 | `ZoneRecvFromDB` | `unsigned long` |
|    58 | `ZoneSendToClient` | `unsigned long` |
|    62 | `ZoneRecvFromClient` | `unsigned long` |
|    66 | `DBRecvFromZone` | `unsigned long` |
|    70 | `DBSendToZone` | `unsigned long` |
|    74 | `DBSendToODBC` | `unsigned long` |
|    78 | `DBRecvFromODBC` | `unsigned long` |
|    82 | `header` | `NETPACKETZONEHEADER` |
|    88 | `buylot` | `PROTO_NC_ITEMDB_BUYLOT_REQ` |


### `PROTO_NC_MISC_PINGTEST_TOOL_WM_ZONE`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ID` | `unsigned long` |
|     4 | `hTool` | `unsigned short` |
|     6 | `ToolSendToWM` | `unsigned long` |
|    10 | `ToolRecvFromWM` | `unsigned long` |
|    14 | `WMTargetZoneNo` | `unsigned long` |
|    18 | `WMSendZone` | `unsigned long` |
|    22 | `WMRecvFromZone` | `unsigned long` |
|    26 | `ZoneRecvFromWMS` | `unsigned long` |
|    30 | `ZoneSendToWMS` | `unsigned long` |


### `PROTO_NC_MISC_PINGTEST_TOOL_WM_ZONE_DB`  (sizeof = 58 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ID` | `unsigned long` |
|     4 | `hTool` | `unsigned short` |
|     6 | `ToolSendToWM` | `unsigned long` |
|    10 | `ToolRecvFromWM` | `unsigned long` |
|    14 | `WMTargetZoneNo` | `unsigned long` |
|    18 | `WMSendZone` | `unsigned long` |
|    22 | `WMRecvFromZone` | `unsigned long` |
|    26 | `ZoneSendToDB` | `unsigned long` |
|    30 | `ZoneRecvFromDB` | `unsigned long` |
|    34 | `ZoneSendToWMS` | `unsigned long` |
|    38 | `ZoneRecvFromWMS` | `unsigned long` |
|    42 | `DBRecvFromZone` | `unsigned long` |
|    46 | `DBSendToZone` | `unsigned long` |
|    50 | `DBSendToODBC` | `unsigned long` |
|    54 | `DBRecvFromODBC` | `unsigned long` |


### `PROTO_NC_MISC_RESTMINUTE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `flag` | `unsigned char` |
|     1 | `second` | `unsigned short` |


### `PROTO_NC_MISC_S2SCONNECTION_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `echo_data` | `unsigned char` |
|     1 | `error` | `unsigned short` |


### `PROTO_NC_MISC_S2SCONNECTION_REQ`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `echo_data` | `unsigned char` |
|     1 | `server_to_id` | `unsigned char` |
|     2 | `server_from_id` | `unsigned char` |
|     3 | `server_from_world_num` | `unsigned char` |
|     4 | `server_from_zone_num` | `unsigned char` |
|     5 | `key` | `unsigned short` |


### `PROTO_NC_MISC_SEED_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `seed` | `unsigned short` |


### `PROTO_NC_MISC_SERVERPARAMETER_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `shoutlevel` | `unsigned char` |
|     1 | `shoutdelay` | `unsigned short` |


### `PROTO_NC_MISC_SPAMMER_RELEASE_CHAT_BAN_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nGMCharNo` | `unsigned long` |
|     4 | `sSpammerName` | `Name4` |


### `PROTO_NC_MISC_SPAMMER_ZONE_CHAT_BAN_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sSpammerName` | `Name4` |
|    16 | `nChatBan` | `unsigned long` |


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


### `PROTO_NC_MISC_TIMERECORD_ACK`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `loop` | `unsigned char` |
|     1 | `client` | `PROTO_NC_MISC_TIMERECORD_REQ` |
|     8 | `server` | `PROTO_NC_MISC_TIMERECORD_REQ` |


### `PROTO_NC_MISC_TIMERECORD_REQ`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hour` | `unsigned char` |
|     1 | `minute` | `unsigned char` |
|     2 | `sec` | `unsigned char` |
|     3 | `milisec` | `unsigned long` |


### `PROTO_NC_MISC_WHSHANDLEREPAIR_CMD`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `packheader` | `NETPACKETZONEHEADER` |
|     6 | `whshandle_old` | `unsigned short` |
|     8 | `whshandle_new` | `unsigned short` |
|    10 | `flag` | `unsigned char` |


### `PROTO_NC_MISC_ZONERINGLINKTEST_RNG`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `start` | `ZONERINGLINKAGESTART` |
|     7 | `mechines` | `unsigned char` |
|     8 | `addr` | `Name4[?] /* 0 bytes */` |


### `PROTO_NC_OPTOOL_CONNECT_BRIF_ACK`  (sizeof = 170 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `echo_data` | `unsigned char` |
|     1 | `my_server_id` | `unsigned char` |
|     2 | `count` | `unsigned short[2]   /* 4 bytes */[2]   /* 8 bytes */[21]   /* 168 bytes */` |


### `PROTO_NC_PARTY_CHANGEMASTER_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `partyregnum` | `unsigned short` |
|     2 | `newmaster_chrregnum` | `unsigned long` |


### `PROTO_NC_PARTY_DISMISS_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `partyregnum` | `unsigned short` |


### `PROTO_NC_PARTY_FUNDAMENTAL_CMD`  (sizeof = 58 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `partyregnum` | `unsigned short` |
|     2 | `master` | `PROTO_PARTY_MEMBER_AND_ZONEMERCHINE` |
|    30 | `member` | `PROTO_PARTY_MEMBER_AND_ZONEMERCHINE` |


### `PROTO_NC_PARTY_ITEM_LOOTING_ZONE_CMD`  (sizeof = 11 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `partyregnum` | `unsigned short` |
|     2 | `LootType` | `unsigned char[5]   /* 5 bytes */` |
|     7 | `LooterNo` | `unsigned long` |


### `PROTO_NC_PARTY_JOIN_CMD`  (sizeof = 30 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `partyregnum` | `unsigned short` |
|     2 | `joiner` | `PROTO_PARTY_MEMBER_AND_ZONEMERCHINE` |


### `PROTO_NC_PARTY_KICKOFF_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `partyregnum` | `unsigned short` |
|     2 | `kickoffmemberregnum` | `unsigned long` |


### `PROTO_NC_PARTY_LEAVE_CMD`  (sizeof = 29 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `partyregnum` | `unsigned short` |
|     2 | `leavemember` | `PROTO_PARTY_MEMBER` |


### `PROTO_NC_PARTY_LOGIN_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `partyregnum` | `unsigned short` |
|     2 | `chrregnum` | `unsigned long` |


### `PROTO_NC_PARTY_LOGOUT_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `partyregnum` | `unsigned short` |
|     2 | `chrregnum` | `unsigned long` |


### `PROTO_NC_PARTY_MEMBERCLASS_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `membernum` | `unsigned char` |
|     1 | `member` | `PROTO_PARTY_MEMBER_CLASS[?] /* 0 bytes */` |


### `PROTO_NC_PARTY_MEMBERINFORM_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `PartyRegnum` | `unsigned short` |
|     2 | `NumberOfMember` | `unsigned char` |
|     3 | `Member` | `PROTO_PARTY_MEMBER[?] /* 0 bytes */` |


### `PROTO_NC_PARTY_MEMBERINFORM_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `membernum` | `unsigned char` |
|     1 | `member` | `PROTO_PARTY_MEMBER_INFORM[?] /* 0 bytes */` |


### `PROTO_NC_PARTY_MEMBERINFORM_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `partyregnum` | `unsigned short` |


### `PROTO_NC_PARTY_MEMBERLOCATION_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `membernum` | `unsigned char` |
|     1 | `member` | `PROTO_NC_PARTY_MEMBERLOCATION_CMD::__unnamed[0]   /* 0 bytes */` |


### `PROTO_NC_PARTY_MEMBERLOCATION_CMD::__unnamed`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `charid` | `Name4` |
|    16 | `loc` | `SHINE_XY_TYPE` |


### `PROTO_NC_PARTY_MEMBERMAPOUT`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `memberid` | `Name4` |


### `PROTO_NC_PARTY_ZONE_SET_LOOTER_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPartyNo` | `unsigned short` |
|     2 | `nNewLooterCharNo` | `unsigned long` |


### `PROTO_NC_PRISON_ADD_REQ`  (sizeof = 25 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wmSessionHandle` | `unsigned short` |
|     2 | `zoneSessionHandle` | `unsigned short` |
|     4 | `regNum` | `unsigned long` |
|     8 | `lastestInfo` | `DISPOSITION_BRIEFINFO` |


### `PROTO_NC_PRISON_END_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `nPrisonSecond` | `unsigned long` |


### `PROTO_NC_QUEST_CLIENT_SCENARIO_DONE_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nScenarioID` | `unsigned short` |


### `PROTO_NC_QUEST_CLIENT_SCENARIO_DONE_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nScenarioID` | `unsigned short` |


### `PROTO_NC_QUEST_DB_CLEAR_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     6 | `nQuestID` | `unsigned short` |
|     8 | `ErrorType` | `unsigned short` |


### `PROTO_NC_QUEST_DB_CLEAR_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     6 | `nQuestID` | `unsigned short` |


### `PROTO_NC_QUEST_DB_DONE_ACK`  (sizeof = 43 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     6 | `nQSC` | `unsigned char` |
|     7 | `QuestInfo` | `PLAYER_QUEST_INFO` |
|    39 | `nLockIndex` | `unsigned short` |
|    41 | `ErrorType` | `unsigned short` |


### `PROTO_NC_QUEST_DB_DONE_REQ`  (sizeof = 77 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     6 | `nQSC` | `unsigned char` |
|     7 | `QuestInfo` | `PLAYER_QUEST_INFO` |
|    39 | `nMoney` | `unsigned long` |
|    43 | `nExp` | `unsigned long` |
|    47 | `nFame` | `unsigned long` |
|    51 | `nCur_Money` | `unsigned __int64` |
|    59 | `nCur_Exp` | `unsigned __int64` |
|    67 | `nCur_Fame` | `unsigned long` |
|    71 | `CT_Type` | `unsigned char` |
|    72 | `CT_ElementNo` | `unsigned char` |
|    73 | `nCur_CT_ElementValue` | `unsigned char` |
|    74 | `nLockIndex` | `unsigned short` |
|    76 | `nItemCreateCount` | `unsigned char` |
|    77 | `ItemCreate` | `PROTO_ITEM_CREATE[?] /* 0 bytes */` |


### `PROTO_NC_QUEST_DB_GIVE_UP_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     6 | `nQuestID` | `unsigned short` |
|     8 | `ErrorType` | `unsigned short` |


### `PROTO_NC_QUEST_DB_GIVE_UP_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     6 | `nQuestID` | `unsigned short` |


### `PROTO_NC_QUEST_DB_READ_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     6 | `err` | `unsigned short` |
|     8 | `nQuestID` | `unsigned short` |


### `PROTO_NC_QUEST_DB_READ_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     6 | `nQuestID` | `unsigned short` |


### `PROTO_NC_QUEST_DB_SET_INFO_ACK`  (sizeof = 41 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     6 | `nQSC` | `unsigned char` |
|     7 | `QuestInfo` | `PLAYER_QUEST_INFO` |
|    39 | `ErrorType` | `unsigned short` |


### `PROTO_NC_QUEST_DB_SET_INFO_REQ`  (sizeof = 39 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ZoneHeader` | `NETPACKETZONEHEADER` |
|     6 | `nQSC` | `unsigned char` |
|     7 | `QuestInfo` | `PLAYER_QUEST_INFO` |


### `PROTO_NC_QUEST_GIVE_UP_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nQuestID` | `unsigned short` |
|     2 | `ErrorCode` | `unsigned short` |


### `PROTO_NC_QUEST_GIVE_UP_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nQuestID` | `unsigned short` |


### `PROTO_NC_QUEST_NOTIFY_MOB_KILL_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `NumOfActionQuest` | `unsigned char` |
|     1 | `mobofquest` | `PROTO_NC_QUEST_NOTIFY_MOB_KILL_CMD::MobOfQuest[?] /* 0 bytes */` |


### `PROTO_NC_QUEST_NOTIFY_MOB_KILL_CMD::MobOfQuest`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nMobID` | `unsigned short` |
|     2 | `nQuestID` | `unsigned short` |


### `PROTO_NC_QUEST_READ_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |
|     2 | `nQuestID` | `unsigned short` |


### `PROTO_NC_QUEST_READ_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nQuestID` | `unsigned short` |


### `PROTO_NC_QUEST_REWARD_NEED_SELECT_ITEM_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nQuestID` | `unsigned short` |


### `PROTO_NC_QUEST_REWARD_SELECT_ITEM_INDEX_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nQuestID` | `unsigned short` |
|     2 | `nSelectedItemIndex` | `unsigned long` |


### `PROTO_NC_QUEST_SCENARIO_RUN_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nScenarioID` | `unsigned short` |


### `PROTO_NC_QUEST_SCRIPT_CMD_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nQuestID` | `unsigned short` |
|     2 | `nQSC` | `unsigned char` |
|     3 | `nResult` | `unsigned long` |


### `PROTO_NC_QUEST_SCRIPT_CMD_REQ`  (sizeof = 103 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nQuestID` | `unsigned short` |
|     2 | `Command` | `STRUCT_QSC` |


### `PROTO_NC_QUEST_SELECT_START_ACK`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNPCID` | `unsigned short` |
|     2 | `nQuestID` | `unsigned short` |
|     4 | `ErrorType` | `unsigned short` |


### `PROTO_NC_QUEST_SELECT_START_REQ`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNPCID` | `unsigned short` |
|     2 | `nQuestID` | `unsigned short` |


### `PROTO_NC_QUEST_START_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `err` | `unsigned short` |


### `PROTO_NC_QUEST_START_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nQuestID` | `unsigned short` |


### `PROTO_NC_RAID_LOCATION_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNum` | `unsigned char` |
|     1 | `RaidMembers` | `PROTO_NC_RAID_LOCATION_CMD::Memberinfo[?] /* 0 bytes */` |


### `PROTO_NC_RAID_LOCATION_CMD::Memberinfo`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPartyNo` | `unsigned short` |
|     2 | `nPartyIndex` | `unsigned char` |
|     3 | `nSlotIndex` | `unsigned char` |
|     4 | `loc` | `SHINE_XY_TYPE` |


### `PROTO_NC_RAID_MAPOUT_CMD`  (sizeof = 16 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `sCharID` | `Name4` |


### `PROTO_NC_RAID_MEMBERCLASS_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nNum` | `unsigned char` |
|     1 | `RaidMembers` | `RAID_MEMBERCLASS[?] /* 0 bytes */` |


### `PROTO_NC_RAID_MEMBERINFORM_CMD`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPartyNo` | `unsigned short` |
|     2 | `nPartyIndex` | `unsigned char` |
|     3 | `nSlotIndex` | `unsigned char` |
|     4 | `hp` | `unsigned long` |
|     8 | `sp` | `unsigned long` |


### `PROTO_NC_RAID_ZONE_DEL_SUB_LEADER_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nRaidNo` | `unsigned short` |
|     2 | `nDelLeaderCharNo` | `unsigned long` |
|     6 | `nPartyNo` | `unsigned short` |
|     8 | `nPartyIndex` | `unsigned char` |
|     9 | `nSlotIndex` | `unsigned char` |


### `PROTO_NC_RAID_ZONE_DISMISS_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nRaidNo` | `unsigned short` |


### `PROTO_NC_RAID_ZONE_LEAVE_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nRaidNo` | `unsigned short` |
|     2 | `nPartyNo` | `unsigned short` |
|     4 | `nPartyIndex` | `unsigned char` |
|     5 | `nSlotIndex` | `unsigned char` |
|     6 | `nCharNo` | `unsigned long` |


### `PROTO_NC_RAID_ZONE_LOOTING_TYPE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nRaidNo` | `unsigned short` |
|     2 | `nItemGrade` | `unsigned char` |
|     3 | `nLootingType` | `unsigned char` |


### `PROTO_NC_RAID_ZONE_MAKE_CMD`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPartyNo` | `unsigned short` |
|     2 | `nRaidNo` | `unsigned short` |
|     4 | `nRaidPartyNo` | `unsigned short[8]   /* 16 bytes */` |
|    20 | `FirstPartyCharRegNo` | `unsigned long[5]   /* 20 bytes */` |


### `PROTO_NC_RAID_ZONE_MEMBER_JOIN_CMD`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nRaidNo` | `unsigned short` |
|     2 | `nPartyNo` | `unsigned short` |
|     4 | `nPartyIndex` | `unsigned char` |
|     5 | `nSlotIndex` | `unsigned char` |
|     6 | `nCharNo` | `unsigned long` |
|    10 | `tInfo` | `PROTO_PARTY_MEMBER_AND_ZONEMERCHINE` |


### `PROTO_NC_RAID_ZONE_SET_LEADER_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nRaidNo` | `unsigned short` |
|     2 | `nNewLeaderCharNo` | `unsigned long` |
|     6 | `nPartyNo` | `unsigned short` |
|     8 | `nPartyIndex` | `unsigned char` |
|     9 | `nSlotIndex` | `unsigned char` |


### `PROTO_NC_RAID_ZONE_SET_LOOTER_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nRaidNo` | `unsigned short` |
|     2 | `nNewLooterCharNo` | `unsigned long` |
|     6 | `nPartyNo` | `unsigned short` |
|     8 | `nPartyIndex` | `unsigned char` |
|     9 | `nSlotIndex` | `unsigned char` |


### `PROTO_NC_RAID_ZONE_SET_SUB_LEADER_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nRaidNo` | `unsigned short` |
|     2 | `nNewSubLeaderCharNo` | `unsigned long` |
|     6 | `nPartyNo` | `unsigned short` |
|     8 | `nPartyIndex` | `unsigned char` |
|     9 | `nSlotIndex` | `unsigned char` |


### `PROTO_NC_RAID_ZONE_SLOT_MOVE_CMD`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nRaidNo` | `unsigned short` |
|     2 | `PrePosition` | `PROTO_NC_RAID_ZONE_SLOT_MOVE_CMD::__unnamed` |
|    26 | `PostPosition` | `PROTO_NC_RAID_ZONE_SLOT_MOVE_CMD::__unnamed` |


### `PROTO_NC_RAID_ZONE_SLOT_MOVE_CMD::__unnamed`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nPartyNo` | `unsigned short` |
|     2 | `nPartyIndex` | `unsigned char` |
|     3 | `nSlotIndex` | `unsigned char` |
|     4 | `nPartySeqCharNo` | `unsigned long[5]   /* 20 bytes */` |


### `PROTO_NC_RAID_ZONE_VANISH_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nRaidNo` | `unsigned short` |
|     2 | `nPartyNo` | `unsigned short` |
|     4 | `nPartyIndex` | `unsigned char` |
|     5 | `nSlotIndex` | `unsigned char` |
|     6 | `nVanishCharNo` | `unsigned long` |


### `PROTO_NC_SCENARIO_ANIMATE_CMD`  (sizeof = 34 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `anicode` | `Name8` |


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

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `effectcode` | `unsigned char` |


### `PROTO_NC_SCENARIO_CAMERA_MOVE_CMD`  (sizeof = 15 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `MoveType` | `unsigned char` |
|     1 | `ViewCoord` | `SHINE_XY_TYPE` |
|     9 | `AngleXZ` | `unsigned short` |
|    11 | `AngleY` | `unsigned short` |
|    13 | `Dist` | `unsigned short` |


### `PROTO_NC_SCENARIO_CHATWIN_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mob` | `unsigned short` |
|     2 | `len` | `unsigned char` |
|     3 | `content` | `unsigned char[0]   /* 0 bytes */` |


### `PROTO_NC_SCENARIO_DOORSTATE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `door` | `unsigned short` |
|     2 | `doorstate` | `unsigned char` |


### `PROTO_NC_SCENARIO_FOG_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `fog` | `unsigned char` |
|     1 | `depth` | `unsigned short` |


### `PROTO_NC_SCENARIO_LIGHT_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `red` | `unsigned char` |
|     1 | `green` | `unsigned char` |
|     2 | `blue` | `unsigned char` |


### `PROTO_NC_SCENARIO_MESSAGENOTICE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `message` | `unsigned long` |


### `PROTO_NC_SCENARIO_MESSAGE_CMD`  (sizeof = 17 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `DialogIndex` | `char[17]   /* 17 bytes */` |


### `PROTO_NC_SCENARIO_NPCCHAT_CMD`  (sizeof = 19 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `DialogIndex` | `char[17]   /* 17 bytes */` |
|    17 | `NPCHandle` | `unsigned short` |


### `PROTO_NC_SCENARIO_OBJTYPECHANGE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `handle` | `unsigned short` |
|     2 | `type` | `unsigned char` |


### `PROTO_NC_SCENARIO_SCRIPTMESSAGE_CMD`  (sizeof = 37 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `DialogIndex` | `char[32]   /* 32 bytes */` |
|    32 | `nlen` | `unsigned char[5]   /* 5 bytes */` |
|    37 | `sContent` | `unsigned char[0]   /* 0 bytes */` |


### `PROTO_NC_SCENARIO_TIMER_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nSecond` | `unsigned short` |


### `PROTO_NC_SKILL_COOLTIME_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `sec` | `unsigned long` |


### `PROTO_NC_SKILL_EMPOWALLOC_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `err` | `unsigned short` |


### `PROTO_NC_SKILL_EMPOWALLOC_DB_ACK`  (sizeof = 13 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zonehandle` | `unsigned short` |
|     2 | `chrregnum` | `unsigned long` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `skill` | `unsigned short` |
|    10 | `err` | `unsigned short` |
|    12 | `bUseItem` | `unsigned char` |


### `PROTO_NC_SKILL_EMPOWALLOC_DB_REQ`  (sizeof = 40 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zonehandle` | `unsigned short` |
|     2 | `chrregnum` | `unsigned long` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `skill` | `unsigned short` |
|    10 | `bUseItem` | `unsigned char` |
|    11 | `empower` | `PROTO_NC_SKILL_EMPOWALLOC_REQ` |
|    17 | `redistitem` | `PROTO_ITEMDELETEREQUEST` |


### `PROTO_NC_SKILL_EMPOWALLOC_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `plus` | `SKILL_EMPOWER` |
|     4 | `minus` | `SKILL_EMPOWER` |


### `PROTO_NC_SKILL_EMPOWPOINT_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `restempow` | `unsigned char` |


### `PROTO_NC_SKILL_EMPOW_RESET_DB_REQ`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `zonehandle` | `unsigned short` |
|     2 | `chrregnum` | `unsigned long` |


### `PROTO_NC_SKILL_EMPOW_RESET_SUC_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `totalempow` | `unsigned char` |


### `PROTO_NC_SKILL_ERASE_ACK`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `skill` | `unsigned short` |
|     8 | `err` | `unsigned short` |


### `PROTO_NC_SKILL_ERASE_REQ`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `header` | `NETPACKETZONEHEADER` |
|     6 | `skill` | `unsigned short` |


### `PROTO_NC_SKILL_JUMP_CMD`  (sizeof = 21 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nObject` | `unsigned short` |
|     2 | `From` | `SHINE_XY_TYPE` |
|    10 | `To` | `SHINE_XY_TYPE` |
|    18 | `nDirect` | `unsigned char` |
|    19 | `SkillID` | `unsigned short` |


### `PROTO_NC_SKILL_PASSIVESKILL_LEARN_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `wmshandle` | `unsigned short` |
|     2 | `chrregnum` | `unsigned long` |
|     6 | `passiveskillid` | `unsigned short` |


### `PROTO_NC_SKILL_PRODUCTFIELD_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mainskill` | `unsigned short` |
|     2 | `err` | `unsigned short` |


### `PROTO_NC_SKILL_PRODUCTFIELD_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mainskill` | `unsigned short` |


### `PROTO_NC_SKILL_REVIVE_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `hp` | `unsigned long` |
|     4 | `sp` | `unsigned long` |


### `PROTO_NC_SKILL_SKILLEXP_CLIENT_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `mastery` | `unsigned long` |


### `PROTO_NC_SKILL_SKILLTEACHSUC_ACK`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skillid` | `unsigned short` |
|     2 | `ispassive` | `unsigned char` |


### `PROTO_NC_SKILL_SKILLTEACH_REQ`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `chrregnum` | `unsigned long` |
|     4 | `skillid` | `unsigned short` |
|     6 | `ispassive` | `unsigned char` |
|     7 | `removeid` | `unsigned short` |


### `PROTO_NC_SKILL_SOMEONEREVIVE_CMD`  (sizeof = 10 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `reviver` | `unsigned short` |
|     2 | `hp` | `unsigned long` |
|     6 | `sp` | `unsigned long` |


### `PROTO_NC_SKILL_UNLEARN_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `skill` | `unsigned short` |
|     2 | `err` | `unsigned short` |


### `PROTO_NC_SKILL_UNLEARN_REQ`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slot` | `unsigned char` |
|     1 | `skill` | `unsigned short` |


### `PROTO_NC_SKILL_WARP_CMD`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `object` | `unsigned short` |
|     2 | `from` | `SHINE_XY_TYPE` |
|    10 | `to` | `SHINE_XY_TYPE` |
|    18 | `skill` | `unsigned short` |


### `PROTO_NC_SOULSTONE_HP_BUY_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `number` | `unsigned short` |


### `PROTO_NC_SOULSTONE_HP_SOMEONEUSE_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `player` | `unsigned short` |


### `PROTO_NC_SOULSTONE_SP_BUY_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `totalnumber` | `unsigned short` |


### `PROTO_NC_SOULSTONE_SP_BUY_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `number` | `unsigned short` |


### `PROTO_NC_SOULSTONE_SP_SOMEONEUSE_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `player` | `unsigned short` |


### `PROTO_NC_TRADE_CENBOARDING_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |


### `PROTO_NC_TRADE_DOWNBOARD_ACK`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slotboard` | `unsigned char` |


### `PROTO_NC_TRADE_DOWNBOARD_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slotboard` | `unsigned char` |


### `PROTO_NC_TRADE_OPPOSITCENBOARDING_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `cen` | `unsigned __int64` |


### `PROTO_NC_TRADE_OPPOSITDOWNBOARD_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slotboard` | `unsigned char` |


### `PROTO_NC_TRADE_OPPOSITUPBOARD_CMD`  (sizeof = 104 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slotboard` | `unsigned char` |
|     1 | `iteminfo` | `SHINE_ITEM_STRUCT` |


### `PROTO_NC_TRADE_PROPOSE_ASK_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `proposer` | `unsigned short` |


### `PROTO_NC_TRADE_PROPOSE_REQ`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `proposee` | `unsigned short` |


### `PROTO_NC_TRADE_START_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `opposite` | `unsigned short` |


### `PROTO_NC_TRADE_UPBOARD_ACK`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slotinven` | `unsigned char` |
|     1 | `slotboard` | `unsigned char` |


### `PROTO_NC_TRADE_UPBOARD_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `slotinven` | `unsigned char` |


### `PROTO_NC_USER_CONNECTCUT2WORLDMANAGER_CMD`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `usrregnum` | `unsigned long` |
|     4 | `wldmanhandle` | `unsigned short` |
|     6 | `reason` | `unsigned short` |


### `PROTO_NC_USER_CONNECTCUT2ZONE_CMD`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `usrregnum` | `unsigned long` |


### `PROTO_NC_USER_CONNECTION_CHANGE_CHAR_ID_ITEM_USE_ACK`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nError` | `unsigned short` |
|     2 | `nUsedItemID` | `unsigned short` |


### `PROTO_NC_USER_CONNECTION_CHANGE_CHAR_ID_ITEM_USE_REQ`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nInvenSlot` | `unsigned char` |


### `PROTO_NC_USER_CONNECTION_DB_CHANGE_CHAR_ID_ITEM_USE_ACK`  (sizeof = 12 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `nError` | `unsigned short` |
|     8 | `lockindex` | `unsigned short` |
|    10 | `nUsedItemID` | `unsigned short` |


### `PROTO_NC_USER_CONNECTION_DB_CHANGE_CHAR_ID_ITEM_USE_REQ`  (sizeof = 38 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharIDChangeItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|     8 | `nCharIDChangeItemSlot` | `ITEM_INVEN` |
|    10 | `bInit` | `unsigned char` |
|    11 | `nUsedItemID` | `unsigned short` |
|    13 | `nUserNo` | `unsigned long` |
|    17 | `CommonData` | `PROTO_NC_ITEMDB_USE_VARIATION_ITEM_REQ` |


### `PROTO_NC_USER_CONNECTION_ZONE_CHANGE_CHAR_ID_CMD`  (sizeof = 37 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |
|     4 | `bInit` | `unsigned char` |
|     5 | `sOldID` | `Name4` |
|    21 | `sNewID` | `Name4` |


### `PROTO_NC_USER_NORMALLOGOUT_CMD`  (sizeof = 1 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `LogoutType` | `unsigned char` |


### `PROTO_NC_USER_POSSIBLE_NEW_CONNECT_CMD`  (sizeof = 6 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nWorldManagerHandle` | `unsigned short` |
|     2 | `nCharNo` | `unsigned long` |


### `PROTO_NC_USER_PROTECTION_FROM_GAME_ADMIN_SET_WORLD_CMD`  (sizeof = 9 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nUserNo` | `unsigned long` |
|     4 | `nIsAdult` | `unsigned char` |
|     5 | `nPlayTime` | `unsigned long` |


### `PROTO_NC_USER_PROTECTION_FROM_GAME_ZONE_CMD`  (sizeof = 5 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharNo` | `unsigned long` |
|     4 | `nPFGType` | `unsigned char` |


### `PROTO_NC_USER_USE_BEAUTY_SHOP_CMD`  (sizeof = 1 bytes)

_no members_


### `PROTO_NC_WT_GRADE_CMD`  (sizeof = 3 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `ItemInven` | `ITEM_INVEN` |
|     2 | `Grade` | `unsigned char` |


### `PROTO_NC_WT_LICENSE_ACK`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |
|     2 | `weaponitem` | `ITEM_INVEN` |
|     4 | `bSet` | `unsigned char` |
|     5 | `err` | `unsigned short` |


### `PROTO_NC_WT_LICENSE_CLR_ACK`  (sizeof = 24 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `echo_data` | `PROTO_NC_WT_LICENSE_CLR_ACK::echo_data` |
|    12 | `WeaponItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `MobID` | `unsigned short` |
|    22 | `ErrorCode` | `unsigned short` |


### `PROTO_NC_WT_LICENSE_CLR_ACK::echo_data`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |
|     2 | `weaponitem` | `ITEM_INVEN` |


### `PROTO_NC_WT_LICENSE_CLR_REQ`  (sizeof = 35 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `echo_data` | `PROTO_NC_WT_LICENSE_CLR_REQ::echo_data` |
|    12 | `WeaponItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    20 | `MobID` | `unsigned short` |
|    22 | `LicenseClearItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    30 | `LicenseClearItemID` | `unsigned short` |
|    32 | `LicenseClearItemDelete` | `unsigned char` |
|    33 | `LicenseClearAttr` | `unsigned short` |


### `PROTO_NC_WT_LICENSE_CLR_REQ::echo_data`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |
|     2 | `weaponitem` | `ITEM_INVEN` |


### `PROTO_NC_WT_LICENSE_REQ`  (sizeof = 7 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |
|     2 | `weaponitem` | `ITEM_INVEN` |
|     4 | `mobid` | `unsigned short` |
|     6 | `bSet` | `unsigned char` |


### `PROTO_NC_WT_LICENSE_SET_ACK`  (sizeof = 14 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `Error` | `unsigned short` |
|     8 | `lockindex` | `unsigned short` |
|    10 | `echo_data` | `PROTO_NC_WT_LICENSE_SET_ACK::echo_data` |


### `PROTO_NC_WT_LICENSE_SET_ACK::echo_data`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |
|     2 | `weaponitem` | `ITEM_INVEN` |


### `PROTO_NC_WT_LICENSE_SET_REQ`  (sizeof = 50 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `lockindex` | `unsigned short` |
|     8 | `chrregnum` | `unsigned long` |
|    12 | `chrname` | `Name4` |
|    28 | `WeaponItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    36 | `LicenItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    44 | `LicenseMobID` | `unsigned short` |
|    46 | `echo_data` | `PROTO_NC_WT_LICENSE_SET_REQ::echo_data` |


### `PROTO_NC_WT_LICENSE_SET_REQ::echo_data`  (sizeof = 4 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `licenseitem` | `ITEM_INVEN` |
|     2 | `weaponitem` | `ITEM_INVEN` |


### `PROTO_NC_WT_MOBINC_CMD`  (sizeof = 2 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `mob` | `unsigned short` |


### `PROTO_NC_WT_MOB_KILLCOUNT_SET_ACK`  (sizeof = 22 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `MobID` | `unsigned short` |
|    16 | `MobKillCount` | `unsigned long` |
|    20 | `Error` | `unsigned short` |


### `PROTO_NC_WT_MOB_KILLCOUNT_SET_REQ`  (sizeof = 20 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `ItemKey` | `SHINE_ITEM_REGISTNUMBER` |
|    14 | `MobID` | `unsigned short` |
|    16 | `MobKillCount` | `unsigned long` |


### `PROTO_NC_WT_TITLE_SET_ACK`  (sizeof = 8 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `Header` | `NETPACKETZONEHEADER` |
|     6 | `Error` | `unsigned short` |


### `PROTO_NC_ZONE_PRISON_END_CMD`  (sizeof = 26 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nCharZoneHandle` | `unsigned short` |
|     2 | `nCharRegNum` | `unsigned long` |
|     6 | `sRegenMapName` | `Name3` |
|    18 | `nRegenCoord` | `SHINE_XY_TYPE` |


### `PROTO_NC_ZONE_PRISON_GO_ACK`  (sizeof = 152 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nGmHandler` | `unsigned short` |
|     2 | `nGmRegNum` | `unsigned long` |
|     6 | `sGmUserID` | `Name18Byte` |
|    24 | `sCharID` | `Name4` |
|    40 | `nCharSessHandler` | `unsigned short` |
|    42 | `nCharZoneHandle` | `unsigned short` |
|    44 | `nCharRegNum` | `unsigned long` |
|    48 | `nMinute` | `unsigned short` |
|    50 | `sReason` | `char[16]   /* 16 bytes */` |
|    66 | `sRemark` | `char[64]   /* 64 bytes */` |
|   130 | `err` | `unsigned short` |
|   132 | `sRegenMapName` | `Name3` |
|   144 | `nRegenCoord` | `SHINE_XY_TYPE` |


### `PROTO_NC_ZONE_PRISON_GO_REQ`  (sizeof = 130 bytes)

| Offset | Field | Type |
|-------:|-------|------|
|     0 | `nGmHandler` | `unsigned short` |
|     2 | `nGmRegNum` | `unsigned long` |
|     6 | `sGmUserID` | `Name18Byte` |
|    24 | `sCharID` | `Name4` |
|    40 | `nCharSessHandle` | `unsigned short` |
|    42 | `nCharZoneHandle` | `unsigned short` |
|    44 | `nCharRegNum` | `unsigned long` |
|    48 | `nMinute` | `unsigned short` |
|    50 | `sReason` | `char[16]   /* 16 bytes */` |
|    66 | `sRemark` | `char[64]   /* 64 bytes */` |

