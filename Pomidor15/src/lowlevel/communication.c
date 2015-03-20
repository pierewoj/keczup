/*
 * communication.c
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

#include "global.h"
#include <stdio.h>
#include <string.h>

/*
 * processes a message received by UART. Messages should end with '\n'.
 */
void messageProcessor(char* msg, int msgLength)
{
	char command[20]; //maximum command length is 20
	sscanf(msg,"%20s",&command);
	if(!strcmp(command, "START")) //received "START" command
	{
		//do something
	}
}

/*
 * sends message via UART
 */
void sendMessage(char* msg, int msgLength)
{

}

/*
 * sends a message via UART, max msg length is 1000
 * message has to end with newline character ('\n')
 */
void sendMessageNewline(char* msg)
{
	int i=0;
	for(i=0;i<1000;i++)
	{
		if( msg[i] == '\n')
		{
			sendMessage(msg, i+1);
			return;
		}
	}
}

//sends values of state variables via UART
void sendAllVariables(void)
{
	char messageBuffer[400];
	snprintf(messageBuffer, 400, "%d\n", state);

	sendMessageNewline(messageBuffer);
}
