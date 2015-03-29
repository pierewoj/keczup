/*
 * commPC.h
 *
 *  Created on: Mar 24, 2015
 *      Author: Kuba
 */

#ifndef COMMPC_H_
#define COMMPC_H_

/*
 * pushes new message to the queue of messages (to be sent by UART)
 * it automatically appends "\r\n" to the msg
 * IMPORTANT: if queue is full this function will do nothing
 */
void sendMessage(char* msg);

/*
 * function called by lowlevel/communication.c
 * processes a message received from PC
 */
void messageReceived(char* msg, int msgLength);

/*
 * this function needs to be called to send current state to PC, should be called
 * every iteration of the main loop
 */
void sendToPC(void);

#endif /* COMMPC_H_ */
