/*
 * pid.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

#ifndef PID_H_
#define PID_H_

/*
 * counts the value of steering signal (output) of the PID controller
 * based on its state (kp,ti,td,feedback...).
 */
void PID(struct controllerState* s);

/*
 * updates the value of feedback in controller s with an average position of
 * line under ktir[]. value is not updated if ktir[i]=false for all i.
 */
void updateFeedbackKtir(struct controllerState *s, bool *ktir, int numKtir);

void countControllers(void);

void motorPIDForward(void);

void setPIDForward(double vmax);

void motorPIDBackward(void);

void setPIDBackward(double vmax);

void motorWheelPWM(void);

void setWheelPWM(double pwmLeft, double pwmRight);

void motorWheelVelocity(void);

void setWheelVelocity(double velLeft, double velRight);

#endif /* PID_H_ */
