/*
 * Drive between crossroads
 */
#include "math.h"
#include "state.h"
#include "../settings.h"
#include "../location.h"
#include "../strategy.h"
#include "../global.h"
#include "../pid.h"

void stateGo(void)
{
	/*
	 * updating position of the next crossroad to visit
	 * if robot is between crossroad, one of 2 nearest crossroads will be chosen
	 * if robot is on crossroad, one of 5 nearest crossroads will be chosen
	 */
	updateNextCrossroad();

	// big angle to the next crossroad, need to rotate on gyro
	if (fabs(angleToNextCrossroad()) > settingAngleToBeginRotate)
	{
		changeState(STATE_ROTATE, REASON_BIG_ANGLE_TO_NEXT_CROSSROAD);
	}

	else if (targetReached())
	{
		Point oldTarget = getRecentTarget();

		//mark this target as reached
		removeRecentTarget();
		Point newTarget = getRecentTarget();

		//at baseline with can
		if (oldTarget.i == 2 && oldTarget.j == 0 && carryingCan)
		{
			changeState(STATE_LEAVE_CAN, REASON_ARRIVED_AT_BASELINE);
		}

		//if the new target is same as old, STOP
		if (newTarget.i == oldTarget.i && newTarget.j == oldTarget.j)
			setDriveStopFast();
	}

	//new can detected
	else if (sharp < settingSharpThresh && !carryingCan)
	{
		changeState(STATE_TAKE_CAN, REASON_CAN_DETECTED_SHARP);
	}

	//follow the line
	else
	{
		setDrivePIDForward(settingPIDForwardPWM);
	}
}
