/*
 *   LinuxMotionTimer.cpp
 *
 *   Author: ROBOTIS
 *
 */

#include <unistd.h>
#include <sys/time.h>
#include "MotionModule.h"
#include "LinuxMotionTimer.h"

using namespace Robot;

MotionManager* LinuxMotionTimer::m_Manager(0);
bool LinuxMotionTimer::m_TimerRunning(false);
timer_t LinuxMotionTimer::m_TimerID(0);


void LinuxMotionTimer::TimerProc(int arg)
{
	if(m_Manager != 0 && m_TimerRunning == true)
		m_Manager->Process();
}

void LinuxMotionTimer::Initialize(MotionManager* manager)
{
	if(m_TimerID > 0)
	{		
		timer_delete(m_TimerID);
		m_TimerID = 0;
	}

	m_Manager = manager;

	struct itimerspec value;
    struct sigevent av_sig_spec;
	long nsec_interval = MotionModule::TIME_UNIT*1000000;

    av_sig_spec.sigev_notify = SIGEV_SIGNAL;
    av_sig_spec.sigev_signo = SIGRTMIN;

    value.it_value.tv_sec = 0;
    value.it_value.tv_nsec = nsec_interval;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_nsec = nsec_interval;

    timer_create(CLOCK_REALTIME, &av_sig_spec, &m_TimerID);
    timer_settime(m_TimerID, 0, &value, NULL);
    signal(SIGRTMIN, TimerProc);
	m_TimerRunning = true;
}

void LinuxMotionTimer::msleep(int Miliseconds)
{
	struct timeval tv;
    gettimeofday(&tv, NULL);
	double time = (double)tv.tv_sec*1000.0 + (double)tv.tv_usec/1000.0;
	double time2;

	do
	{
		usleep(Miliseconds * 1000);
		gettimeofday(&tv, NULL);
		time2 = (double)tv.tv_sec*1000.0 + (double)tv.tv_usec/1000.0;
	}while(time2 - time < (double)Miliseconds);
}
