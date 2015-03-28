/*
 * List of all global variables used in this program
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include<stdbool.h>
#include "geometry.h"
#include "lowlevel/sensors.h"
#include "lowlevel/communication.h"


//-----------------State variables------------------//

/* important variables which denote state of FSM.
 * these variables shouldn't be aceccssed manually, changing states should be
 * done using changeState(int,int) defined in "state.h"
 * Used unit of time is MICROSECONDS
 */

extern unsigned int getMiliseconds(void);
extern unsigned int getMicroseconds(void);

extern int state, prevState, reasonChangeState;
extern void (*driveFunction)(void); //pointer to a function which controls motors
extern unsigned long loopWaitTime; //time to wait between main loop iteratinos
extern unsigned long lastLoopTime; //time of last loop execution in microseconds
extern bool carryingCan; //does the robot "have" the can

//-----------------Location variables------------------//
extern PointMM position; //in mm
extern double direction; //[-180,180], 90 is NORTH
extern unsigned long enemyTimes[5][5]; //enemy detection times in ms
extern unsigned long visitTimes[5][5]; //visit times of crossroads in ms

//-----------------Controllers-------------------//

//struct def for PID controllers
//struct def for PID controllers
typedef struct controllerState
{
	double target, feedback, kp, ti, td, integralMax, integral, output;

	double integralSignal, propSignal, diffSignal;

	double history[2];

	bool enabledP, enabledI, enabledD;

	int diffInterval;
	unsigned long lastTimeDiff;
} ControllerState;

extern ControllerState controllerForward, controllerBackward, controllerLeftKtir,
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
extern bool ktirFront[7], ktirRight[3], ktirBack[7], ktirLeft[3];

/* approx cm to the object. 1000 if no object detected or invalid input. */
extern int sharp;

/* approx cm to the enemy. Clockwise numeration. ultra[0] is front.
 * 1000 is no enemy detected or reading is not valid */
extern int ultra[4];

/* encoder redings from the left and right wheel*/
extern double velocityLeft, velocityRight; // mm/s
extern double totalDistanceLeft, totalDistanceRight; //mm since the restart

/* current direction from gyro. Its value is an angle [-180 .. 180) deg.
 90 is north, 0 is east, -90 is south and +-180 is west. */
extern double gyroDirection;

/*
 * true only if start button is pressed
 */
extern bool buttonStart;

/*
 * current voltage of battery in volts
 */
extern double battery;

/*
 * is frame clossed? readonly
 */
extern bool frameClosed;

/*
 * current pwm steering signals, readonly
 */
extern double pwmLeft, pwmRight;

//-----------------Functions------------------//

/*
 * sets the default values of all variables, function called at the
 * beginning of the program
 */
void initializeGlobalVariables(void);

#endif /* GLOBAL_H_ */
