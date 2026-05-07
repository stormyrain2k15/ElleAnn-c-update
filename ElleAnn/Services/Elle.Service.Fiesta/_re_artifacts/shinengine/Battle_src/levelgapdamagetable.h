// levelgapdamagetable.h
// CLevelGapDamageTable -- level gap damage modifier lookup.
//
// SOURCE AUDIT vs NA2016 SHNs (2024-05-06)
// ---------------------------------------------------------------
// BLOCKER (old): Assumed 200-entry dense arrays for all 3 table types. WRONG.
//
// CONFIRMED from DamageLvGapPVE.shn / DamageLvGapEVP.shn:
//   Sparse tables -- 24 entries each. LvGap stored as UINT16 (int16 when
//   interpreted signed: 0xFFxx = negative gaps). DamageRate x/1000.
//
//   PVE entries (attacker=player, target=mob):
//     LvGap <= -10  -> 1500 (player far below mob -> 150% damage)
//     LvGap  -9..0  -> 1100 down to 1000
//     LvGap   0..150-> 1000 (flat -- no penalty for over-levelling vs mobs)
//
//   EVP entries (attacker=mob, target=player):
//     ALL entries -> 1000 (mobs deal flat damage regardless of level gap)
//
// CONFIRMED from DamageLvGapPVP.shn:
//   151 x 151 matrix. [MyLv][TargetLv] -> DamageRate x/1000.
//   In NA2016 data all sampled entries = 1000 (flat PvP).
//   Stored as full row per MyLv (0-150), cols TargetLv1-150.
//
// PASS: x/1000 fixed-point multiply confirmed consistent with all other tables.

#pragma once
#include "typedef.h"

#define LVGAP_PVP_MAX   151     // confirmed: 0..150

enum eLvGapType
{
    LVGAP_PVE = 0,  // player -> mob   (sparse lookup)
    LVGAP_EVP = 1,  // mob -> player   (sparse lookup, all 1000)
    LVGAP_PVP = 2,  // player -> player (full 151x151 matrix)
    LVGAP_MAX = 3,
};

// One entry in the sparse PVE/EVP table
struct LVGAP_SPARSE_ENTRY
{
    short   nLevelDiff;     // signed: attacker_level - target_level (UINT16 in SHN, cast to int16)
    int     nMultiplier;    // x1000 (1000 = 1.0x)
};

class CLevelGapDamageTable
{
public:
    CLevelGapDamageTable();
    bool    Load( const char* pszDataPath );

    // Returns multiplier x1000 for the given gap + combat type.
    int     GetMultiplier( int nAtkLevel, int nDefLevel, eLvGapType eType ) const;

private:
    bool    LoadSparse( const char* pszPath, LVGAP_SPARSE_ENTRY* pEntries, int& nCount );
    bool    LoadPvPMatrix( const char* pszPath );

    // Sparse tables (PVE + EVP)
    LVGAP_SPARSE_ENTRY  m_pve[32];
    int                 m_nPveCount;
    LVGAP_SPARSE_ENTRY  m_evp[32];
    int                 m_nEvpCount;

    // PvP full matrix [myLv 0..150][targetLv 0..150]
    int     m_pvp[ LVGAP_PVP_MAX ][ LVGAP_PVP_MAX ];

    bool    m_bLoaded;
};
