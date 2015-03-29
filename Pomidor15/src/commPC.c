/*
 * commPC.c
 *
 *  Created on: Mar 24, 2015
 *      Author: Kuba
 */
#include "lowlevel/communication.h"
#include "states/state.h"
#include "global.h"
#include "strategy.h"
#include "location.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "math.h"
#include "pid.h"
#include "settings.h"
#include "lowlevel/effectors.h"

/*
 * pushes new message to the queue of messages (to be sent by UART)
 * it automatically appends "\r\n" to the msg
 * IMPORTANT: if queue is full this function will do nothing
 */
void sendMessage(char* msg)
{
	char msgNew[300];
	strcpy(msgNew, msg);
	strcat(msgNew, "\r\n");
	messageQueuePush(msgNew);
}

/*
 * helper variables for sendToPC()
 * 	last time of sending message, updated by sendToPC()
 * 	current state of message sending
 */
long timeLastSent = 0;
int stateSending = 0;

/*
 * this function needs to be called to send current state to PC, should be called
 * every iteration of the main loop
 */
void sendToPC()
{
	/*
	 * this implementation sends messages sends message reguralry over
	 * "settingBluetoothInterval" time. Instead of pushing all elements to the queue at time
	 * it sends one message "at the time". For example for settBluetInter = 200*000 micros it
	 * adds to the queue:
	 * - sendGLobal() message after first 20ms
	 * - nothing after first 40ms
	 * - nothing after first 60ms (because sendGlobal is long, >150chars)
	 * - ControllerLeftKtir state after first 80ms
	 * - (...)
	 *
	 * (we cannot allocate big enough queue! -> unhandled exception due to memory fault)
	 *
	 */
	double percentSent = (getMicroseconds() - timeLastSent)
			/ settingBluetoothInterval;

	if (percentSent > 0.1 && stateSending < 1)
	{
		sendGlobal();
		stateSending++;
	}
	else if (percentSent > 0.2 && stateSending < 2)
	{

		stateSending++;
	}
	else if (percentSent > 0.3 && stateSending < 3)
	{

		stateSending++;

	}
	else if (percentSent > 0.4 && stateSending < 4)
	{
		printControllerOutput("controllerLeftKtir");
		stateSending++;

	}
	else if (percentSent > 0.5 && stateSending < 5)
	{
		printControllerOutput("controllerRightKtir");
		stateSending++;

	}
	else if (percentSent > 0.6 && stateSending < 6)
	{
		printControllerOutput("controllerLeftWheelSpeed");
		stateSending++;

	}
	else if (percentSent > 0.7 && stateSending < 7)
	{
		printControllerOutput("controllerRightWheelSpeed");
		stateSending++;
	}
	else if (percentSent > 0.8 && stateSending < 8)
	{
		printControllerOutput("controllerForward");
		stateSending++;
	}
	else if (percentSent > 0.9 && stateSending < 9)
	{
		printControllerOutput("controllerBackward");
		stateSending++;
	}
	else if (percentSent > 1)
	{
		timeLastSent = getMicroseconds();
		stateSending = 0;
	}

}

/*
 * helper function, appends d (with 'decimal' decimal places) to msg
 * you need to use this function because snprintf doesnt work with doubles well
 */
void addDoubleToString(char *msg, double d, int decimal)
{
	if (d < 0)
	{
		strcat(msg, "-");
	}

	d = fabs(d);
	int beforeDot = (int) d;
	double remainingAfterDot = d - beforeDot;
	int afterDot = pow(10, decimal) * remainingAfterDot;
	char representation[20];
	snprintf(representation, 20, "%d.%d ", beforeDot, afterDot);
	strcat(msg, representation);
}

/*
 * returns pointer to controller based on its name
 */
ControllerState* getController(char* name)
{
	if (strcmp(name, "controllerForward") == 0)
	{
		return &controllerForward;
	}
	else if (strcmp(name, "controllerBackward") == 0)
	{
		return &controllerBackward;
	}
	else if (strcmp(name, "controllerLeftKtir") == 0)
	{
		return &controllerLeftKtir;
	}
	else if (strcmp(name, "controllerRightKtir") == 0)
	{
		return &controllerRightKtir;
	}
	else if (strcmp(name, "controllerLeftWheelSpeed") == 0)
	{
		return &controllerLeftWheelSpeed;
	}
	else
	{
		return &controllerRightWheelSpeed;
	}
}

