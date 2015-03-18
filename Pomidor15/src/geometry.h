/*
 * geometry.h
 *
 *  Created on: Mar 18, 2015
 *      Author: Kuba
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

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

/*
 * Conversion Point -> PointMM
 */
PointMM ofPoint(Point p);

/*
 * Conversion PointMM -> Point
 */
Point ofPointMM(PointMM p);

/*
 * returns the nearest multiple of "mulfipleOf" to "a"
 * tested: http://ideone.com/vPpGfL
 */
int roundToTheMultipleOf(double a, int multipleOf);

/*
 * Distance between points
 */
double distance(PointMM a, PointMM b);

/*
 * Every angle in this program should be [-180; 180].
 * For any angle a it makes sure is within this range.
 */

double angleMakeInRange(double a);

/*
 * returns difference between angles (to - from)
 */
double angleDifference(double from, double to);

/*
 * returns an angle between [0,1] and vector v
 */
double vectorAngle(Vector v);

Vector vectorBetweenPoints(PointMM from, PointMM to);

Vector vectorNormalize(Vector v);

PointMM translateByVector(PointMM p, Vector v);

Vector vectorMultiplyByScalar(Vector v, double s);

Vector vectorOfDirection(double a);

#endif /* GEOMETRY_H_ */
