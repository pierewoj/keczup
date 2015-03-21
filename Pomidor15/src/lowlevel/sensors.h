/*
 * sensors.h
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include <stddef.h>
#include "stm32f10x.h"

//auxsiliary variables

//ADC
volatile unsigned int pomiar_adc[2];

//IMU
unsigned char dane_z_imu[6];
unsigned char imu[2];
int gyro_x, gyro_y, gyro_z;
int gyro_initial_values[3];
#define gyro_calibration_sample_number 50

//ULTRADZWIEKI
unsigned int ultra__[3], ultra_pom[3];
unsigned int ultra1, ultra2, ultra3;
char pierwsze_zbocze[3];
#define ultra_distance_max 65535

//Markery
double PWM_L, PWM_R, POSX, POSY, dist_L, dist_R;

//KTIRs
void read_ktirs(void);

//Encoders
void encodersReset(void);
void encodersInit(void);
void encodersRead(void);

double V_L, V_R;
#define REV_COUNT 				1021
#define IMP_DIST				0.131946864
#define ENC_L_TIM              	TIM1                     // timer's macros#define ENC_R_TIM              	TIM4

volatile int16_t leftCount;								//speeds
volatile int16_t rightCount;
volatile int16_t fwdCount;
volatile int16_t rotCount;
//distances
volatile int32_t leftTotal;
volatile int32_t rightTotal;
volatile int32_t fwdTotal;
volatile int32_t rotTotal;

// local variables
static volatile int16_t oldLeftEncoder;
static volatile int16_t oldRightEncoder;
static volatile int16_t leftEncoder;
static volatile int16_t rightEncoder;
static volatile int16_t encoderSum;
static volatile int16_t encoderDiff;

//read all sensors and update sensor values defined in "global.h"
void readSensors(void);

inline void sharp_update(void);

inline void battery_update(void);
#endif /* SENSORS_H_ */
