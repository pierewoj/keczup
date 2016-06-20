/*
 * Approach new can and take it
 */

#include "../pid.h"
#include "../global.h"
#include "../location.h"
#include "../geometry.h"
#include "../lowlevel/effectors.h"
#include "../strategy.h"
#include "state.h"

int subStateTakeCan = 0;
int timer;
void stateTakeCan(void)
{
	if (subStateTakeCan == 0)
	{
		setDriveWheelVelocity(200, 200);
		if (distanceToNextCrossroad() < 50)
		{
			closeFrame();
			controllerLeftKtir.feedback = 2;
			controllerRightKtir.feedback = 0;

			subStateTakeCan++;
			timer = getMiliseconds();
		}
	}
	else if (subStateTakeCan == 1)
	{
		setDriveSideKtir();
		int timeSinceTimer = getMiliseconds() - timer;
		//counting number of KTIRS
		int numKtirRight = 0, numKtirFront = 0, numKtirLeft = 0;
		int i;
		for (i = 0; i < 7; i++)
		{
			if (ktirFront[i])
				numKtirFront++;
		}
		for (i = 0; i < 3; i++)
		{
			if (ktirRight[i])
				numKtirRight++;
			if (ktirLeft[i])
				numKtirLeft++;
		}

		//dont make any decitions if 1s didnt passed
		if(timeSinceTimer < 1000)
			return;

		//checking if robot didnt hang on cans
		if ( numKtirFront > 5)
		{
			subStateTakeCan++;
			timer = getMiliseconds();
			openFrame();
		}

		//checking if can was correctyly taken
		else if(fabs(velocityLeft) + fabs(velocityRight) < 10
				&& numKtirRight + numKtirLeft <= 3  )
		{
			carryingCan = true;
			snapPositionAndDirection();

			Point currentPosition = ofPointMM(position);

			//not going back to the first can
			if(! (currentPosition.i == 3 && currentPosition.j == 2))
				addNewTarget(currentPosition.i, currentPosition.j);

			addNewTarget(3, 0);

			subStateTakeCan = 0;
			changeState(STATE_GO, REASON_CAN_TAKEN);
		}
		else
		{
			changeState(STATE_STOP, REASON_UNKNOWN_CONDIDION_TAKING_CAN);
		}

	}
	//w przypadku zawieszenia
	else if (subStateTakeCan == 2)
	{
		setDriveStopFast();

		if (getMiliseconds() - timer > 1000)
		{
			Point currentCrossroad = ofPointMM(position);
			enemyTimes[currentCrossroad.i][currentCrossroad.j] =
					getMiliseconds();

			subStateTakeCan = 0;
			nextCrossroad = previousCrossroad;
			changeState(STATE_GO_PREVIOUS, REASON_HANGED_ON_CAN);
		}
	}
}
