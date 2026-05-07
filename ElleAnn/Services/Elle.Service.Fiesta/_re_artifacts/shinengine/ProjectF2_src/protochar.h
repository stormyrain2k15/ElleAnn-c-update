// protochar.h
// NC_CHAR_* packet structures -- WorldManager + Zone character protocol.
// Source: E:\ProjectF2\CSCode\protocol\protochar.h
//
// Hop 2 packets confirmed from Port 9110 captures.
// Option / key binding packets confirmed from WorldManager session.
// In-game char management opcodes: TODO -- packet capture pending.

#pragma once
#include "protocommon.h"
#include "NCProtocol.h"

#pragma pack( push, 1 )

//------------------------------------------------------------------
// 200F NC_WM_HANDSHAKE_REQ  (client -> worldmanager)
// 80-byte encrypted token from NC_USER_WORLDSELECT_ACK (0C0C).
//------------------------------------------------------------------
struct ProtoNcWmHandshakeReq
{
	BYTE	abToken[ NC_SESSION_TOKEN_LEN ];
};

//------------------------------------------------------------------
// 0C14 NC_USER_LOGINWORLD_ACK  (worldmanager -> client)
// Confirmed: Crystal dwCharID=6, ElleAnn=5, bLevel=0x96, szMap="RouN".
//------------------------------------------------------------------
struct ProtoNcUserLoginWorldAck
{
	WORD	wCharSlot;
	DWORD	dwCharID;
	char	szName[ NAME_LENGTH ];
	BYTE	bPad[2];
	BYTE	bLevel;			// 0x96 = 150
	BYTE	abStats[32];	/* full stat block -- field names TODO */
	char	szMap[ MAP_LENGTH ];
};

//------------------------------------------------------------------
// 1003 NC_CHAR_LOGIN_ACK  (worldmanager -> client)
// Redirects client to Zone. wZonePort confirmed 0x2320 (9120).
//------------------------------------------------------------------
struct ProtoNcCharLoginAck
{
	char			szZoneIP[ IP_LENGTH ];
	BYTE			bNul;
	WORD			wZonePort;	// 0x2320 = 9120
};

//------------------------------------------------------------------
// 7032 NC_CHAR_OPTION_IMPROVE_GET_SHORTCUTDATA_CMD
// Shortcut bar bindings blob.
//------------------------------------------------------------------
struct ProtoNcCharOptionShortcutCmd
{
	BYTE	bCount;
	/* shortcut entries[bCount] -- TODO: packet capture pending */
};

//------------------------------------------------------------------
// 7033 NC_CHAR_OPTION_IMPROVE_GET_KEYMAP_CMD
// 95 key bindings x 4 bytes each.
//------------------------------------------------------------------
#define NC_KEYMAP_COUNT		95
struct ProtoNcCharOptionKeyMapCmd
{
	DWORD	adwKeys[ NC_KEYMAP_COUNT ];
};

//------------------------------------------------------------------
// 7034 NC_CHAR_OPTION_IMPROVE_GET_GAMEOPTION_CMD
// 24 game options x 3 bytes each.
//------------------------------------------------------------------
#define NC_GAMEOPTION_COUNT	24
struct ProtoNcCharOptionGameOptionCmd
{
	BYTE	aOptions[ NC_GAMEOPTION_COUNT * 3 ];
};

//------------------------------------------------------------------
// In-game character management opcodes
// Opcode values and payload layouts: TODO -- packet capture pending
//------------------------------------------------------------------

/* NC_CHAR_CLIENT_BASE_CMD                         TODO */
/* NC_CHAR_CLIENT_GAME_CMD                         TODO */
/* NC_CHAR_CLIENT_SHAPE_CMD                        TODO */
/* NC_CHAR_CLIENT_ITEM_CMD                         TODO */
/* NC_CHAR_CLIENT_PASSIVE_CMD                      TODO */
/* NC_CHAR_OPTION_IMPROVE_SET_SHORTCUTDATA_CMD     TODO */
/* NC_CHAR_OPTION_IMPROVE_SET_KEYMAP_CMD           TODO */
/* NC_CHAR_OPTION_IMPROVE_SET_GAMEOPTION_CMD       TODO */
/* NC_CHAR_STATDISTRIBUTE_REQ                      TODO */
/* NC_CHAR_STATDISTRIBUTE_ACK                      TODO */

#pragma pack( pop )
