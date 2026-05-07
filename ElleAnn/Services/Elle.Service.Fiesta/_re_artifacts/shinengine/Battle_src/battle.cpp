// battle.cpp
// Damage calculation pipeline.
// Source: E:\ProjectF2\Server\5ZoneServer2\battle.cpp
//
// Pipeline (each stage confirmed from source file names in PDB):
//
//   1. GetCompiledStats()     -- battlestat.h / battleobject.cpp
//   2. CalcRawDamage()        -- base ATK vs DEF formula
//   3. ApplyLevelGap()        -- levelgapdamagetable.cpp
//   4. ApplyElemental()       -- reactiontype.shn + mobresist.shn
//   5. ApplyAngle()           -- damagebyangle.cpp
//   6. ApplyCritical()        -- critical chance / damage
//   7. ApplyDodge()           -- dodge / block check
//   8. ApplyAbstateModifiers()-- abstateinfo.shn buffs/debuffs
//   9. ApplyAbsorb()          -- flat absorb
//
// FORMULAS:
// Base physical: TODO -- community verification required
// Base magical:  TODO -- community verification required
// Critical hit:  TODO -- community verification required
//
// Known constants from SHN tables and Fiesta theory crafting:
//   Damage = (ATK - DEF) * LevelGapMod * ElementMod * ...
//   Minimum damage floor exists (cannot go below 1)
//   Critical damage = base * (1.0 + CriticalDmg/100)
//
// TODO: Cross-reference empirical damage formulas from community
//       against SHN field names confirmed here from PDB.

#include "stdafx.h"
#include "battle.h"
#include "battlestat.h"
#include "levelgapdamagetable.h"
#include "mobresisttable.h"
#include "damagebyangle.h"
#include "condition.h"
#include "shineobjectplayer.h"
#include "shineobjectmob.h"
#include "ShineAssert.h"

//------------------------------------------------------------------
// Minimum damage -- cannot deal less than this
//------------------------------------------------------------------
#define DAMAGE_FLOOR		1

//------------------------------------------------------------------
// Global table instances (loaded at zone startup)
//------------------------------------------------------------------
static CLevelGapDamageTable g_lvGapTable;
static CMobResistTable      g_mobResistTable;

bool Battle_LoadTables( const char* pszDataPath )
{
	if( !g_lvGapTable.Load( pszDataPath ) )
	{
		ShineLog( "Battle_LoadTables -- failed to load level gap table" );
		return false;
	}
	if( !g_mobResistTable.Load( pszDataPath ) )
	{
		ShineLog( "Battle_LoadTables -- failed to load mob resist table" );
		return false;
	}
	return true;
}

//------------------------------------------------------------------
// CalcRawDamage
// Base ATK vs DEF before all modifiers.
//
// TODO: exact formula requires community verification.
// Candidate formula (common to ShineEngine variants):
//   nRaw = max( DAMAGE_FLOOR, nATK - nDEF )
//
// Some builds use:
//   nRaw = max( DAMAGE_FLOOR, nATK * nATK / (nATK + nDEF) )
//
// Variance: +/- random spread applied here or in CalcDamage.
// Spread range: TODO -- confirm from SHN or community.
//------------------------------------------------------------------
static int CalcRawDamage( const BATTLESTAT* pAtk,
                           const BATTLESTAT* pDef,
                           int nDmgTypeFlags )
{
	int nATK, nDEF;

	if( nDmgTypeFlags & DMG_TYPE_MAGIC )
	{
		nATK = pAtk->nMATK;
		nDEF = pDef->nMDEF;
	}
	else if( nDmgTypeFlags & DMG_TYPE_RANGED )
	{
		nATK = pAtk->nRATK;
		nDEF = pDef->nRDEF;
	}
	else
	{
		nATK = pAtk->nATK;
		nDEF = pDef->nDEF;
	}

	/* TODO: confirm exact formula with community
	   Using candidate formula: ATK - DEF with damage floor */
	int nRaw = nATK - nDEF;

	/* TODO: add random variance
	   Suspected: nRaw +/- (nRaw * spread%) where spread from SHN */

	return max( DAMAGE_FLOOR, nRaw );
}

//------------------------------------------------------------------
// ApplyLevelGap
// Multiply raw damage by level gap modifier.
// Table source: damagelvgappve.shn / damagelvgappvp.shn / damagelvgapevp.shn
//------------------------------------------------------------------
static int ApplyLevelGap( int nRaw,
                           int nAtkLevel, int nDefLevel,
                           eLvGapType eType )
{
	int nMult = g_lvGapTable.GetMultiplier( nAtkLevel, nDefLevel, eType );
	// nMult is x1000 -- e.g. 1000 = no change, 800 = 80% damage
	return max( DAMAGE_FLOOR, nRaw * nMult / 1000 );
}

