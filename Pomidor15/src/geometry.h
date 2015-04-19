/*
 * geometry.h
 *
 *  Created on: Mar 18, 2015
 *      Author: Kuba
 */

#include <stdbool.h>

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#define DEG_TO_RAD (0.01745329251)
#define RAD_TO_DEG (57.2957795130)

typedef struct pointMM
{
	double x;
	double y;
} PointMM;

typedef struct vector
{
	double x;
	double y;
} Vector;

typedef struct point
{
	int i;
	int j;
} Point;

bool equals(Point a, Point b);

/*
 * Conversion Point -> PointMM
 */
PointMM ofPoint(const Point p);

/*
 * Conversion PointMM -> Point
 */
Point ofPointMM(const PointMM p);

/*
 * returns the nearest multiple of "mulfipleOf" to "a"
 * tested: http://ideone.com/vPpGfL
 */
int roundToTheMultipleOf(const double a, const int multipleOf);

/*
 * Distance between points (Euclidean metric)
 */
double distance(const PointMM a, const PointMM b);

/*
 * Distance between points in manhattan metric
 * d(a,b) = |a.x-b.x| + |a.y-b.y|
 */
double distanceManhattan(const PointMM a, const PointMM b);
/*
 * Every angle in this program should be [-180; 180].
 * For any angle a it makes sure is within this range.
 */

double angleMakeInRange(const double a);

/*
 * returns difference between angles (to - from)
 */
double angleDifference(const double from,const  double to);

/*
 * returns an angle between [0,1] and vector v
 */
double vectorAngle(const Vector v);

Vector vectorBetweenPoints(const PointMM from, const PointMM to);

Vector vectorNormalize(Vector v);

PointMM translateByVector(PointMM p, const Vector v);

Vector vectorMultiplyByScalar(Vector v, const double s);

Vector vectorOfDirection(const double a);

Vector rotateByDegrees(Vector v, const double deg);

#endif /* GEOMETRY_H_ */
