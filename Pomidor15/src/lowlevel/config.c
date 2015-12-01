/*
 * config.c
 *
 *  Created on: Mar 22, 2015
 *      Author: Piotrek
 */
#include "config.h"
#include "../global.h"
#include "sensors.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "communication.h"

void configurePeripherials(void)
{
	//STM32 internal peripherials configurations
	RCC_Config();

	RTC_Config();
	NVIC_Config();
	GPIO_Config();
	TIMERs_Config();
	EXTI_Config();
	//I2C_Config();
	USART_Config();
	ADC_Config(pomiar_adc);

	//External devices configurations
	encodersReset();
	//gyro_config();

	usart_data_number = 0;
	time_old = 0;
	gyro_initial_values[0] = 0;
	gyro_initial_values[1] = 0;
	gyro_initial_values[2] = 0;
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
//****************__Internal_devices_configurations__*********************//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//

void RCC_Config(void)
{
	/* error status */
	ErrorStatus HSEStartUpStatus;

	RCC_DeInit();

	RCC_HSEConfig(RCC_HSE_ON);
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS)
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		FLASH_SetLatency(FLASH_Latency_2);

		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div1);

		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_5);
		RCC_PLLCmd(ENABLE);
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
			;

		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while (RCC_GetSYSCLKSource() != 0x08)
			;
	}

	//peripherial clocks enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
	//RCC->APB1ENR |= RCC_APB1Periph_I2C1;
}

/*
 * Real-time clock configuration  - system time
 * RTC is configured to count with 8 000 000 / 128 Hz frequency
 * Resolution - 16 us
 * At the end of this function clock is set to 0 and may be read by macro - 'time'
 */
void RTC_Config(void)
{
	PWR->CR |= PWR_CR_DBP;

	//RCC->BDCR = (RCC->BDCR & 0xFFFFFCFF) | 0x00000100;
	RCC->BDCR |= (RCC_BDCR_RTCSEL_0 | RCC_BDCR_RTCSEL_1);

	RCC->BDCR |= RCC_BDCR_RTCEN;

	while ((RTC->CRL & RTC_CRL_RSF) == 0)
		;

	while ((RTC->CRL & RTC_CRL_RTOFF) == 0)
		;

	RTC->CRL |= RTC_CRL_CNF;

	RTC->PRLH = 0x0000; //prescaler set to 0  -  counter clocked with HSE/128 Hz
	RTC->PRLL = 0x0000;
	RTC->CNTL = 0;
	RTC->CNTH = 0;

	RTC->CRL &= ~RTC_CRL_CNF;
}
void NVIC_Config(void)
{
	//interrupts controller - NVIC configurations
	NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
	// Jezeli tablica wektorow w RAM, to ustaw jej adres na 0x20000000
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  // VECT_TAB_FLASH	// W przeciwnym wypadku ustaw na 0x08000000	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
	//Wybranie grupy priorytetów
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	//Przerwanie od USART2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//Przerwanie od USART3
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//Przerwanie od TIM3
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    //Przerwanie zewnetrzne EXTI od przycisku USER button
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    //Przerwanie od timera TIM17
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_TRG_COM_TIM17_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_TIM15_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_EnableIRQ(TIM3_IRQn);     //w³¹czenie przerwania od TIM3 - (NVIC level)

}

void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//gyro, accelerometer, compass - gpio configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PWM of engine 1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//adc - gpio configuration: PC0 - VBat, PC1 - analog_distance_sensor
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//PB15 - ultrasonic sensors trigger (PWM)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PB0, PB1 - ultrasonic sensors echo
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PA6 - ultrasonic sensors echo
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//PA7 - ultrasonic sensors echo
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Engines digital inputs
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8
			| GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//PC10 - UART3 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//PC11 - UART3 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//PA2 - USART2 TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//PA3 - USART2 RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//User button configuration PA0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//KTIRs A1/A4/A5/A10/A11/A12/A15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5
			| GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//KTIRs B3/B4/B5/B12/B13/B14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5
			| GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//KTIRs C2/C3/C4/C5/C14/C15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4
			| GPIO_Pin_5 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//KTIRs D2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//left encoder - A8, A9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//right encoder - B6, B7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	AFIO->MAPR |= AFIO_MAPR_TIM2_REMAP_PARTIALREMAP2; //channels: 3,4 remapped (to: PB10, PB11)
	AFIO->MAPR |= AFIO_MAPR_USART3_REMAP_0; //usart rx,tx remapped (to: PC10, PC11)
	AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;            //I2C remapped (to: PB8, PB9)
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

