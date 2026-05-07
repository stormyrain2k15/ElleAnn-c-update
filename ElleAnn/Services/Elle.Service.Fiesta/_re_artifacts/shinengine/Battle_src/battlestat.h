// battlestat.h
// BattleStat -- compiled per-object stat block.
// Built once on zone login, kept hot in memory.
// Rebuilt on equip/unequip, level up, buff apply/remove.
//
// SOURCE AUDIT vs NA2016 SHNs + SHNFile.cs parser (2024-05-06)
// ---------------------------------------------------------------
// MoverMain.shn   -- MOUNTS ONLY (CastingTime, RunSpeed, etc).
//                    NOT character base stats. Previous TODO was WRONG.
// MoverAbility.shn-- 0 rows / empty in NA2016. Does not exist.
//                    Stat->secondary conversion is ChrCommon.txt FreeStatXxxTable.
// MobInfoServer.shn-- Mob Str/Dex/Con/Int/Men are RAW stat points fed
//                    through the same FreeStatXxxTable to derive AC/TB/MR/MB.
// PassiveSkill.shn-- New fields confirmed: WCRateUp, MARateUp, DMG_MinusRate, MaxLP.
// MobResist.shn   -- CC resists only (no elemental). Struct below corrected.
// ReactionType.shn-- 1 row only (LC_Machine special). Elemental system
//                    effectively unused in NA2016.

#pragma once
#include "typedef.h"

//------------------------------------------------------------------
// Element type -- kept for AoE skill type flags but not active in NA2016.
// ReactionType.shn has exactly 1 row (LC_Machine). No general elemental
// reaction table exists. ApplyElemental() is a no-op for all normal mobs.
//------------------------------------------------------------------
enum eElement
{
    ELEMENT_NONE    = 0,
    ELEMENT_FIRE    = 1,
    ELEMENT_WATER   = 2,
    ELEMENT_EARTH   = 3,
    ELEMENT_WIND    = 4,
    ELEMENT_LIGHT   = 5,
    ELEMENT_DARK    = 6,
    ELEMENT_MAX     = 7,
};

//------------------------------------------------------------------
// Damage type flags
//------------------------------------------------------------------
enum eDamageType
{
    DMG_TYPE_NORMAL     = 0x0000,
    DMG_TYPE_MAGIC      = 0x0001,
    DMG_TYPE_SKILL      = 0x0002,
    DMG_TYPE_CRITICAL   = 0x0004,
    DMG_TYPE_RANGED     = 0x0008,
    DMG_TYPE_REFLECT    = 0x0010,
    DMG_TYPE_TRUE       = 0x0020,   // ignore DEF
};

//------------------------------------------------------------------
// BATTLESTAT -- PASS on confirmed fields, noted where sourced.
//------------------------------------------------------------------
struct BATTLESTAT
{
    // -- Core offensive --
    // PASS: nATK assembled in BuildBattleStat from:
    //   ParamXxxServer.txt base WC (per class/level)
    //   + FreeStatStrTable[STR].WCAbsolute  (ChrCommon.txt)
    //   + PassiveSkill.MstPlSword1 (flat, OHSwdMastery)
    //   + weapon.rand(MinWC,MaxWC) * (1000 + PassiveSkill.MstRtSword1) / 1000
    //   + gear option WC bonuses (ItemOptions.shn)
    //   + skill.rand(MinWC,MaxWC)  (ActiveSkill.shn)
    //   + buff.nATK
    int     nATK;           // physical attack (melee) -- compiled total
    int     nMATK;          // magic attack
    int     nRATK;          // ranged attack
    int     nATKSpeed;      // attacks per interval (x1000)
    int     nCritical;      // critical hit chance (x100 = %)
                            // Source: ItemInfo.CriRate + FreeStatMenTable[MEN].CriRate
    int     nCriticalDmg;   // critical damage bonus (x100 = %)
                            // Source: PassiveSkill.CriDmgUpRate
    int     nPenetrate;     // armor penetration (flat) -- WIP, no SHN source confirmed

    // -- Weapon mastery rate (NA2016 confirmed from PassiveSkill.shn) --
    // MstRtSword1: BraveMastery01-09 gives 100->200 (applied as weapon_WC * rate / 1000)
    // MstPlSword1: OHSwdMastery01-09 gives 3->59 (flat WC additive)
    // WCRateUp: separate global WC rate modifier (new in NA2016)
    int     nWCRateUp;      // PASS: PassiveSkill.WCRateUp (x/1000 multiplier on total WC)
    int     nMARateUp;      // PASS: PassiveSkill.MARateUp (x/1000 multiplier on total MA)

