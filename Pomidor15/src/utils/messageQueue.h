/*
 * messageQueue.h
 *
 *  Created on: 29 mar 2015
 *      Author: Kuba
 */

#ifndef UTILS_MESSAGEQUEUE_H_
#define UTILS_MESSAGEQUEUE_H_

#define NUM_MESSAGES 4
#define MAX_MSGLEN 256

// safe buffer for DMA to get message from
extern char safeArray[MAX_MSGLEN];

/*
 * returns current message to be sent
 */
char* messageQueuePeek(void);

/*
 * deletes current message,
 * should be called after completely sending one
 */
void messageQueuePop(void);

/*
 * adds new message to the queue
 */
void messageQueuePush(char* msg);

/*
 * returns ammount of messages in the queue
 */
int messageQueueSize(void);

#endif /* UTILS_MESSAGEQUEUE_H_ */
