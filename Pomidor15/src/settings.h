/*
 * settiings.h
 *
 *  Created on: Mar 18, 2015
 *      Author: Kuba
 */

#ifndef SETTIINGS_H_
#define SETTIINGS_H_

//distance in MM between center of robot to the front KTIR line
double settingDistanceMidToKtir;

//importance of distance to the target in finding nextCrossroad
double settingLocationWeightDistance;

//importance of last time of visit in finding nextCrossroad
double settingLocationWeightVisitTime;

//importance of avoiding driving on our baseline in finding nextCrossroad
double settingLocationWeightBaseline;

//how many seconds it takes to forget enemy location?
double settingLocationTimeEnemy;

//importance of avoiding enemy in finding nextCrossroad
double settingLocationWeightEnemy;

//PWMmax for driving between crossroads
double settingPIDForwardPWM;

//Angle to target to change from GO to ROTATE (degrees)
double settingAngleToBeginRotate;

//Minimum Sharp reading in cm to detect can
double settingSharpThresh;

#endif /* SETTIINGS_H_ */
