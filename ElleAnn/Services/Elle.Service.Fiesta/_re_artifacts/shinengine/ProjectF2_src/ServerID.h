// ServerID.h
// ShineEngine server identifier definitions
// Identifies each process in the server mesh. Used in inter-server
// messaging headers and ODBC connection routing.
//
// Source: recovered from client.exe string table + Login.pdb symbols
// Do not modify order -- wire protocol embeds these values as u8/u16.

#pragma once

//----------------------------------------------------------------
// SERVER_ID
// Every process in the mesh has one of these. The client knows its
// own ID (SERVER_ID_CLIENT) and uses SERVER_ID_LOGIN etc. as
// destination tags in NC packet headers.
//----------------------------------------------------------------
enum eServerID
{
	SERVER_ID_NONE				= 0,	// uninitialised / invalid

	// -- Database servers (never on the packet network directly) --
	SERVER_ID_DB_ACCOUNT		= 1,	// Account Release.exe
	SERVER_ID_DB_ACCOUNTLOG		= 2,	// AccountLog Release.exe
	SERVER_ID_DB_CHARACTER		= 3,	// Character Release.exe
	SERVER_ID_DB_GAMELOG		= 4,	// GameLog Release.exe

	// -- Application servers --
	SERVER_ID_LOGIN				= 5,	// 3LoginServer2.exe     port 9010
	SERVER_ID_WORLDMANAGER		= 6,	// 4WorldManagerServer2  port 9110
	SERVER_ID_ZONE				= 7,	// 5ZoneServer2.exe      port 9120
	SERVER_ID_PATCH				= 8,	// patch server
	SERVER_ID_OPTOOL			= 9,	// OpToolCN.exe          port 8099 (GM tool)
	SERVER_ID_WEB				= 10,	// web/item mall service
	SERVER_ID_UNKNOWN			= 11,	// placeholder

	// -- Reserved slots 12-18 (future expansion) --
	SERVER_ID_12				= 12,
	SERVER_ID_13				= 13,
	SERVER_ID_14				= 14,
	SERVER_ID_15				= 15,
	SERVER_ID_16				= 16,
	SERVER_ID_17				= 17,
	SERVER_ID_18				= 18,

	// -- Client-side identities --
	SERVER_ID_CLIENT			= 19,	// Fiesta.exe / Client.exe
	SERVER_ID_LAUNCHER			= 20,	// MoePromise.exe (launcher + xTrap injector)

	SERVER_ID_MAX				= 21,
};

//----------------------------------------------------------------
// Connection status strings used in the UI server list
// (source: aServerconcheck, aServercongood_ etc. from client.asm)
//----------------------------------------------------------------
#define SERVERCON_CHECK		"ServerConCheck_"	// polling / waiting
#define SERVERCON_GOOD		"ServerConGood_"	// online + accepting
#define SERVERCON_NORMAL	"ServerConNormal_"	// online
#define SERVERCON_BUSY		"ServerConBusy_"	// full / queue
#define SERVERCON_CLOSED	"ServerConClosed_"	// offline / maintenance

//----------------------------------------------------------------
// World name (single world in MoePromise build)
// Source: USER_LOGIN_ACK string literal from packet capture
//----------------------------------------------------------------
#define WORLD_NAME_DEFAULT	"SYN-Villian"
#define WORLD_NAME_LEN		16

//----------------------------------------------------------------
// ODBC logical database names (from _ServerInfo.txt ODBC_INFO block)
// All hit DESKTOP-IIREG0M\ELLEANN via Trusted_Connection=Yes
//----------------------------------------------------------------
#define ODBC_DB_ACCOUNT			"Account"
#define ODBC_DB_ACCOUNTLOG		"AccountLog"
#define ODBC_DB_CHARACTER		"World00_Character"
#define ODBC_DB_GAMELOG			"World00_GameLog"
#define ODBC_DB_STATISTICSDATA	"StatisticsData"
#define ODBC_DB_OPTOOL			"OPTool"
#define ODBC_DB_ELLESYSTEM		"ElleSystem"
#define ODBC_DB_ELLECORE		"ElleCore"
