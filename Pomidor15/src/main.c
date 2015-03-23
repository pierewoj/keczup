/* Steering program of "Pomidor 1.5" mobile robot
 * Authors: Jakub Pierewoj, Piotr Poskart
 * KNR PW
 * March 2015
 */

#include <stddef.h>
#include "stm32f10x.h"
#include "global.h"
#include "lowlevel/config.h"
#include "lowlevel/sensors.h"
#include "lowlevel/effectors.h"
#include "lowlevel/communication.h"
#include "pid.h"
#include "location.h"
#include "strategy.h"
#include "states/state.h"
#include "states/stateStop.h"
#include "states/stateGo.h"
#include "states/stateRotate.h"
#include "states/stateTakeCan.h"
#include "states/stateLeaveCan.h"
#include "states/stateManual.h"
#include "stdio.h"

int main(void)
{
	//Configuration of all peripherials (GPIO, UART, I2C, TIMERS etc)
	initializeGlobalVariables();
	configurePeripherials();

	//Initialize global variables
	changeState( STATE_INIT, REASON_PROGRAM_RESET);

	//Endless loop
	while (1)
	{
		//wait loopWaitTime between iterations
		while (getMicroseconds() - lastLoopTime < 100 * loopWaitTime)
		{
		}
		lastLoopTime = getMicroseconds();

		readSensors();
		char msg[200];
		snprintf(msg, 200, "%d %d %d \n", (int) totalDistanceLeft,
				(int) totalDistanceRight, (int) gyroDirection);
		sendMessage(msg);
		sendMessage("_______\n");

		/*
		 //Update sensor readings and controller output
		 readSensors();
		 countControllers();

		 //Update position of Pomidor and enemy
		 updateOurPosition();
		 updateEnemyPosition();

		 //Finite state machine
		 switch(state)
		 {
		 //Set default values to all global variables
		 case STATE_INIT:
		 initializeGlobalVariables();
		 changeState( STATE_STOP, REASON_VARIABLES_INITIALIZED );
		 break;

		 //Stop and wait till user presses button
		 case STATE_STOP:
		 stateStop();
		 break;

		 //Drive between crossroads
		 case STATE_GO:
		 stateGo();
		 break;

		 //Rotating
		 case STATE_ROTATE:
		 stateRotate();
		 break;

		 //Approach new can and take it
		 case STATE_TAKE_CAN:
		 stateTakeCan();
		 break;

		 //Put can on a base line
		 case STATE_LEAVE_CAN:
		 stateLeaveCan();
		 break;

		 //Steer robot manually using Bluetooth
		 case STATE_MANUAL:
		 stateManual();
		 break;

		 default:
		 break;

		 }

		 //Motor control

		 if(driveFunction != NULL)
		 driveFunction();
		 */

	}
}
