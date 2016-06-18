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
int roundToTheMultipleOf(const double a, const int multipleOf)
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

bool equals(Point a, Point b)
{
	return a.i == b.i && a.j == b.j;
}

/*
 * Conversion Point -> PointMM
 */
PointMM ofPoint(const Point p)
{
	PointMM result;
	result.x = p.i * 200.;
	result.y = p.j * 200.;
	return result;
}

/*
 * Conversion PointMM -> Point
 */
Point ofPointMM(const PointMM p)
{
	Point result;
	result.i = roundToTheMultipleOf(p.x, 200) / 200;
	result.j = roundToTheMultipleOf(p.y, 200) / 200;
	return result;
}

/*
 * Distance between points (Euclidean)
 */
double distance(const PointMM a, const PointMM b)
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
double distanceManhattan(const PointMM a, const PointMM b)
{
	return fabs(a.x - b.x) + fabs(a.y - b.y);
}

/*
 * Every angle in this program should be [-180; 180].
 * For any angle a it makes sure is within this range.
 *
 * Link: http://stackoverflow.com/questions/1878907/
 * 		the-smallest-difference-between-2-angles
 * Test: http://ideone.com/k8eSoD
 */
double angleMod(const double a, const int n)
{
	return a - floor(a / n) * n;
}

double angleMakeInRange(const double a)
{
	return angleMod((a + 180), 360) - 180;
}

/*
 * returns difference between angles (to - from)
 */
double angleDifference(const double from, const double to)
{
	return angleMakeInRange(to - from);
}

/*
 * returns an angle between [0,1] and vector v
 */
double vectorAngle(const Vector v)
{
	return RAD_TO_DEG * atan2(v.y, v.x);
}

Vector vectorBetweenPoints(const PointMM from, const PointMM to)
{
	Vector res =
	{ to.x - from.x, to.y - from.y };
	return res;
}

Vector vectorNormalize(Vector v)
{
	double length = hypot(v.x, v.y);
	v.x /= length;
	v.y /= length;
	return v;
}

PointMM translateByVector(PointMM p, const Vector v)
{
	p.x += v.x;
	p.y += v.y;
	return p;
}

Vector vectorMultiplyByScalar(Vector v, const double s)
{
	v.x *= s;
	v.y *= s;
	return v;
}

Vector vectorOfDirection(const double a)
{
	Vector v =
	{ cos(a), sin(a) };
	return v;
}

Vector rotateByDegrees(Vector v, const double deg)
{
	Vector res = v;
	res.x = v.x * cos(DEG_TO_RAD * deg) - v.y * sin(DEG_TO_RAD * deg);
	res.y = v.x * sin(DEG_TO_RAD * deg) + v.y * cos(DEG_TO_RAD * deg);

	return res;
}

