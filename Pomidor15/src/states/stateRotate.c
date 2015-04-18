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

			snapPositionAndDirection();
		}
	}

	//rotating till last 30 degrees
	else if (subStateRotate == 1)
	{
		int dir = 0;
		if (angleToNextCrossroad() > 0)
			dir = 1;
		else
			dir = -1;

		setDriveWheelVelocity(-dir * settingMaxRotationVelocity,
				dir * settingMaxRotationVelocity);

		if (fabs(angleToNextCrossroad()) < 30)
		{
			subStateRotate++;
			//SETTING PREDICTED SIDES OF LINES for driveSideKtir()

			//rotation couter-clockwise
			if (dir == 1)
			{
				controllerLeftKtir.feedback = 0;
				controllerRightKtir.feedback = 0;
			}

			//rotation clockwise
			else if (dir == -1)
			{
				controllerLeftKtir.feedback = 2;
				controllerRightKtir.feedback = 2;
			}
		}
	}

	//rotating last 30 degrees
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

	//stopping after rotation
	else if (subStateRotate == 3)
	{
		setDriveSideKtir();
		if (fabs(velocityLeft) + fabs(velocityRight) < 10)
		{
			subStateRotate++;

			snapPositionAndDirection();
		}

	}
	else if (subStateRotate == 4)
	{
		subStateRotate = 0;
		changeState(STATE_GO, REASON_ROTATION_FINISHED);
	}

}

