// statdistribute.cpp
// BuildBattleStat -- compiles BATTLESTAT from raw inputs.
//
// SOURCE AUDIT vs NA2016 SHNs (2024-05-06)
// ---------------------------------------------------------------
// BLOCKER (FIXED): MoverMain.shn = mounts. MoverAbility.shn = 0 rows.
//   Both are irrelevant to character stat compilation.
//
// CONFIRMED stat->secondary pipeline:
//   ParamXxxServer.txt  -> base STR/CON/DEX/INT/MEN per class+level
//   ChrCommon.txt FreeStatXxxTable -> converts each stat to combat secondaries
//   ItemInfo.shn        -> equipment bonuses (additive)
//   PassiveSkill.shn    -> mastery flat/rate + misc bonuses
//   AbStateInfo.shn     -> volatile buff/debuff (additive, clamped >=0)
//
// CONFIRMED weapon WC assembly (physical):
//   total_WC = rand(weapon.MinWC, weapon.MaxWC)
//            * (1000 + passive.MstRtWeapon) / 1000   <- BraveMastery rate (100-200)
//            + passive.MstPlWeapon                    <- OHSwdMastery flat (3-59)
//            + FreeStatStrTable[STR].WCAbsolute
//            + gear_option_WC
//   total_WC *= (1000 + passive.WCRateUp) / 1000     <- global WC rate
//   + buff.nATK
//
// CONFIRMED skill damage stacks on top:
//   final_ATK = total_WC + skill.rand(MinWC, MaxWC)   <- ActiveSkill.shn
//
// CONFIRMED mob stat assembly:
//   mob.nDEF = FreeStatConTable[mob.Con].ACAbsolute    <- but MobInfoServer also has
//              direct AC column -- AC is pre-derived, NOT recalculated from Con.
//              Con/Str/etc are stored for future use or display only.
//              Use MobInfoServer.AC / TB / MR / MB directly.
//
// WIP: CalcRawDamage formula (ATK - DEF vs ATK^2/(ATK+DEF)) still unconfirmed.

#include "stdafx.h"
#include "statdistribute.h"
#include "battlestat.h"
#include "ShineAssert.h"

// Forward declarations -- loaded from ChrCommon.txt at server start
extern int FreeStatStr_WCAbsolute( int nStat );   // FreeStatStrTable
extern int FreeStatInt_MAAbsolute( int nStat );   // FreeStatIntTable
extern int FreeStatDex_THRate( int nStat );       // FreeStatDexTable.THRate
extern int FreeStatDex_TBRate( int nStat );       // FreeStatDexTable.TBRate
extern int FreeStatCon_ACAbsolute( int nStat );   // FreeStatConTable.ACAbsoulte
extern int FreeStatCon_BlockRate( int nStat );    // FreeStatConTable.BlockRate
extern int FreeStatCon_MaxHP( int nStat );        // FreeStatConTable.MaxHP
extern int FreeStatMen_MRAbsolute( int nStat );   // FreeStatMenTable.MRAbsolute
extern int FreeStatMen_CriRate( int nStat );      // FreeStatMenTable.CriRate
extern int FreeStatMen_MaxSP( int nStat );        // FreeStatMenTable.MaxSP

