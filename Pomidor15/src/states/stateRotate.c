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
void stateRotate(void)
{
	//STOP
	if (subStateRotate == 0)
	{

		setDriveSideKtir();

		if (fabs(velocityLeft) + fabs(velocityRight) < 10)
		{
			subStateRotate++;

			//direction snap
			direction = roundToTheMultipleOf(direction, 90);
			direction = angleMakeInRange(direction);

			//finding vector from center of robot to its front KTIR line
			position = getNearestCrossroad(position);
		}
	}

	else if (subStateRotate == 1)
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
	else if (subStateRotate == 2)
	{
		setDriveSideKtir();
		if (fabs(velocityLeft) + fabs(velocityRight) < 10)
		{
			subStateRotate++;

			//direction snap
			direction = roundToTheMultipleOf(direction, 90);
			direction = angleMakeInRange(direction);

			//finding vector from center of robot to its front KTIR line
			position = getNearestCrossroad(position);
		}

	}
	else if (subStateRotate == 3)
	{
		subStateRotate = 0;
		changeState(STATE_GO, REASON_ROTATION_FINISHED);
	}

}

