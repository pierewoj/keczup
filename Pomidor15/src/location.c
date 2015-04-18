#include "strategy.h"
#include "settings.h"
#include "global.h"
#include "geometry.h"
#include "math.h"
#include "location.h"

Point nextCrossroad;

/*
 * last value of gyroDirection reading. Difference between current and last value
 * is used to update "direction".
 */
double lastGyroDirection;

/*
 * last values of total mm traveled by left/right wheel. Differences between
 * current and last values are used to update location.
 */
long lastTotalDistanceLeft, lastTotalDistanceRight;
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
	//updating time of crossroad visit
	PointMM nearestCrossroadMM = getNearestCrossroad(position);
	Point nearestCrossroad = ofPointMM(nearestCrossroadMM);

	if (distance(position, nearestCrossroadMM) < 50
			&& pointValid(nearestCrossroad))
		visitTimes[nearestCrossroad.i][nearestCrossroad.j] = getMiliseconds(); //ms

	//updating direction
	direction += angleDifference(lastGyroDirection, gyroDirection);
	direction = angleMakeInRange(direction);

	//updating position
	int dR = totalDistanceRight - lastTotalDistanceRight;
	int dL = totalDistanceLeft - lastTotalDistanceLeft;
	double dMid = (dR + dL) / 2;
	position.x += dMid * cos(DEG_TO_RAD * direction);
	position.y += dMid * sin(DEG_TO_RAD * direction);

	//saving current values
	lastGyroDirection = gyroDirection;
	lastTotalDistanceLeft = totalDistanceLeft;
	lastTotalDistanceRight = totalDistanceRight;

	/*
	 * direction and position are snapped if there is line under more than
	 * 3 KTIRS (maybe setting for this value??)
	 */
	int i, numBlackFrontKtir = 0;
	for (i = 0; i < 7; i++)
		if (ktirFront[i])
			numBlackFrontKtir++;

	if (numBlackFrontKtir > 10)
	{
		//direction snap
		direction = roundToTheMultipleOf(direction, 90);
		direction = angleMakeInRange(direction);

		//finding vector from center of robot to its front KTIR line
		Vector fromCenterToKtir = vectorOfDirection(direction);
		fromCenterToKtir = vectorMultiplyByScalar(fromCenterToKtir,
				settingDistanceMidToKtir);

		//finding position of corssrad nearest to the position of ktirLine
		PointMM nearestCrossroadToKtirLine = getNearestCrossroad(
				translateByVector(position, fromCenterToKtir));

		//position is posKtirLine - fromCenterToKtir
		position = translateByVector(nearestCrossroadToKtirLine,
				vectorMultiplyByScalar(fromCenterToKtir, -1));
	}

}

/*
 * detects enemy using sonars. That information is used to count path to the
 * target. Enemy detection times are saved in enemyTimes[][] array.
 */
void updateEnemyPosition(void)
{

	//direction of ultras for direction=0 (east)
	Vector ultraDirections[4] =
	{
	{ 1, 0 },
	{ 0, -1 },
	{ -1, 0 },
	{ 0, 1 } };

	//rotating ultra directions based on direction of the robot
	int i;
	for (i = 0; i < 4; i++)
	{
		ultraDirections[i] = rotateByDegrees(ultraDirections[i], direction);
	}

	Point enemyPositions[4];
	int enemyCount = 0;

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
		enemyTimes[enemy.i][enemy.j] = getMiliseconds(); //ms
	}
}

/*
 *	It is a cost function for finding next neighbor
 *	its value is minimized (best neighbor = small value)
 */
double countCrossradCost(Point p, Point nearestCrossroad)
{
	double result = 0;

	//adding penalty for being current crossroad
	if(p.i == nearestCrossroad.i && p.j == nearestCrossroad.j)
			result += settingLocationWeightCurrent;

	// distance to the target via this crossroad
	double distanceToTheTarget = distanceManhattan(position, ofPoint(p))
			+ distanceManhattan(ofPoint(p), ofPoint(getRecentTarget()));
	result += settingLocationWeightDistance * distanceToTheTarget;

	// robot should prefer crossroads which were visited ealier
	double msSinceLastVisit = getMiliseconds() - visitTimes[p.i][p.j];
	result += settingLocationWeightVisitTime * msSinceLastVisit;

	// avoiding going to our baseline if we are not carrying can
	if (p.j == 0 && !carryingCan)
		result += settingLocationWeightBaseline;

	// avoiding enemies
	if (getMiliseconds() - enemyTimes[p.i][p.j] < settingLocationTimeEnemy)
		result += settingLocationWeightEnemy;

	return result;
}

/*
 * helper function used to add candidates for next crossroads to the array
 */
void addCandidate(Point* candidates, int* numCandidates, PointMM candidate)
{
	if (pointValid(ofPointMM(candidate)))
	{
		candidates[*numCandidates] = ofPointMM(candidate);
		(*numCandidates)++;
	}
}

/*
 * updates next crossroads based on the location of enemy, can possession,
 * time of crossroads' visit
 */
void updateNextCrossroad(void)
{
	Point candidates[5];
	int numCandidates = 0;

	PointMM nearestCrossroadMM = getNearestCrossroad(position);
	Point nearestCrossroad = ofPointMM(nearestCrossroadMM);

	//nearest crossroad is always a candidate
	addCandidate(candidates, &numCandidates, nearestCrossroadMM);

	Vector neighbourDirections[4] =
	{
	{ 0, 300 },
	{ 300, 0 },
	{ 0, -300 },
	{ -300, 0 } };

	//add all
	int i;
	for (i = 0; i < 4; i++)
		addCandidate(candidates, &numCandidates,
				translateByVector(nearestCrossroadMM, neighbourDirections[i]));

	//finding a candidate for crossroad as the one with the lowest cost
	double minCost = countCrossradCost(nextCrossroad, nearestCrossroad);
	for (i = 0; i < numCandidates; i++)
		if (countCrossradCost(candidates[i], nearestCrossroad) < minCost)
		{
			nextCrossroad = candidates[i];
			minCost = countCrossradCost(candidates[i], nearestCrossroad);
		}

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
	if (distanceToNextCrossroad() < settingCrossroadRadius) //Setting?
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
	double dst = distance(ofPoint(getRecentTarget()), position);
	return dst < settingCrossroadRadius;
}
