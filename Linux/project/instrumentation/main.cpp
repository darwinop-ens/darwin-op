/*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *      Author: François Schwarzentruber
 */

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>
#include <sstream>
#include <iostream>

#include "CM730.h"
#include "LinuxNetwork.h"
#include "LinuxDARwIn.h"

#include "echo.h"
#include "instr.h"
#include "webcam.h"

using namespace std;
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
	cout << "signals: initializing" << endl;
	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGQUIT, &sighandler);
	signal(SIGINT, &sighandler);
	cout << "signals: initialized" << endl;
}

/*main function*/
int main(void)
{
	LinuxCM730 linux_cm730("/dev/ttyUSB0");
	CM730 cm730(&linux_cm730);

	cout << "robot: connecting" << endl;
	if (cm730.Connect())
	{
		cout << "robot: connected" << endl;
	}
	else
	{
		cout << "robot: unable to connect to the robot" << endl;
		exit(0);
	}

	EchoServer echo = EchoServer();
	InstrServer instr = InstrServer(cm730);
	Webcam wc = Webcam();

	signalInitialize();

	//echo.Initialize();
	instr.Initialize();
	wc.Initialize();

	while(1)
	{
		usleep(500); // sleep for about 0.5ms
		//echo.Execute();
		instr.Execute();
		wc.Execute();
	}
}
