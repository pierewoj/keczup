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
	gyro_x = (int16_t)(dane_z_imu[1]<<8 | dane_z_imu[0]);
	gyro_y = (int16_t)(dane_z_imu[3]<<8 | dane_z_imu[2]);
	gyro_z = (int16_t)(dane_z_imu[5]<<8 | dane_z_imu[4]);

	//correction of the initial values
	gyro_x -= gyro_initial_values[0];
	gyro_y -= gyro_initial_values[1];
	gyro_z -= gyro_initial_values[2];
}

void read_ktirs(){   // !!! jeszcze nie ustalone piny !!!!
	ktirFront[0] = (unsigned short int) GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5); //PB5
	ktirFront[1] = (unsigned short int) GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5); //PC5
	ktirFront[2] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4); //PA4
	ktirFront[3] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5); //PA5
	ktirFront[4] = (unsigned short int) GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15); //PB15
	ktirFront[5] = (unsigned short int) GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13); //PC13
	ktirFront[6] = (unsigned short int) GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14); //PB14


	ktirLeft[0] = (unsigned short int) GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4); //PB4
	ktirLeft[1] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10); //PA10
	ktirLeft[2] = (unsigned short int) GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13); //PB13


	ktirBack[0] = (unsigned short int) GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12); //PC12
	ktirBack[1] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15); //PA15
	ktirBack[2] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12); //PA12
	ktirBack[3] = (unsigned short int) GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2); //PD2
	ktirBack[4] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11); //PA11
	ktirBack[5] = (unsigned short int) GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11); //PC11
	ktirBack[6] = (unsigned short int) GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11); //PC11

	ktirRight[0] = (unsigned short int) GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6); //PA6
	ktirRight[1] = (unsigned short int) GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8); //PB8
	ktirRight[2] = (unsigned short int) GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9); //PB9

	int i = 0;
	numberBlack = 0;
	numberBlackBack = 0;

	for(i = 0; i < 7; i++)
	{
		if(ktirFront[i] == true)
			numberBlack++;
		if(ktirBack[i] == true)
			numberBlackBack++;
	}
}