void printControllerOutput(char* name)
{
	ControllerState controller = *getController(name);
	char msg[100];
	snprintf(msg, 100, "CONTROLLER_STATE %s ", name);
	strcat(msg, " ");
	addDoubleToString(msg, controller.propSignal, 3);
	addDoubleToString(msg, controller.integralSignal, 3);
	addDoubleToString(msg, controller.diffSignal, 3);
	sendMessage(msg);
}

void printControllerSettings(char* name)
{
	ControllerState controller = *getController(name);
	char msg[100];
	snprintf(msg, 200, "CONTROLLER_SETTINGS %s ", name);
	strcat(msg, " ");
	addDoubleToString(msg, controller.diffInterval, 3);
	addDoubleToString(msg, controller.integralMax, 3);
	addDoubleToString(msg, controller.kp, 5);
	addDoubleToString(msg, controller.ti, 5);
	addDoubleToString(msg, controller.td, 5);
	addDoubleToString(msg, controller.enabledP, 0);
	addDoubleToString(msg, controller.enabledI, 0);
	addDoubleToString(msg, controller.enabledD, 0);
	sendMessage(msg);
}

void messageReceived(char* msg, int msgLength)
{
	char type[30];
	type[0] = '\0';
	if (sscanf(msg, "%30s", type) != 1)
		return;

	if (strcmp(type, "START_AUTO") == 0)
	{
		changeState(STATE_GO, REASON_BLUETOOTH_COMMAND);
	}
	else if (strcmp(type, "STOP_AUTO") == 0)
	{
		changeState(STATE_STOP, REASON_BLUETOOTH_COMMAND);
	}
	else if (strcmp(type, "RESET") == 0)
	{
		changeState(STATE_INIT, REASON_BLUETOOTH_COMMAND);
	}
	else if (strcmp(type, "MANUAL") == 0)
	{
		changeState(STATE_MANUAL, REASON_BLUETOOTH_COMMAND);
	}
	else if (strcmp(type, "OPEN_FRAME") == 0)
	{
		openFrame();
	}
	else if (strcmp(type, "CLOSE_FRAME") == 0)
	{
		closeFrame();
	}
	else if (strcmp(type, "DRIVE_PID_FORWARD") == 0)
	{
		double PWM;
		if (sscanf(msg, "%30s %lf", type, &PWM) == 2)
			setDrivePIDForward(PWM);
	}
	else if (strcmp(type, "DRIVE_PID_BACKWARD") == 0)
	{
		double PWM;
		if (sscanf(msg, "%30s %lf", type, &PWM) == 2)
			setDrivePIDBackward(PWM);
	}
	else if (strcmp(type, "DRIVE_VWHEEL") == 0)
	{
		double VMAX_LEFT, VMAX_RIGHT;
		if (sscanf(msg, "%s %lf %lf", type, &VMAX_LEFT, &VMAX_RIGHT) == 3)
			setDriveWheelVelocity(VMAX_LEFT, VMAX_RIGHT);
	}
	else if (strcmp(type, "DRIVE_SIDE_KTIR") == 0)
	{
		setDriveSideKtir();
	}
	else if (strcmp(type, "DRIVE_PWM") == 0)
	{
		double PWM_L, PWM_R;
		if (sscanf(msg, "%s %lf %lf", type, &PWM_L, &PWM_R) == 3)
			setDriveWheelPWM(PWM_L, PWM_R);
	}
	else if (strcmp(type, "DRIVE_STOP_FAST") == 0)
	{
		setDriveStopFast();
	}
	else if (strcmp(type, "DRIVE_STOP_SLOW") == 0)
	{
		setDriveStopSlow();
	}
	else if (strcmp(type, "GET_CONTROLLER") == 0)
	{
		char controllerName[30];
		if (sscanf(msg, "%30s %30s", type, controllerName) == 2)
			printControllerSettings(controllerName);
	}
	else if (strcmp(type, "SET_CONTROLLER") == 0)
	{
		// SET_CONTROLLER controllerForward 1 3 50 50 50 0 0 0
		char controllerName[30];
		double diffInterval, integralMax;
		double kp, ti, td;
		bool enabledP, enabledI, enabledD;

		if (sscanf(msg, "%30s %30s %lf %lf %lf %lf %lf %d %d %d", type,
				controllerName, &diffInterval, &integralMax, &kp, &ti, &td,
				&enabledP, &enabledI, &enabledD) == 10)
		{
			ControllerState *ctrl = getController(controllerName);
			ctrl->diffInterval = diffInterval;
			ctrl->integralMax = integralMax;
			ctrl->kp = kp;
			ctrl->ti = ti;
			ctrl->td = td;
			ctrl->enabledP = enabledP;
			ctrl->enabledI = enabledI;
			ctrl->enabledD = enabledD;
		}

	}
	else if (strcmp(type, "GET_SETTINGS") == 0)
	{
		//NOT IMPLEMENTED
	}
	else if (strcmp(type, "SET_SETTINGS") == 0)
	{
		//NOT IMPLEMENTED
	}
}

