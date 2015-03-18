#include "strategy.h"
#include "settings.h"
#include "global.h"
#include "geometry.h"
#include "math.h"

Point nextCrossroad;

/*
 * returns the nearest crossroad to "point"
 */
PointMM getNearestCrossroad(PointMM pos)
{
	Point p = ofPointMM(pos);
	PointMM res = ofPoint(p);
	return res;
}

/*
 * true if the crossroad has valid position
 */
bool pointValid(Point p)
{
	return ((p.i >= 0) && (p.j >= 0) && (p.i <= 4) && (p.j <= 4));
}

/*
 * updates robot position, direction and performs snap
 */
void updateOurPosition(void)
{
	//updating direction
	direction = gyroDirection;

	//snap
	int i, numBlackFrontKtir = 0;
	for (i = 0; i < 7; i++)
		if (ktirFront[i])
			numBlackFrontKtir++;

	if (numBlackFrontKtir > 3)
	{
		//direction snap
		direction = roundToTheMultipleOf(direction, 90);
		direction = angleMakeInRange(direction);

		//position of nearest crossroad
		PointMM nearestCrossroad = getNearestCrossroad(position);

		//v is a vector from the front ktir line to the middle of robot.
		Vector v = vectorOfDirection(direction);
		v = vectorMultiplyByScalar(v, -settingDistanceMidToKtir);

		//position snap
		position = translateByVector(nearestCrossroad, v);
	}

}

/*
 * detects enemy using sonars. That information is used to count path to the
 * target.
 */
void updateEnemyPosition(void)
{

	Vector ultraDirections[4] =
	{
	{ 0, 1 },
	{ 1, 0 },
	{ 0, -1 },
	{ -1, 0 } };
	Point enemyPositions[4];
	int i, enemyCount = 0;

	//check ultra readings and generate list of enemy positions
	for (i = 0; i < 4; i++)
		if (ultra[i] < 300)
		{
			//vector from middle of robot to the enemy.
			// multiplied by 10 because ultra readings are in cm, while pos in mm
			Vector v = vectorMultiplyByScalar(ultraDirections[i],
					ultra[i] * 10);

			PointMM enemyMM = translateByVector(position, v);
			Point enemyPos = ofPointMM(enemyMM);
			if (pointValid(enemyPos))
			{
				enemyPositions[enemyCount] = enemyPos;
				enemyCount++;
			}
		}

	//update time for all detected enemies
	for (i = 0; i < enemyCount; i++)
	{
		Point enemy = enemyPositions[i];
		enemyTimes[enemy.i][enemy.j] = time / 1000; //ms
	}

}

/*
 * updates next crossroads based on the location of enemy, can possession,
 * time of crossroads' visit
 */
void updateNextCrossroads(void)
{
}

/*
 * returns the distance to current next crossroads (mm)
 */
double distanceToNextCrossroad(void)
{
	return distance(position, ofPoint(nextCrossroad));
}

/*
 * returns the difference between our direction and the direction to the
 * recent next crossroads (in degrees)
 */
double angleToNextCrossroad(void)
{
	if (distanceToNextCrossRoad() < 50) //Setting?
		return 0;
	else
	{
		Vector toCrossroad = vectorBetweenPoints(position,
				ofPoint(nextCrossroad));
		return angleDifference(direction, vectorAngle(toCrossroad));
	}
}

/*
 * returns true if robot reached the targetPosition
 */
bool targetReached()
{
	//not implemented
}
