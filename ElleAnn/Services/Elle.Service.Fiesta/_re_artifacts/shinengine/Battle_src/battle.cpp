// battle.cpp
// Damage calculation pipeline.
//
// SOURCE AUDIT vs NA2016 SHNs + SHNFile.cs parser (2024-05-06)
// ---------------------------------------------------------------
// Pipeline order CONFIRMED from pdb/source file names:
//   1. BuildBattleStat()      -- statdistribute.cpp (corrected)
//   2. HitCheck()             -- CONFIRMED: WeaponAttrib.HitRate=500 base (50%)
//                                * SkilPyHitRate=950 skill modifier (95%)
//                                vs (attacker.TH - defender.TB)
//   3. CalcRawDamage()        -- ATK - DEF confirmed candidate; formula TBC
//   4. ApplyLevelGap()        -- CORRECTED: sparse PVE/EVP (24 entries),
//                                full 151x151 matrix for PVP
//   5. ApplyElemental()       -- STUB: ReactionType.shn = 1 row (LC_Machine only)
//                                Elemental system unused in NA2016.
//   6. ApplyAngle()           -- DamageByAngle.txt: 1000-1200 range, back=1200
//   7. ApplySoul()            -- DamageBySoul.txt confirmed
//   8. ApplyCritical()        -- rand(10000) < nCritical; bonus = nCriticalDmg/10000
//   9. ApplyDMGMinusRate()    -- NEW NA2016: PassiveSkill.DMG_MinusRate on defender
//  10. ApplyAbstateModifiers()-- PARTIALLY CONFIRMED from AbState/*.dat (Misc.zip)
//        Absorb/DEF/Dodge already compiled into BATTLESTAT via BuildBattleStat.
//        StaLightDmg (0x002E) + StaArcDmgDown (0x0021) field semantics WIP.
//  11. ApplyAbsorb()          -- StaShield/StaRingShield confirmed from AbState .dat
//  12. floor = max(1, damage)
//
// AggroPerDamage CORRECTED: NA2016 SHN value = 9000 (not 1000 as in XLS).
//   Aggro = damage * 9000 / 1000 = damage * 9.
//
// CalcRawDamage formula: WIP/PENDING PROOF
//   Candidate A: max(1, ATK - DEF)        -- simplest, consistent with DAMAGE_FLOOR
//   Candidate B: max(1, ATK*ATK/(ATK+DEF))-- quadratic, common in Shine variants
//   Community verification required.

#include "stdafx.h"
#include "battle.h"
#include "battlestat.h"
#include "levelgapdamagetable.h"
#include "mobresisttable.h"
#include "damagebyangle.h"
#include "ShineAssert.h"

#define DAMAGE_FLOOR    1

static CLevelGapDamageTable g_lvGapTable;
static CMobResistTable      g_mobResistTable;

bool Battle_LoadTables( const char* pszDataPath )
{
    if( !g_lvGapTable.Load( pszDataPath ) )   { ShineLog("Battle_LoadTables: lvgap failed");   return false; }
    if( !g_mobResistTable.Load( pszDataPath ) ){ ShineLog("Battle_LoadTables: mobresist failed"); return false; }
    return true;
}

//------------------------------------------------------------------
// HitCheck -- CONFIRMED structure, formula WIP
// WeaponAttrib.HitRate = 500 (base 50% weapon accuracy, x/1000)
// ActiveSkillInfoServer.SkilPyHitRate = 950 (95% skill accuracy, x/1000)
//
// Candidate hit formula:
//   baseHit    = WeaponAttrib.HitRate                  // 500 = 50%
//   skillMod   = SkilPyHitRate                         // 950 = x0.95 modifier
//   attackerTH = pAtk->nAccuracy                       // from FreeStatDex + item.TH
//   defenderTB = pDef->nDodge                          // from FreeStatDex + FreeStatCon + item.TB
//   netHit     = baseHit + attackerTH - defenderTB     // x/1000 based
//   finalChance= netHit * skillMod / 1000
//   hit if rand(10000) < finalChance * 10
// WIP/PENDING PROOF -- community verification needed.
//------------------------------------------------------------------
static bool HitCheck( const BATTLESTAT* pAtk, const BATTLESTAT* pDef,
                      int nSkilPyHitRate, int nWeaponHitRate,
                      DAMAGERESULT* pResult )
{
    int nNet = nWeaponHitRate + pAtk->nAccuracy - pDef->nDodge;
    nNet = nNet * nSkilPyHitRate / 1000;
    nNet = max( 0, min( 10000, nNet * 10 ) );
    if( rand() % 10000 >= nNet )
    {
        pResult->bMissed = true;
        return false;
    }
    return true;
}

