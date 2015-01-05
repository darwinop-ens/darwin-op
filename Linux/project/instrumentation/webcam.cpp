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

	ThreadArg.Enable = false;
	ThreadArg.BallEnable = false;
	ThreadArg.BallFinder = new ColorFinder();
	ThreadArg.RedEnable = false;
	ThreadArg.RedFinder = new ColorFinder(0, 15, 45, 100, 0, 100, 0.3, 50.0);
	ThreadArg.YellowEnable = false;
	ThreadArg.YellowFinder = new ColorFinder(60, 15, 45, 100, 0, 100, 0.3, 50.0);
	ThreadArg.BlueEnable = false;
	ThreadArg.BlueFinder = new ColorFinder(225, 15, 45, 100, 0, 100, 0.3, 50.0);

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

void Webcam::ReadTable(unsigned char *buf)
{
	buf[1] = ThreadArg.Enable;
	buf[2] = ThreadArg.BallEnable;
	buf[3] = ThreadArg.RedEnable;
	buf[4] = ThreadArg.YellowEnable;
	buf[5] = ThreadArg.BlueEnable;

	*((int*)&buf[10]) = Camera::WIDTH;
	*((int*)&buf[12]) = Camera::HEIGHT;

	*((int*)&buf[20]) = ThreadArg.BallFinder->m_hue;
	*((int*)&buf[22]) = ThreadArg.BallFinder->m_hue_tolerance;
	buf[24] = ThreadArg.BallFinder->m_min_saturation;
	buf[25] = ThreadArg.BallFinder->m_max_saturation;
	buf[26] = ThreadArg.BallFinder->m_min_value;
	buf[27] = ThreadArg.BallFinder->m_max_value;
	buf[28] = ThreadArg.BallFinder->m_min_percent;
	buf[29] = ThreadArg.BallFinder->m_min_percent;
	*((int*)&buf[30]) = ThreadArg.BallPositionX;
	*((int*)&buf[32]) = ThreadArg.BallPositionY;

	*((int*)&buf[40]) = ThreadArg.RedFinder->m_hue;
	*((int*)&buf[42]) = ThreadArg.RedFinder->m_hue_tolerance;
	buf[44] = ThreadArg.RedFinder->m_min_saturation;
	buf[45] = ThreadArg.RedFinder->m_max_saturation;
	buf[46] = ThreadArg.RedFinder->m_min_value;
	buf[47] = ThreadArg.RedFinder->m_max_value;
	buf[48] = ThreadArg.RedFinder->m_min_percent;
	buf[49] = ThreadArg.RedFinder->m_min_percent;
	*((int*)&buf[50]) = ThreadArg.RedPositionX;
	*((int*)&buf[52]) = ThreadArg.RedPositionY;

	*((int*)&buf[60]) = ThreadArg.YellowFinder->m_hue;
	*((int*)&buf[62]) = ThreadArg.YellowFinder->m_hue_tolerance;
	buf[64] = ThreadArg.YellowFinder->m_min_saturation;
	buf[65] = ThreadArg.YellowFinder->m_max_saturation;
	buf[66] = ThreadArg.YellowFinder->m_min_value;
	buf[67] = ThreadArg.YellowFinder->m_max_value;
	buf[68] = ThreadArg.YellowFinder->m_min_percent;
	buf[69] = ThreadArg.YellowFinder->m_min_percent;
	*((int*)&buf[70]) = ThreadArg.YellowPositionX;
	*((int*)&buf[72]) = ThreadArg.YellowPositionY;

	*((int*)&buf[80]) = ThreadArg.BlueFinder->m_hue;
	*((int*)&buf[82]) = ThreadArg.BlueFinder->m_hue_tolerance;
	buf[84] = ThreadArg.BlueFinder->m_min_saturation;
	buf[85] = ThreadArg.BlueFinder->m_max_saturation;
	buf[86] = ThreadArg.BlueFinder->m_min_value;
	buf[87] = ThreadArg.BlueFinder->m_max_value;
	buf[88] = ThreadArg.BlueFinder->m_min_percent;
	buf[89] = ThreadArg.BlueFinder->m_min_percent;
	*((int*)&buf[90]) = ThreadArg.BluePositionX;
	*((int*)&buf[92]) = ThreadArg.BluePositionY;
}

