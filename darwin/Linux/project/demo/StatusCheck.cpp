/*
 * StatusCheck.cpp
 *
 *  Created on: 2011. 1. 21.
 *      Author: zerom
 */

#include <stdio.h>
#include <unistd.h>

#include "StatusCheck.h"
#include "Head.h"
#include "Action.h"
#include "Walking.h"
#include "MotionStatus.h"
#include "MotionManager.h"
#include "LinuxActionScript.h"

#define PLAY_TIME 120.0
#define REST_TIME 60.0

#define MAX_CHICAGO_CNT 90

using namespace Robot;

int StatusCheck::m_chicago_mode_cnt = 0;

int StatusCheck::m_cur_mode     = READY;
int StatusCheck::m_old_btn      = 0;
int StatusCheck::m_is_started   = 0;

int StatusCheck::m_soccer_sub_mode = SOCCER_PLAY;
double StatusCheck::m_soccer_start_time = 0.0;

void StatusCheck::Check(CM730 &cm730)
{
    struct timeval _tv;
    gettimeofday(&_tv, NULL);
    double _curr_time = _tv.tv_sec + (_tv.tv_usec/1000000.0);

    if(MotionStatus::FALLEN != STANDUP && m_cur_mode == SOCCER && m_is_started != 0)
    {
        Walking::GetInstance()->Stop();
        while(Walking::GetInstance()->IsRunning() == 1) usleep(8000);

        Action::GetInstance()->m_Joint.SetEnableBody(true, true);

        if(MotionStatus::FALLEN == FORWARD)
            Action::GetInstance()->Start(10);   // FORWARD GETUP
        else if(MotionStatus::FALLEN == BACKWARD)
            Action::GetInstance()->Start(11);   // BACKWARD GETUP

        while(Action::GetInstance()->IsRunning() == 1) usleep(8000);

        Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
        Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
    }

    if(m_cur_mode == SOCCER && m_is_started == 2)
    {
        if(m_soccer_sub_mode == SOCCER_PLAY && (_curr_time - m_soccer_start_time) > PLAY_TIME)
        {
            Walking::GetInstance()->Stop();
            while(Walking::GetInstance()->IsRunning() == true) usleep(8000);

            Action::GetInstance()->m_Joint.SetEnableBody(true, true);

            while(Action::GetInstance()->Start(15) == false) usleep(8000);
            while(Action::GetInstance()->IsRunning() == true) usleep(8000);

            // TODO: Torque OFF??
            cm730.WriteByte(254, MX28::P_TORQUE_ENABLE, 0, NULL);

            m_soccer_sub_mode = SOCCER_REST;
            gettimeofday(&_tv, NULL);
            m_soccer_start_time = _tv.tv_sec + (_tv.tv_usec/1000000.0);
        }
        else if(m_soccer_sub_mode == SOCCER_REST && (_curr_time - m_soccer_start_time) > REST_TIME)
        {
            MotionManager::GetInstance()->Reinitialize();
            MotionManager::GetInstance()->SetEnable(true);
            LinuxActionScript::PlayMP3("../../../Data/mp3/Start soccer demonstration.mp3");

            Action::GetInstance()->m_Joint.SetEnableBody(true, true);

            while(Action::GetInstance()->Start(15) == false) usleep(8000);
            while(Action::GetInstance()->IsRunning() == true) usleep(8000);

            Action::GetInstance()->Start(9);
            while(Action::GetInstance()->IsRunning() == true) usleep(8000);

            Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
            Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);

            MotionManager::GetInstance()->ResetGyroCalibration();
            while(1)
            {
                if(MotionManager::GetInstance()->GetCalibrationStatus() == 1)
                {
                    LinuxActionScript::PlayMP3("../../../Data/mp3/Sensor calibration complete.mp3");
                    break;
                }
                else if(MotionManager::GetInstance()->GetCalibrationStatus() == -1)
                {
                    LinuxActionScript::PlayMP3Wait("../../../Data/mp3/Sensor calibration fail.mp3");
                    MotionManager::GetInstance()->ResetGyroCalibration();
                }
                usleep(8000);
            }

            m_soccer_sub_mode = SOCCER_PLAY;
            gettimeofday(&_tv, NULL);
            m_soccer_start_time = _tv.tv_sec + (_tv.tv_usec/1000000.0);
        }
    }

    if(m_old_btn == MotionStatus::BUTTON)
    {
        if(m_cur_mode == READY && m_is_started == 0 && m_chicago_mode_cnt > MAX_CHICAGO_CNT)
        {
            fprintf(stderr, "CHICAGO MODE ! \n\n");
			m_cur_mode = SOCCER;
			m_old_btn == BTN_START;
        }
        else if(m_cur_mode == READY && m_is_started == 0 && m_chicago_mode_cnt <= MAX_CHICAGO_CNT)
        {
            m_chicago_mode_cnt++;
			return;
        }
		else
        {
            m_chicago_mode_cnt = 0;
            return;
        }
    }
    else
    {
        m_chicago_mode_cnt = 0;
        m_old_btn = MotionStatus::BUTTON;
    }

    if(m_old_btn & BTN_MODE)
    {
        fprintf(stderr, "Mode button pressed.. \n");

        if(m_is_started != 0)
        {
            m_is_started    = 0;
            m_cur_mode      = READY;
            LinuxActionScript::m_stop = 1;

            Walking::GetInstance()->Stop();
            Action::GetInstance()->m_Joint.SetEnableBody(true, true);

            while(Action::GetInstance()->Start(15) == false) usleep(8000);
            while(Action::GetInstance()->IsRunning() == true) usleep(8000);
        }
        else
        {
            m_cur_mode++;
            if(m_cur_mode >= MAX_MODE) m_cur_mode = READY;
        }

        MotionManager::GetInstance()->SetEnable(false);
        usleep(10000);

        if(m_cur_mode == READY)
        {
            cm730.WriteByte(CM730::P_LED_PANNEL, 0x01|0x02|0x04, NULL);
            LinuxActionScript::PlayMP3("../../../Data/mp3/Demonstration ready mode.mp3");
        }
        else if(m_cur_mode == SOCCER)
        {
            cm730.WriteByte(CM730::P_LED_PANNEL, 0x01, NULL);
            LinuxActionScript::PlayMP3("../../../Data/mp3/Autonomous soccer mode.mp3");
        }
        else if(m_cur_mode == MOTION)
        {
            cm730.WriteByte(CM730::P_LED_PANNEL, 0x02, NULL);
            LinuxActionScript::PlayMP3("../../../Data/mp3/Interactive motion mode.mp3");
        }
        else if(m_cur_mode == VISION)
        {
            cm730.WriteByte(CM730::P_LED_PANNEL, 0x04, NULL);
            LinuxActionScript::PlayMP3("../../../Data/mp3/Vision processing mode.mp3");
        }
    }

    if(m_old_btn & BTN_START)
    {
        if(m_is_started == 0)
        {
            fprintf(stderr, "Start button pressed.. & started is false.. \n");

            if(m_cur_mode == SOCCER)
            {
                MotionManager::GetInstance()->Reinitialize();
                MotionManager::GetInstance()->SetEnable(true);
                if(m_chicago_mode_cnt > MAX_CHICAGO_CNT)
                {
				    m_chicago_mode_cnt = 0;
                    m_is_started = 2;
                }
				else
                    m_is_started = 1;
                LinuxActionScript::PlayMP3("../../../Data/mp3/Start soccer demonstration.mp3");

                Action::GetInstance()->m_Joint.SetEnableBody(true, true);

                Action::GetInstance()->Start(9);
                while(Action::GetInstance()->IsRunning() == true) usleep(8000);

                Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
                Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);

                MotionManager::GetInstance()->ResetGyroCalibration();
                while(1)
                {
                    if(MotionManager::GetInstance()->GetCalibrationStatus() == 1)
                    {
                        LinuxActionScript::PlayMP3("../../../Data/mp3/Sensor calibration complete.mp3");
                        break;
                    }
                    else if(MotionManager::GetInstance()->GetCalibrationStatus() == -1)
                    {
                        LinuxActionScript::PlayMP3Wait("../../../Data/mp3/Sensor calibration fail.mp3");
                        MotionManager::GetInstance()->ResetGyroCalibration();
                    }
                    usleep(8000);
                }

                m_soccer_sub_mode = SOCCER_PLAY;
                gettimeofday(&_tv, NULL);
                m_soccer_start_time = _tv.tv_sec + (_tv.tv_usec/1000000.0);
            }
            else if(m_cur_mode == MOTION)
            {
                MotionManager::GetInstance()->Reinitialize();
                MotionManager::GetInstance()->SetEnable(true);
                m_is_started = 1;
                LinuxActionScript::PlayMP3("../../../Data/mp3/Start motion demonstration.mp3");

                // Joint Enable..
                Action::GetInstance()->m_Joint.SetEnableBody(true, true);

                Action::GetInstance()->Start(1);
                while(Action::GetInstance()->IsRunning() == true) usleep(8000);
            }
            else if(m_cur_mode == VISION)
            {
                MotionManager::GetInstance()->Reinitialize();
                MotionManager::GetInstance()->SetEnable(true);
                m_is_started = 1;
                LinuxActionScript::PlayMP3("../../../Data/mp3/Start vision processing demonstration.mp3");

                // Joint Enable...
                Action::GetInstance()->m_Joint.SetEnableBody(true, true);

                Action::GetInstance()->Start(1);
                while(Action::GetInstance()->IsRunning() == true) usleep(8000);
            }
        }
        else
        {
            fprintf(stderr, "Start button pressed.. & started is true.. \n");
        }
    }
}
