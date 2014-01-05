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

// link to the variables in instr.cpp
bool ControllerEnable;
int ControllerSamplingTime;
int ControllerReferenceX;
int ControllerErrorX;
int ControllerErrorX1;
float ControllerProportionalX;
float ControllerIntegralX;
float ControllerCommandX;
int ControllerReferenceY;
int ControllerErrorY;
int ControllerErrorY1;
float ControllerProportionalY;
float ControllerIntegralY;
float ControllerCommandY;

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
	ControllerProportionalX = 0.32;
	ControllerIntegralX = 0.096;
	ControllerReferenceY = 120;
	ControllerProportionalY = 0.427;
	ControllerIntegralY = 0.128;

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
			ControllerErrorX = webcam->BallPositionX - ControllerReferenceX;
			ControllerCommandX += ControllerProportionalX*((1+ControllerSamplingTime/ControllerIntegralX)*ControllerErrorX - ControllerErrorX1);
			if (ControllerCommandX < -1000)
				ControllerCommandX = -1000;
			if (ControllerCommandX > 1000)
				ControllerCommandX = 1000;
			m_cm730.WriteWord(19, 30, 2048-ControllerCommandX, 0);

			ControllerErrorY1 = ControllerErrorY;
			ControllerErrorY = webcam->BallPositionY - ControllerReferenceY;
			ControllerCommandY += ControllerProportionalY*((1+ControllerSamplingTime/ControllerIntegralY)*ControllerErrorY - ControllerErrorY1);
			if (ControllerCommandY < -900)
				ControllerCommandY = -900;
			if (ControllerCommandY > 200)
				ControllerCommandY = 200;
			m_cm730.WriteWord(20, 30, 2048-ControllerCommandY, 0);

			m_previous_time = current_time;
		}
	}
	else
	{
		ControllerErrorX1 = 0;
		ControllerCommandX = 0;
		ControllerErrorY1 = 0;
		ControllerCommandY = 0;
		// get current time
		gettimeofday(&m_previous_time, NULL);
	}
}

