#ifndef __WEBCAM_H__
#define __WEBCAM_H__

#include <string>
#include <pthread.h>
#include "minIni.h"
#include "task.h"

#include "LinuxDARwIn.h"

using namespace Robot;
using namespace std;

typedef struct {
	pthread_t ThreadID;
	LinuxCamera* Camera;
	bool Enable;
	bool BallEnable;
	ColorFinder* BallFinder;
	int BallPositionX;
	int BallPositionY;
	bool RedEnable;
	ColorFinder* RedFinder;
	int RedPositionX;
	int RedPositionY;
	bool YellowEnable;
	ColorFinder* YellowFinder;
	int YellowPositionX;
	int YellowPositionY;
	bool BlueEnable;
	ColorFinder* BlueFinder;
	int BluePositionX;
	int BluePositionY;
	bool Continue;
} WebcamThreadArg;

class Webcam: public Task
{
	private:
		minIni *IniSettings;
	public:
		Webcam();
		~Webcam();
		virtual void Initialize(void);
		virtual void Execute(void);
	public:
		WebcamThreadArg ThreadArg;
};

#endif // __WEBCAM_H__

