#include "global.h"
#include "lowlevel/sensors.h"

/*
 * sets the default values of all variables, function called at the
 * beginning of the program
 */
void initializeGlobalVariables(void)
{
	usart_data_number = 0;
	state = 0;
	gyro_initial_values[0] = 0;
	gyro_initial_values[1] = 0;
	gyro_initial_values[2] = 0;
}

