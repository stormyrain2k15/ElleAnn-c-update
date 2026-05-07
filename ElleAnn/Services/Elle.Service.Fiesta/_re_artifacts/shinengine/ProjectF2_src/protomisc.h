// protomisc.h
// NC_MISC_* packet structures -- miscellaneous server protocol.
// Source: E:\ProjectF2\CSCode\protocol\protomisc.h
//
// NC_MISC_GAMETIME_ACK confirmed from Port 9110 capture (opcode 080E).

#pragma once
#include "protocommon.h"
#include "NCProtocol.h"

#pragma pack( push, 1 )

//------------------------------------------------------------------
// 080E NC_MISC_GAMETIME_ACK  (worldmanager -> client)
// Server time: 3 bytes, HH MM SS.
// Example from capture: 12 36 2C = 18:54:44
//------------------------------------------------------------------
struct ProtoNcMiscGameTimeAck
{
	BYTE	bHour;
	BYTE	bMinute;
	BYTE	bSecond;
};

/*------------------------------------------------------------------
  NC_MISC_SERVER_TIME_CMD         TODO
  NC_MISC_SEED_ACK                TODO
  NC_MISC_COININFO_ACK            TODO
  NC_MISC_NOTICE_CMD              TODO
  NC_MISC_ANNOUNCE_CMD            TODO
------------------------------------------------------------------*/

#pragma pack( pop )
