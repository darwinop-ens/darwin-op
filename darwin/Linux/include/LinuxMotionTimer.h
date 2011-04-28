/*
 *   LinuxMotionTimer.h
 *
 *   Author: ROBOTIS
 *
 */

#ifndef _LINUX_MOTION_MANAGER_H_
#define _LINUX_MOTION_MANAGER_H_

#include <pthread.h>
#include <signal.h>
#include "MotionManager.h"

namespace Robot
{
	class LinuxMotionTimer
	{
	private:
		static timer_t m_TimerID;

		static MotionManager* m_Manager;
	    static bool m_TimerRunning;
	    static void TimerProc(int arg);

	protected:

	public:
		static void Initialize(MotionManager* manager);
		static void Start()			{ m_TimerRunning = true; }
		static void Stop()			{ m_TimerRunning = false; }
		static bool IsRunning()		{ return m_TimerRunning; }
	};
}

#endif
