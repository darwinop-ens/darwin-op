/*
 *   MotionManager.cpp
 *
 *   Author: ROBOTIS
 *
 */

#include <stdio.h>
#include "RX28M.h"
#include "MotionManager.h"

using namespace Robot;

MotionManager* MotionManager::m_UniqueInstance = new MotionManager();

MotionManager::MotionManager()
{
	m_CM730 = 0;
	m_ProcessEnable = false;
	m_Enabled = false;
	DEBUG_PRINT = false;	
}

MotionManager::~MotionManager()
{
}

bool MotionManager::Initialize(CM730 *cm730)
{
	int value, error;

	m_CM730 = cm730;
	m_Enabled = false;
	m_ProcessEnable = true;

	if(m_CM730->Connect() == false)
	{
		if(DEBUG_PRINT == true)
			fprintf(stderr, "Fail to connect CM-730\n");
		return false;
	}

	for(int id=JointData::ID_R_SHOULDER_PITCH; id<JointData::NUMBER_OF_JOINTS; id++)
	{
		if(DEBUG_PRINT == true)
			fprintf(stderr, "ID:%d initializing...", id);
		
		if(m_CM730->ReadWord(id, RX28M::P_PRESENT_POSITION_L, &value, &error) == CM730::SUCCESS)
		{
			MotionStatus::m_CurrentJoints.SetValue(id, value);
			MotionStatus::m_CurrentJoints.SetEnable(id, true);

			if(DEBUG_PRINT == true)
				fprintf(stderr, "[%d] Success\n", value);
		}
		else
		{
			MotionStatus::m_CurrentJoints.SetEnable(id, false);

			if(DEBUG_PRINT == true)
				fprintf(stderr, " Fail\n");
		}
	}

	m_SensorCalibrated = false;
	m_CalibrationTime = 0;
	m_FBGyroCenter = 0;
	m_RLGyroCenter = 0;

	return true;
}

bool MotionManager::Reinitialize()
{
	m_ProcessEnable = false;

	m_CM730->DXLPowerOn();

	int value, error;
	for(int id=JointData::ID_R_SHOULDER_PITCH; id<JointData::NUMBER_OF_JOINTS; id++)
	{
		if(DEBUG_PRINT == true)
			fprintf(stderr, "ID:%d initializing...", id);
		
		if(m_CM730->ReadWord(id, RX28M::P_PRESENT_POSITION_L, &value, &error) == CM730::SUCCESS)
		{
			MotionStatus::m_CurrentJoints.SetValue(id, value);
			MotionStatus::m_CurrentJoints.SetEnable(id, true);

			if(DEBUG_PRINT == true)
				fprintf(stderr, "[%d] Success\n", value);
		}
		else
		{
			MotionStatus::m_CurrentJoints.SetEnable(id, false);

			if(DEBUG_PRINT == true)
				fprintf(stderr, " Fail\n");
		}
	}

	m_ProcessEnable = true;
}

