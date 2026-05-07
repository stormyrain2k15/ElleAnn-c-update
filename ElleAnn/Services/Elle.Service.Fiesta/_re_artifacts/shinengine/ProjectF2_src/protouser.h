// protouser.h
// NC_USER_* packet structures -- Login server protocol.
// Source: E:\ProjectF2\CSCode\protocol\protouser.h
//
// All structs confirmed from live packet captures (Port 9010 FSB).
// Regional password check variants confirmed from Login.pdb symbols.

#pragma once
#include "protocommon.h"
#include "NCProtocol.h"

#pragma pack( push, 1 )

//------------------------------------------------------------------
// 0C01 NC_USER_VERSION_REQ  (client -> login)
//------------------------------------------------------------------
struct ProtoNcUserVersionReq
{
	WORD	wYear;		// 0x07D6 = 2006
	BYTE	bMonth;		// 0x04
	BYTE	bDay;		// 0x0A
	BYTE	bUnk;		// 0x00
};

//------------------------------------------------------------------
// 0C03 NC_USER_SEED_ACK  (login -> client)
//------------------------------------------------------------------
struct ProtoNcUserSeedAck
{
	WORD	wSeed;		// LCG init seed for CPacketEncrypt
};

//------------------------------------------------------------------
// 0C06 NC_USER_LOGIN_REQ  (client -> login)
// Fixed-width, NUL-padded. NOT null-terminated if exactly 16 chars.
//------------------------------------------------------------------
struct ProtoNcUserLoginReq
{
	char	szAccount[ ACCOUNT_LENGTH ];
	char	szPassword[ PASSWORD_LENGTH ];
};

//------------------------------------------------------------------
// 0C04 NC_USER_XTRAP_REQ  (client -> login)
// MoePromise launcher injects the static 28-char hex token.
// bLen = 0x1D (29 bytes follow = token[28] + NUL).
//------------------------------------------------------------------
#define NC_XTRAP_TOKEN_LEN	28
struct ProtoNcUserXtrapReq
{
	BYTE	bLen;								// 0x1D
	char	szToken[ NC_XTRAP_TOKEN_LEN ];		// always "33B543B0CA6E7C41E5D1D0651307"
	BYTE	bNul;								// 0x00
};

//------------------------------------------------------------------
// 0C05 NC_USER_XTRAP_ACK  (login -> client)
// bResult = 0x01 always. Login server never rejects xTrap.
//------------------------------------------------------------------
struct ProtoNcUserXtrapAck
{
	BYTE	bResult;	// 0x01 = accepted
};

//------------------------------------------------------------------
// 0C0A NC_USER_LOGIN_ACK  (login -> client)
//------------------------------------------------------------------
struct ProtoNcUserWorldEntry
{
	char	szName[ NAME_LENGTH ];
	BYTE	bStatus;		// 0x05 = online
};

struct ProtoNcUserLoginAck
{
	BYTE				bWorldCount;
	ProtoNcUserWorldEntry worlds[1];	// bWorldCount entries
};

//------------------------------------------------------------------
// 0C1B NC_USER_WORLD_STATUS_REQ  (client -> login)
// Empty body. Client sends every 10 seconds until world selected.
//------------------------------------------------------------------
struct ProtoNcUserWorldStatusReq
{
	// empty
};

//------------------------------------------------------------------
// 0C1C NC_USER_WORLD_STATUS_ACK  (login -> client)
// Same structure as WorldEntry but status byte increments each poll.
//------------------------------------------------------------------
struct ProtoNcUserWorldStatusAck
{
	BYTE				bWorldCount;
	ProtoNcUserWorldEntry worlds[1];
};

//------------------------------------------------------------------
// 0C0B NC_USER_WORLDSELECT_REQ  (client -> login)
//------------------------------------------------------------------
struct ProtoNcUserWorldSelectReq
{
	BYTE	bWorldIdx;		// 0x00 = first world
};

//------------------------------------------------------------------
// 0C0C NC_USER_WORLDSELECT_ACK  (login -> client)
// Contains WorldManager IP + 80-byte encrypted session token.
// Token passed verbatim to 200F WM handshake.
//------------------------------------------------------------------
#define NC_SESSION_TOKEN_LEN	80
struct ProtoNcUserWorldSelectAck
{
	BYTE	bWorldIdx;
	char	szWMIP[ IP_LENGTH ];
	BYTE	bPad[2];
	BYTE	abToken[ NC_SESSION_TOKEN_LEN ];
};

//------------------------------------------------------------------
// Regional login variants (Login.pdb confirmed)
// Opcode values: TODO -- packet capture pending
//------------------------------------------------------------------

/* NC_USER_LOGIN_NETMARBLE_REQ    -- Netmarble (KR) auth flow   TODO */
/* NC_USER_LOGIN_OUTSPARK_REQ     -- Outspark (US) auth flow    TODO */
/* NC_USER_LOGIN_WITH_OTP_REQ     -- OTP two-factor auth        TODO */
/* NC_USER_US_PASSWORD_CHECK_ACK  -- US regional check result   TODO */
/* NC_USER_TW_PASSWORD_CHECK_ACK  -- TW regional check result   TODO */
/* NC_USER_CH_PASSWORD_CHECK_ACK  -- CH regional check result   TODO */
/* NC_USER_GER_PASSWORD_CHECK_ACK -- GER regional check result  TODO */
/* NC_USER_JP_PASSWORD_CHECK_ACK  -- JP regional check result   TODO */

#pragma pack( pop )
