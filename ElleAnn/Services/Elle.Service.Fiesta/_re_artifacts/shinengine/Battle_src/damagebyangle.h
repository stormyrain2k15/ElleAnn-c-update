// damagebyangle.h
// ApplyAngle -- back/side attack damage modifier.
// Source: E:\ProjectF2\Server\5ZoneServer2\damagebyangle.h
//
// Fiesta has positional damage bonuses:
//   Back attack:  damage bonus (confirmed by community)
//   Side attack:  smaller bonus or none
//   Front attack: no bonus
//
// Angle: 0 = attacking from front, 180 = from behind.
// Thresholds and multipliers: TODO -- community verify + SHN check.
// Possible source: ActionRangeFactor.shn or hardcoded in battle.cpp.

#pragma once
#include "typedef.h"

//------------------------------------------------------------------
// Angle thresholds (degrees, facing = 0)
// TODO: confirm exact values
//------------------------------------------------------------------
#define ANGLE_BACK_MIN		135		// candidate: 135-225 = back arc
#define ANGLE_BACK_MAX		225
#define ANGLE_SIDE_MIN		90		// candidate: 90-135 / 225-270 = side arc
#define ANGLE_SIDE_MAX		270

//------------------------------------------------------------------
// Damage multipliers x1000
// TODO: confirm from community formula testing
//------------------------------------------------------------------
#define ANGLE_BACK_MULT		1200	// candidate: +20% from behind
#define ANGLE_SIDE_MULT		1000	// candidate: no bonus from side
#define ANGLE_FRONT_MULT	1000	// no bonus from front

//------------------------------------------------------------------
int ApplyAngle( int nDamage, int nAngleDeg );
