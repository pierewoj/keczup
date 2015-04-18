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

		}
	}
	else if (subStateTakeCan == 1)
	{
		setDriveSideKtir();
		if (fabs(velocityLeft) + fabs(velocityRight) < 10)
		{
			subStateTakeCan=0;
			carryingCan=true;
			snapPositionAndDirection();

			Point currentPosition = ofPointMM(position);
			addNewTarget(currentPosition.i, currentPosition.j);
			addNewTarget(2,0);
			changeState(STATE_GO, REASON_CAN_TAKEN);
		}

	}
}
