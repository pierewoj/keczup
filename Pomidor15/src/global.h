/*
 * List of all global variables used in this program
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include<stdbool.h>
#include<geometry.h>
#include "lowlevel/sensors.h"
#include "lowlevel/communication.h"
#define time ((((RTC->CNTH) << 16) + RTC->CNTL) * 16)               //time - current time value [us]

//-----------------State variables------------------//

/* important variables which denote state of FSM.
 * these variables shouldn't be aceccssed manually, changing states should be
 * done using changeState(int,int) defined in "state.h"
 * Used unit of time is MICROSECONDS
 */
int state, prevState, reasonChangeState;
void (*driveFunction)(void); //pointer to a function which controls motors
//unsigned long time; //time since reset in microseconds
unsigned long loopWaitTime; //time to wait between main loop iteratinos
unsigned long lastLoopTime; //time of last loop execution in microseconds
bool carryingCan; //does the robot "have" the can

//-----------------Location variables------------------//
PointMM position; //in mm
double direction; //[-180,180], 90 is NORTH
unsigned long enemyTimes[5][5]; //enemy detection times in ms
unsigned long visitTimes[5][5]; //visit times of crossroads in ms

//-----------------Controllers-------------------//

//struct def for PID controllers
typedef struct controllerState
{
	double target, feedback, kp, ti, td, integralMax, integral, output;

	double integralSignal, propSignal, diffSignal;

	double history[2];

	bool enabledP, enabledI, enabledD;

	int diffInteral;
	unsigned long lastTimeDiff;
} ControllerState;

ControllerState controllerForward, controllerBackward, controllerLeftKtir,
		controllerRightKtir, controllerRightWheelSpeed,
		controllerLeftWheelSpeed;

//-----------------Sensors-------------------//
/* Values are updated either during interrupts or by using
 * readSensors() function defined in "sensors.h"
 */

/* KTIR readings. KTIRs are numerated clockwise looking at the top of the robot.
 * For example ktirFront[0] is the "most left" front KTIR, while ktirBack[0]
 * is the "most right" back KTIR. True is BLACK, false means WHITE.
 */
bool ktirFront[7], ktirRight[3], ktirBack[7], ktirLeft[3];

/* approx cm to the object. 1000 if no object detected or invalid input. */
int sharp;

/* approx cm to the enemy. Clockwise numeration. ultra[0] is front.
 * 1000 is no enemy detected or reading is not valid */
int ultra[4];

/* encoder redings from the left and right wheel*/
double velocityLeft, velocityRight; // mm/s
double totalDistanceLeft, totalDistanceRight; //mm since the restart

/* current direction from gyro. Its value is an angle [-180 .. 180) deg.
 90 is north, 0 is east, -90 is south and +-180 is west. */
double gyroDirection;

/*
 * true only if start button is pressed
 */
bool buttonStart;

/*
 * current voltage of battery in volts
 */
double battery;

//-----------------Functions------------------//

/*
 * sets the default values of all variables, function called at the
 * beginning of the program
 */
void initializeGlobalVariables(void);

#endif /* GLOBAL_H_ */
