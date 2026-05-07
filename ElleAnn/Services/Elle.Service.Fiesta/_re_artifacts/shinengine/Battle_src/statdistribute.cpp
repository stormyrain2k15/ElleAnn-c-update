// statdistribute.cpp
// Builds the compiled BATTLESTAT from raw character stats + equipment.
// Source: E:\ProjectF2\Server\5ZoneServer2\statdistribute.cpp
//
// Input sources:
//   MoverMain.shn       -- base stats per class / level
//   MoverAbility.shn    -- stat point growth rates (STR->ATK etc.)
//   ItemInfo.shn        -- equipment stat bonuses
//   ActiveSkillInfo.shn -- active skill passive bonuses (if any)
//   PassiveSkillInfo.shn-- passive skill stat bonuses
//   AbStateInfo.shn     -- buff / debuff volatile modifiers
//
// Growth formula: TODO -- community verify
// Candidate (common ShineEngine pattern):
//   nATK = nBaseATK + STR * nSTRtoATK_Rate + nEquipATK + nBuffATK
//
// nSTRtoATK_Rate etc. come from MoverAbility.shn per class.
// Base stats per level from MoverMain.shn HP/SP/ATK/DEF columns.

#include "stdafx.h"
#include "statdistribute.h"
#include "battlestat.h"
#include "ShineAssert.h"

//------------------------------------------------------------------
// BuildBattleStat
// Compiles a BATTLESTAT from the object's current state.
// Called on login, level up, equip change, buff apply/remove.
//------------------------------------------------------------------
void BuildBattleStat( BATTLESTAT*            pOut,
                       const RAWCHARSTAT*     pRaw,
                       const EQUIPSUMMARY*    pEquip,
                       const BUFFMODIFIERS*   pBuff )
{
	SHINE_ASSERT( pOut && pRaw );
	pOut->Clear();

	pOut->nLevel = pRaw->nLevel;
	pOut->bClass = pRaw->bClass;
	pOut->bJob   = pRaw->bJob;

	// -- Base stat points --
	pOut->nSTR = pRaw->nSTR;
	pOut->nEND = pRaw->nEND;
	pOut->nDEX = pRaw->nDEX;
	pOut->nINT = pRaw->nINT;
	pOut->nSPR = pRaw->nSPR;

	// -- Base values from MoverMain.shn (per class / level) --
	/* TODO: load MoverMain.shn rows for bClass / nLevel.
	   Confirmed columns from tableheaders\movermain.h:
	   HP, SP, WalkSpeed, RunSpeed, ATK, DEF, MATK, MDEF -- TODO field names */
	pOut->nMaxHP    = pRaw->nBaseHP;
	pOut->nMaxSP    = pRaw->nBaseSP;
	pOut->nMoveSpeed = pRaw->nBaseMoveSpeed;

	// -- Stat-to-secondary growth (MoverAbility.shn) --
	/* TODO: load MoverAbility.shn for class.
	   Multiply each stat point by its growth rate into secondaries.
	   e.g. nATK += pRaw->nSTR * nSTRtoATKRate / 1000;
	        nDEF += pRaw->nEND * nENDtoDEFRate / 1000;
	   Rates confirmed per class -- TODO community verify values */
	pOut->nATK  = pRaw->nBaseATK;
	pOut->nDEF  = pRaw->nBaseDEF;
	pOut->nMATK = pRaw->nBaseMATK;
	pOut->nMDEF = pRaw->nBaseMDEF;

	// -- Equipment bonuses (ItemInfo.shn) --
	if( pEquip )
	{
		pOut->nATK      += pEquip->nATK;
		pOut->nMATK     += pEquip->nMATK;
		pOut->nRATK     += pEquip->nRATK;
		pOut->nDEF      += pEquip->nDEF;
		pOut->nMDEF     += pEquip->nMDEF;
		pOut->nMaxHP    += pEquip->nHP;
		pOut->nMaxSP    += pEquip->nSP;
		pOut->nCritical += pEquip->nCritical;
		pOut->nDodge    += pEquip->nDodge;
		pOut->nAccuracy += pEquip->nAccuracy;
		pOut->nMoveSpeed+= pEquip->nMoveSpeed;

		// Elemental ATK from weapon
		for( int i = 0; i < ELEMENT_MAX; i++ )
			pOut->nEleATK[i] += pEquip->nEleATK[i];
	}

	// -- Passive skill bonuses (PassiveSkillInfo.shn) --
	/* TODO: iterate learned passive skills, apply bonuses.
	   PassiveSkillInfo.shn columns: StatType, StatValue, ConditionType
	   TODO -- confirm field names from tableheaders\passiveskillinfo.h */

	// -- Buff / debuff volatile modifiers (AbStateInfo.shn) --
	if( pBuff )
	{
		pOut->nATK       += pBuff->nATK;
		pOut->nMATK      += pBuff->nMATK;
		pOut->nDEF       += pBuff->nDEF;
		pOut->nMDEF      += pBuff->nMDEF;
		pOut->nMaxHP     += pBuff->nMaxHP;
		pOut->nMoveSpeed += pBuff->nMoveSpeed;
		pOut->nCritical  += pBuff->nCritical;
		pOut->nDodge     += pBuff->nDodge;

		for( int i = 0; i < ELEMENT_MAX; i++ )
			pOut->nEleResist[i] += pBuff->nEleResist[i];
	}

	// -- Clamp negatives to 0 (debuffs cannot invert) --
	pOut->nATK      = max( 0, pOut->nATK );
	pOut->nMATK     = max( 0, pOut->nMATK );
	pOut->nDEF      = max( 0, pOut->nDEF );
	pOut->nMDEF     = max( 0, pOut->nMDEF );
	pOut->nMaxHP    = max( 1, pOut->nMaxHP );
	pOut->nMaxSP    = max( 0, pOut->nMaxSP );
	pOut->nMoveSpeed= max( 0, pOut->nMoveSpeed );
}