/*
 * writes an array of format '11101011' to dst depending on 'source'
 */
void printKtir(char* dst, bool* source, int num)
{
	int i = 0;
	for (i = 0; i < num; i++)
		dst[i] = source[i] + '0';

	dst[i] = '\0';

}

/*
 * this function constructs a message containing most important variables and
 * sends them via UART
 */
void sendGlobal(void)
{
	/*
	 * KTIRS
	 */
	char ktFront[10], ktRight[10], ktBack[10], ktLeft[10];
	printKtir(ktFront, ktirFront, 7);
	printKtir(ktRight, ktirRight, 3);
	printKtir(ktBack, ktirBack, 7);
	printKtir(ktLeft, ktirLeft, 3);

	Point recentTarget = getRecentTarget();

	/*
	 * messages are constructed in parts to help debugging
	 * after all parts (msg1, msg2...) are created they are concateneted to form
	 * 'msg' which is sent via UART
	 */
	char msg[200];
	char msgTmp[100];
	msg[0] = '\0';

	msgTmp[0] = '\0';
	snprintf(msgTmp, 100, "GLOBAL %s %s %s %s ", ktFront, ktRight, ktBack,
			ktLeft);
	strcat(msg, msgTmp);

	msgTmp[0] = '\0';
	snprintf(msgTmp, 100, "%d %d %d %d %d ", sharp, ultra[0], ultra[1],
			ultra[2], ultra[3]);
	strcat(msg, msgTmp);

	msgTmp[0] = '\0';
	addDoubleToString(msgTmp, battery, 3);
	addDoubleToString(msgTmp, pwmRight, 3);
	addDoubleToString(msgTmp, pwmLeft, 3);
	addDoubleToString(msgTmp, velocityRight, 3);
	addDoubleToString(msgTmp, velocityLeft, 3);
	strcat(msg, msgTmp);

	msgTmp[0] = '\0';
	addDoubleToString(msgTmp, position.x, 3);
	addDoubleToString(msgTmp, position.y, 3);
	addDoubleToString(msgTmp, direction, 3);
	addDoubleToString(msgTmp, 300 * recentTarget.i, 3);
	addDoubleToString(msgTmp, 300 * recentTarget.j, 3);
	strcat(msg, msgTmp);

	msgTmp[0] = '\0';
	snprintf(msgTmp, 100, "%d %d %s ", 300 * nextCrossroad.i,
			300 * nextCrossroad.j, "NO_ENEMIES");
	strcat(msg, msgTmp);

	snprintf(msgTmp, 100, "%d %d %d %d ", state, prevState, reasonChangeState,
			getMiliseconds());
	strcat(msg, msgTmp);

	msgTmp[0] = '\0';
	if (driveFunction == NULL)
		strcat(msgTmp, "null ");
	else if (driveFunction == drivePIDForward)
		strcat(msgTmp, "drivePIDForward ");
	else if (driveFunction == drivePIDBackward)
		strcat(msgTmp, "drivePIDBackward ");
	else if (driveFunction == driveWheelVelocity)
		strcat(msgTmp, "driveWheelVelocity ");
	else if (driveFunction == driveSideKtir)
		strcat(msgTmp, "driveSideKtir ");
	else if (driveFunction == driveWheelPWM)
		strcat(msgTmp, "driveWheelPWM ");
	else if (driveFunction == driveStopFast)
		strcat(msgTmp, "driveStopFasst ");
	else if (driveFunction == driveStopSlow)
		strcat(msgTmp, "driveStopSlow ");
	else
		strcat(msgTmp, "");
	strcat(msg, msgTmp);

	msgTmp[0] = '\0';
	snprintf(msgTmp, 100, "%d %d ", (int) carryingCan, (int) frameClosed);
	strcat(msg, msgTmp);

	/*
	 * send message via UART
	 */
	sendMessage(msg);
}



