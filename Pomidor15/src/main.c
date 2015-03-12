/* Steering program of "Pomidor 1.5" mobile robot
 * Authors: Jakub Pierewoj, Piotr Poskart
 * KNR PW
 * March 2015
 */

#include <stddef.h>
#include "stm32f10x.h"
#include "global.h"
#include "stateStop.h"
#include "stateGo.h"
#include "stateRotate.h"
#include "stateTakeCan.h"
#include "stateLeaveCan.h"
#include "stateManual.h"

int main(void)
{
	//Configure peripherals
	// (...)

	//Initialize global variables
	state = STATE_INIT;

	//Endless loop
	while(1)
	{
		//Update sensor readings
		// (...)

		//Finite state machine
		switch(state)
		{
			//Set default values to all global variables
			case STATE_INIT:
				initializeGlobalVariables();
				state = STATE_STOP;
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
		driveFunction();
	}
}
