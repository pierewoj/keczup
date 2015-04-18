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


	//reseting feedback for side KTIR controllers
	if (80 < distanceToNextCrossroad() && distanceToNextCrossroad() < 100)
	{
		controllerLeftKtir.feedback = 2;
		controllerRightKtir.feedback = 0;
	}

	if(distanceToNextCrossroad() < 100 && sharp < settingSharpThresh && !carryingCan)
	{
		changeState(STATE_TAKE_CAN, REASON_CAN_DETECTED_SHARP);
	}

	if (distanceToNextCrossroad() < settingCrossroadRadius)
	{
		//wyznaczenie nowego celu jak dojechal
		if (targetReached())
		{
			removeRecentTarget();
			if(carryingCan && position.y < 150)
			{
				changeState(STATE_LEAVE_CAN, REASON_BASELINE_REACHED);
				return;
			}
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
