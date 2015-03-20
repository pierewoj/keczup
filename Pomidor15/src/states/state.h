/*
 * state.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

#ifndef STATE_H_
#define STATE_H_

#define STATE_INIT (-1)
#define STATE_STOP 0
#define STATE_GO 1
#define STATE_ROTATE 2
#define STATE_TAKE_CAN 3
#define STATE_LEAVE_CAN 4
#define STATE_MANUAL 5

#define REASON_PROGRAM_RESET 0
#define REASON_VARIABLES_INITIALIZED 1

void changeState(int newState, int reason);

#endif /* STATE_H_ */