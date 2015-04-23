/*
 * effectors.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

#include "communication.h"
#include <math.h>

#ifndef EFFECTORS_H_
#define EFFECTORS_H_

/*
 * time in ms of last change of frame state
 */
extern unsigned int lastTimeFrameChanged;

//unlocks the can
void openFrame(void);

//locks the can
void closeFrame(void);

//set PWM values controlling engines. Values [-1.0 .. +1.0]. + is forward.
void setRightPWM(double v);
void setLeftPWM(double v);

//Motor stop functions using low/high impedance
void stopFast(void);
void stopSlow(void);

#endif /* EFFECTORS_H_ */
