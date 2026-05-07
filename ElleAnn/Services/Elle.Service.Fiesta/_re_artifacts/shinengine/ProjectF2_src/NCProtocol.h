// NCProtocol.h
// NC_ NetCommand opcode definitions
//
// Recovered from Login.pdb (485), WorldManager.pdb (2173),
// 5ZoneServer2.pdb (2429) -- 5087 unique symbols.
// Hex values for confirmed opcodes are from live FSB packet captures.
// Unconfirmed values are 0x0000 -- fill in as captures arrive.
//
// Naming convention: NC_[SUBSYSTEM]_[VERB]_[REQ|ACK|CMD|NFY|ERR]
//   REQ  = client -> server request
//   ACK  = server -> client response
//   CMD  = server -> client command (no response expected)
//   NFY  = server -> client notification (broadcast)
//   ERR  = error response

#pragma once

//================================================================
// Confirmed opcodes (hex values from live captures)
//================================================================

// -- Login server (port 9010) -------------------------------------
#define NC_USER_VERSION_REQ				0x0C01	// -> version hello
#define NC_USER_SEED_ACK				0x0C03	// <- cipher seed
#define NC_USER_LOGIN_REQ				0x0C06	// -> credentials
#define NC_USER_XTRAP_REQ				0x0C04	// -> xTrap hash (static)
#define NC_USER_XTRAP_ACK				0x0C05	// <- 01 = accepted
#define NC_USER_LOGIN_ACK				0x0C0A	// <- world list
#define NC_USER_WORLD_STATUS_REQ		0x0C1B	// -> poll world status
#define NC_USER_WORLD_STATUS_ACK		0x0C1C	// <- world status
#define NC_USER_WORLDSELECT_REQ			0x0C0B	// -> select world
#define NC_USER_WORLDSELECT_ACK			0x0C0C	// <- WM IP + session token

// -- WorldManager (port 9110) ------------------------------------
#define NC_WM_HANDSHAKE_REQ				0x200F	// -> 80-byte encrypted token
#define NC_USER_LOGINWORLD_ACK			0x0C14	// <- char data block
#define NC_MISC_GAMETIME_ACK			0x080E	// <- server time HH MM SS
#define NC_CHAR_LOGIN_ACK				0x1003	// <- Zone IP + port
#define NC_CHAR_OPTION_SHORTCUT_CMD		0x7032	// <- shortcut bindings
#define NC_CHAR_OPTION_KEYMAP_CMD		0x7033	// <- key bindings (95 * 4 bytes)
#define NC_CHAR_OPTION_GAMEOPTION_CMD	0x7034	// <- game options (24 * 3 bytes)

// -- Zone server (port 9120) -------------------------------------
#define NC_MAP_LOGIN_REQ				0x1801	// -> char slot + name + SHA token + SHN hash
#define NC_MAP_LOGIN_ACK				0x1038	// <- char id + stats + map
#define NC_MAP_CHARINFO_CMD				0x1039	// <- char info broadcast
#define NC_MAP_INVENTORY_CMD			0x103A	// <- inventory state
#define NC_MAP_EQUIPMENT_CMD			0x103B	// <- equipment state
#define NC_MAP_SKILLBUFF_CMD			0x10CE	// <- skill/buff state
#define NC_MAP_QUEST_CMD				0x10D7	// <- quest state
#define NC_MAP_WORLDTICK_CMD			0x7C07	// <- zone world tick u32
#define NC_MAP_WORLDPARAMS_CMD			0x700D	// <- 23 floats + 9 u32 constants
#define NC_MAP_ZONELIST_CMD				0x7005	// <- zone count + zone ID array

//================================================================
// Unconfirmed opcodes -- names recovered from PDB symbols
// Values TBD from capture analysis
//================================================================

// -- Login subsystem (NC_USER_) ----------------------------------
// Opcode values unknown -- names from Login.pdb
// Regional password check variants:
//   NC_USER_US_PASSWORD_CHECK_ACK
//   NC_USER_TW_PASSWORD_CHECK_ACK
//   NC_USER_CH_PASSWORD_CHECK_ACK
//   NC_USER_GER_PASSWORD_CHECK_ACK
//   NC_USER_JP_PASSWORD_CHECK_ACK
//   NC_USER_LOGIN_NETMARBLE_REQ
//   NC_USER_LOGIN_OUTSPARK_REQ
//   NC_USER_LOGIN_WITH_OTP_REQ
//   NC_USER_LOGOUT_REQ
//   NC_USER_CHARSELECT_REQ
//   NC_USER_CHARDELETE_REQ
//   NC_USER_CHARCREATE_REQ

// -- Character subsystem (NC_CHAR_) ------------------------------
//   NC_CHAR_CLIENT_BASE_CMD
//   NC_CHAR_CLIENT_GAME_CMD
//   NC_CHAR_CLIENT_SHAPE_CMD
//   NC_CHAR_CLIENT_ITEM_CMD
//   NC_CHAR_CLIENT_PASSIVE_CMD
//   NC_CHAR_OPTION_IMPROVE_GET_SHORTCUTDATA_CMD   = 0x7032
//   NC_CHAR_OPTION_IMPROVE_GET_KEYMAP_CMD         = 0x7033
//   NC_CHAR_OPTION_IMPROVE_GET_GAMEOPTION_CMD     = 0x7034
//   NC_CHAR_OPTION_IMPROVE_SET_SHORTCUTDATA_CMD
//   NC_CHAR_OPTION_IMPROVE_SET_KEYMAP_CMD
//   NC_CHAR_OPTION_IMPROVE_SET_GAMEOPTION_CMD

