// protocommon.h
// Shared typedefs and base structures used across all proto*.h files.
// Source: E:\ProjectF2\CSCode\protocol\protocommon.h

#pragma once
#include "typedef.h"

#pragma pack( push, 1 )

//------------------------------------------------------------------
// Common field sizes
//------------------------------------------------------------------
#define NAME_LENGTH			16		// character / guild / world name
#define MAP_LENGTH			8		// map name (e.g. "RouN")
#define IP_LENGTH			16		// dotted decimal IP string
#define ACCOUNT_LENGTH		16		// account name
#define PASSWORD_LENGTH		16		// password (fixed-width)
#define CHARNAME_LENGTH		NAME_LENGTH

//------------------------------------------------------------------
// Proto base -- every NC packet body struct inherits from this
// or is cast from the raw data buffer.
//------------------------------------------------------------------
struct ProtoNcBase
{
	// intentionally empty -- used as cast target only
};

//------------------------------------------------------------------
// Common embedded sub-structs
//------------------------------------------------------------------

// Minimal mobile object position
struct ProtoPosition
{
	WORD	wX;
	WORD	wY;
};

// Brief character identity (used in party / guild member lists)
struct ProtoBriefChar
{
	DWORD	dwCharID;
	char	szName[ NAME_LENGTH ];
	BYTE	bClass;
	BYTE	bLevel;
};

#pragma pack( pop )
