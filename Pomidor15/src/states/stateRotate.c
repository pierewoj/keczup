/*
 * Rotate
 */

#include "state.h"
#include "../global.h"
#include "../pid.h"
#include "../settings.h"
#include "../location.h"
#include "../geometry.h"
#include <math.h>

//current state of rotation process
int subStateRotate;
int waiter;
void stateRotate(void)
{
	//STOP
	if (subStateRotate == 0)
	{

		setDriveSideKtir();

		if (fabs(velocityLeft) + fabs(velocityRight) < 50)
		{
			subStateRotate++;
			waiter = getMicroseconds();
			//direction snap
			direction = roundToTheMultipleOf(direction, 90);
			direction = angleMakeInRange(direction);

			//finding vector from center of robot to its front KTIR line
			position = getNearestCrossroad(position);
		}
	}

#warning DEBUG
	//for debug purposes, to see which part of rotation goes wrong
	else if(subStateRotate == 1)
	{
		setDriveStopSlow();
		if(getMicroseconds() - waiter > 2000)
		{
			subStateRotate ++ ;
		}
	}

	else if (subStateRotate == 2)
	{
		int dir = 0;
		if (angleToNextCrossroad() > 0)
			dir = 1;
		else
			dir = -1;

		setDriveWheelVelocity(-dir * settingMaxRotationVelocity,
				dir * settingMaxRotationVelocity);

		if (fabs(angleToNextCrossroad()) < 5)
			subStateRotate++;
	}
	else if (subStateRotate ==3)
	{
		subStateRotate = 0;
		changeState(STATE_GO, REASON_ROTATION_FINISHED);
	}
}