// -- Map/Zone subsystem (NC_MAP_) --------------------------------
//   NC_MAP_LOGOFF_REQ
//   NC_MAP_HEARTBEAT_REQ
//   NC_MAP_HEARTBEAT_ACK
//   NC_MAP_CHANGETARGET_CMD
//   NC_MAP_ENTER_CMD
//   NC_MAP_EXIT_CMD
//   NC_MAP_MOVE_REQ
//   NC_MAP_MOVE_CMD
//   NC_MAP_JUMP_REQ
//   NC_MAP_JUMP_CMD
//   NC_MAP_STOP_MOVE_REQ
//   NC_MAP_STOP_MOVE_CMD
//   NC_MAP_SITDOWN_REQ
//   NC_MAP_SITDOWN_CMD

// -- Actor/NPC subsystem (NC_ACT_) --------------------------------
//   NC_ACT_SOMONEUSE_REQ
//   NC_ACT_NPCCLICK_REQ
//   NC_ACT_NPCMENU_REQ

// -- Battle subsystem (NC_BAT_) ----------------------------------
//   NC_BAT_ATTACK_REQ
//   NC_BAT_ATTACK_CMD
//   NC_BAT_SKILL_REQ
//   NC_BAT_SKILL_ACK
//   NC_BAT_DEAD_CMD
//   NC_BAT_RESURRECT_REQ

// -- Item subsystem (NC_ITEM_) -----------------------------------
//   NC_ITEM_PICKUP_REQ
//   NC_ITEM_DROP_REQ
//   NC_ITEM_EQUIP_REQ
//   NC_ITEM_UNEQUIP_REQ
//   NC_ITEM_USE_REQ
//   NC_ITEM_MOVE_REQ

// -- Inventory/Storage (NC_STORE_) --------------------------------
//   NC_STORE_BUY_REQ
//   NC_STORE_SELL_REQ
//   NC_STORE_CLOSE_REQ

// -- Party subsystem (NC_PARTY_) ---------------------------------
//   NC_PARTY_CREATE_REQ
//   NC_PARTY_JOIN_REQ
//   NC_PARTY_LEAVE_REQ
//   NC_PARTY_INFO_CMD
//   NC_PARTY_MEMBER_CMD

// -- Guild subsystem (NC_GUILD_) ---------------------------------
//   NC_GUILD_CREATE_REQ
//   NC_GUILD_JOIN_REQ
//   NC_GUILD_LEAVE_REQ
//   NC_GUILD_INFO_CMD

// -- Quest subsystem (NC_QUEST_) ---------------------------------
//   NC_QUEST_START_REQ
//   NC_QUEST_END_REQ
//   NC_QUEST_GIVEUP_REQ
//   NC_QUEST_CLIENT_ACTIVATE_CMD
//   NC_QUEST_DO_CMD

// -- Chat subsystem (NC_CHAT_) -----------------------------------
//   NC_CHAT_NORMAL_REQ
//   NC_CHAT_WHISPER_REQ
//   NC_CHAT_PARTY_REQ
//   NC_CHAT_GUILD_REQ
//   NC_CHAT_SHOUT_REQ
//   NC_CHAT_GM_REQ
//   NC_CHAT_NORMAL_CMD
//   NC_CHAT_WHISPER_CMD

// -- Friend subsystem (NC_FRIEND_) --------------------------------
//   NC_FRIEND_ADD_REQ
//   NC_FRIEND_DELETE_REQ
//   NC_FRIEND_LIST_CMD

// -- Skill subsystem (NC_SKILL_) ---------------------------------
//   NC_SKILL_GET_SKILLUP_REQ
//   NC_SKILL_SKILLUP_ACK
//   NC_SKILL_LIST_CMD
//   NC_SKILL_ACTIVATE_REQ
//   NC_SKILL_DEACTIVATE_REQ

// -- Misc subsystem (NC_MISC_) -----------------------------------
//   NC_MISC_GAMETIME_ACK         = 0x080E
//   NC_MISC_SERVER_TIME_CMD
//   NC_MISC_SEED_ACK
//   NC_MISC_COININFO_ACK

//================================================================
// xTrap static token
// Confirmed identical across all observed sessions.
// MoePromise launcher injects this into NC_USER_XTRAP_REQ.
// Login server handler fc_NC_USER_XTRAP_REQ always ACKs 0x01.
//================================================================
#define XTRAP_TOKEN_LEN		28
#define XTRAP_TOKEN_STR		"33B543B0CA6E7C41E5D1D0651307"

//================================================================
// NC_MAP_LOGIN_REQ SHA token
// 326-character hex string. Same every session -- SHA1 of
// client binary (SHA1Digest_PeFile_4096 over first 4096 bytes).
// DO NOT recompute at runtime -- cache this on first call.
//================================================================
#define MAP_LOGIN_SHA_LEN	326
#define MAP_LOGIN_SHA_STR	\
	"82ff8a3ac5bcbc3ddf944bbc085e017d2329a707fd18d89a4dcc1b40b1b9150247cb62ddaf8656737b4ef5" \
	"86a04ca6d40994dad6f41c1ced5865d4ba905a01fb91dc5d8cbc1f97495aae0eaa8ba82245073b53a9b86" \
	"d6b628242fa237ad636e11046be5cd5eb1c8fd6b774417b935cb68bab0cfbe71c71ebb89862bb10acecda" \
	"9940006b5a8996ddee4087b60acde3ff1e347edf8404659a96942bd0b52b19ed4bed62"