void Webcam::WriteTable(unsigned char start, unsigned char end, unsigned char *buf)
{
	if (start <= 1 && end >= 1)
		ThreadArg.Enable = buf[1];
	if (start <= 2 && end >= 2)
		ThreadArg.BallEnable = buf[2];
	if (start <= 3 && end >= 3)
		ThreadArg.RedEnable = buf[3];
	if (start <= 4 && end >= 4)
		ThreadArg.YellowEnable = buf[4];
	if (start <= 5 && end >= 5)
		ThreadArg.BlueEnable = buf[5];

	if (start <= 20 && end >= 21)
		ThreadArg.BallFinder->m_hue = *((int*)&buf[20]);
	if (start <= 22 && end >= 23)
		ThreadArg.BallFinder->m_hue_tolerance = *((int*)&buf[22]);
	if (start <= 24 && end >= 24)
		ThreadArg.BallFinder->m_min_saturation = buf[24];
	if (start <= 25 && end >= 25)
		ThreadArg.BallFinder->m_max_saturation = buf[25];
	if (start <= 26 && end >= 26)
		ThreadArg.BallFinder->m_min_value = buf[26];
	if (start <= 27 && end >= 27)
		ThreadArg.BallFinder->m_max_value = buf[27];
	if (start <= 28 && end >= 28)
		ThreadArg.BallFinder->m_min_percent = buf[28];
	if (start <= 29 && end >= 29)
		ThreadArg.BallFinder->m_min_percent = buf[29];

	if (start <= 40 && end >= 41)
		ThreadArg.RedFinder->m_hue = *((int*)&buf[40]);
	if (start <= 42 && end >= 43)
		ThreadArg.RedFinder->m_hue_tolerance = *((int*)&buf[42]);
	if (start <= 44 && end >= 44)
		ThreadArg.RedFinder->m_min_saturation = buf[44];
	if (start <= 45 && end >= 45)
		ThreadArg.RedFinder->m_max_saturation = buf[45];
	if (start <= 46 && end >= 46)
		ThreadArg.RedFinder->m_min_value = buf[46];
	if (start <= 47 && end >= 47)
		ThreadArg.RedFinder->m_max_value = buf[47];
	if (start <= 48 && end >= 48)
		ThreadArg.RedFinder->m_min_percent = buf[48];
	if (start <= 49 && end >= 49)
		ThreadArg.RedFinder->m_min_percent = buf[49];

	if (start <= 60 && end >= 61)
		ThreadArg.YellowFinder->m_hue = *((int*)&buf[60]);
	if (start <= 62 && end >= 63)
		ThreadArg.YellowFinder->m_hue_tolerance = *((int*)&buf[62]);
	if (start <= 64 && end >= 64)
		ThreadArg.YellowFinder->m_min_saturation = buf[64];
	if (start <= 65 && end >= 65)
		ThreadArg.YellowFinder->m_max_saturation = buf[65];
	if (start <= 66 && end >= 66)
		ThreadArg.YellowFinder->m_min_value = buf[66];
	if (start <= 67 && end >= 67)
		ThreadArg.YellowFinder->m_max_value = buf[67];
	if (start <= 68 && end >= 68)
		ThreadArg.YellowFinder->m_min_percent = buf[68];
	if (start <= 69 && end >= 69)
		ThreadArg.YellowFinder->m_min_percent = buf[69];

	if (start <= 80 && end >= 81)
		ThreadArg.BlueFinder->m_hue = *((int*)&buf[80]);
	if (start <= 82 && end >= 83)
		ThreadArg.BlueFinder->m_hue_tolerance = *((int*)&buf[82]);
	if (start <= 84 && end >= 84)
		ThreadArg.BlueFinder->m_min_saturation = buf[84];
	if (start <= 85 && end >= 85)
		ThreadArg.BlueFinder->m_max_saturation = buf[85];
	if (start <= 86 && end >= 86)
		ThreadArg.BlueFinder->m_min_value = buf[86];
	if (start <= 87 && end >= 87)
		ThreadArg.BlueFinder->m_max_value = buf[87];
	if (start <= 88 && end >= 88)
		ThreadArg.BlueFinder->m_min_percent = buf[88];
	if (start <= 89 && end >= 89)
		ThreadArg.BlueFinder->m_min_percent = buf[89];
}

