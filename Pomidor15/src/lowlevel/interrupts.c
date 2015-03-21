/*
 * interrupts.c
 *
 *  Created on: Mar 17, 2015
 *      Author: Piotrek
 */
#include "interrupts.h"

void TIM1_UP_TIM16_IRQHandler(void) //timer od wysylania rzeczy przez bluetooth do PC
{
	if (TIM16->SR & TIM_SR_UIF) // if UIF flag is set
	{
		TIM16->SR &= ~TIM_SR_UIF; // clear UIF flag
		melduj();
		DMA_Config();
		//loopCounter = 0;
	}
}

void TIM1_TRG_COM_TIM17_IRQHandler(void) //timer od wysylania rzeczy przez bluetooth do PC
{
	if (TIM17->SR & TIM_SR_UIF) // if UIF flag is set
	{
		TIM17->SR &= ~TIM_SR_UIF; // clear UIF flag
		TIM15->CCR2 = 17;

		ultra1 = ultra__[0];
		ultra2 = ultra__[1];
		ultra3 = ultra__[2];
	}
}

void TIM1_BRK_TIM15_IRQHandler(void)
{
	if (TIM15->SR & TIM_SR_CC2IF) // if CC2IF flag is set
	{
		//Timer 15 -> reset
		TIM15->SR &= !(TIM_SR_CC2IF | TIM_SR_CC2OF);
		TIM15->CCR2 = 0;
		//Timer 3 -> reset interrupt, overrun flags, set polarity (high), reset counter
		TIM3->SR &= !(TIM_SR_CC3IF | TIM_SR_CC3OF | TIM_SR_CC4IF | TIM_SR_CC4OF
				| TIM_SR_CC2IF | TIM_SR_CC2OF);
		TIM3->CCER &= ~(TIM_CCER_CC3P | TIM_CCER_CC4P | TIM_CCER_CC2P); //reakcja na zbocze narast.
		TIM3->CNT = 0;
		int a;
		for (a = 0; a < 3; a++)
		{
			if (pierwsze_zbocze[a])
			{
				ultra__[a] = ultra_distance_max;
			}
		}
	}
}

void TIM3_IRQHandler(void) //timer od wysylania rzeczy przez bluetooth do PC
{
	if (TIM3->SR & TIM_SR_CC3IF) // if CC3IF flag is set
	{

		TIM3->SR &= !(TIM_SR_CC3IF | TIM_SR_CC3OF);
		if (!(TIM3->CCER & TIM_CCER_CC3P))
		{
			ultra_pom[2] = (TIM3->CCR3);
			pierwsze_zbocze[2] = 1;
		}
		else
		{
			ultra__[2] = (TIM3->CCR3) - ultra_pom[2];
			pierwsze_zbocze[2] = 0;
		}
		TIM3->CCER ^= TIM_CCER_CC3P;
	}

	if (TIM3->SR & TIM_SR_CC4IF) // if CC3IF flag is set
	{
		TIM3->SR &= !(TIM_SR_CC4IF | TIM_SR_CC4OF);
		if (!(TIM3->CCER & TIM_CCER_CC4P))
		{
			ultra_pom[1] = (TIM3->CCR4);
			pierwsze_zbocze[1] = 1;
		}
		else
		{
			ultra__[1] = (TIM3->CCR4) - ultra_pom[1];
			pierwsze_zbocze[1] = 0;
		}
		TIM3->CCER ^= TIM_CCER_CC4P;
	}

	if (TIM3->SR & TIM_SR_CC2IF) // if CC3IF flag is set
	{
		TIM3->SR &= !(TIM_SR_CC2IF | TIM_SR_CC2OF);
		if (!(TIM3->CCER & TIM_CCER_CC2P))
		{
			ultra_pom[0] = (TIM3->CCR2);
			pierwsze_zbocze[0] = 1;
		}
		else
		{
			ultra__[0] = (TIM3->CCR2) - ultra_pom[0];
			pierwsze_zbocze[0] = 0;
		}
		TIM3->CCER ^= TIM_CCER_CC2P;
	}
}

void TIM7_IRQHandler(void)
{
	if (TIM7->SR & TIM_SR_UIF) // if UIF flag is set
	{
		TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
		//ENKODERY
		encodersRead();
		//Time update
		time += TIM7_interrupt_time;

		//Gyroscope
		if (time > 1000)
		{

		}

		//SHARPY

	}

}

void EXTI15_10_IRQHandler(void)
{
	if (( EXTI->PR & EXTI_PR_PR13) != 0)
	{

	}
	EXTI->PR |= EXTI_PR_PR13;
}

void DMA1_Channel7_IRQHandler(void)
{
	if (DMA1->ISR & DMA_ISR_TCIF7)
	{
		while (!(USART2->SR & USART_SR_TC))
			;   //jezeli flaga TC nie ustawiona to czekaj
		DMA1_Channel7->CCR &= ~DMA_CCR7_EN;
		DMA1->IFCR |= DMA_IFCR_CTCIF7;       //clear TCIF6 flag
	}
}
