// battle.h
// Battle system declarations.
// Source: E:\ProjectF2\Server\5ZoneServer2\battle.h

#pragma once
#include "battlestat.h"

// Load level gap + mob resist tables at zone startup.
bool	Battle_LoadTables( const char* pszDataPath );

// Main damage calculation entry point.
void	CalcDamage( const BATTLESTAT* pAtk,
                   const BATTLESTAT* pDef,
                   int               nDmgTypeFlags,
                   eElement          eAtkElem,
                   eElement          eDefElem,
                   WORD              wMobID,
                   int               nAngle,
                   bool              bPvP,
                   DAMAGERESULT*     pResult );

// Skill damage (wraps CalcDamage with ActiveSkillInfo.shn params).
void	CalcSkillDamage( const BATTLESTAT* pAtk,
                         const BATTLESTAT* pDef,
                         WORD              wSkillID,
                         int               nAngle,
                         bool              bPvP,
                         DAMAGERESULT*     pResult );

// Abnormal state (DoT) damage.
void	CalcAbstateDamage( const BATTLESTAT* pTarget,
                           WORD              wAbStateID,
                           DAMAGERESULT*     pResult );
