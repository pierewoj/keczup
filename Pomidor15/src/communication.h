/*
 * communication.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

//processes a message received by UART. Messages shuold end with '\n'
void messageProcessor(char* msg, int msgLength);

//sends a message via UART
void sendMessage(char* msg, int msgLength);


#endif /* COMMUNICATION_H_ */
