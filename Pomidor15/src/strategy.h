/*
 * strategy.h
 *
 *  Created on: Mar 17, 2015
 *      Author: Kuba
 */

#ifndef STRATEGY_H_
#define STRATEGY_H_

typedef struct point
{
	int i;
	int j;
} Point;

/*
 * adds new priority target
 */
void addNewTarget(int i, int j);

/*
 * returns current target
 */
Point getRecentTarget(void);

/*
 * removes recent target
 */
void removeRecentTarget(void);

/*
 * strategy initialization, this function should be called at the beginning
 * of the program
 */
void strategyInit();


#endif /* STRATEGY_H_ */
