/*
 * settiings.h
 *
 *  Created on: Mar 18, 2015
 *      Author: Kuba
 */

#ifndef SETTIINGS_H_
#define SETTIINGS_H_

//distance in MM between center of robot to the front KTIR line
extern double settingDistanceMidToKtir;

//distance from baseline to center of robot at the beggining of the round
extern double settingDistanceMidBeginning;

//importance of distance to the target in finding nextCrossroad
extern double settingLocationWeightDistance;

//importance of last time of visit in finding nextCrossroad
extern double settingLocationWeightVisitTime;

//importance of avoiding driving on our baseline in finding nextCrossroad
extern double settingLocationWeightBaseline;

//how many seconds it takes to forget enemy location?
extern double settingLocationTimeEnemy;

//importance of avoiding enemy in finding nextCrossroad
extern double settingLocationWeightEnemy;

//PWMmax for driving between crossroads
extern double settingPIDForwardPWM;

//Angle to target to change from GO to ROTATE (degrees)
extern double settingAngleToBeginRotate;

//Minimum Sharp reading in cm to detect can
extern double settingSharpThresh;

//Send uart message every X microseconds
extern double settingBluetoothInterval;

//Max speed during rotations
extern double settingMaxRotationVelocity;
#endif /* SETTIINGS_H_ */
