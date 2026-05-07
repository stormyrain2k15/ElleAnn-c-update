// battlestat.h
// BattleStat -- compiled per-object stat block.
// Built once on zone login then kept hot in memory.
// Rebuilt on equip/unequip, level up, buff apply/remove.
// Source: E:\ProjectF2\Server\5ZoneServer2\battlestat.h
//
// Field names derived from SHN table headers:
//   movermain.shn      -- base growth stats per class/level
//   moverability.shn   -- ability modifiers
//   iteminfo.shn       -- equipment stat contributions
//   weaponatrrib.shn   -- weapon element + attribute
//   activeskillinfo.shn    -- active skill modifiers
//   passiveskillinfo.shn   -- passive bonus stats
//   abstateinfo.shn    -- buff / debuff modifiers (volatile)
//   mobinfo.shn / mobinfoserver.shn -- mob base stats

#pragma once
#include "typedef.h"

//------------------------------------------------------------------
// Element type enum
// Matches EleType field in weaponatrrib.shn / abstateinfo.shn
//------------------------------------------------------------------
enum eElement
{
	ELEMENT_NONE	= 0,
	ELEMENT_FIRE	= 1,
	ELEMENT_WATER	= 2,
	ELEMENT_EARTH	= 3,
	ELEMENT_WIND	= 4,
	ELEMENT_LIGHT	= 5,
	ELEMENT_DARK	= 6,
	ELEMENT_MAX		= 7,
};

//------------------------------------------------------------------
// Damage type flags
// Used in skill damage calculation and resist checks
//------------------------------------------------------------------
enum eDamageType
{
	DMG_TYPE_NORMAL		= 0x0000,
	DMG_TYPE_MAGIC		= 0x0001,
	DMG_TYPE_SKILL		= 0x0002,
	DMG_TYPE_CRITICAL	= 0x0004,
	DMG_TYPE_RANGED		= 0x0008,
	DMG_TYPE_REFLECT	= 0x0010,
	DMG_TYPE_TRUE		= 0x0020,	// ignore DEF
};

//------------------------------------------------------------------
// BATTLESTAT
// All values are post-formula, ready for the damage calculation.
// Signed int -- debuffs can push values negative (clamped at 0).
//
// Naming follows MoverMain.shn / MoverAbility.shn column names
// as confirmed from tableheaders\movermain.h / moverability.h.
//------------------------------------------------------------------
struct BATTLESTAT
{
	// -- Core offensive --
	int		nATK;			// physical attack (melee)
	int		nMATK;			// magic attack
	int		nRATK;			// ranged attack
	int		nATKSpeed;		// attacks per interval (x1000)
	int		nCritical;		// critical hit chance (x100 = %)
	int		nCriticalDmg;	// critical damage bonus (x100 = %)
	int		nPenetrate;		// armor penetration (flat)

	// -- Core defensive --
	int		nDEF;			// physical defense
	int		nMDEF;			// magic defense
	int		nRDEF;			// ranged defense
	int		nDodge;			// dodge chance (x100 = %)
	int		nBlock;			// block chance (x100 = %)
	int		nBlockDef;		// damage reduced on block (x100 = %)
	int		nAbsorbDmg;		// flat damage absorption

	// -- Vitals --
	int		nMaxHP;
	int		nMaxSP;			// mana / spirit points
	int		nHPRegen;		// HP regen per tick
	int		nSPRegen;		// SP regen per tick

	// -- Movement --
	int		nMoveSpeed;		// base move speed
	int		nMoveSpeedPct;	// move speed % modifier from buffs

	// -- Elemental resist (one per element) --
	// Matches MobResist.shn columns: FireResist, WaterResist, etc.
	int		nEleResist[ ELEMENT_MAX ];

	// -- Elemental attack bonus --
	int		nEleATK[ ELEMENT_MAX ];

	// -- Level / class --
	int		nLevel;
	BYTE	bClass;			// char class (Fighter/Cleric/Archer/Mage/Trickster)
	BYTE	bJob;			// job tier (0=base 1=1st 2=2nd 3=3rd)

	// -- Stat points (STR/END/DEX/INT/SPR) --
	// Source: MoverMain.shn STR/END/DEX/INT/SPR growth columns
	int		nSTR;
	int		nEND;
	int		nDEX;
	int		nINT;
	int		nSPR;

	// -- PvP modifiers --
	int		nPvPATKBonus;	// % bonus to ATK vs players
	int		nPvPDEFBonus;	// % bonus to DEF vs players

	// -- Misc --
	int		nAccuracy;		// hit rate modifier
	int		nLuck;			// luck (affects item drop, critical variance)
	int		nAbstateResist;	// general abnormal state resistance (x100 = %)

	void	Clear() { memset( this, 0, sizeof(*this) ); }
};

//------------------------------------------------------------------
// DAMAGERESULT
// Output of the damage calculation pipeline.
// Populated by CalcDamage() in battle.cpp.
//------------------------------------------------------------------
struct DAMAGERESULT
{
	int		nDamage;		// final damage value
	int		nDamageType;	// eDamageType flags
	bool	bCritical;
	bool	bDodged;
	bool	bBlocked;
	bool	bMissed;
	int		nAbsorbed;		// flat absorbed before applying
	eElement eElement;

	void	Clear() { memset( this, 0, sizeof(*this) ); }
};
