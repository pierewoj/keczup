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

	if (distanceToNextCrossroad() < 100 && sharp < settingSharpThresh
			&& !carryingCan)
	{
		changeState(STATE_TAKE_CAN, REASON_CAN_DETECTED_SHARP);
	}

	if (distanceToNextCrossroad() < settingCrossroadRadius)
	{
		//wyznaczenie nowego celu jak dojechal
		if (targetReached())
		{
			Point currentTarget = getRecentTarget();
			Point pos = ofPointMM(position);
			//end game tactics
			if (currentTarget.i == 2 && currentTarget.j == 4
					&& endGameTacticsEnabled)
			{
				changeState(STATE_STOP, REASON_ALL_CANS_COLLECTED);
				return;
			}

			//regular behaviour
			removeRecentTarget();

			//leaving can
			if (carryingCan && pos.i == 2 && pos.j == 0)
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
	else if (ultra[0] < settingDistanceToRunFromEnemy && ktirBack[3]
			&& (distance(position, ofPoint(previousCrossroad))
					+ distance(position, ofPoint(nextCrossroad)) < 400)
			&& !equals(previousCrossroad, nextCrossroad)
			&& distance(position, ofPoint(previousCrossroad))
					> 1.1 * settingCrossroadRadius)
	{
		nextCrossroad = previousCrossroad;
		changeState(STATE_GO_PREVIOUS, REASON_ENEMY_DETECTED_IN_FRONT);
	}
	else
	{
		setDrivePIDForward(settingPIDForwardPWM);
	}

}
