#include "global.h"
#include "lowlevel/sensors.h"

//jakies tam komentarze dupa dupa dupa...
/*
 * sets the default values of all variables, function called at the
 * beginning of the program
 */


void initializeGlobalVariables(void)
{
	time_old = 0;
	loopWaitTime = 1000; 					//time to wait between main loop iteratinos
	lastLoopTime = 0; 		 			//time of last loop execution in microseconds
	bool carryingCan = 0;
	usart_data_number = 0;
	state = 0;
	gyro_initial_values[0] = 0;
	gyro_initial_values[1] = 0;
	gyro_initial_values[2] = 0;
}

unsigned int getMicroseconds(void)
{
	return ((((RTC->CNTH) << 16) + RTC->CNTL) * 16);               //time - current time value [us]
}

unsigned int getMiliseconds(void)
{
	return getMicroseconds() / 1000;
}
