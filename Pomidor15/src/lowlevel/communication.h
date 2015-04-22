/*
 * communication.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <stddef.h>
#include "stm32f10x.h"
#include "stdbool.h"

//***********************__USART__*********************************//

extern unsigned short int U3_bufTxIndex,U3_bufTxMaxIndex;
extern char U2_buforRx[300];
extern int U2_buforRx_Size;
extern int U2_bufRxIndex;
extern int usart_data_number;
extern unsigned short int U3_buforTx[50];
extern bool flaga;


void dynamixel_ustawPozycje(double procent);

void USART3_IRQHandler(void);

void USART2_IRQHandler(void);

void uart3_sendArray(unsigned short int *arr, unsigned short int n);

//processes a message received by UART. Messages shuold end with '\n'
void messageProcessor(char* msg, int msgLength);


//************************__I2C_protocol__*************************//

#define BITBAND_PERI_REF   0x40000000
#define BITBAND_PERI_BASE  0x42000000
#define BITBAND_PERI(a,b) ((BITBAND_PERI_BASE + ((unsigned long)a-BITBAND_PERI_REF)*32 + (b*4)))  // Convert PERI address
#define I2C1_CR1_START_bb *((volatile unsigned long*) BITBAND_PERI(&I2C1->CR1,8))
#define I2C1_CR1_PE_bb *((volatile unsigned long*) BITBAND_PERI(&I2C1->CR1,0))
#define I2C1_CR1_STOP_bb *((volatile unsigned long*) BITBAND_PERI(&I2C1->CR1,9))
#define I2C1_CR1_ACK_bb *((volatile unsigned long*) BITBAND_PERI(&I2C1->CR1,10))
#define I2C1_SR1_TxE_bb *((volatile unsigned long*) BITBAND_PERI(&I2C1->SR1,7))
#define I2C1_SR1_ADDR_bb *((volatile unsigned long*) BITBAND_PERI(&I2C1->SR1,1))
#define I2C1_SR1_SB_bb *((volatile unsigned long*) BITBAND_PERI(&I2C1->SR1,0))
#define I2C1_SR1_BTF_bb *((volatile unsigned long*) BITBAND_PERI(&I2C1->SR1,2))
#define I2C1_SR1_RxNE_bb *((volatile unsigned long*) BITBAND_PERI(&I2C1->SR1,6))
#define I2C1_CR1_SWRST_bb *((volatile unsigned long*) BITBAND_PERI(&I2C1->CR1,15))
#define imu_write_address 0xD6
#define gyro_LOW_ODR 0x39
#define gyro_CTRL_REG4 0x23
#define gyro_CTRL_REG1 0x20

void __i2c_write(uint8_t address, uint8_t* data, uint32_t length);
void __i2c_read(uint8_t address,uint8_t reg_address, uint8_t* data);

//**********************************__I2C_protocol_END__*************************************//


#endif /* COMMUNICATION_H_ */
