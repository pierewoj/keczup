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
	position.y = - settingDistanceMidBeginning;
	direction = 90;

	int i,j;
	for(i=0;i<5;i++)
		for(j=0;j<5;j++)
		{
			enemyTimes[i][j] = -60*1000;
			visitTimes[i][j] = -60*1000;
		}

	nextCrossroad.i = 2;
	nextCrossroad.j = 1;
	strategyInit();

	controllerForward.diffInteral  = 1;
	controllerForward.enabledP = 1;
	controllerForward.kp = 1;
	controllerForward.target = 3.5;

	controllerLeftWheelSpeed.diffInteral = 1;
	controllerLeftWheelSpeed.enabledP = 1;
	controllerLeftWheelSpeed.kp = 0.001;

	controllerRightWheelSpeed.diffInteral = 1;
	controllerRightWheelSpeed.enabledP = 1;
	controllerRightWheelSpeed.kp = 0.001;

}

unsigned int getMicroseconds(void)
{
	return ((((RTC->CNTH) << 16) + RTC->CNTL) * 16); //time - current time value [us]
}

unsigned int getMiliseconds(void)
{
	return getMicroseconds() / 1000;
}
