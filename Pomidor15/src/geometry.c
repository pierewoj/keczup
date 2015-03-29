/*
 * geometry.c
 *
 *  Created on: Mar 18, 2015
 *      Author: Kuba
 */
#include "geometry.h"
#include "math.h"

/*
 * returns the nearest multiple of "mulfipleOf" to "a"
 * tested: http://ideone.com/vPpGfL
 */
int roundToTheMultipleOf(double a, int multipleOf)
{
	if (a > 0)
	{
		if ((((int) a) % multipleOf) < ((multipleOf + 1) / 2))
			return (((int) a) / multipleOf) * multipleOf;
		else
			return (((int) a) / multipleOf + 1) * multipleOf;
	}
	else
	{
		if ((((int) a) % multipleOf) > -((multipleOf + 1) / 2))
			return (((int) a) / multipleOf) * multipleOf;
		else
			return (((int) a) / multipleOf - 1) * multipleOf;
	}
}

/*
 * Conversion Point -> PointMM
 */
PointMM ofPoint(Point p)
{
	PointMM result;
	result.x = p.i * 300.;
	result.y = p.j * 300.;
	return result;
}

/*
 * Conversion PointMM -> Point
 */
Point ofPointMM(PointMM p)
{
	Point result;
	result.i = roundToTheMultipleOf(p.x, 300) / 300;
	result.j = roundToTheMultipleOf(p.y, 300) / 300;
	return result;
}

/*
 * Distance between points (Euclidean)
 */
double distance(PointMM a, PointMM b)
{
	/*
	 * hypot is a function from math.h
	 * http://www.cplusplus.com/reference/cmath/hypot/
	 */
	return hypot(a.x - b.x, a.y - b.y);
}

/*
 * Distance between points in manhattan metric
 * d(a,b) = |a.x-b.x| + |a.y-b.y|
 */
double distanceManhattan(PointMM a, PointMM b)
{
	return fabs(a.x-b.x) + fabs(a.y-b.y);
}

/*
 * Every angle in this program should be [-180; 180].
 * For any angle a it makes sure is within this range.
 *
 * Link: http://stackoverflow.com/questions/1878907/
 * 		the-smallest-difference-between-2-angles
 * Test: http://ideone.com/k8eSoD
 */
double angleMod(double a, int n)
{
	return a - floor(a / n) * n;
}

double angleMakeInRange(double a)
{
	return angleMod((a + 180), 360) - 180;
}

/*
 * returns difference between angles (to - from)
 */
double angleDifference(double from, double to)
{
	return angleMakeInRange(to - from);
}

/*
 * returns an angle between [0,1] and vector v
 */
double vectorAngle(Vector v)
{
	return RAD_TO_DEG * atan2(v.y, v.x);
}

Vector vectorBetweenPoints(PointMM from, PointMM to)
{
	Vector res = {to.x - from.x , to.y - from.y};
	return res;
}

Vector vectorNormalize(Vector v)
{
	double length = hypot(v.x,v.y);
	v.x /= length;
	v.y /= length;
	return v;
}

PointMM translateByVector(PointMM p, Vector v)
{
	p.x += v.x;
	p.y += v.y;
	return p;
}

Vector vectorMultiplyByScalar(Vector v, double s)
{
	v.x *= s;
	v.y *= s;
	return v;
}

Vector vectorOfDirection(double a)
{
	Vector v = {cos(a), sin(a)};
	return v;
}



