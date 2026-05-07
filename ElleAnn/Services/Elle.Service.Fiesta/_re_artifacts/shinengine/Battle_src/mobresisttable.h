// mobresisttable.h
// CMobResistTable -- mob CC and damage type resist lookup.
//
// SOURCE AUDIT vs NA2016 MobResist.shn (2024-05-06)
// ---------------------------------------------------------------
// BLOCKER (old): Struct had elemental resists (fire/water/earth/wind/light/dark).
//   WRONG -- MobResist.shn has NO elemental columns.
//
// CONFIRMED columns from MobResist.shn (37 rows):
//   InxName, ResDot, ResStun, ResMoveSpeed, ResFear,
//   ResBinding, ResReverse, ResMesmerize, ResSeverBone, ResKnockBack, ResTBMinus
//
// Linked via MobInfoServer.ResIndex (string key e.g. "NormalBeast").
//
// CONFIRMED resist tiers (all values in raw resist points):
//   Normal:  species_immune=200, others=0
//   Elite:   species_immune=300, others=100
//   Chief:   species_immune=500, others=300
//   Hero:    species_immune=800, others=600
//   Boss:    ALL=1000 (immune to all CC)
//
// Species immunities:
//   Undead    -> ResFear=immune
//   Human     -> ResBinding=immune (ResReverse for Elite+)
//   Beast     -> ResStun=immune
//   Elemental -> ResMoveSpeed=immune
//   Magiclife -> ResMesmerize=immune
//   Spirit    -> ResSeverBone=immune (ResKnockBack for Elite+)
//   Devildom  -> ResKnockBack=immune (ResSeverBone for Elite+)
//
// PASS: ReactionType.shn has 1 row only (LC_Machine, Ecode=800000).
//   Elemental combat system unused in NA2016. No elemental resist here.

#pragma once
#include "typedef.h"

#define MOB_RESIST_INDEX_MAX    64

struct MOBRESIST_ENTRY
{
    char    szIndex[32];    // matches MobInfoServer.ResIndex

    // CC resists (raw resist points -- subtracted from status success rate)
    // Attacker status_rate - target_resist = effective chance (x/1000)
    int     nResDot;
    int     nResStun;
    int     nResMoveSpeed;
    int     nResFear;
    int     nResBinding;
    int     nResReverse;
    int     nResMesmerize;
    int     nResSeverBone;
    int     nResKnockBack;
    int     nResTBMinus;    // reduces target's dodge (TB) chance
};

class CMobResistTable
{
public:
    CMobResistTable();
    bool    Load( const char* pszPath );

    const MOBRESIST_ENTRY*  Find( const char* pszIndex ) const;

    // Returns resist value for a given CC type against a named resist profile.
    int     GetCCResist( const char* pszIndex, int nCCType ) const;

private:
    MOBRESIST_ENTRY m_entries[ MOB_RESIST_INDEX_MAX ];
    int             m_nCount;
    bool            m_bLoaded;
};

// CC type indices matching MOBRESIST_ENTRY fields
enum eCCType
{
    CC_DOT          = 0,
    CC_STUN         = 1,
    CC_MOVESPEED    = 2,
    CC_FEAR         = 3,
    CC_BINDING      = 4,
    CC_REVERSE      = 5,
    CC_MESMERIZE    = 6,
    CC_SEVERBONE    = 7,
    CC_KNOCKBACK    = 8,
    CC_TBMINUS      = 9,
    CC_MAX          = 10,
};