#define WINDOW_SIZE 30
void MotionManager::Process()
{
	if(m_ProcessEnable == false)
		return;

    int value, error, sum = 0, avr = 512;
    static int fb_array[WINDOW_SIZE] = {512};
    static int buf_idx = 0;

    if(m_CM730->ReadWord(CM730::P_BUTTON, &value, &error) == CM730::SUCCESS)
        MotionStatus::BUTTON = value;

	if(m_Enabled == false)
		return;

	if(m_SensorCalibrated == true)
	{		
		if(m_CM730->ReadWord(CM730::P_GYRO_Y_L, &value, &error) == CM730::SUCCESS)
			MotionStatus::FB_GYRO = value - m_FBGyroCenter;
		if(m_CM730->ReadWord(CM730::P_GYRO_X_L, &value, &error) == CM730::SUCCESS)
			MotionStatus::RL_GYRO = value - m_RLGyroCenter;			
		if(m_CM730->ReadWord(CM730::P_ACCEL_X_L, &value, &error) == CM730::SUCCESS)
			MotionStatus::RL_ACCEL = value;
        if(m_CM730->ReadWord(CM730::P_ACCEL_Y_L, &value, &error) == CM730::SUCCESS)
        {
            MotionStatus::FB_ACCEL = value;
            fb_array[buf_idx] = value;
            if(++buf_idx >= WINDOW_SIZE) buf_idx = 0;
        }

        for(int idx = 0; idx < WINDOW_SIZE; idx++)
            sum += fb_array[idx];
        avr = sum / WINDOW_SIZE;

        if(avr < MotionStatus::FALLEN_F_LIMIT)
            MotionStatus::FALLEN = FORWARD;
        else if(avr > MotionStatus::FALLEN_B_LIMIT)
            MotionStatus::FALLEN = BACKWARD;
        else
            MotionStatus::FALLEN = STANDUP;
	}
	else
	{
		if(m_CalibrationTime <= 20)
		{
			if(m_CM730->ReadWord(CM730::P_GYRO_Y_L, &value, &error) == CM730::SUCCESS)
				m_FBGyroCenter += value;
			if(m_CM730->ReadWord(CM730::P_GYRO_X_L, &value, &error) == CM730::SUCCESS)
				m_RLGyroCenter += value;
			m_CalibrationTime++;
		}
		else
		{
			m_FBGyroCenter = (int)((double)m_FBGyroCenter / (double)m_CalibrationTime);
			m_RLGyroCenter = (int)((double)m_RLGyroCenter / (double)m_CalibrationTime);
			m_SensorCalibrated = true;
		}
	}

	if(m_Modules.size() != 0)
    {
        for(std::list<MotionModule*>::iterator i = m_Modules.begin(); i != m_Modules.end(); i++)
        {
			(*i)->Process();
			for(int id=JointData::ID_R_SHOULDER_PITCH; id<JointData::NUMBER_OF_JOINTS; id++)
			{
				if((*i)->m_Joint.GetEnable(id) == true)
				{
					MotionStatus::m_CurrentJoints.SetSlope(id, (*i)->m_Joint.GetCWSlope(id), (*i)->m_Joint.GetCCWSlope(id));
					MotionStatus::m_CurrentJoints.SetValue(id, (*i)->m_Joint.GetValue(id));
				}
			}
        }
    }

	int param[JointData::NUMBER_OF_JOINTS * 5];
	int n = 0;
	int joint_num = 0;
	for(int id=JointData::ID_R_SHOULDER_PITCH; id<JointData::NUMBER_OF_JOINTS; id++)
	{
		if(MotionStatus::m_CurrentJoints.GetEnable(id) == true)
		{
			param[n++] = id;
			param[n++] = MotionStatus::m_CurrentJoints.GetCWSlope(id);
			param[n++] = MotionStatus::m_CurrentJoints.GetCCWSlope(id);
			param[n++] = CM730::GetLowByte(MotionStatus::m_CurrentJoints.GetValue(id));
			param[n++] = CM730::GetHighByte(MotionStatus::m_CurrentJoints.GetValue(id));
			joint_num++;
		}

		if(DEBUG_PRINT == true)
			fprintf(stderr, "ID[%d] : %d \n", id, MotionStatus::m_CurrentJoints.GetValue(id));
	}
	if(joint_num > 0)
	    m_CM730->SyncWrite(RX28M::P_CW_COMPLIANCE_SLOPE, 5, joint_num, param);
}

void MotionManager::SetEnable(bool enable)
{
	m_Enabled = enable;
	if(m_Enabled == true)
		m_CM730->WriteWord(CM730::ID_BROADCAST, RX28M::P_MOVING_SPEED_L, 0, 0);
}

void MotionManager::AddModule(MotionModule *module)
{
	module->Initialize();
	m_Modules.push_back(module);
}

void MotionManager::RemoveModule(MotionModule *module)
{
	m_Modules.remove(module);
}
