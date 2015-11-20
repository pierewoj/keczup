/*
 * Stop engines and wait till button is pressed
 */
#include "../pid.h"
#include "state.h"
#include "../global.h"

extern bool programMode;

void stateStop(void)
{
	setDriveStopFast();
	if(buttonStart)
		changeState(STATE_GO, REASON_BUTTON_PRESSED);
}
