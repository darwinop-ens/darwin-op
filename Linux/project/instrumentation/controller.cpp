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

	// initialize controller settings
	settings.ControllerEnable = false;
	settings.ControllerSamplingTime = 10; // ms
	settings.ControllerReferenceX = 160;
	settings.ControllerProportionalX = 0.32;
	settings.ControllerIntegralX = 0.096;
	settings.ControllerReferenceY = 120;
	settings.ControllerProportionalY = 0.427;
	settings.ControllerIntegralY = 0.128;

	cout << "controller: initialized" << endl;
}

void Controller::Execute(void)
{
	if (settings.ControllerEnable)
	{
		struct timeval current_time, diff;

		// get current time
		gettimeofday(&current_time, NULL);
		// compute time difference
		timeval_subtract(&diff, &current_time, &m_previous_time);

		// print result
		if ((diff.tv_sec*1000 + diff.tv_usec/1000) >= settings.ControllerSamplingTime)
		{
			settings.ControllerErrorX1 = settings.ControllerErrorX;
			settings.ControllerErrorX = webcam->BallPositionX - settings.ControllerReferenceX;
			settings.ControllerCommandX += settings.ControllerProportionalX*((1+settings.ControllerSamplingTime/settings.ControllerIntegralX)*settings.ControllerErrorX - settings.ControllerErrorX1);
			if (settings.ControllerCommandX < -1000)
				settings.ControllerCommandX = -1000;
			if (settings.ControllerCommandX > 1000)
				settings.ControllerCommandX = 1000;
			m_cm730.WriteWord(19, 30, 2048-settings.ControllerCommandX, 0);

			settings.ControllerErrorY1 = settings.ControllerErrorY;
			settings.ControllerErrorY = webcam->BallPositionY - settings.ControllerReferenceY;
			settings.ControllerCommandY += settings.ControllerProportionalY*((1+settings.ControllerSamplingTime/settings.ControllerIntegralY)*settings.ControllerErrorY - settings.ControllerErrorY1);
			if (settings.ControllerCommandY < -900)
				settings.ControllerCommandY = -900;
			if (settings.ControllerCommandY > 200)
				settings.ControllerCommandY = 200;
			m_cm730.WriteWord(20, 30, 2048-settings.ControllerCommandY, 0);

			m_previous_time = current_time;
		}
	}
	else
	{
		settings.ControllerErrorX1 = 0;
		settings.ControllerCommandX = 0;
		settings.ControllerErrorY1 = 0;
		settings.ControllerCommandY = 0;
		// get current time
		gettimeofday(&m_previous_time, NULL);
	}
}