//------------------------------------------------------------------
// ApplyElemental
// Apply element reaction multiplier + target elemental resist.
//
// reactiontype.shn defines multiplier for (AttackerElement, TargetElement)
// mobresist.shn defines target's resist % per element.
//
// TODO: load reactiontype.shn and confirm field layout.
// Community knowledge: Fire > Earth > Wind > Water > Fire cycle.
// Bonus/penalty values: TODO -- SHN editor + community verify.
//------------------------------------------------------------------
static int ApplyElemental( int nDmg,
                            eElement eAtkElem,
                            eElement eDefElem,
                            WORD wMobID )
{
	if( eAtkElem == ELEMENT_NONE ) return nDmg;

	/* TODO: reaction type table lookup
	   int nReactionMult = g_reactionTable.Get( eAtkElem, eDefElem );
	   nDmg = nDmg * nReactionMult / 1000; */

	// Apply mob elemental resist
	if( wMobID != 0 )
	{
		int nResist = g_mobResistTable.GetElementResist( wMobID, eAtkElem );
		// nResist is 0-100 (percentage reduction)
		nDmg = nDmg * ( 100 - nResist ) / 100;
	}

	return max( DAMAGE_FLOOR, nDmg );
}

//------------------------------------------------------------------
// ApplyCritical
// Roll for critical hit and apply critical damage multiplier.
//
// CritChance: pAtk->nCritical (x100 = %, so 500 = 5.00%)
// CritDmg:    pAtk->nCriticalDmg (x100, so 5000 = 50% extra)
// CritResist: pDef->nAbstateResist (partial -- TODO confirm)
//
// TODO: exact crit formula community verify.
// Candidate: roll rand(10000) < nCritical -> critical
//------------------------------------------------------------------
static int ApplyCritical( int nDmg,
                           const BATTLESTAT* pAtk,
                           const BATTLESTAT* pDef,
                           DAMAGERESULT* pResult )
{
	// Roll for critical
	int nRoll = rand() % 10000;
	int nChance = pAtk->nCritical;

	/* TODO: subtract defender crit resist
	   nChance -= pDef->nCritResist; */

	if( nRoll < nChance )
	{
		pResult->bCritical = true;
		// Apply crit damage bonus
		// nCriticalDmg stored x100: 5000 = +50%
		nDmg = nDmg + ( nDmg * pAtk->nCriticalDmg / 10000 );
		pResult->nDamageType |= DMG_TYPE_CRITICAL;
	}

	return max( DAMAGE_FLOOR, nDmg );
}

//------------------------------------------------------------------
// ApplyDodge
// Roll dodge and block checks against attacker accuracy.
//
// TODO: exact dodge / accuracy formula community verify.
// Candidate: dodged if rand(10000) < (nDodge - nAccuracy)
//------------------------------------------------------------------
static bool ApplyDodge( const BATTLESTAT* pAtk,
                         const BATTLESTAT* pDef,
                         DAMAGERESULT* pResult )
{
	/* TODO: dodge formula
	   int nDodgeChance = max(0, pDef->nDodge - pAtk->nAccuracy);
	   if( rand()%10000 < nDodgeChance ) { pResult->bDodged=true; return true; }
	*/

	/* TODO: block formula
	   int nBlockChance = max(0, pDef->nBlock - pAtk->nPenetrate);
	   if( rand()%10000 < nBlockChance ) { pResult->bBlocked=true; return false; }
	*/

	return false;	// not dodged
}

