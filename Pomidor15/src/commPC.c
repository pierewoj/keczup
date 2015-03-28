/*
 * commPC.c
 *
 *  Created on: Mar 24, 2015
 *      Author: Kuba
 */
#include "lowlevel/communication.h"
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
	char msg[200];
	snprintf(msg, 100, "CONTROLLER_STATE %s ", name);
	strcat(msg," ");
	addDoubleToString(msg, controller.propSignal, 3);
	addDoubleToString(msg, controller.integralSignal, 3);
	addDoubleToString(msg, controller.diffSignal, 3);
	sendMessage(msg);
}

void printControllerSettings(char* name)
{
	ControllerState controller = *getController(name);
	char msg[300];
	snprintf(msg, 200, "CONTROLLER_SETTINGS %s ", name);
	strcat(msg," ");
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
	sscanf(msg, "%30s", type);

	if (strcmp(type, "START_AUTO") == 0)
	{
		//NOT IMPLEMENTED
	}
	else if (strcmp(type, "STOP_AUT0") == 0)
	{
		//NOT IMPLEMENTED
	}
	else if (strcmp(type, "RESET") == 0)
	{
		//NOT IMPLEMENTED
	}
	else if (strcmp(type, "MANUAL") == 0)
	{
		//NOT IMPLEMENTED
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
		//NOT IMPLEMENTED
	}
	else if (strcmp(type, "DRIVE_PID_BACKWARD") == 0)
	{
		//NOT IMPLEMENTED
	}
	else if (strcmp(type, "DRIVE_VWHEEL") == 0)
	{
		//NOT IMPLEMENTED
	}
	else if (strcmp(type, "DRIVE_SIDE_KTIR") == 0)
	{
		//NOT IMPLEMENTED
	}
	else if (strcmp(type, "DRIVE_PWM") == 0)
	{
		//NOT IMPLEMENTED
	}
	else if (strcmp(type, "DRIVE_STOP_FAST") == 0)
	{
		//NOT IMPLEMENTED
	}
	else if (strcmp(type, "DRIVE_STOP_SLOW") == 0)
	{
		//NOT IMPLEMENTED
	}
	else if (strcmp(type, "GET_CONTROLLER") == 0)
	{
		char controllerName[30];
		sscanf(msg,"%s %s", type, controllerName);
		printControllerSettings(controllerName);
	}
	else if (strcmp(type, "SET_CONTROLLER") == 0)
	{
		//NOT IMPLEMENTED
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
	char msg[500];
	msg[0] = '\0';

	char msg1[100];
	msg1[0] = '\0';
	snprintf(msg1, 100, "GLOBAL %s %s %s %s ", ktFront, ktRight, ktBack,
			ktLeft);

	char msg2[100];
	msg2[0] = '\0';
	snprintf(msg2, 100, "%d %d %d %d %d ", sharp, ultra[0], ultra[1], ultra[2],
			ultra[3]);

	char msg3[100];
	msg3[0] = '\0';
	addDoubleToString(msg3, battery, 3);
	addDoubleToString(msg3, pwmRight, 3);
	addDoubleToString(msg3, pwmLeft, 3);
	addDoubleToString(msg3, velocityRight, 3);
	addDoubleToString(msg3, velocityLeft, 3);

	char msg4[100];
	msg4[0] = '\0';
	addDoubleToString(msg4, position.x, 3);
	addDoubleToString(msg4, position.y, 3);
	addDoubleToString(msg4, direction, 3);
	addDoubleToString(msg4, 300 * recentTarget.i, 3);
	addDoubleToString(msg4, 300 * recentTarget.j, 3);

	char msg5[100];
	msg5[0] = '\0';
	snprintf(msg5, 100, "%d %d %s ", 300 * nextCrossroad.i,
			300 * nextCrossroad.j, "NO_ENEMIES");

	char msg6[100];
	msg6[0] = '\0';
	snprintf(msg6, 100, "%d %d %d %d ", state, prevState, reasonChangeState,
			getMiliseconds());

	/*
	 * Drive function print
	 */
	if (driveFunction == NULL)
		strcat(msg6, "null ");
	else if (driveFunction == drivePIDForward)
		strcat(msg6, "drivePIDForward ");
	else if (driveFunction == drivePIDBackward)
		strcat(msg6, "drivePIDBackward ");
	else if (driveFunction == driveWheelVelocity)
		strcat(msg6, "driveWheelVelocity ");
	else if (driveFunction == driveSideKtir)
		strcat(msg6, "driveSideKtir ");
	else if (driveFunction == driveWheelPWM)
		strcat(msg6, "driveWheelPWM ");
	else if (driveFunction == driveStopFast)
		strcat(msg6, "driveStopFasst ");
	else if (driveFunction == driveStopSlow)
		strcat(msg6, "driveStopSlow ");
	else
		strcat(msg6, "");

	char msg7[100];
	msg7[0] = '\0';
	snprintf(msg7, 100, "%d %d ", (int) carryingCan, (int) frameClosed);

	strcat(msg, msg1);
	strcat(msg, msg2);
	strcat(msg, msg3);
	strcat(msg, msg4);
	strcat(msg, msg5);
	strcat(msg, msg6);
	strcat(msg, msg7);

	/*
	 * send message via UART
	 */
	sendMessage(msg);
}

/*
 * last time of sending message, updated by sendToPC()
 */
long timeLastSent = 0;

/*
 * this function needs to be called to send current state to PC
 */
void sendToPC()
{
	/*
	 * will not send if message was sent later than settingBluetoothInterval
	 * iterations ago
	 */
	if (getMicroseconds() - timeLastSent
			> settingBluetoothInterval * loopWaitTime)
	{
		timeLastSent = getMicroseconds();

		sendGlobal();
		/*
		printControllerOutput("controllerForward");
		printControllerOutput("controllerBackward");
		printControllerOutput("controllerLeftKtir");
		printControllerOutput("controllerRightKtir");
		printControllerOutput("controllerLeftWheelSpeed");
		printControllerOutput("controllerRightWheelSpeed");*/
	}

}

