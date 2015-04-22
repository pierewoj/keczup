/*
 * communication.c
 *
 *  Created on: Mar 22, 2015
 *      Author: Piotrek
 */

#include "stm32f10x_usart.h"
#include "../global.h"
#include <stdio.h>
#include <string.h>
#include "communication.h"
#include "../utils/messageQueue.h"
#include "../commPC.h"
#include "config.h"
#include <stdlib.h>
#include "stdbool.h"

//***********************__USART__*********************************//

unsigned short int U3_bufTxIndex, U3_bufTxMaxIndex;
unsigned short int U2_bufTxIndex, U2_bufTxMaxIndex;
char U2_buforRx[300];
int U2_buforRx_Size;
int U2_bufRxIndex;
int usart_data_number;s
unsigned short int U3_buforTx[50];
bool flaga = 0;

void dynamixel_ustawPozycje(double procent);

void USART3_IRQHandler(void);

void USART2_IRQHandler(void);

void uart3_sendArray(unsigned short int *arr, unsigned short int n);


//****************************************************//
//********************__USART__***********************//
//****************************************************//

/*
 * dynamixel USART interrupts configuration
 */
void USART3_IRQHandler(void)
{

	if (USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	//if transmit register waits for data
	{

		if (U3_bufTxIndex == U3_bufTxMaxIndex - 1) //if the last character sent..
		{
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE); //Interrupt disable - end of trasmission
			U3_bufTxIndex = 0;	//set index to 0
		}

		else
		{
			USART_SendData(USART3, U3_buforTx[U3_bufTxIndex]);//load USART data register
			U3_bufTxIndex++;
		}
	}
}

/*
 * bluetooth USART interrupts configuration
 */
void USART2_IRQHandler(void)
{
	//receive data
	if (USART_GetITStatus(USART2, USART_IT_RXNE))//if data register not empty (receive register)
	{
		DMA1_Channel7->CCR &= ~DMA_CCR7_EN;	      //DMA disable
		DMA1->IFCR |= DMA_IFCR_CTCIF7;
		USART2->SR &= ~USART_SR_TC;

		flaga = true;
		U2_buforRx[U2_bufRxIndex] = USART_ReceiveData(USART2);//get register value
		U2_bufRxIndex++;

		if (U2_buforRx[U2_bufRxIndex - 1] == '\n' || U2_bufRxIndex > 100)//end of transmission
		{
			U2_buforRx_Size = U2_bufRxIndex - 2;
			U2_bufRxIndex = 0;
			flaga = false;
			messageProcessor(U2_buforRx, U2_buforRx_Size);	//carry out a task
		}
	}

}

void uart3_sendArray(unsigned short int *arr, unsigned short int n)
{
	U3_bufTxMaxIndex = n;
	int i;
	for (i = 0; i < n; i++)
	{
		U3_buforTx[i] = *arr;
		arr++;
	}
	U3_bufTxIndex = 0;
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

/*
 * Set frame position
 * //0.2 - horizontaly front
 * //0.8 - horizontaly rear
 */
void dynamixel_ustawPozycje(double procent)
{
	if (procent < 0 || procent > 1)
		return;

	unsigned int a = procent * 0x03FF;

	unsigned short int dynamixelData[11] =
	{ 0xFF, 0xFF, 0xFE, 0x07, 0x03, 0x1E, 0x00, 0x01, 0x00, 0x00, 0xD6 };
	dynamixelData[6] = (a & 0x00FF) % 0xFF;
	dynamixelData[7] = (a >> 8) % 0xFF;
	dynamixelData[10] = ~(dynamixelData[2] + dynamixelData[3] + dynamixelData[4]
			+ dynamixelData[5] + dynamixelData[6] + dynamixelData[7]
			+ dynamixelData[8] + dynamixelData[9]);
	dynamixelData[10] = dynamixelData[10] & 0xFF;
	uart3_sendArray(dynamixelData, 12);
}

/*
 * processes a message received by UART. Messages should end with '\n'.
 */
void messageProcessor(char* msg, int msgLength)
{
	messageReceived(msg, msgLength);
}

//****************************************************//
//***************__I2C_protocol__*********************//
//****************************************************//

/*
 * write one byte to slave device; adress - device adress, data - array name, length - length of the array
 * function called only in gyro config (once at the begining of the main program)
 */

void __i2c_write(uint8_t address, uint8_t* data, uint32_t length)
{
	uint32_t dummy;

	I2C1_CR1_START_bb = 1;              // request a start
	while (I2C1_SR1_SB_bb == 0)
		;        // wait for start to finish
	dummy = I2C1->SR1;                  // read of SR1 clears the flag
	I2C1->DR = address;                 // transfer address
	while (I2C1_SR1_ADDR_bb == 0)
		;      // wait for address transfer
	dummy = I2C1->SR1;                  // clear the flag
	dummy = I2C1->SR2;

	while (length > 0)                  // transfer whole block
	{
		while (I2C1_SR1_TxE_bb == 0)
			;      // wait for DR empty
		I2C1->DR = *data++;              // transfer one byte, increment pointer
		length--;
	}

	while (I2C1_SR1_TxE_bb == 0 || I2C1_SR1_BTF_bb == 1)
		;   // wait for bus not-busy
	I2C1_CR1_STOP_bb = 1;               // request a stop
}

//read 6 bytes from slave device; address - device address, reg_address - first reg. address
//data - name of the array to store measured values
void __i2c_read(uint8_t address, uint8_t reg_address, uint8_t* data)
{

	uint32_t dummy;
	uint8_t iter = 0;

	I2C1_CR1_START_bb = 1;
	while ( I2C1_SR1_SB_bb == 0)
		;
	I2C1->DR = address;
	while (I2C1_SR1_ADDR_bb == 0)
		;
	dummy = I2C1->SR2;
	while (I2C1_SR1_TxE_bb == 0)
		;
	I2C1->DR = reg_address | 0x80; //0x80 - causes auto-incrementation (readed block of data)
	while (I2C1_SR1_BTF_bb == 0)
		;
	I2C1_CR1_START_bb = 1;
	while (I2C1_SR1_SB_bb == 0)
		;
	I2C1->DR = address | 0x01;
	while (I2C1_SR1_ADDR_bb == 0)
		;
	dummy = I2C1->SR2;

	while (iter < 6)
	{
		if (iter == 5)
			I2C1_CR1_ACK_bb = 0;

		while ( I2C1_SR1_RxNE_bb == 0)
			;
		data[iter] = I2C1->DR;
		iter++;

	}

	I2C1_CR1_STOP_bb = 1;
	I2C1_CR1_ACK_bb = 1;
}

//****************************************************//
//***************__I2C_protocol_END__*****************//
//****************************************************//

/*
 * sends a message via UART, max msg length is 1000
 * message has to end with newline character ('\n')
 */

//sends values of state variables via UART
