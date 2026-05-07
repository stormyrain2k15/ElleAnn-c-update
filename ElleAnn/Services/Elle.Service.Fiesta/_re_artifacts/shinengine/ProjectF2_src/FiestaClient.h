// FiestaClient.h
// ShineEngine client login state machine
//
// Implements the full three-hop login sequence:
//   Hop 1: Client <-> Login Server  (port 9010)
//   Hop 2: Client <-> WorldManager  (port 9110, IP from 0C0C ACK)
//   Hop 3: Client <-> Zone Server   (port 9120, IP from 1003 ACK)
//
// All packet layouts confirmed from live FSB captures (Crystal + ElleAnn sessions).
// All static tokens confirmed identical across sessions.
//
// Cipher: active on WAN (172.20.x.x), inactive on LAN (192.168.x.x).
// See FiestaCipher.h.

#pragma once
#include <windows.h>
#include <winsock2.h>
#include "NCProtocol.h"
#include "FiestaCipher.h"
#include "ServerID.h"

//================================================================
// Packet payload structures
// All fields little-endian unless noted.
// All strings NUL-padded to fixed width.
//================================================================

#pragma pack(push, 1)

// -- Hop 1: Login server -----------------------------------------

// 0C01 -> version hello (5 bytes payload)
struct PKT_VERSION_HELLO
{
	WORD	wYear;		// 0x07D6 = 2006
	BYTE	bMonth;		// 0x04
	BYTE	bDay;		// 0x0A
	BYTE	bUnk;		// 0x00
};

// 0C06 -> credentials (32 bytes payload)
// Username and password are fixed-width, NUL-padded, NOT null-terminated
// if exactly 16 chars.
#define SHINE_ACCOUNT_LEN	16
#define SHINE_PASS_LEN		16
struct PKT_LOGIN_REQ
{
	char	szAccount[SHINE_ACCOUNT_LEN];
	char	szPassword[SHINE_PASS_LEN];
};

// 0C04 -> xTrap request (30 bytes payload)
// bLen = 0x1D (29). szHash is always XTRAP_TOKEN_STR.
struct PKT_XTRAP_REQ
{
	BYTE	bLen;					// 0x1D
	char	szHash[XTRAP_TOKEN_LEN];// static hex string
	BYTE	bNul;					// 0x00
};

// 0C0A <- login ACK (variable)
// World entry count + array of world descriptors
#define SHINE_WORLDNAME_LEN	16
struct WORLD_ENTRY
{
	char	szName[SHINE_WORLDNAME_LEN];
	BYTE	bStatus;	// 05 = online
};
struct PKT_LOGIN_ACK
{
	BYTE		bCount;
	WORLD_ENTRY	worlds[1];	// bCount entries follow
};

// 0C0C <- WorldSelect ACK
// IP string is ASCII, NUL-padded to 16 bytes.
// Token is 80 bytes encrypted -- passed directly to 200F handshake.
#define SHINE_IP_LEN		16
#define SHINE_SESSION_TOKEN	80
struct PKT_WORLDSELECT_ACK
{
	BYTE	bWorldIdx;
	char	szWMIP[SHINE_IP_LEN];
	BYTE	bPad[2];
	BYTE	abToken[SHINE_SESSION_TOKEN];
};

// -- Hop 2: WorldManager -----------------------------------------

// 200F -> WM handshake (80 bytes, encrypted)
struct PKT_WM_HANDSHAKE
{
	BYTE	abToken[SHINE_SESSION_TOKEN];	// token from 0C0C verbatim
};

// 0C14 <- LoginWorld ACK
// Variable length. Layout confirmed from Crystal (char_id=6) and
// ElleAnn (char_id=5) sessions.
#define SHINE_CHARNAME_LEN	16
#define SHINE_MAPNAME_LEN	8
struct PKT_LOGINWORLD_ACK
{
	WORD	wCharSlot;
	DWORD	dwCharID;
	char	szName[SHINE_CHARNAME_LEN];
	BYTE	bUnk[2];
	BYTE	bLevel;			// 0x96 = 150
	BYTE	abStats[32];	// HP cur/max, MP cur/max, gold, exp, etc.
	char	szMap[SHINE_MAPNAME_LEN];	// "RouN" = Roumen
};

// 080E <- game time (3 bytes)
struct PKT_GAMETIME_ACK
{
	BYTE	bHour;
	BYTE	bMinute;
	BYTE	bSecond;
};

// 1003 <- char login ACK (Zone redirect)
struct PKT_CHARLOGIN_ACK
{
	char	szZoneIP[SHINE_IP_LEN];
	BYTE	bNul;
	WORD	wZonePort;	// 0x2320 = 9120
};

// -- Hop 3: Zone -------------------------------------------------

// 1801 -> map login request
// The 326-char SHA string is ASCII hex, NOT binary.
// abSHNHash is the encrypted SHN file verification blob (~500 bytes).
struct PKT_MAP_LOGIN_REQ
{
	WORD	wCharSlot;
	char	szName[SHINE_CHARNAME_LEN];
	char	szSHA[MAP_LOGIN_SHA_LEN];	// static -- see NCProtocol.h
	// BYTE abSHNHash[...] follows -- variable, encrypted
};

