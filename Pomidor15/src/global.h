/*
 * List of all global variables used in this program
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define STATE_INIT (-1)
#define STATE_STOP 0
#define STATE_GO 1
#define STATE_ROTATE 2
#define STATE_TAKE_CAN 3
#define STATE_LEAVE_CAN 4
#define STATE_MANUAL 5

#include<stdbool.h>

//struct def for PID controllers
struct controllerState
{
	double target,
		feedback,
		lastError,
		kp,
		ti,
		td,
		integralMax,
		integral,
		output;

	double integralSignal,
		propSignal,
		diffSignal;

	double history[2];

	bool enabledP,
		enabledI,
		enabledD;

	int diffInteral;
	unsigned long lastTimeDiff;
};

int state; //important variable which denotes state of FSM
void (*driveFunction)(void); //pointer to a function which controls motors
unsigned long time; //time since reset in microseconds
unsigned int loopWaitTime; //time in microseconds to wait between main loop iteratinos
unsigned long lastLoopTime; //time of last loop execution in microseconds

//controllers
struct controllerState
	controllerForward,
	controllerBackward,
	controllerRightWheelSpeed,
	controllerLeftWheelSpeed;

void initializeGlobalVariables(void);

#endif /* GLOBAL_H_ */
