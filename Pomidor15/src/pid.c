/*
 * pid.c
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */
#include "global.h"
#include "effectors.h"
#include <math.h>

/*
 * counts the value of steering signal (output) of the PID controller
 * based on its state (kp,ti,td,feedback...).
 */
void PID(ControllerState* s, bool currentlyRunning)
{
	double error = s->target - s->feedback;

	//if controller is not enabled right now, don't count I
	if( !currentlyRunning  || !(s->enabledI) )
	{
		s -> integral = 0;
	}

	//update history for D every "diffInterval" iterations
	if(time - s->lastTimeDiff > s->diffInteral * loopWaitTime )
	{
		s -> history[1] = s -> history[0];
		s -> history[0] = error;
		s -> lastTimeDiff = time;
	}
	double d = (s->history[0]) - (s->history[1]);

	//count integral
	s->integral += error;
	s->integral = fmax ( s->integral ,  - s->integralMax * s->ti);
	s->integral = fmin ( s->integral ,    s->integralMax * s->ti);

	//count output from P, I and D
	s->propSignal = s->enabledP * s->kp * error;
	s->integralSignal = s->enabledI * s->kp / s->ti * s->integral ;
	s->diffSignal = s->enabledD * s->kp * s->td * d;

	//count total controller output
	s->output = s->propSignal + s->integralSignal + s->diffSignal;
}


/*
 * updates the value of feedback in controller s with an average position of
 * line under ktir[]. value is not updated if ktir[i]=false for all i.
 */
void updateFeedbackKtir(ControllerState *s, bool *ktir, int numKtir)
{
	int numBlack = 0;
	double sum = 0;
	int i;
	for(i=0; i<numKtir; i++)
		if (ktir[i])
		{
			numBlack++;
			sum += i;
		}

	if(numBlack > 0)
		s -> feedback = sum / numBlack;
}

/*
 * function used to calculate pwm signal from PID controller output
 * plot of this function is file "pid.nb"
 */
double followLineFunction(double x)
{
	if( x<=-1 )
		return 0;
	else if ( x<=0 )
		return x+1;
	else
		return 1;
}

//----------------Functions for specific drive modes-------------------

/*
 * Follow the line forward drive functions
 */
double drivePIDForwardPWMMax;

void drivePIDForward(void)
{
	double s = controllerForward.output;

	double sl=followLineFunction(s),
			sr = followLineFunction(-s);

	setLeftPWM(sl * drivePIDForwardPWMMax);
	setRightPWM(sr *drivePIDForwardPWMMax);
}

void setDrivePIDForward(double PWMMax)
{
	driveFunction = drivePIDForward;
	drivePIDForwardPWMMax = PWMMax;
	controllerForward.target = 3;
}

/*
 * Follow the line backward drive functions
 */
double drivePIDBackwardPWMMax;

void drivePIDBackward(void)
{
	double s = controllerBackward.output;

	double sl=followLineFunction(s),
			sr = followLineFunction(-s);

	setLeftPWM(-sl * drivePIDBackwardPWMMax);
	setRightPWM(-sr * drivePIDBackwardPWMMax);
}

void setDrivePIDBackward(double PWMMax)
{
	driveFunction = drivePIDBackward;
	drivePIDBackwardPWMMax = PWMMax;
	controllerBackward.target = 3;
}

/*
 * Wheel velocity controller drive functions
 */
void driveWheelVelocity(void)
{
	setLeftPWM( controllerLeftWheelSpeed.output );
	setRightPWM( controllerRightWheelSpeed.output );
}

void setDriveWheelVelocity(double velLeft, double velRight)
{
	driveFunction = driveWheelVelocity;
	controllerLeftWheelSpeed.target = velLeft;
	controllerRightWheelSpeed.target = velRight;
}

/*
 * Line under middle left/right ktir controller drive functions
 */
void driveSideKtir(void)
{
	setLeftPWM( controllerLeftWheelSpeed.output );
	setRightPWM( controllerRightWheelSpeed.output );
}

void setDriveSideKtir(void)
{
	driveFunction = driveSideKtir;
	controllerLeftKtir.target = 1;
	controllerRightKtir.target = 1;
}

/*
 * Constant PWM on motors drive function
 */
double driveWheelPWMLeft, driveWheelPWMRight;

void driveWheelPWM(void)
{
	setLeftPWM(driveWheelPWMLeft);
	setRightPWM(driveWheelPWMRight);
}

void setDriveWheelPWM(double pwmLeft, double pwmRight)
{
	driveFunction = driveWheelPWM;
	driveWheelPWMLeft = pwmLeft;
	driveWheelPWMRight = pwmRight;
}

/*
 * Fast stop motor drive functions
 */
void driveFastStop(void)
{
	stopFast();
}

void setDriveFastStop(void)
{
	driveFunction = driveFastStop;
}

/*
 * Slow stop motor drive functions
 */

void driveSlowStop(void)
{
	stopSlow();
}

void setDriveSlowStop(void)
{
	driveFunction = driveSlowStop;
}

/*
 * update feedback and recalculate output in all controllers
 */
void countControllers(void)
{
	/*
	 * counting average position under KTIR lines and updating controller states
	 */
	updateFeedbackKtir(&controllerForward, ktirFront, 7);
	updateFeedbackKtir(&controllerBackward, ktirBack, 7);
	updateFeedbackKtir(&controllerRightKtir, ktirRight, 3);
	updateFeedbackKtir(&controllerLeftKtir, ktirLeft, 3);

	/*
	 * feedback for wheel speed controllers are encoder readings
	 */
	controllerRightWheelSpeed.feedback = velocityRight;
	controllerLeftWheelSpeed.feedback = velocityLeft;


	/*
	 * updating PID controller outputs
	 */
	PID(&controllerForward, driveFunction == drivePIDForward);
	PID(&controllerBackward, driveFunction == drivePIDBackward);
	PID(&controllerRightKtir, driveFunction == driveSideKtir);
	PID(&controllerLeftKtir, driveFunction == driveSideKtir);
	PID(&controllerRightWheelSpeed, driveFunction == driveWheelVelocity);
	PID(&controllerLeftWheelSpeed, driveFunction == driveWheelVelocity);
}