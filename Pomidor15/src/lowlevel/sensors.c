/*
 * sensors.c
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */
#include "sensors.h"
#include <stddef.h>
#include "stm32f10x.h"
#include "communication.h"
#include "../global.h"

void readSensors(void)
{
	encodersRead();
	sharp_update();
	battery_update();
	read_ktirs();
}

inline void battery_update(void)
{
	battery = pomiar_adc[0];
}

inline void sharp_update(void)
{
	sharp = pomiar_adc[1];
}

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

void read_ktirs()
{   // !!! jeszcze nie ustalone piny !!!!
	ktirFront[1] = (unsigned short int) GPIO_ReadInputDataBit(GPIOC,
			GPIO_Pin_5);
	ktirFront[0] = (unsigned short int) GPIO_ReadInputDataBit(GPIOB,
			GPIO_Pin_5);
	ktirFront[3] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA,
			GPIO_Pin_5);
	ktirFront[2] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA,
			GPIO_Pin_4);
	ktirFront[5] = (unsigned short int) GPIO_ReadInputDataBit(GPIOC,
			GPIO_Pin_13);
	ktirFront[4] = (unsigned short int) GPIO_ReadInputDataBit(GPIOB,
			GPIO_Pin_15);
	ktirFront[6] = (unsigned short int) GPIO_ReadInputDataBit(GPIOB,
			GPIO_Pin_14);

	ktirLeft[2] = (unsigned short int) GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_2);
	ktirLeft[1] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA,
			GPIO_Pin_11);
	ktirLeft[0] = (unsigned short int) GPIO_ReadInputDataBit(GPIOC,
			GPIO_Pin_11);

	ktirBack[0] = false;
	ktirBack[1] = false;
	ktirBack[2] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
	ktirBack[3] = (unsigned short int) GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
	ktirBack[4] = (unsigned short int) GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
	ktirBack[5] = false;
	ktirBack[6] = false;

	ktirRight[0] = (unsigned short int) GPIO_ReadInputDataBit(GPIOC,
			GPIO_Pin_12);
	ktirRight[1] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA,
			GPIO_Pin_15);
	ktirRight[2] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA,
			GPIO_Pin_12);

}

void encodersReset(void)
{
	__disable_irq();
	oldLeftEncoder = 0;
	oldRightEncoder = 0;
	leftTotal = 0;
	rightTotal = 0;
	fwdTotal = 0;
	rotTotal = 0;
	TIM_SetCounter(ENC_L_TIM, 0); //lewy
	TIM_SetCounter(ENC_R_TIM, 0); //prawy
	__enable_irq();
}

void encodersRead(void)
{
	//static unsigned int time_old = 0;
	//wywolywac np co 1ms
	oldLeftEncoder = leftEncoder;
	leftEncoder = -TIM_GetCounter(ENC_L_TIM);
	oldRightEncoder = rightEncoder;
	rightEncoder = TIM_GetCounter(ENC_R_TIM);

	//predkosci w impulsach na szczytanie
	leftCount = leftEncoder - oldLeftEncoder;
	rightCount = rightEncoder - oldRightEncoder;

	//predkosci w mm/s
	velocityLeft = leftCount * IMP_DIST
			* (1000000. / ((double) (getMicroseconds() - time_old)));
	velocityRight = rightCount * IMP_DIST
			* (1000000. / ((double) (getMicroseconds() - time_old)));
	time_old = getMicroseconds();

	//dystanse
	//fwdCount = leftCount + rightCount;
	//rotCount = - (leftCount - rightCount);
	//fwdTotal += fwdCount;
	//rotTotal += rotCount;

	//dystans calkowity w impulsach
	leftTotal += leftCount;
	rightTotal += rightCount;

	//dystans calkowity w mm
	totalDistanceLeft = leftTotal * IMP_DIST;
	totalDistanceRight = rightTotal * IMP_DIST;
}
