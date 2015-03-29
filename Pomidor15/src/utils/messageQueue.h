/*
 * messageQueue.h
 *
 *  Created on: 29 mar 2015
 *      Author: Kuba
 */

#ifndef UTILS_MESSAGEQUEUE_H_
#define UTILS_MESSAGEQUEUE_H_

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
