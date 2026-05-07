// damagebyangle.cpp
// Source: E:\ProjectF2\Server\5ZoneServer2\damagebyangle.cpp

#include "stdafx.h"
#include "damagebyangle.h"

int ApplyAngle( int nDamage, int nAngleDeg )
{
	// Normalise angle to 0-359
	nAngleDeg = ( (nAngleDeg % 360) + 360 ) % 360;

	int nMult;

	if( nAngleDeg >= ANGLE_BACK_MIN && nAngleDeg <= ANGLE_BACK_MAX )
		nMult = ANGLE_BACK_MULT;
	else
		nMult = ANGLE_FRONT_MULT;

	/* TODO: confirm thresholds and multipliers with community.
	   Also confirm whether side arc has a separate bonus. */

	return nDamage * nMult / 1000;
}
