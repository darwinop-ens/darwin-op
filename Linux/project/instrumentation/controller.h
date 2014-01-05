#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <string>
#include <pthread.h>
#include "minIni.h"
#include "task.h"
#include "time.h"

#include "LinuxDARwIn.h"
#include "webcam.h"

using namespace Robot;
using namespace std;

typedef struct {
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
} ControllerSettings;

class Controller: public Task
{
	private:
		CM730 m_cm730;
		struct timeval m_previous_time;
	public:
		Controller(CM730 &cm730);
		~Controller();
		virtual void Initialize(void);
		virtual void Execute(void);
	public:
		WebcamThreadArg* webcam;
		ControllerSettings settings;
};

#endif // __CONTROLLER_H__