    // -- Core defensive --
    // PASS: nDEF assembled from:
    //   ParamXxxServer.txt base AC (per class/level)
    //   + FreeStatConTable[CON].ACAbsolute  (ChrCommon.txt)
    //   + gear option AC bonuses
    //   + buff.nDEF
    // For mobs: MobInfoServer.Con fed through FreeStatConTable -> ACAbsolute
    int     nDEF;           // physical defense
    int     nMDEF;          // magic defense
    int     nRDEF;          // ranged defense
    int     nDodge;         // dodge/block chance (x100 = %)
                            // Source: FreeStatDexTable[DEX].TBRate + FreeStatConTable[CON].BlockRate
    int     nBlock;         // block (shield) chance (x100 = %)
    int     nBlockDef;      // damage reduced on block (x100 = %)
    int     nAbsorbDmg;     // flat damage absorption (from AbState buffs)
    int     nDMG_MinusRate; // PASS (new NA2016): PassiveSkill.DMG_MinusRate -- incoming dmg reduction rate

    // -- Vitals --
    int     nMaxHP;         // ParamXxxServer.MaxHP + FreeStatConTable[CON].MaxHP + gear
    int     nMaxSP;         // ParamXxxServer.MaxSP + FreeStatMenTable[MEN].MaxSP + gear
    int     nMaxLP;         // PASS (new NA2016): PassiveSkill.MaxLP (Love Points system)
    int     nHPRegen;       // PceHPRcv from MobInfoServer for mobs
    int     nSPRegen;

    // -- Movement --
    int     nMoveSpeed;
    int     nMoveSpeedPct;

    // -- CC resists (from MobResist.shn -- CC only, NO elemental) --
    // CORRECTED: previous version had elemental resists here. Wrong.
    // MobResist.shn columns: ResDot, ResStun, ResMoveSpeed, ResFear,
    //   ResBinding, ResReverse, ResMesmerize, ResSeverBone, ResKnockBack, ResTBMinus
    int     nResDot;
    int     nResStun;
    int     nResMoveSpeed;
    int     nResFear;
    int     nResBinding;
    int     nResReverse;
    int     nResMesmerize;
    int     nResSeverBone;
    int     nResKnockBack;
    int     nResTBMinus;

    // -- Elemental (stub only -- effectively unused in NA2016) --
    int     nEleResist[ ELEMENT_MAX ];  // all zero in practice
    int     nEleATK[ ELEMENT_MAX ];     // all zero in practice

    // -- Level / class --
    int     nLevel;
    BYTE    bClass;
    BYTE    bJob;

    // -- Raw stat points (for FreeStatXxxTable lookups) --
    // Source for chars: character DB allocated points
    // Source for mobs:  MobInfoServer.Str/Dex/Con/Int/Men
    int     nSTR;
    int     nDEX;
    int     nCON;
    int     nINT;
    int     nMEN;

    // -- Hit resolution --
    // PASS: nAccuracy = FreeStatDexTable[DEX].THRate + item.TH + HitRatePlus
    //       nDodge    = FreeStatDexTable[DEX].TBRate + FreeStatConTable[CON].BlockRate + item.TB
    // WeaponAttrib.HitRate = 500 (base 50% hit, ÷1000) applied in hit roll
    // SkilPyHitRate = 950 (95% skill accuracy modifier, ÷1000) from ActiveSkillInfoServer
    int     nAccuracy;

    // -- PvP modifiers --
    int     nPvPATKBonus;
    int     nPvPDEFBonus;

    // -- Misc --
    int     nLuck;
    int     nAbstateResist; // general CC resist (x100 = %)
    int     nAggroMult;     // AggroPerDamage from ActiveSkillInfoServer (NA2016: 9000 = 9x)

    void    Clear() { memset( this, 0, sizeof(*this) ); }
};

//------------------------------------------------------------------
// DAMAGERESULT -- PASS
//------------------------------------------------------------------
struct DAMAGERESULT
{
    int     nDamage;
    int     nDamageType;
    bool    bCritical;
    bool    bDodged;
    bool    bBlocked;
    bool    bMissed;
    int     nAbsorbed;
    eElement eElement;

    void    Clear() { memset( this, 0, sizeof(*this) ); }
};
