#include "global.h"
#include "lowlevel/sensors.h"
#include "states/state.h"
#include "settings.h"
#include "location.h"
#include "strategy.h"

/* important variables which denote state of FSM.
 * these variables shouldn't be aceccssed manually, changing states should be
 * done using changeState(int,int) defined in "state.h"
 * Used unit of time is MICROSECONDS
 */

unsigned int getMiliseconds(void);
unsigned int getMicroseconds(void);
unsigned int time__;

int state, prevState, reasonChangeState;
void (*driveFunction)(void); //pointer to a function which controls motors
unsigned long loopWaitTime; //time to wait between main loop iteratinos
unsigned long lastLoopTime; //time of last loop execution in microseconds
bool carryingCan; //does the robot "have" the can

/*
 * end game tactics
 */
bool endGameTacticsEnabled = false;

/*
 * Location variables
 * They are updated in location.c. They are not sensor readings!
 * These values are updated based on gyroDyrection and encoderSpeeds
 */
PointMM position; //in mm
double direction; //[-180,180], 90 is NORTH.
unsigned long enemyTimes[5][5]; //enemy detection times in ms
unsigned long visitTimes[5][5]; //visit times of crossroads in ms

/*
 * Controller states, represent controller settings and outputs
 */
ControllerState controllerForward, controllerBackward, controllerLeftKtir,
		controllerRightKtir, controllerRightWheelSpeed,
		controllerLeftWheelSpeed;

/*
 * KTIR readings. KTIRs are numerated clockwise looking at the top of the robot.
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

/*
 * current direction from gyro. Its value is an angle [-180 .. 180) deg.
 * 90 is north, 0 is east, -90 is south and +-180 is west.
 * updated in sensors.c, DO NOT CHANGE IT MANUALLY!! Changes in this value
 * are used to update "direction" variable
 */
double gyroDirection;

/*
 * true only if start button is pressed
 */
bool buttonStart;

/*
 * current voltage of battery in volts
 */
double battery;

/*
 * is frame clossed? readonly
 */
bool frameClosed;

/*
 * current pwm steering signals, readonly
 */
double pwmLeft, pwmRight;

//-----------------Functions------------------//

/*
 * sets the default values of all variables, function called at the
 * beginning of the program
 */
void initializeGlobalVariables(void)
{

	loopWaitTime = 5000; 			//time to wait between main loop iteratinos
	lastLoopTime = 0; 		 	//time of last loop execution in microseconds
	carryingCan = 0;
	state = STATE_INIT;
	prevState = STATE_INIT;
	reasonChangeState = REASON_PROGRAM_RESET;
	position.x = 600;
	position.y = -settingDistanceMidBeginning;
	direction = 90;
	gyroDirection = 90;
	sharp = 1000;

	int i, j;
	for (i = 0; i < 4; i++)
		ultra[i] = 1000;

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
	controllerForward.enabledD = 1;
	controllerForward.kp = 0.5;
	controllerForward.ti = 10;
	controllerForward.td = 2;
	controllerForward.target = 3.5;
	controllerForward.lastTimeDiff = getMicroseconds();
	controllerForward.integralMax = 3;
	controllerForward.diffInterval = 1;
	controllerForward.feedback = 3;

	controllerBackward.enabledP = 1;
	controllerBackward.enabledI = 0;
	controllerBackward.enabledD = 1;
	controllerBackward.kp = 0.7;
	controllerBackward.ti = 10;
	controllerBackward.td =5;
	controllerBackward.target = 3.5;
	controllerBackward.lastTimeDiff = getMicroseconds();
	controllerBackward.integralMax = 3;
	controllerBackward.diffInterval = 1;

	controllerLeftKtir.enabledP = 1;
	controllerLeftKtir.enabledI = 0;
	controllerLeftKtir.enabledD = 0;
	controllerLeftKtir.kp = 0.2;
	controllerLeftKtir.ti = 10;
	controllerLeftKtir.td = 2;
	controllerLeftKtir.target = 1;
	controllerLeftKtir.lastTimeDiff = getMicroseconds();
	controllerLeftKtir.integralMax = 3;
	controllerLeftKtir.diffInterval = 1;

	controllerRightKtir.enabledP = 1;
	controllerRightKtir.enabledI = 0;
	controllerRightKtir.enabledD = 0;
	controllerRightKtir.kp = 0.2;
	controllerRightKtir.ti = 10;
	controllerRightKtir.td = 2;
	controllerRightKtir.target = 1;
	controllerRightKtir.lastTimeDiff = getMicroseconds();
	controllerRightKtir.integralMax = 3;
	controllerRightKtir.diffInterval = 1;

	controllerLeftWheelSpeed.enabledP = 0;
	controllerLeftWheelSpeed.enabledI = 1;
	controllerLeftWheelSpeed.enabledD = 0;
	controllerLeftWheelSpeed.kp = 0.0015000;
	controllerLeftWheelSpeed.ti = 50;
	controllerLeftWheelSpeed.td = 0.01;
	controllerLeftWheelSpeed.target = 300;
	controllerLeftWheelSpeed.lastTimeDiff = getMicroseconds();
	controllerLeftWheelSpeed.integralMax = 500;
	controllerLeftWheelSpeed.diffInterval = 10;

	controllerRightWheelSpeed.enabledP = 0;
	controllerRightWheelSpeed.enabledI = 1;
	controllerRightWheelSpeed.enabledD = 0;
	controllerRightWheelSpeed.kp = 0.0015000;
	controllerRightWheelSpeed.ti = 50;
	controllerRightWheelSpeed.td =  0.01;
	controllerRightWheelSpeed.target = 300;
	controllerRightWheelSpeed.lastTimeDiff = getMicroseconds();
	controllerRightWheelSpeed.integralMax = 500;
	controllerRightWheelSpeed.diffInterval = 10;

	openFrame();
}

/*
 * returns current time since uC restart in microseconds (us)
 */
unsigned int getMicroseconds(void)
{
	return ((((RTC->CNTH) << 16) + RTC->CNTL) * 16); //time - current time value [us]
}

/*
 * returns current time since uC restart in miliseconds (ms)
 */
unsigned int getMiliseconds(void)
{
	return getMicroseconds() / 1000;
}
