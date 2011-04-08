/*
 *   MotionManager.h
 *
 *   Author: ROBOTIS
 *
 */

#ifndef _MOTION_MANGER_H_
#define _MOTION_MANGER_H_

#include <list>
#include "MotionStatus.h"
#include "MotionModule.h"
#include "CM730.h"


namespace Robot
{
	class MotionManager
	{
	private:
		static MotionManager* m_UniqueInstance;
		std::list<MotionModule*> m_Modules;
		CM730 *m_CM730;
		bool m_ProcessEnable;
		bool m_Enabled;
		int m_FBGyroCenter;
		int m_RLGyroCenter;
		bool m_SensorCalibrated;
		int m_CalibrationTime;

		bool m_IsRunning;

        MotionManager();

	protected:

	public:
		bool DEBUG_PRINT;

		~MotionManager();

		static MotionManager* GetInstance() { return m_UniqueInstance; }

		bool Initialize(CM730 *cm730);
		bool Reinitialize();
        void Process();
		void SetEnable(bool enable);
		bool GetEnable()				{ return m_Enabled; }
		void AddModule(MotionModule *module);
		void RemoveModule(MotionModule *module);

		void ResetGyroCalibration() { m_SensorCalibrated = false; m_CalibrationTime = 0; m_FBGyroCenter = 0; m_RLGyroCenter = 0; }
	};
}

#endif
