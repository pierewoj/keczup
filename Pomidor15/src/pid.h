/*
 * pid.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

#ifndef PID_H_
#define PID_H_

/*
 * File pid.c includes implementation of motor controllers.
 * To set robot drive mode you should only use functions from "user interface"
 * (that functions which names begin with "set").
 * For example to make robot follow the line forward you should call
 * 		setDrivePIDForward(0.5);
 * 	You MUST NOT assign driveFunction pointer to any function manually!!!
 */

//----------------User interface-------------------

/*
 * follow the line forward, PWMMax is the maximum PWM value [0, 1.0]
 */
void setDrivePIDForward(double PWMMax);

/*
 * follow the line backward, PWMMax is the maximum PWM value [0, 1.0]
 */
void setDrivePIDBackward(double PWMMax);

/*
 * control the velocity of wheels.
 * velLeft, velRight - desired velocities in mm/s
 */
void setDriveWheelVelocity(double velLeft, double velRight);

/*
 * Control position of the line under left/right ktirs
 */
void setDriveSideKtir(void);

/*
 * Set constant PWM values on both motors.
 * pwmLeft, pwmRight are in [-1.0, 1.0]
 */
void setDriveWheelPWM(double pwmLeft, double pwmRight);

/*
 * Fast stop
 */
void setDriveFastStop(void);

/*
 * Slow stop
 */
void setDriveSlowStop(void);


//----------------Controller functions-------------------
/*
 * counts the value of steering signal (output) of the PID controller
 * based on its state (kp,ti,td,feedback...).
 */
void PID(struct controllerState* s, bool currentlyRunning);

/*
 * updates the value of feedback in controller s with an average position of
 * line under ktir[]. value is not updated if ktir[i]=false for all i.
 */
void updateFeedbackKtir(struct controllerState *s, bool *ktir, int numKtir);

/*
 * function used to calculate pwm signal from PID controller output
 * plot of this function is file "pid.nb"
 */
double followLineFunction(double x);

/*
 * update feedback and recalculate output in all controllers
 */
void countControllers(void);


//----------------Drive functions-------------------

/*
 * Follow the line forward drive functions
 */
double drivePIDForwardPWMMax;
void drivePIDForward(void);

/*
 * Follow the line backward drive functions
 */
double drivePIDBackwardPWMMax;
void drivePIDBackward(void);

/*
 * Wheel velocity controller drive functions
 */
void driveWheelVelocity(void);

/*
 * Line under middle left/right ktir controller drive functions
 */
void driveSideKtir(void);

/*
 * Constant PWM on motors drive function
 */
double driveWheelPWMLeft, driveWheelPWMRight;
void driveWheelPWM(void);

/*
 * Fast stop motor drive functions
 */
void driveFastStop(void);

/*
 * Slow stop motor drive functions
 */
void driveSlowStop(void);

#endif /* PID_H_ */
