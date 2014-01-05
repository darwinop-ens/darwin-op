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
	ColorFinder* BallFinder;
	int BallPositionX;
	int BallPositionY;
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
		WebcamThreadArg ThreadArg;
};

#endif // __WEBCAM_H__

