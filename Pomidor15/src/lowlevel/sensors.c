/*
 * sensors.c
 *
 *  Created on: Mar 22, 2015
 *      Author: Piotrek
 */
#include "sensors.h"
#include <stddef.h>
#include "stm32f10x.h"
#include "communication.h"
#include "../global.h"
#include "stm32f10x_gpio.h"
#include "effectors.h"

//ADC
volatile unsigned int pomiar_adc[2];

//IMU
unsigned volatile char dane_z_imu[6];
unsigned volatile char imu[2];
int volatile gyro_x, gyro_y, gyro_z;
int volatile gyro_initial_values[3];

void gyro_update_direction(void);

//ULTRADZWIEKI
unsigned volatile int ultra__[4], ultra_pom[4];
char volatile pierwsze_zbocze[4]; //pierwsze_zbocze[i] is set to 1 if rising edge was detected from sensor i,
unsigned int const_distance_from_the_middle_of_the_robot[4] = {7,13,7,13};
volatile unsigned int ultra_history[4]={0,0,0,0};

//it is cleared after detecting falling edge - OK state; (value 1 implicate wrong detection)

//KTIRs
void read_ktirs(void);

//Encoders
void encodersReset(void);
void encodersRead(void);

unsigned int time_old;
volatile int16_t leftCount;								//speeds
volatile int16_t rightCount;
volatile int16_t fwdCount;
volatile int16_t rotCount;
//distances
volatile int32_t leftTotal;
volatile int32_t rightTotal;
volatile int32_t fwdTotal;
volatile int32_t rotTotal;

// local variables
static volatile int16_t oldLeftEncoder;
static volatile int16_t oldRightEncoder;
static volatile int16_t leftEncoder;
static volatile int16_t rightEncoder;

//read selected sensors
void readSensors(void)
{
	sharp_update();
	battery_update();
	read_ktirs();
	//gyro_update_direction();
}

//battery voltage updating
void battery_update(void)
{
	//Voltage = 3 * 3,3V * register_value/4096
	battery = pomiar_adc[0] * 0.0025217;
}

//sharp distance updating
void sharp_update(void)
{
	if (pomiar_adc[1] > 700)
		sharp = 15068 / (pomiar_adc[1] + 1);
	else
		sharp = 1000;
}

//angular velocity updating
void gyro_update_direction(void)
{
	 __i2c_read(imu_write_address, 0x28, dane_z_imu);
	 gyro_x = (int16_t) (dane_z_imu[1] << 8 | dane_z_imu[0]);
	 gyro_y = (int16_t) (dane_z_imu[3] << 8 | dane_z_imu[2]);
	 gyro_z = (int16_t) (dane_z_imu[5] << 8 | dane_z_imu[4]);

	 //correction of the initial values
	 gyro_x -= gyro_initial_values[0];
	 gyro_y -= gyro_initial_values[1];
	 gyro_z -= gyro_initial_values[2];
}

