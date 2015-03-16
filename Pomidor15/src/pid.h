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
void updateFeedbackKTir(struct controllerState *s, bool *ktir, int numKtir);

#endif /* PID_H_ */
