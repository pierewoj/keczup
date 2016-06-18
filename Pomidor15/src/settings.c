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

//how many miliseconds it takes to forget enemy location?
double settingLocationTimeEnemy = 3000;

//PWMmax for driving between crossroads
double settingPIDForwardPWM = 0.4;

//Angle to target to change from GO to ROTATE (degrees)
double settingAngleToBeginRotate = 50;

//Minimum Sharp reading in cm to detect can
double settingSharpThresh = 10;

//Send uart messages every X microseconds
double settingBluetoothInterval = 100*1000;

//Max speed during rotations
double settingMaxRotationVelocity = 300;

//Robot is at crossroad when distance to is is less than this value
double settingCrossroadRadius = 70;

//Distance between cans on our baseline [mm]
double settingDistanceBetweenCans = 60;

//Distance between center of can and center of robot [mm]
double settingDistanceCanMiddle = 100;

//Distance between target position in leaving can to start going back [mm]
double settingDistanceToGoBack = 30;

//Distance from enemy to go to previous crossroad [cm]
double settingDistanceToRunFromEnemy = 25;

/*
 * Engine saving sytem.
 * Robot will stop if PWM is higher than settingPWMToBlocked and velociuy of any wheel
 * is lower than settingVelocityToblocked for longer than settingTimeBlockedToFail
 */
int settingTimeBlockedToFail = 600;

double settingVelocityToBlocked = 30;

double settingPWMToBlocked = 0.25;