//------------------------------------------------------------------
// CalcRawDamage -- WIP/PENDING PROOF
// Two confirmed candidates -- neither definitively proven from SHN data alone.
//------------------------------------------------------------------
static int CalcRawDamage( const BATTLESTAT* pAtk, const BATTLESTAT* pDef, int nDmgTypeFlags )
{
    int nATK, nDEF;
    if(      nDmgTypeFlags & DMG_TYPE_MAGIC  ) { nATK = pAtk->nMATK; nDEF = pDef->nMDEF; }
    else if( nDmgTypeFlags & DMG_TYPE_RANGED ) { nATK = pAtk->nRATK; nDEF = pDef->nRDEF; }
    else                                        { nATK = pAtk->nATK;  nDEF = pDef->nDEF;  }

    // Apply attacker WCRateUp (global rate from PassiveSkill)
    if( pAtk->nWCRateUp && !(nDmgTypeFlags & DMG_TYPE_MAGIC) )
        nATK = nATK * (1000 + pAtk->nWCRateUp) / 1000;
    if( pAtk->nMARateUp && (nDmgTypeFlags & DMG_TYPE_MAGIC) )
        nATK = nATK * (1000 + pAtk->nMARateUp) / 1000;

    // WIP: formula unconfirmed -- using simpler candidate A
    int nRaw = nATK - nDEF;
    return max( DAMAGE_FLOOR, nRaw );
}

//------------------------------------------------------------------
// ApplyLevelGap -- CORRECTED
// PVE/EVP: sparse table lookup (24 entries, signed int16 gap)
// PVP: full 151x151 matrix [myLv][targetLv]
//------------------------------------------------------------------
static int ApplyLevelGap( int nRaw, int nAtkLv, int nDefLv, eLvGapType eType )
{
    int nMult = g_lvGapTable.GetMultiplier( nAtkLv, nDefLv, eType );
    return max( DAMAGE_FLOOR, nRaw * nMult / 1000 );
}

//------------------------------------------------------------------
// ApplyElemental -- STUB (effectively unused in NA2016)
// ReactionType.shn has 1 row: LC_Machine, Ecode=800000.
// No general elemental reaction table exists.
//------------------------------------------------------------------
static int ApplyElemental( int nDmg, eElement eAtk, eElement eDef, WORD wMobID )
{
    (void)eAtk; (void)eDef; (void)wMobID;
    return nDmg;    // no-op
}

//------------------------------------------------------------------
// ApplyAngle -- CONFIRMED from DamageByAngle.txt
// Multipliers: front(0°)=1000, side(90°)=1100, back(180°)=1200 (x/1000)
// Interpolated between defined angle anchors.
//------------------------------------------------------------------
static int ApplyAngle( int nDmg, int nAngle )
{
    return ::ApplyAngle( nDmg, nAngle );    // damagebyangle.cpp
}

//------------------------------------------------------------------
// ApplySoul -- CONFIRMED from DamageBySoul.txt
// DemandSoul=1: 1 soul=1000, 2=1400, 3=1800, 4=2200, 5=2600, 6=3000, 7=3500
// DemandSoul=2-7: 1=1000, 2-3=1500, 4-5=2000, 6-7=2500
//------------------------------------------------------------------
static int ApplySoul( int nDmg, int nDemandSoul, int nSoulCount )
{
    // WIP: full DamageBySoul table lookup (table not loaded in pipeline yet)
    // Placeholder returns flat for 0 souls
    (void)nDemandSoul; (void)nSoulCount;
    return nDmg;
}

//------------------------------------------------------------------
// ApplyCritical -- CONFIRMED structure
// nCritical stored x100 (e.g. 500 = 5.00%)
// nCriticalDmg stored x100 (e.g. 5000 = +50% extra damage)
// Roll: rand(10000) < nCritical
//------------------------------------------------------------------
static int ApplyCritical( int nDmg, const BATTLESTAT* pAtk, DAMAGERESULT* pResult )
{
    if( rand() % 10000 < pAtk->nCritical )
    {
        pResult->bCritical = true;
        nDmg = nDmg + ( nDmg * pAtk->nCriticalDmg / 10000 );
        pResult->nDamageType |= DMG_TYPE_CRITICAL;
    }
    return max( DAMAGE_FLOOR, nDmg );
}

//------------------------------------------------------------------
// ApplyDMGMinusRate -- NEW NA2016 (PassiveSkill.DMG_MinusRate on defender)
// Reduces incoming damage by rate x/1000
//------------------------------------------------------------------
static int ApplyDMGMinusRate( int nDmg, const BATTLESTAT* pDef )
{
    if( pDef->nDMG_MinusRate > 0 )
        nDmg = nDmg * max( 0, 1000 - pDef->nDMG_MinusRate ) / 1000;
    return max( DAMAGE_FLOOR, nDmg );
}

//------------------------------------------------------------------
// ApplyAbsorb -- PASS (StaShield / StaRingShield confirmed from AbState .dat)
//------------------------------------------------------------------
static int ApplyAbsorb( int nDmg, const BATTLESTAT* pDef, DAMAGERESULT* pResult )
{
    if( pDef->nAbsorbDmg > 0 )
    {
        int nAbsorb = min( nDmg - DAMAGE_FLOOR, pDef->nAbsorbDmg );
        pResult->nAbsorbed = nAbsorb;
        nDmg -= nAbsorb;
    }
    return max( DAMAGE_FLOOR, nDmg );
}

