/*
 * settings.c
 *
 *  Created on: Mar 18, 2015
 *      Author: Kuba
 */


//distance in MM between center of robot to the front KTIR line
double settingDistanceMidToKtir = 70;

//importance of distance to the target in finding nextCrossroad
double settingLocationWeightDistance = 10;

//importance of last time of visit in finding nextCrossroad
double settingLocationWeightVisitTime = 0.1;

//importance of avoiding driving on our baseline in finding nextCrossroad
double settingLocationWeightBaseline = 60000;

//how many miliseconds it takes to forget enemy location?
double settingLocationTimeEnemy = 5000;

//importance of avoiding enemy in finding nextCrossroad
double settingLocationWeightEnemy = 5000;