// 1038 <- map login ACK
struct PKT_MAP_LOGIN_ACK
{
	DWORD	dwCharID;
	char	szName[SHINE_CHARNAME_LEN];
	BYTE	bLevel;
	char	szMap[SHINE_MAPNAME_LEN];
	DWORD	dwHPCur;
	DWORD	dwHPMax;
	DWORD	dwMPCur;
	DWORD	dwMPMax;
	DWORD	dwGold;
	DWORD	dwExp;
};

#pragma pack(pop)

//================================================================
// Login state machine
//================================================================

enum eLoginState
{
	LS_IDLE = 0,

	// Hop 1 -- Login server
	LS_CONNECT_LOGIN,
	LS_WAIT_SEED,			// waiting 0C03
	LS_SEND_CREDENTIALS,	// sent 0C06
	LS_WAIT_XTRAP_ACK,		// waiting 0C05
	LS_WAIT_LOGIN_ACK,		// waiting 0C0A
	LS_POLL_WORLD_STATUS,	// sending 0C1B, waiting 0C1C
	LS_SEND_WORLDSELECT,	// sent 0C0B
	LS_WAIT_WORLDSELECT_ACK,// waiting 0C0C

	// Hop 2 -- WorldManager
	LS_CONNECT_WM,
	LS_SEND_WM_HANDSHAKE,	// sent 200F
	LS_WAIT_LOGINWORLD_ACK,	// waiting 0C14
	LS_WAIT_ZONE_REDIRECT,	// waiting 1003

	// Hop 3 -- Zone
	LS_CONNECT_ZONE,
	LS_SEND_MAP_LOGIN,		// sent 1801
	LS_WAIT_MAP_LOGIN_ACK,	// waiting 1038
	LS_WAIT_ZONE_FLOOD,		// waiting 1039 103A 103B 10CE 10D7 700D 7005 7C07

	LS_INGAME,
	LS_ERROR,
};

//----------------------------------------------------------------
// CLoginNetFrameWork
// Manages the login connection to 3LoginServer2.exe.
// Name confirmed: aLoginnetframew in client.asm (0x87D194)
//----------------------------------------------------------------
class CLoginNetFrameWork
{
public:
	CLoginNetFrameWork();

	bool	Connect( const char* pszIP, unsigned short usPort );
	void	Disconnect();
	bool	SendVersionHello();
	bool	SendCredentials( const char* pszAccount, const char* pszPass );
	bool	SendXTrap();
	bool	SendWorldStatusPoll();
	bool	SendWorldSelect( BYTE bWorldIdx );

	// Returns the session token for WM handshake after 0C0C
	const BYTE*	GetSessionToken() const { return m_abToken; }
	const char*	GetWMIP()         const { return m_szWMIP; }
	unsigned short GetWMPort()      const { return m_usWMPort; }

private:
	bool	SendPacket( WORD wOpCode, const void* pData, int nLen );
	bool	RecvPacket( WORD* pwOpCode, void* pBuf, int nBufLen, int* pnRecv );

	SOCKET			m_hSocket;
	CFiestaCipher	m_cipher;
	eLoginState		m_eState;

	// Populated from 0C0C
	BYTE			m_abToken[SHINE_SESSION_TOKEN];
	char			m_szWMIP[SHINE_IP_LEN];
	unsigned short	m_usWMPort;
};

//----------------------------------------------------------------
// CLoginWorldNetFrameWork
// Manages hop 2 (WorldManager) and hop 3 (Zone) connections.
// Name confirmed: aLoginworldnetf in client.asm (0x87BE08)
//----------------------------------------------------------------
class CLoginWorldNetFrameWork
{
public:
	CLoginWorldNetFrameWork();

	bool	ConnectWM( const char* pszIP, unsigned short usPort,
	                   const BYTE* pToken );
	bool	SendCharSelect( WORD wSlot );
	bool	ConnectZone( const char* pszIP, unsigned short usPort );
	bool	SendMapLogin( WORD wSlot, const char* pszName );
	bool	WaitZoneFlood();	// drain 1039..7C07 initial state packets

	DWORD	GetCharID()   const { return m_dwCharID; }
	BYTE	GetLevel()    const { return m_bLevel; }
	const char* GetMap() const { return m_szMap; }

private:
	bool	SendPacket( SOCKET hSock, WORD wOpCode, const void* pData, int nLen );
	bool	RecvPacket( SOCKET hSock, WORD* pwOpCode, void* pBuf, int nBufLen, int* pnRecv );

	SOCKET			m_hWM;
	SOCKET			m_hZone;
	CFiestaCipher	m_cipherWM;
	CFiestaCipher	m_cipherZone;
	eLoginState		m_eState;

	// Populated from 0C14
	DWORD	m_dwCharID;
	char	m_szName[SHINE_CHARNAME_LEN];
	BYTE	m_bLevel;
	char	m_szMap[SHINE_MAPNAME_LEN];

	// Zone connection info from 1003
	char			m_szZoneIP[SHINE_IP_LEN];
	unsigned short	m_usZonePort;
};

//================================================================
// Known hardcoded client values
//================================================================

// IP baked into client.exe (confirmed in client.asm)
#define CLIENT_HARDCODED_IP		"192.168.1.107"
#define CLIENT_DEFAULT_LOGIN_PORT	9010

// Port 59507 keepalive: 5 bytes out -> "01 00" ACK
// Observed on every session, purpose unknown (possibly OPTool probe)
#define KEEPALIVE_PORT			59507
static const BYTE abKeepaliveProbe[] = { 0x1C, 0xA4, 0x53, 0x5B, 0xF0 };
