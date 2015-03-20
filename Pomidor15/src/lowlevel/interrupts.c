/*
 * interrupts.c
 *
 *  Created on: Mar 17, 2015
 *      Author: Piotrek
 */
#include "interrupts.h"

void TIM1_UP_TIM16_IRQHandler(void) //timer od wysylania rzeczy przez bluetooth do PC
{
if(TIM16->SR & TIM_SR_UIF) // if UIF flag is set
  {
	TIM16->SR &= ~TIM_SR_UIF; // clear UIF flag
	melduj();
	DMA_Config();
	loopCounter=0;
  }
}

void TIM1_TRG_COM_TIM17_IRQHandler(void) //timer od wysylania rzeczy przez bluetooth do PC
{
if(TIM17->SR & TIM_SR_UIF) // if UIF flag is set
  {
	TIM17->SR &= ~TIM_SR_UIF; // clear UIF flag
	TIM15->CCR2 = 17;

	ultra1 = ultra__[0];
	ultra2 = ultra__[1];
	ultra3 = ultra__[2];
  }
}

void TIM15_IRQHandler(void)
{
	if(TIM15->SR & TIM_SR_CC2IF) // if CC2IF flag is set
	{
		//Timer 15 -> reset
		ile_przerwan++;
		TIM15->SR&=!(TIM_SR_CC2IF | TIM_SR_CC2OF);
		TIM15->CCR2 = 0;
		//Timer 3 -> reset interrupt, overrun flags, set polarity (high), reset counter
		TIM3->SR&=!(TIM_SR_CC3IF | TIM_SR_CC3OF | TIM_SR_CC4IF | TIM_SR_CC4OF | TIM_SR_CC2IF | TIM_SR_CC2OF);
		TIM3->CCER &= ~(TIM_CCER_CC3P | TIM_CCER_CC4P | TIM_CCER_CC2P);       //reakcja na zbocze narast.
		TIM3->CNT = 0;
		int a;
		for ( a = 0 ; a < 3 ; a++)
		{
			if(pierwsze_zbocze[a])
			{
				ultra__[a] = distance_max;
				ile_maxow++;
			}
		}
	}
}

void TIM3_IRQHandler(void) //timer od wysylania rzeczy przez bluetooth do PC
{
	if(TIM3->SR & TIM_SR_CC3IF) // if CC3IF flag is set
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

	if(TIM3->SR & TIM_SR_CC4IF) // if CC3IF flag is set
		{
			TIM3->SR&=!(TIM_SR_CC4IF | TIM_SR_CC4OF);
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

	if(TIM3->SR & TIM_SR_CC2IF) // if CC3IF flag is set
		{
			TIM3->SR&=!(TIM_SR_CC2IF | TIM_SR_CC2OF);
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
																						//*****!!###### KONIEC ZMIANY
void TIM7_IRQHandler(void){
	if(TIM7->SR & TIM_SR_UIF) // if UIF flag is set
	  {
		TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
		//ENKODERY
		encodersRead();
		TIME+=ENC_READ;
		if(UZYW_KOMPAS){
			if(TIME>1000){
			__i2c_read(kompas_write_address,0x03, dane_z_kompasu);

			skladowa_x = dane_z_kompasu[0]<<8 | dane_z_kompasu[1];
			skladowa_z = dane_z_kompasu[2]<<8 | dane_z_kompasu[3];
			skladowa_y = dane_z_kompasu[4]<<8 | dane_z_kompasu[5];

			if((skladowa_x & (1 << 15)) != 0)
				skladowa_x = skladowa_x | ~((1 << 16) - 1);
			if((skladowa_z & (1 << 15)) != 0)
						skladowa_z = skladowa_z | ~((1 << 16) - 1);
			if((skladowa_y & (1 << 15)) != 0)
						skladowa_y = skladowa_y | ~((1 << 16) - 1);

			double norm_x, norm_y;
			norm_x = skladowa_x * 2./((double)KOMPASX_maxVal - (double)KOMPASX_minVal) + 1 - 2*(double)KOMPASX_maxVal/((double)KOMPASX_maxVal-(double)KOMPASX_minVal);
			norm_y = skladowa_y * 2./((double)KOMPASY_maxVal - (double)KOMPASY_minVal) + 1 - 2*(double)KOMPASY_maxVal/((double)KOMPASY_maxVal-(double)KOMPASY_minVal);
			KOMPAS_KIER_RAW = RAD_TO_DEG * atan2(norm_y, norm_x);   //dzialajacy kierunek z kompasu
			KOMPAS_KIER =  roznica_katow( KOMPAS_KIER_RAW, KOMPAS_ZERO);
			KIER = KOMPAS_KIER;
		}




		}
    	//SHARPY
		int i=0,j=0;
		for(;i<7;i++)
			SHARP_FILTR[i][FILTR_INDEX]=SHARP[i];
		if(FILTR_INDEX==19){
			for(i=0;i<7;i++){
				int suma=0;
				for(j=0;j<20;j++)
					suma+=SHARP_FILTR[i][j];
				SHARP_AVG[i]=suma/20;

			}
		}

		FILTR_INDEX++;
		FILTR_INDEX%=20;



	  }

}

void EXTI15_10_IRQHandler(void)
{
	if(( EXTI->PR & EXTI_PR_PR13 ) != 0 )
	{

	}
    EXTI->PR |= EXTI_PR_PR13;
}

void DMA1_Channel7_IRQHandler(void)
{
	if(DMA1->ISR & DMA_ISR_TCIF7)
		{
			while (!(USART2->SR & USART_SR_TC));   //jezeli flaga TC nie ustawiona to czekaj
			DMA1_Channel7->CCR &= ~DMA_CCR7_EN;
			DMA1->IFCR |= DMA_IFCR_CTCIF7;       //clear TCIF6 flag
		}
}
