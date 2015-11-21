/*
 * Stop engines and wait till button is pressed
 */
#include "../pid.h"
#include "state.h"
#include "../global.h"

void stateStop(void)
{
	setDriveStopFast();
	if(buttonStart)
		changeState(STATE_GO, REASON_BUTTON_PRESSED);
}
