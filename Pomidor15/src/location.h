#ifndef LOCATION_H_
#define LOCATION_H_

/*
 * functions in this file provide finding a way to the recent target
 * (Point getRecentTarget() from "staregy.c")
 *
 * Changing current target should be done "manually" in states.
 */

/*
 * updates robot position, direction and performs snap
 */
void updateOurPosition(void);

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

#endif 
