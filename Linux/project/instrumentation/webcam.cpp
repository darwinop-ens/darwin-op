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

// link to the variables in instr.cpp
extern int BallPositionX;
extern int BallPositionY;

Webcam::Webcam()
{
	IniSettings = new minIni(INI_FILE_PATH);
	BallFinder = new ColorFinder();
	Camera = LinuxCamera::GetInstance();
}

Webcam::~Webcam()
{
	delete IniSettings;
	delete BallFinder;
}

void Webcam::Initialize(void)
{
	cout << "webcam: initializing" << endl;

	// initialize webcam
	Camera->Initialize(0);
	Camera->SetCameraSettings(CameraSettings());    // set default settings
	Camera->LoadINISettings(IniSettings);           // load from ini

	// initialize ball finder
    	BallFinder->LoadINISettings(IniSettings);

	cout << "webcam: initialized" << endl;
}

void Webcam::Execute(void)
{
	Camera->CaptureFrame();
	BallPosition = BallFinder->GetPosition(Camera->fbuffer->m_HSVFrame);
	BallPositionX = BallPosition.X;
	BallPositionY = BallPosition.Y;
}

