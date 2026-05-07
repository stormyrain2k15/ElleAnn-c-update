// statdistribute.h
// Input structures for BuildBattleStat.
//
// SOURCE AUDIT vs NA2016 SHNs (2024-05-06)
// ---------------------------------------------------------------
// BLOCKER (old): RAWCHARSTAT referenced MoverMain.shn for nBaseHP/nBaseATK/nBaseDEF.
//   MoverMain.shn = MOUNTS ONLY. No character base stats there.
//
// BLOCKER (old): statdistribute.cpp referenced MoverAbility.shn for
//   stat->secondary growth rates. MoverAbility.shn has 0 rows in NA2016.
//
// CONFIRMED character base stat sources:
//   ParamXxxServer.txt (per class, per level):
//     MaxHP, MaxSP, Strength, Constitution, Intelligence, Wizdom,
//     Dexterity, MentalPower -- these are the BASE stat point values.
//   ChrCommon.txt FreeStatXxxTable:
//     STR points -> WCAbsolute  (FreeStatStrTable)
//     INT points -> MAAbsolute  (FreeStatIntTable)
//     DEX points -> THRate, TBRate (FreeStatDexTable)
//     CON points -> ACAbsolute, BlockRate, MaxHP bonus (FreeStatConTable)
//     MEN points -> MRAbsolute, CriRate, MaxSP bonus (FreeStatMenTable)
//   These ARE the stat->secondary conversion system (not MoverAbility).
//
// CONFIRMED mob base stat source:
//   MobInfoServer.shn: Str, Dex, Con, Int, Men (raw stat points)
//   -> fed through same FreeStatXxxTable lookups to derive AC/TB/MR/MB.
//   MobWeapon.shn: MinWC, MaxWC, TH, MinMA, MaxMA per attack slot.

#pragma once
#include "battlestat.h"

//------------------------------------------------------------------
// RAWCHARSTAT -- PASS (corrected sources)
//------------------------------------------------------------------
struct RAWCHARSTAT
{
    int     nLevel;
    BYTE    bClass;         // 1=Fighter, 6=Cleric, 11=Archer, 16=Mage, 21=Joker, 26=Sentinel
    BYTE    bJob;           // 0=base, 1=first, 2=second, 3=third

    // Allocated stat points -- from ParamXxxServer.txt per level
    // + any free stat point allocations by the player (cap: MaxFreeStat=180)
    int     nSTR, nCON, nDEX, nINT, nMEN;

    // Base HP/SP from ParamXxxServer.txt MaxHP/MaxSP columns
    int     nBaseHP;
    int     nBaseSP;

    // Base combat values from ParamXxxServer.txt
    // Note: AtkPerAP and DmgPerAP are 0 in all ParamXxx files -- populated by skill/weapon
    // JobChangeDmgUp = 1000 baseline (x1.0), modified after job change events
    int     nJobChangeDmgUp;   // x/1000 multiplier confirmed from ParamXxxServer

    int     nBaseMoveSpeed;    // from ChrCommon.txt Common.RunSpeed = 127
};

//------------------------------------------------------------------
// RAWMOBSTAT -- PASS
// Mob base stats from MobInfoServer.shn + MobWeapon.shn
//------------------------------------------------------------------
struct RAWMOBSTAT
{
    int     nLevel;         // MobInfo.Level
    char    szResIndex[32]; // MobInfoServer.ResIndex -> MobResist lookup

    // Raw stat points (MobInfoServer -- fed through FreeStatXxxTable)
    int     nStr, nDex, nCon, nInt, nMen;

    // Direct combat stats (MobInfoServer -- pre-derived, not from FreeStatXxx)
    int     nAC, nTB, nMR, nMB;

    // Weapon stats (MobWeapon per attack slot)
    int     nMinWC, nMaxWC, nTH;
    int     nMinMA, nMaxMA;
    int     nAtkSpd;        // MobWeapon.AtkSpd (x/1000 per base interval)

    int     nMaxHP;         // MobInfo.MaxHP
    int     nMaxSP;         // MobInfoServer.MaxSP
    int     nPceHPRcv;      // MobInfoServer.PceHPRcv (regen out of combat)
    int     nAtkHPRcv;      // MobInfoServer.AtkHPRcv (regen in combat)
    int     nRank;          // MobInfoServer.Rank (0=normal,1=elite,2=chief,3=hero,4=boss)
};

