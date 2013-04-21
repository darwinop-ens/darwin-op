#ifndef __WEBCAM_H__
#define __WEBCAM_H__

#include <string>
#include "minIni.h"
#include "task.h"

#include "LinuxDARwIn.h"

using namespace Robot;
using namespace std;

class Webcam: public Task
{
	private:
		minIni *IniSettings;
		ColorFinder* BallFinder;
		Point2D BallPosition;
		LinuxCamera* Camera;
	public:
		Webcam();
		~Webcam();
		virtual void Initialize(void);
		virtual void Execute(void);
};

#endif // __WEBCAM_H__

