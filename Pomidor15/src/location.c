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

void snapPositionAndDirection()
{
	//direction snap
	direction = roundToTheMultipleOf(direction, 90);
	direction = angleMakeInRange(direction);

	//finding vector from center of robot to its front KTIR line
	position = getNearestCrossroad(position);

}

void snapHorizontalVerticalDirection()
{
	//snapping direction
	direction = roundToTheMultipleOf(direction, 90);
	direction = angleMakeInRange(direction);

	/*
	 * horizontal = true  when direction is E or W ( <- or -> )
	 * horizontal = false when direction is N or S ( /\ or \/ )
	 */
	bool horizontal = true;

	//setting horizontal to false if robot goes vertical
	if (fabs(angleDifference(direction, 90)) < 5
			|| fabs(angleDifference(direction, -90)) < 5)
	{
		horizontal = false;
	}

	if (horizontal)
		position.y = roundToTheMultipleOf(position.y, 300);
	else
		position.x = roundToTheMultipleOf(position.x, 300);

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
 * array of elements which have already been visited in BFS in
 * updateNextCrossroad()
 */
Point queue[25];

/*
 * returns time since last visit for a given crossroad
 */
bool visited[5][5];

inline int getMsSinceLastVisit(Point a)
{
	if (pointValid(a))
		return getMiliseconds() - visitTimes[a.i][a.j];
	else
		return 0;
}

/*
 * returns true if enemy was detected on a given crossroad
 */
inline bool isEnemy(Point a)
{
	if (pointValid(a))
		return getMiliseconds() - enemyTimes[a.i][a.j]
				< settingLocationTimeEnemy;
	else
		return true;

}

/*
 * Queue of points for BFS
 */
int queueFirstElem = 0, queueLastElem = 0;

void queueAddElem(Point a)
{
	if (queueLastElem < 25)
	{
		queue[queueLastElem] = a;
		queueLastElem++;
		visited[a.i][a.j] = true;
	}
}


inline int queueSize()
{
	return queueLastElem - queueFirstElem;
}

Point queuePop()
{
	queueFirstElem++;
	return queue[queueFirstElem - 1];
}

bool solutionFound;

/*
 * neighbours are added in the order of decreasing time since last visit
 * neighbourr is only added if visit[i][j] == false
 */
void queueAddNeighbours(Point a)
{
	int maxMsSinceLastVisit = -10;
	Point bestResult =
	{ -1, -1 };
	Point directions[4] =
	{
	{ 0, -1 },
	{ 0, 1 },
	{ -1, 0 },
	{ 1, 0 } };
	int i;

	/*
	 * finding not-added neighbour with longest time since last visit
	 */
	for (i = 0; i < 4; i++)
	{
		Point neighbour;
		neighbour.i = a.i + directions[i].i;
		neighbour.j = a.j + directions[i].j;

		if (!pointValid(neighbour))
			continue;

		//if currently analyzed neighbour is position then current crossroad
		//is should be the one we go to
		if (equals(neighbour, ofPointMM(position)))
		{
			solutionFound = true;
			return;
		}
		if (getMsSinceLastVisit(neighbour) > maxMsSinceLastVisit
				&& visited[neighbour.i][neighbour.j] == false
				&& !isEnemy(neighbour))
		{
			maxMsSinceLastVisit = getMsSinceLastVisit(neighbour);
			bestResult = neighbour;
		}
	}

	if (maxMsSinceLastVisit != -10)
	{
		queueAddElem(bestResult);

		//try to find next not-visited neighbour
		queueAddNeighbours(a);
	}
	else
		return;
}

/*
 * array of random targets. used if our base crossrad has enemy or
 * endGameTactic crosssroad has enemy
 */
Point randomTarget[4] =
{
{ 0, 1 },
{ 4, 1 },
{ 0, 3 },
{ 4, 3 } };
int randomTargetIndex = 0;

/*
 * updates next crossroads based on the location of enemy, can possession,
 * time of crossroads' visit
 * algorithm used is BFS
 */
void updateNextCrossroad(void)
{
//clearing visited
	int i, j;
	for (i = 0; i < 5; i++)
		for (j = 0; j < 5; j++)
		{
			//do not create path through baseline
			if (j == 0)
				visited[i][j] = true;
			else
				visited[i][j] = false;
		}
	queueFirstElem = 0;
	queueLastElem = 0;
	solutionFound = false;

	Point target = getRecentTarget();
	if (!isEnemy(target))
		queueAddElem(target);

	while (queueSize() > 0)
	{
		//currently analyzed point
		Point p = queuePop();
		queueAddNeighbours(p);
		if (solutionFound)
		{
			nextCrossroad = p;
			return;
		}
	}

	//what if no solution has been found?
	Point canTarget =
	{ 2, 0 }, endGameTarget =
	{ 2, 4 };

	/*
	 * add some random target if carrying can or endgame tactics
	 */
	if ((carryingCan && equals(target, canTarget))
			|| (endGameTacticsEnabled && equals(target, endGameTarget)))
	{
		Point random = randomTarget[randomTargetIndex];
		randomTargetIndex++;
		randomTargetIndex %= 4;
		addNewTarget(random.i, random.j);
	}
	/*
	 * otherwise ignore current target and find new one!
	 */
	else
	{
		removeRecentTarget();
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
