// pf_client.cpp
// Zone <-> Client packet handler functions.
// Source: E:\ProjectF2\Server\5ZoneServer2\pf_client.cpp
//
// Confirmed handlers (from Port 9120 captures):
//   1801  NC_MAP_LOGIN_REQ
//
// All in-game handlers:
//   TODO -- packet capture pending (gameplay session required)
//
// Handler naming convention: On_NC_[SUBSYSTEM]_[VERB]
// All functions declared in clientsession.h.

#include "stdafx.h"
#include "ClientSession.h"
#include "protomap.h"
#include "protobat.h"
#include "protoitem.h"
#include "protoskill.h"
#include "protoquest.h"
#include "protoparty.h"
#include "protofriend.h"
#include "protoguild.h"
#include "NCProtocol.h"
#include "ShineAssert.h"

//==================================================================
// MAP -- confirmed from captures
//==================================================================

//------------------------------------------------------------------
// 1801 NC_MAP_LOGIN_REQ
// szSHA is 326-char static hex token (same every session).
// Triggers LuaScriptSecurity -> CHttpConnect -> Elle HTTP.
//------------------------------------------------------------------
void CClientSession::On_NC_MAP_LOGIN_REQ(
    const ProtoNcMapLoginReq* pPkt, int nLen )
{
	/* TODO -- full handler:
	   1. Validate SHA token against SHN checksum
	   2. Fire LuaScriptSecurity::cExecCheck -> CHttpConnect
	   3. On Elle ACK: load char data, send 1038 + flood packets
	*/
	ShineLog( "On_NC_MAP_LOGIN_REQ slot=%u name=%s",
	          pPkt->wCharSlot, pPkt->szName );
}

//==================================================================
// MAP -- in-game movement / interaction
// TODO -- packet capture pending
//==================================================================

void CClientSession::On_NC_MAP_LOGOFF_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_MAP_HEARTBEAT_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_MAP_MOVE_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_MAP_JUMP_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_MAP_STOP_MOVE_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_MAP_SITDOWN_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_MAP_REVIVE_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_MAP_MAPCHANGE_REQ( const void* p, int n )
{ /* TODO */ }

//==================================================================
// BATTLE -- TODO packet capture pending
//==================================================================

void CClientSession::On_NC_BAT_ATTACK_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_BAT_SKILL_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_BAT_RESURRECT_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_BAT_CHARGEATTACK_REQ( const void* p, int n )
{ /* TODO */ }

//==================================================================
// ITEM -- TODO packet capture pending
//==================================================================

void CClientSession::On_NC_ITEM_PICKUP_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_ITEM_DROP_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_ITEM_EQUIP_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_ITEM_UNEQUIP_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_ITEM_USE_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_ITEM_MOVE_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_ITEM_UPGRADE_REQ( const void* p, int n )
{ /* TODO */ }

//==================================================================
// SKILL -- TODO packet capture pending
//==================================================================

void CClientSession::On_NC_SKILL_GET_SKILLUP_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_SKILL_ACTIVATE_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_SKILL_DEACTIVATE_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_SKILL_LEARN_REQ( const void* p, int n )
{ /* TODO */ }

//==================================================================
// QUEST -- TODO packet capture pending
//==================================================================

void CClientSession::On_NC_QUEST_START_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_QUEST_END_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_QUEST_GIVEUP_REQ( const void* p, int n )
{ /* TODO */ }

//==================================================================
// PARTY -- TODO packet capture pending
//==================================================================

void CClientSession::On_NC_PARTY_CREATE_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_PARTY_JOIN_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_PARTY_LEAVE_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_PARTY_KICKOUT_REQ( const void* p, int n )
{ /* TODO */ }

//==================================================================
// FRIEND -- TODO packet capture pending
//==================================================================

void CClientSession::On_NC_FRIEND_ADD_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_FRIEND_DELETE_REQ( const void* p, int n )
{ /* TODO */ }

//==================================================================
// GUILD -- TODO packet capture pending
//==================================================================

void CClientSession::On_NC_GUILD_CREATE_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_GUILD_JOIN_REQ( const void* p, int n )
{ /* TODO */ }

void CClientSession::On_NC_GUILD_LEAVE_REQ( const void* p, int n )
{ /* TODO */ }

