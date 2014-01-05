#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <ctype.h>
#include <unistd.h>

#include "webcam.h"

using namespace std;
using namespace Robot;

#define INI_FILE_PATH       "/darwin/Data/config.ini"

static void* thread_webcam(void* arg)
{
	WebcamThreadArg* ThreadArg = (WebcamThreadArg*) arg;
	Point2D Position;

	while(ThreadArg->Continue)
	{
		if (ThreadArg->Enable)
		{
			ThreadArg->Camera->CaptureFrame();
			if (ThreadArg->BallEnable)
			{
				Position = ThreadArg->BallFinder->GetPosition(ThreadArg->Camera->fbuffer->m_HSVFrame);
				ThreadArg->BallPositionX = Position.X;
				ThreadArg->BallPositionY = Position.Y;
			}
			if (ThreadArg->RedEnable)
			{
				Position = ThreadArg->RedFinder->GetPosition(ThreadArg->Camera->fbuffer->m_HSVFrame);
				ThreadArg->RedPositionX = Position.X;
				ThreadArg->RedPositionY = Position.Y;
			}
			if (ThreadArg->YellowEnable)
			{
				Position = ThreadArg->YellowFinder->GetPosition(ThreadArg->Camera->fbuffer->m_HSVFrame);
				ThreadArg->YellowPositionX = Position.X;
				ThreadArg->YellowPositionY = Position.Y;
			}
			if (ThreadArg->BlueEnable)
			{
				Position = ThreadArg->BlueFinder->GetPosition(ThreadArg->Camera->fbuffer->m_HSVFrame);
				ThreadArg->BluePositionX = Position.X;
				ThreadArg->BluePositionY = Position.Y;
			}
		}
		else
		{
			usleep(1000);
		}
	}

	return NULL;
}

Webcam::Webcam()
{
	IniSettings = new minIni(INI_FILE_PATH);
	ThreadArg.ThreadID = 0;

	ThreadArg.Enable = true;
	ThreadArg.BallEnable = true;
	ThreadArg.BallFinder = new ColorFinder();
	ThreadArg.RedEnable = false;
	ThreadArg.RedFinder = new ColorFinder(0, 15, 45, 100, 0, 100, 0.3, 50.0);
	ThreadArg.YellowEnable = false;
	ThreadArg.YellowFinder = new ColorFinder(60, 15, 45, 0, 0.3, 50.0);
	ThreadArg.BlueEnable = false;
	ThreadArg.BlueFinder = new ColorFinder(225, 15, 45, 0, 0.3, 50.0);

	ThreadArg.Camera = LinuxCamera::GetInstance();
	ThreadArg.Continue = true;
}

Webcam::~Webcam()
{
	ThreadArg.Continue = false;
	pthread_join(ThreadArg.ThreadID, NULL);
	delete ThreadArg.BlueFinder;
	delete ThreadArg.YellowFinder;
	delete ThreadArg.RedFinder;
	delete ThreadArg.BallFinder;
	delete ThreadArg.Camera;
	delete IniSettings;
}

void Webcam::Initialize(void)
{
	cout << "webcam: initializing" << endl;

	// initialize webcam
	ThreadArg.Camera->Initialize(0);
	ThreadArg.Camera->SetCameraSettings(CameraSettings());    // set default settings
	ThreadArg.Camera->LoadINISettings(IniSettings);           // load from ini

	// initialize ball finder
    	ThreadArg.BallFinder->LoadINISettings(IniSettings);
	ThreadArg.RedFinder->LoadINISettings(IniSettings, "RED");
	ThreadArg.YellowFinder->LoadINISettings(IniSettings, "YELLOW");
	ThreadArg.BlueFinder->LoadINISettings(IniSettings, "BLUE");

	// initialize working thread
	if (pthread_create(&ThreadArg.ThreadID, NULL, &thread_webcam, &ThreadArg))
		cout << "webcam: failed to create working thread" << endl;

	cout << "webcam: initialized" << endl;
}

void Webcam::Execute(void)
{
	// everything is done in working thread
}

