#ifndef LOCATION_H_
#define LOCATION_H_

#include "stdbool.h"
#include "geometry.h"
/*
 * functions in this file provide finding a way to the recent target
 * (Point getRecentTarget() from "staregy.c")
 *
 * Changing current target should be done "manually" in states.
 */

/*
 *  next crossroad is the crossroad to be visited next
 *  previous crossroad is used in state 'goPrevious'
 */
extern Point nextCrossroad;
extern Point previousCrossroad;

/*
 * updates robot position, direction and performs snap
 */
void updateOurPosition(void);

/*
 * returns true if enemy was detected on a given crossroad
 */
bool isEnemy(Point a);

/*
 * returns the nearest crossroad to "point"
 */
PointMM getNearestCrossroad(PointMM pos);

/*
 * snaps direction and BOTH coordinates
 */
void snapPositionAndDirection(void);

/*
 * snaps direction and ONE of coordinates depending on direction of the robot
 */
void snapHorizontalVerticalDirection(void);

/*
 * detects enemy using sonars. That information is used to count path to the
 * target.
 */
void updateEnemyPosition(void);

/*
 * updates next crossroads based on the location of enemy, can possession,
 * time of crossroads' visit
 */
void updateNextCrossroad(void);

/*
 * returns the distance to current next crossroads (mm)
 */
double distanceToNextCrossroad(void);

/*
 * returns the difference between our direction and the direction to the
 * recent next crossroads (in degrees)
 */
double angleToNextCrossroad(void);

/*
 * returns true if robot reached the targetPosition
 */
bool targetReached();

/*
 * returns number of elements in queue
 */
int queueSize();

#endif 
