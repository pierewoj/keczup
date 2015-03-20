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

void TIM1_UP_TIM16_IRQHandler(void) __attribute__ ((interrupt));

void TIM3_IRQHandler(void) __attribute__ ((interrupt));

void TIM2_IRQHandler(void) __attribute__ ((interrupt));

#endif /* INTERRUPTS_H_ */
