/*
 * sensors.h
 *
 *  Created on: Mar 22, 2015
 *      Author: Piotrek
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include <stddef.h>
#include "stm32f10x.h"

//auxsiliary variables

//ADC
extern volatile unsigned int pomiar_adc[2];

//IMU
extern volatile unsigned char dane_z_imu[6];
extern volatile unsigned char imu[2];
extern volatile int gyro_x, gyro_y, gyro_z;
extern volatile int gyro_initial_values[3];
#define gyro_calibration_sample_number 50

void gyro_update_direction(void);

//ULTRADZWIEKI
extern unsigned short int pwm_tim_count;
extern volatile unsigned int ultra__[4], ultra_pom[4];
extern volatile char pierwsze_zbocze[4];
#define ultra_distance_max 65535
#define const_distance_from_the_middle_of_the_robot 10    //(9.55 cm - approx. mean distance)
#define linear_coefficient_distance 46.4

void ultra_data_processing(void);

//KTIRs
void read_ktirs(void);

//Encoders
void encodersReset(void);
void encodersInit(void);
void encodersRead(void);

extern double V_L, V_R;
#define REV_COUNT 				1021
#define IMP_DIST				0.1122226619
#define ENC_L_TIM              	TIM1                     // timer's macros
#define ENC_R_TIM              	TIM4

extern unsigned int time_old;
extern volatile int16_t leftCount;								//speeds
extern volatile int16_t rightCount;
extern volatile int16_t fwdCount;
extern volatile int16_t rotCount;
//distances
extern volatile int32_t leftTotal;
extern volatile int32_t rightTotal;
extern volatile int32_t fwdTotal;
extern volatile int32_t rotTotal;


//read all sensors and update sensor values defined in "global.h"
void readSensors(void);

inline void sharp_update(void);

inline void battery_update(void);
#endif /* SENSORS_H_ */
