/*
 * Manual robot control (for example by using Bluetooth)
 */
#include "../pid.h"
#include "state.h"
#include "../global.h"

void stateManual(void)
{
	setDriveWheelPWM(0.3,0.3);
}

