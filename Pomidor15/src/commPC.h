/*
 * commPC.h
 *
 *  Created on: Mar 24, 2015
 *      Author: Kuba
 */

#ifndef COMMPC_H_
#define COMMPC_H_

/*
 * This function checks if DMA is off (ready to transfer more messages) and there
 * are some messages left to be sent (in messageQueue). If those conditions
 * are met, new message is written to be sent by DMA via UART
 *
 * This function can be called as frequently as you want, preferably during wait
 * time in main loop
 *
 */
void trySendRemainingMessages(void);

/*
 * pushes new message to the queue of messages (to be sent by UART)
 * it automatically appends "\r\n" to the msg
 * IMPORTANT: if queue is full this function will do nothing
 */
void sendMessage(char* msg);

/*
 * this function needs to be called to send current state to PC, should be called
 * every iteration of the main loop
 */
void sendToPC();

/*
 * helper variables for sendToPC()
 * 	last time of sending message, updated by sendToPC()
 * 	current state of message sending
 */
extern long timeLastSent ;
extern int stateSending;

/*
 * helper function, appends d (with 'decimal' decimal places) to msg
 * you need to use this function because snprintf doesnt work with doubles well
 */
void addDoubleToString(char *msg, double d, int decimal);
/*
 * returns pointer to controller based on its name
 */
ControllerState* getController(char* name);

void printControllerOutput(char* name);

void printControllerSettings(char* name);

/*
 * function called after new message arrived, decides what action to take
 */
void messageReceived(char* msg, int msgLength);

/*
 * writes enemy position to the dst
 */
void printEnemy(char* dst);

/*
 * writes an array of format '11101011' to dst depending on 'source'
 */
void printKtir(char* dst, bool* source, int num);

/*
 * this function constructs a message containing most important variables and
 * sends them via UART
 */
void sendGlobal(void);

#endif /* COMMPC_H_ */
