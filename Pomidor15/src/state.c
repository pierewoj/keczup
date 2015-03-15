/*
 * state.c
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */
#include "global.h"


//Changing states MUST be only changed using this function
void changeState(int newState, int reason)
{
	prevState = state;
	state = newState;
	reasonChangeState = reason;
}
