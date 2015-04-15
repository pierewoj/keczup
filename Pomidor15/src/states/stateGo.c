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
	// big angle to the next crossroad, rotating
	if (fabs(angleToNextCrossroad()) > settingAngleToBeginRotate
			&& distanceToNextCrossroad() > settingCrossroadRadius)
	{
		changeState(STATE_ROTATE, REASON_BIG_ANGLE_TO_NEXT_CROSSROAD);
	}

	//reseting feedback for side KTIR controllers
	else if (80 < distanceToNextCrossroad() && distanceToNextCrossroad() < 100)
	{
		controllerLeftKtir.feedback = 2;
		controllerRightKtir.feedback = 0;
	}

	else if (distanceToNextCrossroad() < settingCrossroadRadius)
	{
		updateNextCrossroad();
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
