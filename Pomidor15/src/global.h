/*
 * List of all global variables used in this program
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define STATE_INIT (-1)
#define STATE_STOP 0
#define STATE_GO 1
#define STATE_ROTATE 2
#define STATE_TAKE_CAN 3
#define STATE_LEAVE_CAN 4
#define STATE_MANUAL 5

int state; //important variable which denotes state of FSM
void (*driveFunction)(void); //pointer to a function which controls motors

void initializeGlobalVariables(void);

#endif /* GLOBAL_H_ */
