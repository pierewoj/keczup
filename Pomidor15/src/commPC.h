/*
 * commPC.h
 *
 *  Created on: Mar 24, 2015
 *      Author: Kuba
 */

#ifndef COMMPC_H_
#define COMMPC_H_

/*
 * function called by lowlevel/communication.c
 * processes a message received from PC
 */
void messageReceived(char* msg, int msgLength);

/*
 * sends messages to PC
 * will not send anything if last msg was later than settingBluetoothInterval
 * main loop iterations ago
 */
void sendToPC(void);

#endif /* COMMPC_H_ */
