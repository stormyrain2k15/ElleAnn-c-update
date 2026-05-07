// mobresisttable.h
// CMobResistTable -- elemental and damage type resist lookup.
// Source: E:\ProjectF2\Server\1SharedCode\mobresisttable.h
//
// Data source: MobResist.shn (tableheaders\mobresist.h confirmed).
// Each mob species has resist values per element and damage type.
// Resists stored as integer percentage (100 = 100% = immune).

#pragma once
#include "typedef.h"
#include "battlestat.h"		// eElement

#define MOB_RESIST_MAX		512		// max distinct mob species

//------------------------------------------------------------------
// MOBRESIST_ENTRY
// Mirrors MobResist.shn column layout.
// TODO: open MobResist.shn in editor and confirm exact field names.
//------------------------------------------------------------------
struct MOBRESIST_ENTRY
{
	WORD	wMobID;			// links to MobInfo.shn InxNo

	// Elemental resists (% damage reduction, 0-100)
	BYTE	bFireResist;
	BYTE	bWaterResist;
	BYTE	bEarthResist;
	BYTE	bWindResist;
	BYTE	bLightResist;
	BYTE	bDarkResist;

	// Damage type resists
	BYTE	bPhysResist;	// vs normal physical
	BYTE	bMagicResist;	// vs magic damage
	BYTE	bRangedResist;	// vs ranged physical

	/* TODO: additional fields -- SHN editor capture pending */
};

//------------------------------------------------------------------
class CMobResistTable
{
public:
	CMobResistTable();

	bool	Load( const char* pszPath );

	const MOBRESIST_ENTRY*	Find( WORD wMobID ) const;

	// Returns resist value (0-100) for element against mob.
	int		GetElementResist( WORD wMobID, eElement eElem ) const;

	// Returns resist value for damage type flags.
	int		GetDamageTypeResist( WORD wMobID, int nDmgTypeFlags ) const;

private:
	MOBRESIST_ENTRY	m_entries[ MOB_RESIST_MAX ];
	int				m_nCount;
	bool			m_bLoaded;
};
