/*
 * effectors.c
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

//unlocks the can
#include "effectors.h"
#include "../global.h"
#include <math.h>

unsigned int lastTimeFrameChanged = 0;

void openFrame(void)
{
	dynamixel_ustawPozycje(0.54);
	frameClosed = false;
	lastTimeFrameChanged = getMiliseconds();
}

//locks the can
void closeFrame(void)
{
	dynamixel_ustawPozycje(0.8);
	frameClosed = true;
	lastTimeFrameChanged = getMiliseconds();
}

//set PWM values controlling engines. Values [-1.0 .. +1.0]. + is forward.
void setRightPWM(double v)
{
	//make sure v is in [-1;1]
	v = fmin(1,fmax(-1,v));
	double modul = fabs(v);

	//for sending it to PC
	pwmRight = v;

	if (v > 0)
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
		TIM2->CCR4 = modul * 1000;
}

void setLeftPWM(double v)
{
	//make sure v is in [-1;1]
	v = fmin(1,fmax(-1,v));
	double modul = fabs(v);

	//for sending it to PC
	pwmLeft = v;

	if (v > 0)
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);
		GPIO_SetBits(GPIOC, GPIO_Pin_7);
	}
	else
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_7);
		GPIO_SetBits(GPIOC, GPIO_Pin_6);
	}

	if (modul < 1.0000001)
		TIM2->CCR3 = modul * 1000;
}

//Motor stop functions using low/high impedance
void stopFast(void)
{
	TIM2->CCR4 = 1000;
	TIM2->CCR3 = 1000;
	GPIO_SetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
}

void stopSlow(void)
{
	TIM2->CCR4 = 1000;
	TIM2->CCR3 = 1000;
	GPIO_ResetBits(GPIOC, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
}
