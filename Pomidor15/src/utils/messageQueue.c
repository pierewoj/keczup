/*
 * queue.c
 *
 *  Created on: 29 mar 2015
 *      Author: Kuba
 */
#include <string.h>
#include "messageQueue.h"

char queue[NUM_MESSAGES][MAX_MSGLEN];
char safeArray[MAX_MSGLEN];
int volatile nextPopped = 0;
int nextPushed = 0;
int volatile messageCount = 0;

char* messageQueuePeek(void)
{
	return queue[nextPopped];
}

void messageQueuePop(void)
{
	if (messageCount > 0)
	{
		messageCount--;
		nextPopped++;
		nextPopped %= NUM_MESSAGES;
	}
}

void messageQueuePush(char* msg)
{
	if (strlen(msg) < MAX_MSGLEN && messageCount < NUM_MESSAGES)
	{
		strcpy(queue[nextPushed], msg);

		messageCount++;

		nextPushed++;
		nextPushed %= NUM_MESSAGES;
	}
}

int messageQueueSize(void)
{
	return messageCount;
}
