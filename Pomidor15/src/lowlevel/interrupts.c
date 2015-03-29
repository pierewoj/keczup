/*
 * interrupts.c
 *
 *  Created on: Mar 17, 2015
 *      Author: Piotrek
 */
#include "interrupts.h"

//interrupt for ultrasonic sensors data processing
void TIM1_TRG_COM_TIM17_IRQHandler(void)
{
	if (TIM17->SR & TIM_SR_UIF) // if UIF flag is set
	{
		TIM17->SR &= ~TIM_SR_UIF; // clear UIF flag
		TIM2->CCR2 = 17;       //about 10 ms high PWM state - ultrasonic trigger

		//normalization of ultrasonic sensors data
		encodersRead();
		ultra_data_processing();
		ultra1 = ultra[0];
		ultra2 = ultra[1];
		ultra3 = ultra[3];
	}
}

void TIM2_IRQHandler(void) //interrupt after each ultrasonic trigger pulse generated
{
	if (TIM2->SR & TIM_SR_CC2IF) // if CC2IF flag is set
	{
		//Timer 2 reset
		TIM2->SR &= !(TIM_SR_CC2IF | TIM_SR_CC2OF);			//clear flags
		TIM2->CCR2 = 0;										//clear counter

		//Timer 3 -> reset interrupt and overwrite flags, set polarity (high), reset counter
		TIM3->SR &= !(TIM_SR_CC3IF | TIM_SR_CC3OF | TIM_SR_CC4IF | TIM_SR_CC4OF
				| TIM_SR_CC2IF | TIM_SR_CC2OF);
		TIM3->CCER &= ~(TIM_CCER_CC3P | TIM_CCER_CC4P | TIM_CCER_CC2P);
		//interrupt after rising edge detected
		TIM3->CNT = 0;	//clear counter register
	}
}

void TIM3_IRQHandler(void) //ultrasonic sensor echo capturing timer
{
	//front ultrasonic sensor PA7
	if (TIM3->SR & TIM_SR_CC2IF) // if CC3IF flag is set
	{
		TIM3->SR &= !(TIM_SR_CC2IF | TIM_SR_CC2OF);
		if (!(TIM3->CCER & TIM_CCER_CC2P))	//rising edge response
		{
			ultra_pom[0] = (TIM3->CCR2);	//capture counter value
			pierwsze_zbocze[0] = 1;
		}
		else								//falling edge response
		{
			ultra__[0] = (TIM3->CCR2) - ultra_pom[0];
			//capture counter value (falling edge) and substract previous value (rising edge)
			pierwsze_zbocze[0] = 0;		//both edges detected - ok
		}
		TIM3->CCER ^= TIM_CCER_CC2P;	//change polarity (rising/falling)
	}
	//right ultrasonic sensor PB0
	if (TIM3->SR & TIM_SR_CC3IF) // if CC3IF flag is set
	{

		TIM3->SR &= !(TIM_SR_CC3IF | TIM_SR_CC3OF);
		if (!(TIM3->CCER & TIM_CCER_CC3P))
		{
			ultra_pom[1] = (TIM3->CCR3);
			pierwsze_zbocze[1] = 1;
		}
		else
		{
			ultra__[1] = (TIM3->CCR3) - ultra_pom[1];
			pierwsze_zbocze[1] = 0;
		}
		TIM3->CCER ^= TIM_CCER_CC3P;
	}
	//left ultrasonic sensor PB1
	if (TIM3->SR & TIM_SR_CC4IF) // if CC3IF flag is set
	{
		TIM3->SR &= !(TIM_SR_CC4IF | TIM_SR_CC4OF);
		if (!(TIM3->CCER & TIM_CCER_CC4P))
		{
			ultra_pom[3] = (TIM3->CCR4);
			pierwsze_zbocze[3] = 1;
		}
		else
		{
			ultra__[3] = (TIM3->CCR4) - ultra_pom[3];
			pierwsze_zbocze[3] = 0;
		}
		TIM3->CCER ^= TIM_CCER_CC4P;
	}
}

//user button interrupt
void EXTI0_IRQHandler(void)
{
	if ( EXTI->PR & EXTI_PR_PR0)
	{
		EXTI->PR |= EXTI_PR_PR0;
		buttonStart = !buttonStart;
		//do not send any message - there is a conflict between DMA and EXTI interrupts
	}
}

void DMA1_Channel7_IRQHandler(void)
{
	if (DMA1->ISR & DMA_ISR_TCIF7)
	{
		while (!(USART2->SR & USART_SR_TC))
			;   //if TC flag is not set then wait
		DMA1_Channel7->CCR &= ~DMA_CCR7_EN;	 //DMA disable
		DMA1->IFCR |= DMA_IFCR_CTCIF7;       //clear TCIF7 flag
	}
}
