#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <ctype.h>
#include <unistd.h>

#include "timeutils.h"
#include "controller.h"

using namespace std;
using namespace Robot;

#define INI_FILE_PATH       "/darwin/Data/config.ini"

// link to the variables in webcam.cpp
extern int BallPositionX;
extern int BallPositionY;

// link to the variables in instr.cpp
bool ControllerEnable;
int ControllerSamplingTime;
int ControllerReferenceX;
int ControllerErrorX;
int ControllerErrorX1;
float ControllerProportionalX;
float ControllerIntegralX;
int ControllerCommandX;
int ControllerReferenceY;
int ControllerErrorY;
int ControllerErrorY1;
float ControllerProportionalY;
float ControllerIntegralY;
int ControllerCommandY;

Controller::Controller(CM730 &cm730):
	m_cm730(cm730)
{

}

Controller::~Controller()
{

}

void Controller::Initialize(void)
{
	cout << "controller: initializing" << endl;

	// initialize controller
	ControllerEnable = false;
	ControllerSamplingTime = 10; // ms
	ControllerReferenceX = 160;
	ControllerProportionalX = 1.0;
	ControllerIntegralX = 0.0;
	ControllerReferenceY = 120;
	ControllerProportionalY = 1.0;
	ControllerIntegralY = 0.0;

	cout << "controller: initialized" << endl;
}

void Controller::Execute(void)
{
	if (ControllerEnable)
	{
		struct timeval current_time, diff;

		// get current time
		gettimeofday(&current_time, NULL);
		// compute time difference
		timeval_subtract(&diff, &current_time, &m_previous_time);

		// print result
		if ((diff.tv_sec*1000 + diff.tv_usec/1000) >= ControllerSamplingTime)
		{
			ControllerErrorX1 = ControllerErrorX;
			ControllerErrorX = BallPositionX - ControllerReferenceX;
			ControllerCommandX += ControllerProportionalX*((1+ControllerSamplingTime/ControllerIntegralX)*ControllerErrorX - ControllerErrorX1);
			m_cm730.WriteWord(19, 30, ControllerCommandX, 0);

			ControllerErrorY1 = ControllerErrorY;
			ControllerErrorY = BallPositionY - ControllerReferenceY;
			ControllerCommandY += ControllerProportionalY*((1+ControllerSamplingTime/ControllerIntegralY)*ControllerErrorY - ControllerErrorY1);
			m_cm730.WriteWord(20, 30, ControllerCommandY, 0);

			m_previous_time = current_time;
		}
	}
	else
	{
		ControllerErrorX1 = 0;
		ControllerErrorY1 = 0;
		// get current time
		gettimeofday(&m_previous_time, NULL);
	}
}

