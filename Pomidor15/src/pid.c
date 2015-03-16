/*
 * pid.c
 *
 *  Created on: Mar 15, 2015
 *      Author: Kuba
 */
#include "global.h"
#include <math.h>

/*
 * counts the value of steering signal (output) of the PID controller
 * based on its state (kp,ti,td,feedback...).
 */
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
	s->propSignal = s->enabledP * s->kp * error;
	s->integralSignal = s->enabledI * s->kp / s->ti * s->integral ;
	s->diffSignal = s->enabledD * s->kp * s->td * d;

	//count total controller output
	s->output = s->propSignal + s->integralSignal + s->diffSignal;
}


/*
 * updates the value of feedback in controller s with an average position of
 * line under ktir[]. value is not updated if ktir[i]=false for all i.
 */
void updateFeedbackKTir(struct controllerState *s, bool *ktir, int numKtir)
{
	int numBlack = 0;
	double sum = 0;
	int i;
	for(i=0; i<numKtir; i++)
		if (ktir[i])
		{
			numBlack++;
			sum += i;
		}

	if(numBlack > 0)
		s -> feedback = sum / numBlack;
}