//read line sensors
void read_ktirs()
{
	//front sensors
	ktirFront[6] = (GPIOC->IDR & GPIO_Pin_15);
	ktirFront[5] = (GPIOC->IDR & GPIO_Pin_14);
	ktirFront[4] = (GPIOA->IDR & GPIO_Pin_15);
	ktirFront[3] = (GPIOD->IDR & GPIO_Pin_2);
	ktirFront[2] = (GPIOA->IDR & GPIO_Pin_1);
	ktirFront[1] = (GPIOC->IDR & GPIO_Pin_3);
	ktirFront[0] = (GPIOC->IDR & GPIO_Pin_2);

	//left sensors
	ktirLeft[0] = (GPIOB->IDR & GPIO_Pin_12);
	ktirLeft[1] = (GPIOA->IDR & GPIO_Pin_12);
	ktirLeft[2] = (GPIOC->IDR & GPIO_Pin_5);

	//rear sensors
	ktirBack[0] = (GPIOA->IDR & GPIO_Pin_4);
	ktirBack[1] = (GPIOA->IDR & GPIO_Pin_5);
	ktirBack[2] = (GPIOC->IDR & GPIO_Pin_4);
	ktirBack[3] = (GPIOB->IDR & GPIO_Pin_13);
	ktirBack[4] = (GPIOB->IDR & GPIO_Pin_14);
	ktirBack[5] = (GPIOA->IDR & GPIO_Pin_10);
	ktirBack[6] = (GPIOA->IDR & GPIO_Pin_11);

	//right sensors
	ktirRight[2] = (GPIOB->IDR & GPIO_Pin_3);
	ktirRight[1] = (GPIOB->IDR & GPIO_Pin_4);
	ktirRight[0] = (GPIOB->IDR & GPIO_Pin_5);
}
//encoders reset
void encodersReset(void)
{
	__disable_irq();	//interrupts disable
	oldLeftEncoder = 0;
	oldRightEncoder = 0;
	leftTotal = 0;
	rightTotal = 0;
	fwdTotal = 0;
	rotTotal = 0;
	TIM_SetCounter(ENC_L_TIM, 0); //left
	TIM_SetCounter(ENC_R_TIM, 0); //right
	__enable_irq();
	gyroDirection = 90;
}

//update encoders values
void encodersRead(void)
{
//read encoder sensors and set old values
	oldLeftEncoder = leftEncoder;
	leftEncoder = -TIM_GetCounter(ENC_L_TIM);
	oldRightEncoder = rightEncoder;
	rightEncoder = TIM_GetCounter(ENC_R_TIM);

//velocity impulse per reading
	leftCount = leftEncoder - oldLeftEncoder;
	rightCount = rightEncoder - oldRightEncoder;

//velocity computing mm/s
	velocityLeft = leftCount * IMP_DIST
			* (1000000. / ((double) (getMicroseconds() - time_old)));
	velocityRight = rightCount * IMP_DIST
			* (1000000. / ((double) (getMicroseconds() - time_old)));
	time_old = getMicroseconds();

//total distance in impulses
	leftTotal += leftCount;
	rightTotal += rightCount;

	//direction update
	gyroDirection += (IMP_DIST * (rightCount - leftCount) / 3.7972);
	//const = 3.7972 = 1367/360; 360 degrees,
	//1367/2 = 683.5 ->mean number of impulses per 1 rotate
	gyroDirection = angleMakeInRange(gyroDirection);

	//total distance in mm
	totalDistanceLeft = leftTotal * IMP_DIST;
	totalDistanceRight = rightTotal * IMP_DIST;
}

//normalization of ultrasonic sensors data
inline void ultra_data_processing(void)
{
	int a;

	for (a = 0; a < 4; a++)
	{

		if(a == 2)
		{
			ultra[a] = 1002;
			continue;
		}
		if (pierwsze_zbocze[a])	//if second edge was not detected - max value
		{
			ultra[a] = 1000;
			continue;
		}

		ultra_history[a] = (int) (((ultra__[a]) / linear_coefficient_distance)
						+ const_distance_from_the_middle_of_the_robot[a]);
		if(ultra_history[a] < MAX_ULTRA_ENEMY_DETECT)
		{
			ultra[a] = (int) (((ultra__[a]) / linear_coefficient_distance)
				+ const_distance_from_the_middle_of_the_robot[a]);
		}
		else
			ultra[a] = 1000;
					//register value to enemy distance [cm] conversion
			//register value to enemy distance [cm] conversion

		if (ultra[a] > MAX_ULTRA_ENEMY_DETECT)	//low-pass filter, high bandwidth - unstable
		{
			ultra[a] = 1000;
		}
	}

	/*
	 * ignore ultra if frame state was changed less than 2s ago
	 */
	if(getMiliseconds() - lastTimeFrameChanged < 2000)
		ultra[0] = 1000;
}