//==================================================================
// Dispatch
//==================================================================
void CClientSession::OnPacket( WORD wOpCode,
                                const void* pData, int nLen )
{
	switch( wOpCode )
	{
	// -- Map --
	case NC_MAP_LOGIN_REQ:
		On_NC_MAP_LOGIN_REQ( (ProtoNcMapLoginReq*)pData, nLen );  break;
	case NC_MAP_LOGOFF_REQ:
		On_NC_MAP_LOGOFF_REQ( pData, nLen );                      break;
	case NC_MAP_HEARTBEAT_REQ:
		On_NC_MAP_HEARTBEAT_REQ( pData, nLen );                   break;
	case NC_MAP_MOVE_REQ:
		On_NC_MAP_MOVE_REQ( pData, nLen );                        break;
	case NC_MAP_JUMP_REQ:
		On_NC_MAP_JUMP_REQ( pData, nLen );                        break;
	case NC_MAP_STOP_MOVE_REQ:
		On_NC_MAP_STOP_MOVE_REQ( pData, nLen );                   break;
	case NC_MAP_SITDOWN_REQ:
		On_NC_MAP_SITDOWN_REQ( pData, nLen );                     break;
	case NC_MAP_REVIVE_REQ:
		On_NC_MAP_REVIVE_REQ( pData, nLen );                      break;
	case NC_MAP_MAPCHANGE_REQ:
		On_NC_MAP_MAPCHANGE_REQ( pData, nLen );                   break;

	// -- Battle --
	case NC_BAT_ATTACK_REQ:
		On_NC_BAT_ATTACK_REQ( pData, nLen );                      break;
	case NC_BAT_SKILL_REQ:
		On_NC_BAT_SKILL_REQ( pData, nLen );                       break;
	case NC_BAT_RESURRECT_REQ:
		On_NC_BAT_RESURRECT_REQ( pData, nLen );                   break;
	case NC_BAT_CHARGEATTACK_REQ:
		On_NC_BAT_CHARGEATTACK_REQ( pData, nLen );                break;

	// -- Item --
	case NC_ITEM_PICKUP_REQ:
		On_NC_ITEM_PICKUP_REQ( pData, nLen );                     break;
	case NC_ITEM_DROP_REQ:
		On_NC_ITEM_DROP_REQ( pData, nLen );                       break;
	case NC_ITEM_EQUIP_REQ:
		On_NC_ITEM_EQUIP_REQ( pData, nLen );                      break;
	case NC_ITEM_UNEQUIP_REQ:
		On_NC_ITEM_UNEQUIP_REQ( pData, nLen );                    break;
	case NC_ITEM_USE_REQ:
		On_NC_ITEM_USE_REQ( pData, nLen );                        break;
	case NC_ITEM_MOVE_REQ:
		On_NC_ITEM_MOVE_REQ( pData, nLen );                       break;
	case NC_ITEM_UPGRADE_REQ:
		On_NC_ITEM_UPGRADE_REQ( pData, nLen );                    break;

	// -- Skill --
	case NC_SKILL_GET_SKILLUP_REQ:
		On_NC_SKILL_GET_SKILLUP_REQ( pData, nLen );               break;
	case NC_SKILL_ACTIVATE_REQ:
		On_NC_SKILL_ACTIVATE_REQ( pData, nLen );                  break;
	case NC_SKILL_DEACTIVATE_REQ:
		On_NC_SKILL_DEACTIVATE_REQ( pData, nLen );                break;
	case NC_SKILL_LEARN_REQ:
		On_NC_SKILL_LEARN_REQ( pData, nLen );                     break;

	// -- Quest --
	case NC_QUEST_START_REQ:
		On_NC_QUEST_START_REQ( pData, nLen );                     break;
	case NC_QUEST_END_REQ:
		On_NC_QUEST_END_REQ( pData, nLen );                       break;
	case NC_QUEST_GIVEUP_REQ:
		On_NC_QUEST_GIVEUP_REQ( pData, nLen );                    break;

	// -- Party --
	case NC_PARTY_CREATE_REQ:
		On_NC_PARTY_CREATE_REQ( pData, nLen );                    break;
	case NC_PARTY_JOIN_REQ:
		On_NC_PARTY_JOIN_REQ( pData, nLen );                      break;
	case NC_PARTY_LEAVE_REQ:
		On_NC_PARTY_LEAVE_REQ( pData, nLen );                     break;
	case NC_PARTY_KICKOUT_REQ:
		On_NC_PARTY_KICKOUT_REQ( pData, nLen );                   break;

	// -- Friend --
	case NC_FRIEND_ADD_REQ:
		On_NC_FRIEND_ADD_REQ( pData, nLen );                      break;
	case NC_FRIEND_DELETE_REQ:
		On_NC_FRIEND_DELETE_REQ( pData, nLen );                   break;

	// -- Guild --
	case NC_GUILD_CREATE_REQ:
		On_NC_GUILD_CREATE_REQ( pData, nLen );                    break;
	case NC_GUILD_JOIN_REQ:
		On_NC_GUILD_JOIN_REQ( pData, nLen );                      break;
	case NC_GUILD_LEAVE_REQ:
		On_NC_GUILD_LEAVE_REQ( pData, nLen );                     break;

	default:
		ShineLog( "CClientSession::OnPacket -- unknown opcode 0x%04X len=%d",
		          wOpCode, nLen );
		break;
	}
}