//------------------------------------------------------------------
// ApplyAbsorb
// Flat damage absorption (from abstate / item effects).
//------------------------------------------------------------------
static int ApplyAbsorb( int nDmg, const BATTLESTAT* pDef,
                         DAMAGERESULT* pResult )
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
// CalcDamage
// Main entry point -- runs full pipeline.
//
// pAtk, pDef    : compiled stat blocks (from GetCompiledStats)
// nDmgTypeFlags : eDamageType flags
// eAtkElem      : attacker's weapon element
// eDefElem      : target's element (mob species element)
// wMobID        : target mob ID for resist table (0 = player)
// nAngle        : attack angle relative to target facing (degrees)
// bPvP          : true = player vs player, false = vs mob
// pResult       : output -- populated by this function
//------------------------------------------------------------------
void CalcDamage( const BATTLESTAT* pAtk,
                 const BATTLESTAT* pDef,
                 int               nDmgTypeFlags,
                 eElement          eAtkElem,
                 eElement          eDefElem,
                 WORD              wMobID,
                 int               nAngle,
                 bool              bPvP,
                 DAMAGERESULT*     pResult )
{
	SHINE_ASSERT( pAtk && pDef && pResult );
	pResult->Clear();
	pResult->nDamageType = nDmgTypeFlags;
	pResult->eElement    = eAtkElem;

	// 1. Dodge check -- if dodged, damage = 0
	if( ApplyDodge( pAtk, pDef, pResult ) )
	{
		pResult->nDamage = 0;
		return;
	}

	// 2. Raw damage (ATK vs DEF)
	int nDmg = CalcRawDamage( pAtk, pDef, nDmgTypeFlags );

	// 3. Level gap modifier
	eLvGapType eGapType = bPvP ? LVGAP_PVP
	                    : (wMobID != 0 ? LVGAP_EVP : LVGAP_PVE);

	/* Note: EVP = mob attacking player, PVE = player attacking mob
	   Caller passes bPvP=false + wMobID=0 when mob attacks player
	   TODO: confirm attacker/defender role handling */
	nDmg = ApplyLevelGap( nDmg, pAtk->nLevel, pDef->nLevel, eGapType );

	// 4. Elemental reaction + mob resist
	nDmg = ApplyElemental( nDmg, eAtkElem, eDefElem, wMobID );

	// 5. Angle modifier (back attack bonus etc.)
	/* TODO: damagebyangle.cpp
	   nDmg = ApplyAngle( nDmg, nAngle ); */

	// 6. Critical hit roll
	nDmg = ApplyCritical( nDmg, pAtk, pDef, pResult );

	// 7. AbState modifiers (buffs on attacker, debuffs on target)
	/* TODO: abstateinfo.shn integration
	   nDmg = ApplyAbstateModifiers( nDmg, pAtk, pDef ); */

	// 8. PvP damage modifier
	if( bPvP )
	{
		/* TODO: PvP damage cap / modifier from SHN
		   nDmg = nDmg * pAtk->nPvPATKBonus / 100; */
	}

	// 9. Flat absorb
	nDmg = ApplyAbsorb( nDmg, pDef, pResult );

	// 10. Final floor
	pResult->nDamage = max( DAMAGE_FLOOR, nDmg );
}

//------------------------------------------------------------------
// CalcSkillDamage
// Wraps CalcDamage with skill-specific modifiers.
// Skill parameters sourced from ActiveSkillInfo.shn.
//
// TODO: ActiveSkillInfoServer.shn -- server-side skill columns:
//   DamageType, ElementType, ATKRate (% of base ATK),
//   FixedDamage, HitCount, AbStateApply, AbStateRate
//------------------------------------------------------------------
void CalcSkillDamage( const BATTLESTAT* pAtk,
                       const BATTLESTAT* pDef,
                       WORD              wSkillID,
                       int               nAngle,
                       bool              bPvP,
                       DAMAGERESULT*     pResult )
{
	/* TODO: look up skill from ActiveSkillInfoServer.shn by wSkillID
	   - Get DamageType flags
	   - Get ElementType
	   - Get ATKRate (multiply base ATK before CalcRawDamage)
	   - Get FixedDamage (add after pipeline)
	   - Get HitCount (call CalcDamage HitCount times)
	*/

	// Placeholder: treat as normal attack for now
	CalcDamage( pAtk, pDef,
	            DMG_TYPE_SKILL,
	            ELEMENT_NONE, ELEMENT_NONE,
	            0, nAngle, bPvP, pResult );

	ShineLog( "CalcSkillDamage skill=%u -- TODO: load skill params from SHN",
	          wSkillID );
}

//------------------------------------------------------------------
// CalcAbstateDamage
// Damage from DoT / HoT abnormal states.
// Source: AbStateInfo.shn -- confirmed column: AbStataIndex maps to
// damage rate, tick interval, element.
//
// TODO: AbStateInfo.shn field layout -- SHN editor pending.
//------------------------------------------------------------------
void CalcAbstateDamage( const BATTLESTAT* pTarget,
                         WORD              wAbStateID,
                         DAMAGERESULT*     pResult )
{
	/* TODO: look up abstate from AbStateInfo.shn
	   - Get damage rate (% of target max HP or flat value)
	   - Get element
	   - Apply resist
	*/

	pResult->Clear();
	pResult->nDamage = DAMAGE_FLOOR;

	ShineLog( "CalcAbstateDamage abstate=%u -- TODO: SHN field layout pending",
	          wAbStateID );
}
