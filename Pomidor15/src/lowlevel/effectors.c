/*
 * effectors.c
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

//unlocks the can
#include "effectors.h"

void openFrame(void)
{
	dynamixel_ustawPozycje(0.51);
}

//locks the can
void closeFrame(void)
{
	dynamixel_ustawPozycje(0.8);
}

//set PWM values controlling engines. Values [-1.0 .. +1.0]. + is forward.
void setRightPWM(double v)
{
	double modul = fabs(v);

	if (v < 0)
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_8);
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
	}

	if (modul < 1.0000001)
		TIM2->CCR4 = modul * 500;
}

void setLeftPWM(double v)
{
	double modul = fabs(v);

	if (v > 0)
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		GPIO_SetBits(GPIOC, GPIO_Pin_7);
	}

	if (modul < 1.0000001)
		TIM2->CCR3 = modul * 500;
}

//Motor stop functions using low/high impedance
void stopFast(void)
{
	TIM2->CCR4 = 500;
	TIM2->CCR3 = 500;
	GPIO_ResetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
}

void stopSlow(void)
{
	TIM2->CCR4 = 500;
	TIM2->CCR3 = 500;
	GPIO_SetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
}