//
void TIMERs_Config(void)
{
	//configuration of left wheel's encoder
	TIM1->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1; // count impulses from both of timers channels
	TIM1->CCER &= !(TIM_CCER_CC1P | TIM_CCER_CC2P); // set polarization on rising edge
	TIM1->ARR = 0xffff;									// auto reload value
	TIM1->CR1 |= TIM_CR1_CEN;							// TIMER enable

	//configuration of right wheel's encoder
	TIM4->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;// count impulses from both of timers channels
	TIM4->CCER &= !(TIM_CCER_CC1P | TIM_CCER_CC2P);	// set polarization on rising edge
	TIM4->ARR = 0xffff;									// auto reload value
	TIM4->CR1 |= TIM_CR1_CEN;							// TIMER enable

	//engine's PWM configuration
	TIM2->PSC = 7;// Set prescaler to 8 (PSC + 1) -> PWM Frequency = 5.25 KHz for TIM2->ARR = 1000
	TIM2->ARR = 1000;	 // Auto reload value 1000
	TIM2->CCR3 = 0;	   // Start PWM duty for channel 3
	TIM2->CCR4 = 1000; // Start PWM duty for channel 4
	TIM2->CCMR2 = TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC3M_2
			| TIM_CCMR2_OC3M_1;	// PWM mode on channels 3, 4
	TIM2->CCER = TIM_CCER_CC4E | TIM_CCER_CC3E; // Enable compare on channel 3 & 4
	TIM2->CR1 = TIM_CR1_CEN;	 // Enable timer

	//ultrasonic sensor echo capturing timer
	TIM3->PSC = 49;                 //prescaler set to 50
	TIM3->ARR = ultra_distance_max; //max counter value
	TIM3->CR1 = 0;                  //reset controls register
	TIM3->CCMR1 |= TIM_CCMR1_CC2S_0 | TIM_CCMR1_CC1S_0; //capture-compare mode CH2
	TIM3->CCMR2 |= TIM_CCMR2_CC3S_0 | TIM_CCMR2_CC4S_0; //capture-compare mode CH3, CH4

	TIM3->CCMR2 |= TIM_CCMR2_IC3F_0 | TIM_CCMR2_IC3F_1 | TIM_CCMR2_IC4F_0
			| TIM_CCMR2_IC4F_1;	//input capture filter
	TIM3->CCMR1 |= TIM_CCMR1_IC2F_0 | TIM_CCMR1_IC2F_1 | TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1;	//input capture filter
	TIM3->CCER |= TIM_CCER_CC3E | TIM_CCER_CC4E | TIM_CCER_CC2E | TIM_CCER_CC1E;
	//enable capture compare mode
	TIM3->DIER |= TIM_DIER_CC3IE | TIM_DIER_CC4IE | TIM_DIER_CC2IE | TIM_DIER_CC1IE;
	//capture - compare interrupt enable
	TIM3->CR1 |= TIM_CR1_CEN;	//counter enable
	TIM3->CNT = 0x0000;			//clear counter

	//ultrasonic sensors data processing timer
	TIM17->PSC = 39999;	         // Set prescaler to 40000 (PSC + 1)
	TIM17->ARR = 10;	         // Auto reload value [ms]
	TIM17->DIER = TIM_DIER_UIE;  // Enable update interrupt (timer level)
	TIM17->CR1 = TIM_CR1_CEN;    // Enable timer

	TIM15->PSC = 39;	         // Set prescaler to 40 (PSC + 1); one tick-1us
	TIM15->ARR = 10;	         // Auto reload value [us]
	TIM15->DIER = TIM_DIER_UIE;  // Enable update interrupt (timer level)
	TIM15->CR1 = TIM_CR1_CEN;    // Enable timer
}

//External interrupt configuration - user button
void EXTI_Config(void)
{
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;         //manual strona 124, 4V3!
	EXTI->IMR |= EXTI_IMR_MR0;           				//interrupt (not ivent)
	EXTI->RTSR |= EXTI_RTSR_TR0;        			//Raising edge - interrupt
}

void I2C_Config(void)
{
	I2C1_CR1_SWRST_bb = 1;            // force software reset of I2C peripheral
	I2C1_CR1_SWRST_bb = 0;
	I2C1->TRISE = 27;               // limit slope
	I2C1->CR2 = I2C_CR2_FREQ_3 | I2C_CR2_FREQ_4;      // config I2C1 module
	I2C1->CCR = 120;               // setup speed (100kHz)
	I2C1_CR1_PE_bb = 1;            // enable peripheral
	I2C1_CR1_ACK_bb = 1;
}

