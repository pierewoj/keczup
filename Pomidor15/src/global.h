/*
 * List of all global variables used in this program
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include<stdbool.h>

//-----------------State variables------------------//

/* important variables which denote state of FSM.
 * these variables shouldn't be aceccssed manually, changing states should be
 * done using changeState(int,int) defined in "state.h"
 * Used unit of time is MICROSECONDS
 */
int state, prevState, reasonChangeState;
void (*driveFunction)(void); //pointer to a function which controls motors
unsigned long time; //time since reset in microseconds
unsigned long loopWaitTime; //time in micros to wait between main loop iteratinos
unsigned long lastLoopTime; //time of last loop execution in microseconds

//-----------------Controllers-------------------//

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

struct controllerState
	controllerForward,
	controllerBackward,
	controllerRightWheelSpeed,
	controllerLeftWheelSpeed;

//-----------------Sensors-------------------//
/* Values are updated either during interrupts or by using
 * readSensors() function defined in "sensors.h"
 */

/* KTIR readings. KTIRs are numerated clockwise looking at the top of the robot.
 * For example ktirFront[0] is the "most left" front KTIR, while ktirBack[0]
 * is the "most right" back KTIR. True is BLACK, false means WHITE.
 */
bool ktirFront[7],
	 ktirRigth[3],
	 ktirBack[7],
	 ktirLeft[3];

/* approx cm to the object. 1000 if no object detected. */
int sharp;

/* approx cm to the enemy. Clockwise numeration. ultra[0] is front. */
int ultra[4];

/* encoder redings from the left and right wheel*/
int velocityLeft, velocityRight; // mm/s
long totalDistanceLeft, totalDistanceRight; //mm since the restart

/* current direction from gyro. Its value is an angle [-180 .. 180) deg.
 90 is north, 0 is east, -90 is south and +-180 is west. */
double gyroDirection;


//-----------------Functions------------------//
void initializeGlobalVariables(void);

#endif /* GLOBAL_H_ */
