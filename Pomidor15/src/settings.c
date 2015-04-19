/*
 * settings.c
 *
 *  Created on: Mar 18, 2015
 *      Author: Kuba
 */


//distance in MM between center of robot to the front KTIR line
double settingDistanceMidToKtir = 60;

//distance from baseline to center of robot at the beggining of the round
double settingDistanceMidBeginning = 80;

//importance of distance to the target in finding nextCrossroad
double settingLocationWeightDistance = 10;

//importance of last time of visit in finding nextCrossroad
double settingLocationWeightVisitTime = -0.001;

//importance of avoiding driving on our baseline in finding nextCrossroad
double settingLocationWeightBaseline = 1000000;

//importance of changing crossroads (avoiding stuck)
double settingLocationWeightCurrent = 50000;

//how many miliseconds it takes to forget enemy location?
double settingLocationTimeEnemy = 5000;

//importance of avoiding enemy in finding nextCrossroad
double settingLocationWeightEnemy = 500000;

//PWMmax for driving between crossroads
double settingPIDForwardPWM = 0.5;

//Angle to target to change from GO to ROTATE (degrees)
double settingAngleToBeginRotate = 50;

//Minimum Sharp reading in cm to detect can
double settingSharpThresh = 8;

//Send uart messages every X microseconds
double settingBluetoothInterval = 100*1000;

//Max speed during rotations
double settingMaxRotationVelocity = 300;

//Robot is at crossroad when distance to is is less than this value
double settingCrossroadRadius = 65;

//Distance between cans on our baseline [mm]
double settingDistanceBetweenCans = 60;

//Distance between center of can and center of robot [mm]
double settingDistanceCanMiddle = 100;

//Distance between target position in leaving can to start going back [mm]
double settingDistanceToGoBack = 30;
