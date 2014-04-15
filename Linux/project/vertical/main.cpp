/*
 * main.cpp
 *
 *  Created on: 2013. 5. 4.
 *      Author: Florent Ouchet
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>
#include <math.h>

#include "CM730.h"
#include "LinuxDARwIn.h"

#include "accelerometer.h"
#include "left_arm.h"
#include "right_arm.h"

using namespace Robot;

/*handling the signals (Ctrl + C to quit etc.)*/


/*this is the function that kills the program (it is called by callback)*/
void sighandler(int sig)
{
	exit(0);
}


/*this initializes the signals (Ctrl + C to quit etc.)*/
void signalInitialize()
{
	printf("Initialization of the signals...\n");
	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGQUIT, &sighandler);
	signal(SIGINT, &sighandler);
	printf("Signals initialized\n");
}

/*main function*/
int main(void)
{
	printf("Demonstration...\n");
	signalInitialize();

	LinuxCM730 linux_cm730("/dev/ttyUSB0");
	CM730 cm730(&linux_cm730);

	printf("Try to connect to the robot...\n");
	if(cm730.Connect() == false)
	{
		printf("ERROR: impossible to connect to the robot\n");
		return 0;
	}
	printf("Connexion succeeded\n");

	Accelerometer Acc(cm730);
	LeftArm LA(cm730);
	RightArm RA(cm730);

	while(1) {
		double magnitude, polar, azimuth;
		Acc.ReadSphericalValues(magnitude, polar, azimuth);
		printf("mag = %.3f pol=%.3f, az=%.3f\n", magnitude, polar/M_PI*180.0, azimuth/M_PI*180.0);
		LA.WriteSphericalPosition(polar,azimuth);
		RA.WriteSphericalPosition(polar,azimuth);
		usleep(1000000);
	}
}
