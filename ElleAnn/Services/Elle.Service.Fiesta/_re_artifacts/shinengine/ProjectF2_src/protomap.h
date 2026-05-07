// protomap.h
// NC_MAP_* packet structures -- Zone server protocol.
// Source: E:\ProjectF2\CSCode\protocol\protomap.h
//
// Login flood opcodes 1038-10D7 confirmed from captures.
// In-game movement / interaction opcodes: TODO -- packet capture pending.

#pragma once
#include "protocommon.h"
#include "NCProtocol.h"

#pragma pack( push, 1 )

//------------------------------------------------------------------
// 1801 NC_MAP_LOGIN_REQ  (client -> zone)
// szSHA is a 326-char ASCII hex string -- SHA of client binary.
// Static: same every session for the same client version.
// abSHNHash is variable-length encrypted SHN verification blob.
//------------------------------------------------------------------
#define NC_MAP_SHA_LEN		326
struct ProtoNcMapLoginReq
{
	WORD	wCharSlot;
	char	szName[ NAME_LENGTH ];
	char	szSHA[ NC_MAP_SHA_LEN ];
	/* BYTE abSHNHash[] -- variable, follows szSHA */
};

//------------------------------------------------------------------
// 1038 NC_MAP_LOGIN_ACK  (zone -> client)
// Confirmed: dwCharID=6 for Crystal, 5 for ElleAnn.
// bLevel=0x96 (150). szMap="RouN" (Roumen).
//------------------------------------------------------------------
struct ProtoNcMapLoginAck
{
	DWORD	dwCharID;
	char	szName[ NAME_LENGTH ];
	BYTE	bLevel;
	char	szMap[ MAP_LENGTH ];
	DWORD	dwHPCur;
	DWORD	dwHPMax;
	DWORD	dwMPCur;
	DWORD	dwMPMax;
	DWORD	dwGold;
	DWORD	dwExp;
	/* extended stat block follows -- size TBD from captures */
};

//------------------------------------------------------------------
// 1039 NC_MAP_CHARINFO_CMD  (zone -> client)
// char_id + extended stat block broadcast to nearby clients.
//------------------------------------------------------------------
struct ProtoNcMapCharInfoCmd
{
	DWORD	dwCharID;
	/* extended stat block -- TODO: packet capture pending */
};

//------------------------------------------------------------------
// 103A NC_MAP_INVENTORY_CMD  (zone -> client)
//------------------------------------------------------------------
struct ProtoNcMapInventoryCmd
{
	DWORD	dwCharID;
	WORD	wItemCount;
	/* item array[wItemCount] -- TODO: packet capture pending */
};

//------------------------------------------------------------------
// 103B NC_MAP_EQUIPMENT_CMD  (zone -> client)
//------------------------------------------------------------------
struct ProtoNcMapEquipmentCmd
{
	DWORD	dwCharID;
	WORD	wSlotCount;
	/* slot array[wSlotCount] -- TODO: packet capture pending */
};

//------------------------------------------------------------------
// 10CE NC_MAP_SKILLBUFF_CMD  (zone -> client)
//------------------------------------------------------------------
struct ProtoNcMapSkillBuffCmd
{
	DWORD	dwCharID;
	WORD	wBuffCount;
	/* buff array[wBuffCount] -- TODO: packet capture pending */
};

//------------------------------------------------------------------
// 10D7 NC_MAP_QUEST_CMD  (zone -> client)
//------------------------------------------------------------------
struct ProtoNcMapQuestCmd
{
	DWORD	dwCharID;
	WORD	wQuestFlags;
	/* quest data -- TODO: packet capture pending */
};

//------------------------------------------------------------------
// 7C07 NC_MAP_WORLDTICK_CMD  (zone -> client)
//------------------------------------------------------------------
struct ProtoNcMapWorldTickCmd
{
	DWORD	dwTick;		// world time reference
};

//------------------------------------------------------------------
// 700D NC_MAP_WORLDPARAMS_CMD  (zone -> client)
// 23 floats (game constants / stat modifiers) + 9 DWORD counters.
// Full field names: TODO -- packet capture + SHN cross-ref pending.
//------------------------------------------------------------------
struct ProtoNcMapWorldParamsCmd
{
	float	fParams[23];
	DWORD	dwCounters[9];
};

//------------------------------------------------------------------
// 7005 NC_MAP_ZONELIST_CMD  (zone -> client)
//------------------------------------------------------------------
struct ProtoNcMapZoneListCmd
{
	BYTE	bZoneCount;
	DWORD	adwZoneID[1];	// bZoneCount entries
};

//------------------------------------------------------------------
// In-game movement / interaction opcodes
// Opcode values and payload layouts: TODO -- packet capture pending
//------------------------------------------------------------------

/* NC_MAP_LOGOFF_REQ                  TODO */
/* NC_MAP_HEARTBEAT_REQ               TODO */
/* NC_MAP_HEARTBEAT_ACK               TODO */
/* NC_MAP_CHANGETARGET_CMD            TODO */
/* NC_MAP_ENTER_CMD                   TODO */
/* NC_MAP_EXIT_CMD                    TODO */
/* NC_MAP_MOVE_REQ                    TODO */
/* NC_MAP_MOVE_CMD                    TODO */
/* NC_MAP_JUMP_REQ                    TODO */
/* NC_MAP_JUMP_CMD                    TODO */
/* NC_MAP_STOP_MOVE_REQ               TODO */
/* NC_MAP_STOP_MOVE_CMD               TODO */
/* NC_MAP_SITDOWN_REQ                 TODO */
/* NC_MAP_SITDOWN_CMD                 TODO */
/* NC_MAP_REVIVE_REQ                  TODO */
/* NC_MAP_REVIVE_CMD                  TODO */
/* NC_MAP_CHANGEDIRECTION_CMD         TODO */
/* NC_MAP_MAPCHANGE_REQ               TODO */
/* NC_MAP_MAPCHANGE_CMD               TODO */

#pragma pack( pop )
