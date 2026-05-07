// levelgapdamagetable.h
// CLevelGapDamageTable -- loads and queries the three level gap
// damage modifier SHN tables.
// Source: E:\ProjectF2\Server\5ZoneServer2\levelgapdamagetable.h
//
// Three tables from CSCode\tableheaders\:
//   damagelvgappve.h  -- player attacks mob  (PvE)
//   damagelvgappvp.h  -- player attacks player (PvP)
//   damagelvgapevp.h  -- mob attacks player  (EvP)
//
// Each table maps level difference (attacker - target)
// to a damage multiplier (stored as integer, divide by 1000
// to get float -- e.g. 1000 = 1.0x, 850 = 0.85x).
//
// Level gap clamped to table bounds on both ends.

#pragma once
#include "typedef.h"

#define LVGAP_TABLE_SIZE	200		// confirmed: enough for -100..+100
#define LVGAP_OFFSET		100		// index = gap + LVGAP_OFFSET

//------------------------------------------------------------------
enum eLvGapType
{
	LVGAP_PVE = 0,	// player -> mob
	LVGAP_PVP = 1,	// player -> player
	LVGAP_EVP = 2,	// mob -> player
	LVGAP_MAX = 3,
};

//------------------------------------------------------------------
struct LVGAP_ENTRY
{
	int		nLevelDiff;		// attacker level - target level
	int		nMultiplier;	// x1000 (1000 = 1.0x)
	/* TODO: confirm exact column names from damagelvgappve.shn
	   open in SHN editor and cross-reference field names */
};

//------------------------------------------------------------------
class CLevelGapDamageTable
{
public:
	CLevelGapDamageTable();

	bool	Load( const char* pszDataPath );

	// Get damage multiplier for a given level gap and combat type.
	// Returns value x1000 (divide by 1000 for float multiplier).
	int		GetMultiplier( int nAttackerLevel, int nTargetLevel,
	                       eLvGapType eType ) const;

private:
	bool	LoadTable( const char* pszPath, int* pTable );

	// Raw multiplier arrays indexed by (gap + LVGAP_OFFSET)
	int		m_nPvE[ LVGAP_TABLE_SIZE ];
	int		m_nPvP[ LVGAP_TABLE_SIZE ];
	int		m_nEvP[ LVGAP_TABLE_SIZE ];
	bool	m_bLoaded;
};
