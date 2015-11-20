/*
 * interrupts.h
 *
 *  Created on: Mar 17, 2015
 *      Author: Piotrek
 */

#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include <stddef.h>
#include "stm32f10x.h"
#include "sensors.h"
#include "config.h"
#include "../global.h"

#define TIME_TO_WAIT_FOR_2_PROGRAM_MS	20000

/*
 * TIMERs
 */

void TIM3_IRQHandler(void) __attribute__ ((interrupt));

void TIM1_TRG_COM_TIM17_IRQHandler(void) __attribute__ ((interrupt));

void TIM1_BRK_TIM15_IRQHandler(void) __attribute__ ((interrupt));

/*
 * EXTI
 */

void EXTI0_IRQHandler(void) __attribute__ ((interrupt));

/*
 * DMA
 */
void DMA1_Channel7_IRQHandler(void) __attribute__ ((interrupt));


#endif /* INTERRUPTS_H_ */
