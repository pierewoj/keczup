/* Steering program of "Pomidor 1.5" mobile robot
 * Authors: Jakub Pierewoj, Piotr Poskart
 * KNR PW
 * March 2015
 */

#include <stddef.h>
#include <string.h>
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
#include "states/stateGoPrevious.h"
#include "states/stateFail.h"
#include "stdio.h"
#include "commPC.h"
#include "utils/messageQueue.h"


extern bool programMode;

int main(void)
{
	//Configuration of all peripherials (GPIO, UART, I2C, TIMERS etc)
  	configurePeripherials();

	//Initialize global variables
	changeState( STATE_INIT, REASON_PROGRAM_RESET);

	if(!(GPIOA->IDR & GPIO_Pin_7))
	{
		programMode = true;
		TIM15->CR1 &= !(TIM_CR1_CEN);
	}

	//Endless loop
	while (1)
	{

		if(position.x < -100 || position.x > 1300 || position.y < -100 || position.y > 1300)
			changeState(STATE_FAIL, REASON_OUT_OF_BOARD);

		//wait loopWaitTime between iterations
		while (getMicroseconds() - lastLoopTime < loopWaitTime)
		{
			//trySendRemainingMessages();
		}
		lastLoopTime = getMicroseconds();

		//try to send any remaining messages
		trySendRemainingMessages();

		//Update sensor readings and controller output
		readSensors();
		countControllers();

		//Update position of Pomidor and enemy
		updateOurPosition();
		updateEnemyPosition();

		sendToPC();

		//Finite state machine
		switch (state)
		{
		//Set default values to all global variables
		case STATE_INIT:
			initializeGlobalVariables();
			changeState( STATE_STOP, REASON_VARIABLES_INITIALIZED);
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

		case STATE_GO_PREVIOUS :
			stateGoPrevious();
			break;

		case STATE_FAIL:
			stateFail();
			break;

		default:
			break;

		}

		//Motor control
		if (driveFunction != NULL)
			driveFunction();

	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	/* Infinite loop */
	/* Use GDB to find out why we're here */
//while (1);
}
#endif