//------------------------------------------------------------------
// CalcDamage -- main entry point
//------------------------------------------------------------------
void CalcDamage( const BATTLESTAT* pAtk,
                 const BATTLESTAT* pDef,
                 int               nDmgTypeFlags,
                 eElement          eAtkElem,
                 eElement          eDefElem,
                 WORD              wMobID,
                 int               nAngle,
                 int               nDemandSoul,
                 int               nSoulCount,
                 int               nSkilPyHitRate,   // from ActiveSkillInfoServer (default 950)
                 int               nWeaponHitRate,   // from WeaponAttrib (default 500)
                 bool              bPvP,
                 DAMAGERESULT*     pResult )
{
    SHINE_ASSERT( pAtk && pDef && pResult );
    pResult->Clear();
    pResult->nDamageType = nDmgTypeFlags;
    pResult->eElement    = eAtkElem;

    // 1. Hit check
    if( !HitCheck( pAtk, pDef, nSkilPyHitRate, nWeaponHitRate, pResult ) )
    {
        pResult->nDamage = 0;
        return;
    }

    // 2. Raw damage
    int nDmg = CalcRawDamage( pAtk, pDef, nDmgTypeFlags );

    // 3. Level gap
    eLvGapType eGap = bPvP ? LVGAP_PVP : ( wMobID ? LVGAP_PVE : LVGAP_EVP );
    nDmg = ApplyLevelGap( nDmg, pAtk->nLevel, pDef->nLevel, eGap );

    // 4. Elemental (stub -- no-op in NA2016)
    nDmg = ApplyElemental( nDmg, eAtkElem, eDefElem, wMobID );

    // 5. Angle modifier
    nDmg = ApplyAngle( nDmg, nAngle );

    // 6. Soul charge
    nDmg = ApplySoul( nDmg, nDemandSoul, nSoulCount );

    // 7. Critical
    nDmg = ApplyCritical( nDmg, pAtk, pResult );

    // 8. Defender DMG_MinusRate (NA2016)
    nDmg = ApplyDMGMinusRate( nDmg, pDef );

    // 7. AbState modifiers
    // Absorb (StaShield/StaRingShield), DEF bonus (StaHardSkin), Dodge (StaDodge)
    // all compiled into pAtk/pDef->nAbsorbDmg/nDEF/nDodge via BuildBattleStat.
    //
    // WIP: StaLightDmg (0x002E) -- increases damage taken by defender
    //      StaArcDmgDown(0x0021) -- reduces damage dealt by attacker
    //      Both have effect[str] values (1150-23500) but units unconfirmed.
    //      Apply here once semantics confirmed from empirical testing.

    // 10. PvP modifier -- WIP
    // if( bPvP ) nDmg = nDmg * pAtk->nPvPATKBonus / 100;

    // 11. Flat absorb
    nDmg = ApplyAbsorb( nDmg, pDef, pResult );

    // 12. Floor
    pResult->nDamage = max( DAMAGE_FLOOR, nDmg );
}

//------------------------------------------------------------------
// CalcSkillDamage -- wraps CalcDamage with ActiveSkillInfoServer params
// CONFIRMED fields from ActiveSkillInfoServer.shn:
//   SkilPyHitRate = 950 (physical skills, x/1000 hit modifier)
//   SkilMaHitRate = for magic skills
//   DmgIncRate / DmgIncValue = 0 for basic skills (non-zero for charged)
//   AggroPerDamage = 9000 in NA2016 (not 1000 as in 2012 XLS)
//   AddSoul = soul orbs granted on successful use
//   DemandSoul = links to DamageBySoul table
//------------------------------------------------------------------
void CalcSkillDamage( const BATTLESTAT* pAtk,
                       const BATTLESTAT* pDef,
                       int               nSkilPyHitRate,
                       int               nSkilMaHitRate,
                       int               nDemandSoul,
                       int               nSoulCount,
                       int               nDmgIncRate,
                       int               nDmgIncValue,
                       int               nAngle,
                       bool              bPvP,
                       WORD              wMobID,
                       DAMAGERESULT*     pResult )
{
    int nDmgTypeFlags = DMG_TYPE_SKILL;
    bool bMagic = (nSkilMaHitRate > 0);
    if( bMagic ) nDmgTypeFlags |= DMG_TYPE_MAGIC;

    int nHitRate = bMagic ? nSkilMaHitRate : nSkilPyHitRate;

    // DmgIncRate / DmgIncValue applied to attacker ATK before pipeline
    // WIP: exact application point unconfirmed -- applied here as pre-modifier
    BATTLESTAT atkMod = *pAtk;
    if( nDmgIncRate > 0 )
    {
        if( bMagic ) atkMod.nMATK += atkMod.nMATK * nDmgIncRate / 1000 + nDmgIncValue;
        else         atkMod.nATK  += atkMod.nATK  * nDmgIncRate / 1000 + nDmgIncValue;
    }

    CalcDamage( &atkMod, pDef,
                nDmgTypeFlags,
                ELEMENT_NONE, ELEMENT_NONE,
                wMobID,
                nAngle,
                nDemandSoul, nSoulCount,
                nHitRate,
                500,    // WeaponAttrib.HitRate default
                bPvP, pResult );
}
