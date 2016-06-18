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
	 * change sideKtir PID feedback so that the robot
	 * will go to the line in front of him
	 */
	if (80 < distanceToNextCrossroad() && distanceToNextCrossroad() < 135)
	{
		controllerLeftKtir.feedback = 2; //front left ktir
		controllerRightKtir.feedback = 0; //front right ktir
	}

	/*
	 * taking can
	 */
	if (distanceToNextCrossroad() < 100 && sharp < settingSharpThresh
			&& !carryingCan && (ultra[0] == 1000))
	{
		changeState(STATE_TAKE_CAN, REASON_CAN_DETECTED_SHARP);
	}

	/*
	 * if we are near to the crossroad
	 */
	if (distanceToNextCrossroad() < settingCrossroadRadius)
	{
		//wyznaczenie nowego celu jak dojechal
		if (targetReached())
		{
			Point currentTarget = getRecentTarget();
			Point pos = ofPointMM(position);

			//end game tactics
			if (currentTarget.i == 3 && currentTarget.j == 6
					&& endGameTacticsEnabled)
			{
				changeState(STATE_STOP, REASON_ALL_CANS_COLLECTED);
				return;
			}

			//regular behaviour
			removeRecentTarget();

			//leaving can
			if (carryingCan && pos.i == 3 && pos.j == 0)
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
			setDriveSideKtir();
		}

		// big angle to the next crossroad, rotating
		if (fabs(angleToNextCrossroad()) > settingAngleToBeginRotate)
		{
			changeState(STATE_ROTATE, REASON_BIG_ANGLE_TO_NEXT_CROSSROAD);
		}
	}

	else if (isEnemy(nextCrossroad) && ktirBack[3]
				&& (distance(position, ofPoint(previousCrossroad))
						+ distance(position, ofPoint(nextCrossroad)) < 400)
				&& !equals(previousCrossroad, nextCrossroad))
		{
			nextCrossroad = previousCrossroad;
			changeState(STATE_GO_PREVIOUS, REASON_ENEMY_DETECTED_IN_FRONT);
		}
	else
	{
		setDrivePIDForward(settingPIDForwardPWM);
	}

}
