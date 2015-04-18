/*
 * Leaving can on a base line
 */

#include "../pid.h"
#include "../global.h"
#include "../location.h"
#include "../lowlevel/effectors.h"
#include "../strategy.h"
#include "state.h"
#include "../settings.h"

int subStateLeave = 0;
int directionOfLeavingCan = 1; //(1) - prawo, (-1) - lewo
int lastPositionLeft = 0;
int lastPositionRight = 1200;
int targetPosition = 0;
void stateLeaveCan(void)
{
	if(lastPositionLeft > 450 || lastPositionRight < 750)
	{
		changeState(STATE_STOP, REASON_TOO_MUCH_CANS);
	}


	//adjusting position on KTIRS
	if (subStateLeave == 0)
	{
		setDriveSideKtir();
		if (fabs(velocityLeft) + fabs(velocityRight) < 10)
		{
			subStateLeave++;
			snapPositionAndDirection();
			//side of leaving is different than in the last time
			directionOfLeavingCan *= -1;
		}
	}

	//rotation
	if (subStateLeave == 1)
	{
		int directionOfRotation = directionOfLeavingCan;

		setDriveWheelVelocity(-directionOfRotation * settingMaxRotationVelocity,
				directionOfRotation * settingMaxRotationVelocity);

		int angleLeft = 0;
		if (directionOfLeavingCan == 1)
			angleLeft = fabs(angleDifference(direction, 0));
		else
			angleLeft = fabs(angleDifference(direction, 180));

		if (angleLeft < 5)
			subStateLeave++;
	}

	//adjusting position on KTIRS again
	if (subStateLeave == 2)
	{
		setDriveSideKtir();
		if (fabs(velocityLeft) + fabs(velocityRight) < 10)
		{
			subStateLeave++;
			snapPositionAndDirection();
			openFrame();
			carryingCan = 0;

			//setting target position
			if(directionOfLeavingCan == 1)
			{
				targetPosition = lastPositionRight - settingDistanceCanMiddle;
				lastPositionRight -= settingDistanceBetweenCans;
			}
			else
			{
				targetPosition = lastPositionLeft + settingDistanceCanMiddle;
				lastPositionLeft += settingDistanceBetweenCans;
			}
		}
	}

	//driving PID forward
	if(subStateLeave == 3)
	{
		setDrivePIDForward(settingPIDForwardPWM);
		if(fabs(position.x - targetPosition) < settingDistanceToGoBack)
		{
			subStateLeave ++;
		}
	}

	//driveing PID backward
	if(subStateLeave == 4)
	{
		setDrivePIDBackward(settingPIDForwardPWM);
		if(fabs(position.x - 600) < settingCrossroadRadius)
		{
			subStateLeave = 0;

			//next line will be behind robot
			controllerLeftKtir.feedback = 0;
			controllerRightKtir.feedback = 2;
			changeState(STATE_GO, REASON_LEAVING_CAN_FINISHED);
		}
	}


}