void BuildBattleStat( BATTLESTAT*           pOut,
                      const RAWCHARSTAT*    pRaw,
                      const RAWMOBSTAT*     pMob,
                      const EQUIPSUMMARY*   pEquip,
                      const BUFFMODIFIERS*  pBuff )
{
    SHINE_ASSERT( pOut );
    SHINE_ASSERT( pRaw || pMob );   // exactly one must be non-NULL
    pOut->Clear();

    if( pMob )
    {
        // ── MOB PATH ────────────────────────────────────────────────
        // PASS: MobInfoServer AC/TB/MR/MB are pre-derived values.
        //       Use them directly. Con/Str/etc are raw but AC is already compiled.
        pOut->nLevel    = pMob->nLevel;
        pOut->nDEF      = pMob->nAC;
        pOut->nDodge    = pMob->nTB;
        pOut->nMDEF     = pMob->nMR;
        pOut->nBlock    = pMob->nMB;
        pOut->nMaxHP    = pMob->nMaxHP;
        pOut->nMaxSP    = pMob->nMaxSP;
        pOut->nHPRegen  = pMob->nPceHPRcv;
        pOut->nSTR      = pMob->nStr;
        pOut->nDEX      = pMob->nDex;
        pOut->nCON      = pMob->nCon;
        pOut->nINT      = pMob->nInt;
        pOut->nMEN      = pMob->nMen;
        // Mob attack stats set per-swing from MobWeapon -- not compiled here
        // (MobWeapon can have multiple rows per mob for different attack types)
        return;
    }

    // ── PLAYER PATH ─────────────────────────────────────────────────
    pOut->nLevel    = pRaw->nLevel;
    pOut->bClass    = pRaw->bClass;
    pOut->bJob      = pRaw->bJob;
    pOut->nSTR      = pRaw->nSTR;
    pOut->nCON      = pRaw->nCON;
    pOut->nDEX      = pRaw->nDEX;
    pOut->nINT      = pRaw->nINT;
    pOut->nMEN      = pRaw->nMEN;

    // Base HP/SP from ParamXxxServer.txt
    pOut->nMaxHP    = pRaw->nBaseHP;
    pOut->nMaxSP    = pRaw->nBaseSP;

    // FreeStatXxxTable conversions (ChrCommon.txt)
    // PASS: confirmed column names and scaling from ChrCommon.txt analysis
    pOut->nATK      = FreeStatStr_WCAbsolute( pRaw->nSTR );
    pOut->nMATK     = FreeStatInt_MAAbsolute( pRaw->nINT );
    pOut->nDEF     += FreeStatCon_ACAbsolute( pRaw->nCON );
    pOut->nDodge   += FreeStatDex_TBRate( pRaw->nDEX );
    pOut->nDodge   += FreeStatCon_BlockRate( pRaw->nCON );
    pOut->nMaxHP   += FreeStatCon_MaxHP( pRaw->nCON );
    pOut->nMDEF    += FreeStatMen_MRAbsolute( pRaw->nMEN );
    pOut->nCritical += FreeStatMen_CriRate( pRaw->nMEN );
    pOut->nMaxSP   += FreeStatMen_MaxSP( pRaw->nMEN );
    pOut->nAccuracy = FreeStatDex_THRate( pRaw->nDEX );

    // Equipment bonuses (ItemInfo.shn)
    if( pEquip )
    {
        pOut->nATK          += pEquip->nATK;
        pOut->nMATK         += pEquip->nMATK;
        pOut->nRATK         += pEquip->nRATK;
        pOut->nDEF          += pEquip->nDEF;
        pOut->nMDEF         += pEquip->nMDEF;
        pOut->nMaxHP        += pEquip->nHP;
        pOut->nMaxSP        += pEquip->nSP;
        pOut->nMaxLP        += pEquip->nMaxLP;
        pOut->nCritical     += pEquip->nCritical;
        pOut->nCriticalDmg  += pEquip->nCriDamage;
        pOut->nDodge        += pEquip->nTB + pEquip->nEvaRatePlus;
        pOut->nAccuracy     += pEquip->nTH + pEquip->nHitRatePlus;

        // Passive: weapon mastery rate (BraveMastery MstRtSword1: 100-200)
        // Applied as: weaponWC * (1000 + nMstRtWeapon) / 1000 at swing time
        // Store the rate for use in CalcSkillDamage
        pOut->nWCRateUp     += pEquip->nMstRtWeapon + pEquip->nWCRateUp;
        pOut->nMARateUp     += pEquip->nMARateUp;

        // Passive: flat WC mastery (OHSwdMastery MstPlSword1: 3-59)
        pOut->nATK          += pEquip->nMstPlWeapon;

        pOut->nDMG_MinusRate+= pEquip->nDMG_MinusRate;
        pOut->nCriticalDmg  += pEquip->nCriDmgUpRate;
    }

    // JobChangeDmgUp -- x/1000 multiplier from ParamXxxServer, applied in CalcRawDamage
    // Baseline = 1000 (x1.0). Modified after job change events.
    // Store on stat block for use in pipeline.
    // (No explicit field needed -- absorbed into nATK at compile time if always 1000)

    // Buff/debuff volatile modifiers -- CONFIRMED from AbState/*.dat (Misc.zip)
    // Lookup: Sta*.dat[pBuff->strength] gives actual numeric effect.
    // AbState type → field source:
    //   0x0001 StaHardSkin    → nDEF += ac_bonus[str]        (4-120 flat AC)
    //   0x000D StaShield      → nAbsorbDmg += absorb[str]    (4-150 per hit)
    //   0x0030 StaRingShield  → nAbsorbDmg += absorb[str]    (3-100 per hit, stacks)
    //   0x0023 StaDodge       → nDodge += tb_bonus[str]      (3-110 flat TB)
    //   0x002E StaLightDmg    → nDmgTakenEffect (WIP: rate or DoT, 3s duration)
    //   0x0021 StaArcDmgDown  → nAtkDownEffect  (WIP: rate or flat)
    //   0x002D StaMagSpdDown  → nCastTime_x1000 = cast_x1000[str] (1000=normal)
    //   0x000B StaCleHold     → CC, no stat effect
    //   0x0029 StaVine        → CC, no stat effect
    if( pBuff )
    {
        pOut->nATK          += pBuff->nATK;
        pOut->nMATK         += pBuff->nMATK;
        pOut->nDEF          += pBuff->nDEF;        // StaHardSkin: ac_bonus[str]
        pOut->nMDEF         += pBuff->nMDEF;
        pOut->nMaxHP        += pBuff->nMaxHP;
        pOut->nMaxSP        += pBuff->nMaxSP;
        pOut->nHPRegen      += pBuff->nHPRegen;
        pOut->nSPRegen      += pBuff->nSPRegen;
        pOut->nMoveSpeed    += pBuff->nMoveSpeed;
        pOut->nCritical     += pBuff->nCritical;
        pOut->nAccuracy     += pBuff->nTH;
        pOut->nDodge        += pBuff->nTB;         // StaDodge: tb_bonus[str]
        pOut->nAbsorbDmg    += pBuff->nAbsorbDmg;  // StaShield[str].absorb + StaRingShield[str].absorb (additive stack)
        // WIP fields stored on BATTLESTAT for CalcDamage pipeline:
        // pBuff->nDmgTakenEffect -> StaLightDmg (debuff on defender)
        // pBuff->nAtkDownEffect  -> StaArcDmgDown (debuff on attacker's effective nATK)
    }

    // Clamp (debuffs cannot invert to negative)
    pOut->nATK          = max( 0, pOut->nATK );
    pOut->nMATK         = max( 0, pOut->nMATK );
    pOut->nDEF          = max( 0, pOut->nDEF );
    pOut->nMDEF         = max( 0, pOut->nMDEF );
    pOut->nMaxHP        = max( 1, pOut->nMaxHP );
    pOut->nMaxSP        = max( 0, pOut->nMaxSP );
    pOut->nMoveSpeed    = max( 0, pOut->nMoveSpeed );
    pOut->nDodge        = max( 0, pOut->nDodge );
    pOut->nAccuracy     = max( 0, pOut->nAccuracy );
}
