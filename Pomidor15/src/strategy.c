/*
 * strategy.c
 *
 *  Created on: Mar 17, 2015
 *      Author: Kuba
 */

#define STACK_MAX_SIZE 100
#define STACK_LOOP_SIZE 8
#include "strategy.h"

/*
 * This structure is a stack + list with cycle (loop)
 * It allows to place priority targets on the stack, if there are no priotiy
 * targets (stack size==0) an element from loop is returned
 */
typedef struct strategy
{
	struct point stack[STACK_MAX_SIZE];
	int size; //index of a first free element

	struct point loop[STACK_LOOP_SIZE];
	int loopIndex; //index of a next loop element
} Strategy;

/*
 * Used strategy
 */
Strategy robotStrategy;

/*
 * adds new element to the stack
 * if stack is full, it does nothing
 */
void stackPush(Strategy* stack, Point val)
{
	if (stack->size < STACK_MAX_SIZE)
	{
		(stack->stack)[stack->size] = val;
		stack->size++;
	}
}

/*
 * if stack is not empty, it removes top elem. on stack
 * if stack is empty it moves loop index to next element
 */
void stackPop(Strategy* stack)
{
	if (stack->size > 0)
	{
		stack->size--;
	}
	else
	{
		stack->loopIndex++;
		stack->loopIndex %= STACK_LOOP_SIZE;
	}
}

/*
 * if stack is not empty, top stack elem is returned
 * if stack is empty elem from loop is returned
 */
Point stackPeek(Strategy* stack)
{
	if (stack->size > 0)
		return (stack->stack)[stack->size - 1];
	else
		return (stack->loop)[stack->loopIndex];
}

/*
 * adds new elem on top of the stack
 * argument is int,int instead of Point because in C you can't do sth like this:
 * 		addNewTarget({1,3});   //not allowed in C, allowed in C++
 *
 * if there was Point argument you'd have to do something like this:
 * 		Point t = {1,3};
 * 		addNewTarget(t);
 * 	and this is bad
 */
void addNewTarget(int i, int j)
{
	Point t;
	t.i = i;
	t.j = j;
	stackPush(&robotStrategy, t);
}

Point getRecentTarget(void)
{
	return stackPeek(&robotStrategy);
}

void removeRecentTarget(void)
{
	stackPop(&robotStrategy);
}

/*
 * initializes robotStrategy
 * this function needs to be called at the beginning of the program!
 */
void strategyInit(void)
{
	robotStrategy.size = 0;
	robotStrategy.loopIndex = 0;

	//initializing loop
	struct point t[STACK_LOOP_SIZE] =
	{
	{ 4, 1 },
	{ 4, 2 },
	{ 0, 2 },
	{ 0, 3 },
	{ 4, 3 },
	{ 4, 4 },
	{ 0, 4 },
	{ 0, 1 } };
	int i;
	for (i = 0; i < STACK_LOOP_SIZE; i++)
		robotStrategy.loop[i] = t[i];

	addNewTarget(1, 2);
	addNewTarget(3, 2);
}