//------------------------------------------------------------------
// EQUIPSUMMARY -- WIP/PENDING PROOF (field names from ItemInfo.shn confirmed,
// passive skill fields now confirmed from NA2016 PassiveSkill.shn)
//------------------------------------------------------------------
struct EQUIPSUMMARY
{
    // From ItemInfo.shn (MinWC..MaxWC random roll on equip)
    int     nATK, nMATK, nRATK;
    int     nDEF, nMDEF, nRDEF;
    int     nHP, nSP;
    int     nCritical;      // ItemInfo.CriRate
    int     nCriDamage;     // ItemInfo.CriDamPlus
    int     nTH;            // ItemInfo.TH (to-hit)
    int     nTB;            // ItemInfo.TB (to-dodge)
    int     nHitRatePlus;   // ItemInfo.HitRatePlus
    int     nEvaRatePlus;   // ItemInfo.EvaRatePlus
    int     nMACriPlus;     // ItemInfo.MACriPlus
    int     nMagCriDamPlus; // ItemInfo.MagCriDamPlus
    int     nShieldAC;      // ItemInfo.ShieldAC

    // From PassiveSkill.shn (NA2016 confirmed)
    int     nMstRtWeapon;   // weapon mastery rate (BraveMastery: 100-200, x/1000 applied to weaponWC)
    int     nMstPlWeapon;   // weapon mastery flat (OHSwdMastery: 3-59, flat WC additive)
    int     nWCRateUp;      // global WC rate multiplier (x/1000)
    int     nMARateUp;      // global MA rate multiplier (x/1000)
    int     nDMG_MinusRate; // incoming damage reduction rate (x/1000)
    int     nCriDmgUpRate;  // PassiveSkill.CriDmgUpRate
    int     nMACriRate;     // PassiveSkill.MACriRate
    int     nMaxLP;         // PassiveSkill.MaxLP
    int     nSPRecover;     // PassiveSkill.SPRecover
};

//------------------------------------------------------------------
// BUFFMODIFIERS
// Volatile modifiers from active AbStates. Rebuilt on buff apply/remove.
//
// SOURCE: AbState/*.dat (Misc.zip), fully confirmed per-strength tables.
//
// Each Sta*.dat has 151 rows (strength 0-150). Fields per state:
//
// StaShield    (0x000D): absorb[str] = flat HP absorbed per hit (4-150)
//                        duration    = 303000 + str*3000 ms
// StaRingShield(0x0030): absorb[str] = flat HP absorbed per hit (3-100)
//                        duration    = same formula, slightly weaker
// StaHardSkin  (0x0001): ac_bonus[str] = flat DEF bonus (4-120)
//                        duration    = same formula
// StaDodge     (0x0023): tb_bonus[str] = flat TB/dodge bonus (3-110)
//                        duration    = same formula
// StaLightDmg  (0x002E): effect[str] = rate or DoT value (1150-23500)
//                        ticks[str]  = number of ticks (4-120)
//                        duration    = 3000ms fixed
//                        WIP: field semantics (rate x/1000 vs flat DoT)
// StaArcDmgDown(0x0021): effect[str] = ATK reduction value (same range as LightDmg)
//                        WIP: applied as flat nATK reduction or rate
// StaMagSpdDown(0x002D): cast_x1000[str] = cast time multiplier (1000=normal, 1450=+45%)
// StaCleHold   (0x000B): CC hold, duration = hold_ms[str] (2010-3500ms)
// StaVine      (0x0029): CC root, duration = root_ms[str] (2010-3500ms)
//------------------------------------------------------------------
struct BUFFMODIFIERS
{
    // -- Offensive --
    int     nATK;           // additive ATK bonus from offensive buffs
    int     nMATK;          // additive MATK bonus
    int     nRATK;

    // -- Defensive --
    int     nDEF;           // StaHardSkin: ac_bonus[strength] added here
    int     nMDEF;

    // -- Absorb -- CONFIRMED: StaShield + StaRingShield stacked additively
    int     nAbsorbDmg;     // flat HP absorbed per hit before damage lands
                            // StaShield[str].absorb + StaRingShield[str].absorb

    // -- Vitals --
    int     nMaxHP, nMaxSP;
    int     nHPRegen, nSPRegen;

    // -- Hit resolution --
    int     nTH;            // accuracy bonus
    int     nTB;            // StaDodge: tb_bonus[strength] added here

    // -- Movement --
    int     nMoveSpeed;

    // -- Combat modifiers --
    int     nCritical;
    int     nCastTime_x1000; // StaMagSpdDown: cast_x1000[strength] (1000=normal)

    // -- Debuff effects on attacker (applied to DEFENDER's BATTLESTAT) --
    // WIP: StaLightDmg and StaArcDmgDown -- field semantics unconfirmed
    // Tracked here as raw values for now, applied in CalcDamage pipeline
    int     nDmgTakenEffect;  // StaLightDmg effect[str] -- debuff on defender
    int     nAtkDownEffect;   // StaArcDmgDown effect[str] -- debuff on attacker
};

//------------------------------------------------------------------
void BuildBattleStat( BATTLESTAT*          pOut,
                      const RAWCHARSTAT*   pRaw,      // player
                      const RAWMOBSTAT*    pMob,      // OR mob (one must be NULL)
                      const EQUIPSUMMARY*  pEquip,
                      const BUFFMODIFIERS* pBuff );
