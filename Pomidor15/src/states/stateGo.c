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
	//snapping
	if ((ktirFront[3]  || ktirBack[3]) && (ktirRight[1] || ktirLeft[1]))
	{
		//direction snap
		direction = roundToTheMultipleOf(direction, 90);
		direction = angleMakeInRange(direction);

		//finding vector from center of robot to its front KTIR line
		position = getNearestCrossroad(position);
	}

	//reseting feedback for side KTIR controllers
	if (80 < distanceToNextCrossroad() && distanceToNextCrossroad() < 100)
	{
		controllerLeftKtir.feedback = 2;
		controllerRightKtir.feedback = 0;
	}

	if (distanceToNextCrossroad() < settingCrossroadRadius)
	{
		//wyznaczenie nowego celu jak dojechal
		if (targetReached())
		{
			removeRecentTarget();
		}

		//wyznaczanie drogi do celu
		updateNextCrossroad();

		//sprawdzenie czy kolejne skrzyzowanie sie zmienilo
		if (distanceToNextCrossroad() < settingCrossroadRadius)
		{
			setDriveStopFast();
		}

		// big angle to the next crossroad, rotating
		if (fabs(angleToNextCrossroad()) > settingAngleToBeginRotate)
		{
			changeState(STATE_ROTATE, REASON_BIG_ANGLE_TO_NEXT_CROSSROAD);
		}
	}
	else
	{
		setDrivePIDForward(settingPIDForwardPWM);
	}

}
