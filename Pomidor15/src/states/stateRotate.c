/*
 * Rotate
 */

#include "state.h"
#include "../global.h"
#include "../pid.h"
#include "../settings.h"
#include "../location.h"
#include <math.h>

//current state of rotation process
int subStateRotate;

void stateRotate(void)
{
	//STOP
	if (subStateRotate == 0)
	{
		setDriveStopSlow();
		if (fabs(velocityLeft) + fabs(velocityRight) < 50)
			subStateRotate ++;
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

		if(fabs(angleToNextCrossroad())< 5)
			subStateRotate ++;
	}
	else if (subStateRotate == 2)
	{
		subStateRotate = 0;
		changeState(STATE_GO, REASON_ROTATION_FINISHED);
	}
}

