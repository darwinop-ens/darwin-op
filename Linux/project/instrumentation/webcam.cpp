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
	Point2D BallPosition;

	while(ThreadArg->Continue)
	{
		ThreadArg->Camera->CaptureFrame();
		BallPosition = ThreadArg->BallFinder->GetPosition(ThreadArg->Camera->fbuffer->m_HSVFrame);
		ThreadArg->BallPositionX = BallPosition.X;
		ThreadArg->BallPositionY = BallPosition.Y;
	}

	return NULL;
}

Webcam::Webcam()
{
	IniSettings = new minIni(INI_FILE_PATH);
	ThreadArg.ThreadID = 0;
	ThreadArg.BallFinder = new ColorFinder();
	ThreadArg.Camera = LinuxCamera::GetInstance();
	ThreadArg.Continue = true;
}

Webcam::~Webcam()
{
	ThreadArg.Continue = false;
	pthread_join(ThreadArg.ThreadID, NULL);
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

	// initialize working thread
	if (pthread_create(&ThreadArg.ThreadID, NULL, &thread_webcam, &ThreadArg))
		cout << "webcam: failed to create working thread" << endl;

	cout << "webcam: initialized" << endl;
}

void Webcam::Execute(void)
{
	// everything is done in working thread
}

