/*
 * Stop engines and wait till button is pressed
 */
#include "../pid.h"
#include "state.h"
#include "../global.h"

void stateStop(void)
{
	setDriveStopSlow();
	if(buttonStart)
		changeState(STATE_MANUAL, REASON_BUTTON_PRESSED);
}
