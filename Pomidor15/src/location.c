#include "strategy.h"
#include "settings.h"
#include "global.h"
#include "geometry.h"
#include "math.h"

/*
 * position of next crossroad to be visited. Should be accessed directly from
 * current position
 */
Point nextCrossroad;

/*
 * last value of gyroDirection reading. Difference between current and las value
 * is used tu update "direction".
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
		visitTimes[nearestCrossroad.i][nearestCrossroad.j] = time/1000; //ms


	//updating direction
	direction += angleDifference(lastGyroDirection, gyroDirection);
	direction = angleMakeInRange(direction);

	//updating position
	int dR = totalDistanceRight - lastTotalDistanceRight, dL = totalDistanceLeft
			- lastTotalDistanceLeft;
	double dMid = (dR + dL) / 2;
	position.x += dMid * cos(DEG_TO_RAD * direction);
	position.y += dMid * sin(DEG_TO_RAD * direction);

	//saving current values
	lastGyroDirection = gyroDirection;
	lastTotalDistanceLeft = totalDistanceLeft;
	lastTotalDistanceRight = totalDistanceRight;

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
 * target. Enemy detection times are saved in enemyTimes[][] array.
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
 *	It is a cost function for finding next neighbour
 *	its value is minimized (best neighbour = small value)
 */
double countCrossradCost(Point p)
{
	double result = 0;

	// distance to the target via this crossroad
	double distanceToTheTarget = distanceManhattan(position, ofPoint(p))
			+ distanceManhattan(ofPoint(p), ofPoint(getRecentTarget()));
	result += settingLocationWeightDistance * distanceToTheTarget;

	// robot should prefer crossroads which were visited ealier
	double msSinceLastVisit = time / 1000 - visitTimes[p.i][p.j];
	result -= settingLocationWeightVisitTime * msSinceLastVisit;

	// avoiding going to our baseline if we are not carrying can
	if (p.j == 0 && !carryingCan)
		result += settingLocationWeightBaseline;

	// avoiding enemies
	if (time / 1000 - enemyTimes[p.i][p.j] < settingLocationTimeEnemy)
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

	PointMM nearestCrossroad = getNearestCrossroad(position);

	//nearest crossroad is always a candidate
	addCandidate(candidates, &numCandidates, nearestCrossroad);

	//robot is between crossroads, only 2 candidates.
	if (distance(position, nearestCrossroad) > 50)
	{
		//counting position of the other crossroad on the current line
		Vector v = vectorBetweenPoints(position, nearestCrossroad);
		v = vectorNormalize(v);
		v = vectorMultiplyByScalar(v, -300); //300mm in the rev. direction
		PointMM otherCrossroad = translateByVector(nearestCrossroad, v);

		addCandidate(candidates, &numCandidates, otherCrossroad);
	}

	/*
	 * robot is on a crossroad, add all neighbour crossroads, checking if
	 * position is valid (for ex if point is not (-500,-600)) is done in
	 * addCandidate() function
	 */
	else
	{
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
					translateByVector(nearestCrossroad,
							neighbourDirections[i]));

	}

	//finding a candidate for crossroad as the one with the lowest cost
	double minCost = countCrossradCost(nextCrossroad);
	int i;
	for (i = 0; i < numCandidates; i++)
		if (countCrossradCost(candidates[i]) < minCost)
		{
			nextCrossroad = candidates[i];
			minCost = countCrossradCost(candidates[i]);
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
	if (distanceToNextCrossroad() < 50) //Setting?
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
	return dst < 50;
}