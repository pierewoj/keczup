/*
 * config.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "misc.h"
#include "stm32f10x.h"
#include "sensors.h"
#include <stddef.h>
#include "stdio.h"
#include "communication.h"
#include "antilib_adc.h"
//Runs configuration of all peripherials used in this program.
//Invoked once, at the beginning of the program
void configurePeripherials(void);

//STM32 internal peripherials configurations
void GPIO_Config(void);
void NVIC_Config(void);
void RCC_Config(void);
void I2C_Config(void);
void ADC_Config(volatile unsigned int *tab);
void TIMERs_Config(void);
void DMA_Config(void);

//External devices configurations
void gyro_config(void);

#endif /* CONFIG_H_ */
