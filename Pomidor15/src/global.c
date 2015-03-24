#include "global.h"
#include "lowlevel/sensors.h"
#include "states/state.h"
#include "settings.h"
#include "location.h"
#include "strategy.h"

//jakies tam komentarze dupa dupa dupa...
/*
 * sets the default values of all variables, function called at the
 * beginning of the program
 */

void initializeGlobalVariables(void)
{

	loopWaitTime = 1000; 			//time to wait between main loop iteratinos
	lastLoopTime = 0; 		 	//time of last loop execution in microseconds
	carryingCan = 0;
	state = STATE_INIT;
	prevState = STATE_INIT;
	reasonChangeState = REASON_PROGRAM_RESET;
	position.x = 600;
	position.y = -settingDistanceMidBeginning;
	direction = 90;

	int i, j;
	for (i = 0; i < 5; i++)
		for (j = 0; j < 5; j++)
		{
			enemyTimes[i][j] = -60 * 1000;
			visitTimes[i][j] = -60 * 1000;
		}

	nextCrossroad.i = 2;
	nextCrossroad.j = 1;
	strategyInit();

	/*
	 * Controller default settings!!
	 */
	controllerForward.enabledP = 1;
	controllerForward.enabledI = 0;
	controllerForward.enabledD = 0;
	controllerForward.kp = 1;
	controllerForward.ti = 10;
	controllerForward.td = 2;
	controllerForward.target = 3.5;
	controllerForward.lastTimeDiff = getMicroseconds();
	controllerForward.integralMax = 3;
	controllerForward.diffInterval = 1;

	controllerBackward.enabledP = 1;
	controllerBackward.enabledI = 0;
	controllerBackward.enabledD = 0;
	controllerBackward.kp = 1;
	controllerBackward.ti = 10;
	controllerBackward.td = 2;
	controllerBackward.target = 3.5;
	controllerBackward.lastTimeDiff = getMicroseconds();
	controllerBackward.integralMax = 3;
	controllerBackward.diffInterval = 1;

	controllerLeftKtir.enabledP = 1;
	controllerLeftKtir.enabledI = 0;
	controllerLeftKtir.enabledD = 0;
	controllerLeftKtir.kp = 1;
	controllerLeftKtir.ti = 10;
	controllerLeftKtir.td = 2;
	controllerLeftKtir.target = 1;
	controllerLeftKtir.lastTimeDiff = getMicroseconds();
	controllerLeftKtir.integralMax = 3;
	controllerLeftKtir.diffInterval = 1;

	controllerRightKtir.enabledP = 1;
	controllerRightKtir.enabledI = 0;
	controllerRightKtir.enabledD = 0;
	controllerRightKtir.kp = 1;
	controllerRightKtir.ti = 10;
	controllerRightKtir.td = 2;
	controllerRightKtir.target = 1;
	controllerRightKtir.lastTimeDiff = getMicroseconds();
	controllerRightKtir.integralMax = 3;
	controllerRightKtir.diffInterval = 1;

	controllerLeftWheelSpeed.enabledP = 1;
	controllerLeftWheelSpeed.enabledI = 0;
	controllerLeftWheelSpeed.enabledD = 0;
	controllerLeftWheelSpeed.kp = 0.0001;
	controllerLeftWheelSpeed.ti = 10000;
	controllerLeftWheelSpeed.td = 2;
	controllerLeftWheelSpeed.target = 300;
	controllerLeftWheelSpeed.lastTimeDiff = getMicroseconds();
	controllerLeftWheelSpeed.integralMax = 3;
	controllerLeftWheelSpeed.diffInterval = 1;

	controllerRightWheelSpeed.enabledP = 1;
	controllerRightWheelSpeed.enabledI = 0;
	controllerRightWheelSpeed.enabledD = 0;
	controllerRightWheelSpeed.kp = 0.0001;
	controllerRightWheelSpeed.ti = 10000;
	controllerRightWheelSpeed.td = 2;
	controllerRightWheelSpeed.target = 300;
	controllerRightWheelSpeed.lastTimeDiff = getMicroseconds();
	controllerRightWheelSpeed.integralMax = 3;
	controllerRightWheelSpeed.diffInterval = 1;

}

unsigned int getMicroseconds(void)
{
	return ((((RTC->CNTH) << 16) + RTC->CNTL) * 16); //time - current time value [us]
}

unsigned int getMiliseconds(void)
{
	return getMicroseconds() / 1000;
}