void USART_Config()
{
	USART_InitTypeDef USART_InitStructure;

	//USART3 (dynamixel)
	USART_InitStructure.USART_BaudRate = 1000000;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	USART_Cmd(USART3, ENABLE);

	//USART2 (bluetooth)
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_Init(USART2, &USART_InitStructure);
	USART2->CR3 |= USART_CR3_DMAT;
	USART_Cmd(USART2, ENABLE);
}

//DMA for USART2 configuration
void DMA_Config(char *message)
{
	DMA1_Channel7->CPAR = (uint32_t) (&(USART2->DR));   //periph adress register
	DMA1_Channel7->CMAR = (uint32_t) message; //memory address register
	DMA1_Channel7->CNDTR = usart_data_number;   // number of data to transfer
	DMA1_Channel7->CCR |= DMA_CCR7_TCIE | DMA_CCR7_MINC // DMA_CCR7_MSIZE_0 | | DMA_CCR7_PSIZE_0
			| DMA_CCR7_DIR | DMA_CCR7_PL_1;
	//memory size - half | periph size - half | Periph to memory: 0x00000000
	// | circular mode | memory increment | enable | priority

	USART2->SR &= ~USART_SR_TC;

	DMA1_Channel7->CCR |= DMA_CCR7_EN; //Enable DMA Channel1
}

void ADC_Config(volatile unsigned int *tab)
{
	//*******************__ADC_configuration__******************************//

	ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_CONT | ADC_CR2_DMA; // Turn on ADC, enable continuos mode, DMA mode
	ADC1->CR1 = ADC_CR1_SCAN;          //scan mode
	ADC1->SQR1 = ADC_SEQUENCE_LENGTH(1);         // two channel in sequence
	ADC1->SQR3 = ADC_SEQ1(10) | ADC_SEQ2(11); // ADC channel 10 is first in sequence, channel 13 is second in sequence and so on...
	ADC1->SMPR1 = ADC_SAMPLE_TIME0(SAMPLE_TIME_41_5) | // sample time for first channel
			ADC_SAMPLE_TIME1(SAMPLE_TIME_41_5); // sample time for second channel in sequence

	//**************__DMA_for_ADC_configuration__***************//

	DMA1_Channel1->CPAR = (uint32_t) (&(ADC1->DR));   //periph adress register
	DMA1_Channel1->CMAR = (uint32_t) tab; 			//memory address register
	DMA1_Channel1->CNDTR = 2; 					// number of data to transfer
	DMA1_Channel1->CCR |= DMA_CCR1_MSIZE_1 | DMA_CCR1_CIRC | DMA_CCR1_MINC
			| DMA_CCR1_PSIZE_0;
	//memory size - half | periph size - half | Periph to memory| circular mode
	// | memory increment | enable
	DMA1_Channel1->CCR |= DMA_CCR1_EN; //Enable DMA Channel1
	ADC1->CR2 |= ADC_CR2_ADON;         //Enable ADC
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//
//****************__External_devices_configurations__*********************//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>//

void gyro_config(void)
{
	//various default modes...
	imu[0] = gyro_LOW_ODR;         //0x00;
	imu[1] = 0x00;
	__i2c_write(imu_write_address, imu, 2);

	//continuous data registers updating, scale: 245 deg/s
	imu[0] = gyro_CTRL_REG4;         //0x00;
	imu[1] = 0x00;
	__i2c_write(imu_write_address, imu, 2);

	//bandwidth, output data rate - 200 Hz, normal power mode, axis enable
	imu[0] = gyro_CTRL_REG1;         //0x00;
	imu[1] = 0x6F;
	__i2c_write(imu_write_address, imu, 2);

	//getting initial values of angular velocity during immobility of the robot
	//compute mean value of initial angular velocity
	int a;
	for (a = 0; a < gyro_calibration_sample_number; a++)
	{
		__i2c_read(imu_write_address, 0x28, dane_z_imu);
		gyro_initial_values[0] +=
				(int16_t) (dane_z_imu[1] << 8 | dane_z_imu[0]);
		gyro_initial_values[1] +=
				(int16_t) (dane_z_imu[3] << 8 | dane_z_imu[2]);
		gyro_initial_values[2] +=
				(int16_t) (dane_z_imu[5] << 8 | dane_z_imu[4]);
	}
	gyro_initial_values[0] /= gyro_calibration_sample_number;
	gyro_initial_values[1] /= gyro_calibration_sample_number;
	gyro_initial_values[2] /= gyro_calibration_sample_number;
}
