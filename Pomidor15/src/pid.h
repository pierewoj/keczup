/*
 * pid.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

#ifndef PID_H_
#define PID_H_

/*
 * File pid.c includes implementation of motor controllers. Pointer driveFunction
 * points to one of the functions declared in pid.c. This function is executed every
 * iteration of the main loop.
 *
 * To set robot drive mode you should only use functions from "user interface"
 * (that functions which names begin with "set").
 * For example to make robot follow the line forward you should call
 * 		setDrivePIDForward(0.5);
 * 	You MUST NOT assign driveFunction pointer to any function manually!!!
 */

/*
 * follow the line forward, PWMMax is the maximum PWM value [0, 1.0]
 */
void setDrivePIDForward(double PWMMax);
void drivePIDForward(void);

/*
 * follow the line backward, PWMMax is the maximum PWM value [0, 1.0]
 */
void setDrivePIDBackward(double PWMMax);
void drivePIDBackward(void);

/*
 * control the velocity of wheels.
 * velLeft, velRight - desired velocities in mm/s
 */
void setDriveWheelVelocity(double velLeft, double velRight);
void driveWheelVelocity(void);

/*
 * Control position of the line under left/right ktirs
 */
void setDriveSideKtir(void);
void driveSideKtir(void);

/*
 * Set constant PWM values on both motors.
 * pwmLeft, pwmRight are in [-1.0, 1.0]
 */
void setDriveWheelPWM(double pwmLeft, double pwmRight);
void driveWheelPWM(void);

/*
 * Fast stop
 */
void setDriveStopFast(void);
void driveStopFast(void);

/*
 * Slow stop
 */
void setDriveStopSlow(void);
void driveStopSlow(void);

/*
 * Update controller feedback and calculate output
 */

void countControllers(void);

#endif /* PID_H_ */
