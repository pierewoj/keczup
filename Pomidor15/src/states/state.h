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
#define REASON_BIG_ANGLE_TO_NEXT_CROSSROAD 2
#define REASON_ARRIVED_AT_BASELINE 3
#define REASON_CAN_DETECTED_SHARP 4
#define REASON_BUTTON_PRESSED 5
#define REASON_BLUETOOTH_COMMAND 6
#define REASON_ROTATION_FINISHED 7
#define REASON_CAN_TAKEN 8
#define REASON_BASELINE_REACHED 9
#define REASON_LEAVING_CAN_FINISHED 10
#define REASON_ALL_CANS_COLLECTED 11

void changeState(int newState, int reason);

#endif /* STATE_H_ */
