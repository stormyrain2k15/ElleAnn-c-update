// pf_worldmanager.cpp
// Zone <-> WorldManager packet handler functions.
// Source: E:\ProjectF2\Server\5ZoneServer2\pf_worldmanager.cpp
//
// Hop 2 confirmed opcodes handled here:
//   0C14 NC_USER_LOGINWORLD_ACK
//   080E NC_MISC_GAMETIME_ACK
//   1003 NC_CHAR_LOGIN_ACK
//   7032 NC_CHAR_OPTION_IMPROVE_GET_SHORTCUTDATA_CMD
//   7033 NC_CHAR_OPTION_IMPROVE_GET_KEYMAP_CMD
//   7034 NC_CHAR_OPTION_IMPROVE_GET_GAMEOPTION_CMD
//
// Remaining handlers: TODO -- packet capture pending

#include "stdafx.h"
#include "WorldManagerSession.h"
#include "protochar.h"
#include "protomisc.h"
#include "NCProtocol.h"
#include "ShineAssert.h"

//------------------------------------------------------------------
// 0C14 NC_USER_LOGINWORLD_ACK
// Delivers character data for the selected character.
// dwCharID=6 for Crystal, 5 for ElleAnn (confirmed from captures).
//------------------------------------------------------------------
void CWorldManagerSession::On_NC_USER_LOGINWORLD_ACK(
    const ProtoNcUserLoginWorldAck* pPkt, int nLen )
{
	/* TODO -- full handler: load char into session, notify zone */
	ShineLog( "On_NC_USER_LOGINWORLD_ACK char=%u name=%s map=%s",
	          pPkt->dwCharID, pPkt->szName, pPkt->szMap );
}

//------------------------------------------------------------------
// 080E NC_MISC_GAMETIME_ACK
//------------------------------------------------------------------
void CWorldManagerSession::On_NC_MISC_GAMETIME_ACK(
    const ProtoNcMiscGameTimeAck* pPkt, int nLen )
{
	/* TODO -- sync server time to zone tick */
	ShineLog( "On_NC_MISC_GAMETIME_ACK %02d:%02d:%02d",
	          pPkt->bHour, pPkt->bMinute, pPkt->bSecond );
}

//------------------------------------------------------------------
// 1003 NC_CHAR_LOGIN_ACK
// Zone IP + port to hand off to client.
//------------------------------------------------------------------
void CWorldManagerSession::On_NC_CHAR_LOGIN_ACK(
    const ProtoNcCharLoginAck* pPkt, int nLen )
{
	/* TODO -- redirect client to zone */
	ShineLog( "On_NC_CHAR_LOGIN_ACK zone=%s:%u",
	          pPkt->szZoneIP, pPkt->wZonePort );
}

//------------------------------------------------------------------
// 7032 / 7033 / 7034  Character option CMDs
//------------------------------------------------------------------
void CWorldManagerSession::On_NC_CHAR_OPTION_SHORTCUT_CMD(
    const void* pData, int nLen )
{
	/* TODO -- packet capture pending */
}

void CWorldManagerSession::On_NC_CHAR_OPTION_KEYMAP_CMD(
    const ProtoNcCharOptionKeyMapCmd* pPkt, int nLen )
{
	/* TODO -- packet capture pending */
}

void CWorldManagerSession::On_NC_CHAR_OPTION_GAMEOPTION_CMD(
    const ProtoNcCharOptionGameOptionCmd* pPkt, int nLen )
{
	/* TODO -- packet capture pending */
}

//------------------------------------------------------------------
// Dispatch
//------------------------------------------------------------------
void CWorldManagerSession::OnPacket( WORD wOpCode,
                                      const void* pData, int nLen )
{
	switch( wOpCode )
	{
	case NC_USER_LOGINWORLD_ACK:
		On_NC_USER_LOGINWORLD_ACK( (ProtoNcUserLoginWorldAck*)pData, nLen );
		break;

	case NC_MISC_GAMETIME_ACK:
		On_NC_MISC_GAMETIME_ACK( (ProtoNcMiscGameTimeAck*)pData, nLen );
		break;

	case NC_CHAR_LOGIN_ACK:
		On_NC_CHAR_LOGIN_ACK( (ProtoNcCharLoginAck*)pData, nLen );
		break;

	case NC_CHAR_OPTION_SHORTCUT_CMD:
		On_NC_CHAR_OPTION_SHORTCUT_CMD( pData, nLen );
		break;

	case NC_CHAR_OPTION_KEYMAP_CMD:
		On_NC_CHAR_OPTION_KEYMAP_CMD(
		    (ProtoNcCharOptionKeyMapCmd*)pData, nLen );
		break;

	case NC_CHAR_OPTION_GAMEOPTION_CMD:
		On_NC_CHAR_OPTION_GAMEOPTION_CMD(
		    (ProtoNcCharOptionGameOptionCmd*)pData, nLen );
		break;

	default:
		ShineLog( "CWorldManagerSession::OnPacket -- unknown opcode 0x%04X",
		          wOpCode );
		break;
	}
}
