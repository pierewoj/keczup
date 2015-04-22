/*
 * stateGoPrevious.c
 *
 *  Created on: 22 kwi 2015
 *      Author: Kuba
 */

#include "../pid.h"
#include "../global.h"
#include "../location.h"
#include "../lowlevel/effectors.h"
#include "../strategy.h"
#include "state.h"
#include "../settings.h"

static int substateGoPrevious;

void stateGoPrevious(void)
{
	/*
	 * waiting for robot to full stop
	 */
	if (substateGoPrevious == 0)
	{
		setDriveStopFast();
		if (fabs(velocityLeft) + fabs(velocityRight) < 10)
		{
			substateGoPrevious++;
		}
	}

	/*
	 * driving backward till next crossroad
	 */
	else if (substateGoPrevious == 1)
	{
		setDrivePIDBackward(settingPIDForwardPWM);
		if (distanceToNextCrossroad() < settingCrossroadRadius)
		{
			//next line will be behind robot
			substateGoPrevious++;
			controllerLeftKtir.feedback = 0;
			controllerRightKtir.feedback = 2;
		}

		/*
		 * if distance went too big stop the robot
		 */
		if(distanceToNextCrossroad() > 350)
		{
			changeState(STATE_STOP, REASON_LEAVING_CAN_TOO_BIG_DISTANCE);
		}
	}

	/*
	 * centering at crossroad on KTIRS and finishing going back
	 */
	if (substateGoPrevious == 2)
	{
		setDriveSideKtir();
		if (fabs(velocityLeft) + fabs(velocityRight) < 10)
		{
			substateGoPrevious  = 0;
			changeState(STATE_GO, REASON_FINISHED_GOING_PREVIOUS);
		}
	}
}
