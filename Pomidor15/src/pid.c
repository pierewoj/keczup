/*
 * pid.c
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */
#include "global.h"
#include <math.h>

void PID(struct controllerState* s)
{
	double error = s->target - s->feedback;

	//reset integral and last error if controller
	//has not been recalculated for a long time
	if( time - s->lastTimeDiff > 10 * loopWaitTime )
	{
		s -> integral = 0;
		s -> lastError = error;
	}

	//update history for D every "diffInterval" iterations
	if(time - s->lastTimeDiff > s->diffInteral * loopWaitTime )
	{
		s -> history[1] = s -> history[0];
		s -> history[0] = error;
		s -> lastTimeDiff = time;
	}
	double d = (s->history[0]) - (s->history[1]);

	//count integral
	s->integral += error;
	s->integral = fmax ( s->integral ,  - s->integralMax * s->ti);
	s->integral = fmin ( s->integral ,    s->integralMax * s->ti);

	//count output from P, I and D
	s->propSignal =
			s->enabledP * s->kp * error;

	s->integralSignal = s->enabledI * s->kp / s->ti * s->integral ;

	s->diffSignal = s->enabledD * s->kp * s->td * d;

	//count total controller output
	s->output = s->propSignal + s->integralSignal + s->diffSignal;
}
