/*
 * communication.c
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

#include "global.h"
#include <stdio.h>
#include <string.h>
#include "communication.h"

/*
 * sends message via UART, message hast to end with newline
 * character ('\n')
 */
void sendMessage(char* msg)
{
	int i = 0;
	for (i = 0; i < 1000; i++)
	{
		if (msg[i] == '\n')
		{
			break;
		}
	}

	uart2_sendArray(msg, i + 1 );
}

//****************************************************//
//********************__USART__***********************//
//****************************************************//

void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{

		if (U3_bufTxIndex == U3_bufTxMaxIndex - 1) //Jesli wysylany zostal ostatni znak (\r)
		{
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE); //Wylacz przerwanie = koniec transmisji
			U3_bufTxIndex = 0;
		}

		else
		{
			USART_SendData(USART3, U3_buforTx[U3_bufTxIndex]);
			U3_bufTxIndex++;

		}
	}
}

void USART2_IRQHandler(void)
{
	/*
	 if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	 {


	 if(U2_bufTxIndex==U2_bufTxMaxIndex-1)                   //Jesli wysylany zostal ostatni znak (\r)
	 {
	 USART_ITConfig(USART2, USART_IT_TXE, DISABLE);    //Wylacz przerwanie = koniec transmisji
	 U2_bufTxIndex = 0;
	 }

	 else{
	 USART_SendData(USART2, U2_buforTx[U2_bufTxIndex]);
	 U2_bufTxIndex++;

	 }
	 }
	 */
	if (USART_GetITStatus(USART2, USART_IT_RXNE))
	{
		U2_buforRx[U2_bufRxIndex] = USART_ReceiveData(USART2);
		U2_bufRxIndex++;

		if (U2_buforRx[U2_bufRxIndex - 1] == 10 || U2_bufRxIndex > 50)
		{
			U2_buforRx_Size = U2_bufRxIndex - 2;
			U2_bufRxIndex = 0;
			messageProcessor(U2_buforRx, U2_buforRx_Size);
		}

		//a = USART_ReceiveData(USART2);
		//USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

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

void uart2_sendArray(char *arr, unsigned short int n)
{
	U2_bufTxMaxIndex = n;
	int i;
	for (i = 0; i < n; i++)
	{
		U2_buforTx[i] = (unsigned short int) *arr;
		arr++;
	}
	usart_data_number = n;
	U2_bufTxIndex = 0;
	DMA_Config();
	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

void dynamixel_ustawPozycje(double procent)
{
//0.2 - poziomo z tylu
//0.8 - poziomo przod

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
	char command[20]; //maximum command length is 20
	sscanf(msg, "%20s", command);
	if (strcmp(command, "START") == 0) //received "START" command
	{
		sendMessage("JAZDA!!!!!!!\n");
	}
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
void sendAllVariables(void)
{
	char messageBuffer[400];
	snprintf(messageBuffer, 400, "%d\n", state);

	sendMessage(messageBuffer);
}
