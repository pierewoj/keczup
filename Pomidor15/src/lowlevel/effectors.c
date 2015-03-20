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
	PWM_R=a;
	double modul = fabs(a);

	if(a<0)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);
		GPIO_SetBits(GPIOC,GPIO_Pin_8);
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
		GPIO_SetBits(GPIOC,GPIO_Pin_9);
	}

	if(modul<1.0000001)
		TIM2->CCR4 = modul*500;
}

void setLeftPWM(double a)
{
	PWM_L=a;
	double modul = fabs(a);

	if(a>0){
		GPIO_ResetBits(GPIOC,GPIO_Pin_7);
		GPIO_SetBits(GPIOC,GPIO_Pin_6);
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_6);
		GPIO_SetBits(GPIOC,GPIO_Pin_7);
	}

	if(modul<1.0000001)
		TIM2->CCR3 = modul*500;
}

//Motor stop functions using low/high impedance
void stopFast(void)
{
	PWM_R=0;
	PWM_L=0;
	TIM2->CCR4 = 500;
	TIM2->CCR3 = 500;
	GPIO_ResetBits(GPIOC,GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
}

void stopSlow(void)
{
	PWM_R=0;
	PWM_L=0;
	TIM2->CCR4 = 500;
	TIM2->CCR3 = 500;
	GPIO_SetBits(GPIOC,GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);
}