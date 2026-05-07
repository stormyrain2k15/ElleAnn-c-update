// statdistribute.h
// Input structures for BuildBattleStat.
// Source: E:\ProjectF2\Server\5ZoneServer2\statdistribute.h

#pragma once
#include "battlestat.h"

//------------------------------------------------------------------
// RAWCHARSTAT
// Base stats before any compilation.
// Sourced from character DB + MoverMain.shn lookup.
//------------------------------------------------------------------
struct RAWCHARSTAT
{
	int		nLevel;
	BYTE	bClass;
	BYTE	bJob;

	// Allocated stat points
	int		nSTR, nEND, nDEX, nINT, nSPR;

	// Base values from MoverMain.shn for this class / level
	// TODO: confirm exact MoverMain.shn column names
	int		nBaseHP;
	int		nBaseSP;
	int		nBaseATK;
	int		nBaseDEF;
	int		nBaseMATK;
	int		nBaseMDEF;
	int		nBaseMoveSpeed;
};

//------------------------------------------------------------------
// EQUIPSUMMARY
// Summed equipment bonuses across all equipped slots.
// Built by iterating inventory equipped slots against ItemInfo.shn.
// TODO: confirm ItemInfo.shn bonus column names.
//------------------------------------------------------------------
struct EQUIPSUMMARY
{
	int		nATK, nMATK, nRATK;
	int		nDEF, nMDEF, nRDEF;
	int		nHP, nSP;
	int		nCritical, nDodge, nBlock, nAccuracy;
	int		nMoveSpeed;
	int		nEleATK[ ELEMENT_MAX ];
	int		nEleResist[ ELEMENT_MAX ];
	/* TODO: additional fields from ItemInfo.shn */
};

//------------------------------------------------------------------
// BUFFMODIFIERS
// Volatile modifiers from active abnormal states.
// Rebuilt each time a buff applies or expires.
// Source: AbStateInfo.shn stat columns.
// TODO: confirm AbStateInfo.shn column names.
//------------------------------------------------------------------
struct BUFFMODIFIERS
{
	int		nATK, nMATK, nRATK;
	int		nDEF, nMDEF;
	int		nMaxHP, nMaxSP;
	int		nHPRegen, nSPRegen;
	int		nMoveSpeed;
	int		nCritical, nDodge, nAccuracy;
	int		nEleResist[ ELEMENT_MAX ];
	int		nEleATK[ ELEMENT_MAX ];
	/* TODO: additional fields */
};

//------------------------------------------------------------------
void BuildBattleStat( BATTLESTAT*          pOut,
                       const RAWCHARSTAT*   pRaw,
                       const EQUIPSUMMARY*  pEquip,   // may be NULL
                       const BUFFMODIFIERS* pBuff );  // may be NULL
