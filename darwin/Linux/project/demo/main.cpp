/*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *      Author: robotis
 */

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>

#include "mjpg_streamer.h"
#include "LinuxDARwIn.h"

#include "StatusCheck.h"
#include "VisionMode.h"

#define INI_FILE_PATH       "config.ini"
#define SCRIPT_FILE_PATH    "script.asc"
#define MOTION_FILE_PATH    "../../../Data/motion.bin"

#define U2D_DEV_NAME0       "/dev/ttyUSB0"
#define U2D_DEV_NAME1       "/dev/ttyUSB1"

LinuxCM730 linux_cm730(U2D_DEV_NAME0);
CM730 cm730(&linux_cm730);

void change_current_dir()
{
    char exepath[1024] = {0};
    if(readlink("/proc/self/exe", exepath, sizeof(exepath)) != -1)
        chdir(dirname(exepath));
}

int main(void)
{
    change_current_dir();

    minIni* ini = new minIni(INI_FILE_PATH);
    Image* rgb_output = new Image(Camera::WIDTH, Camera::HEIGHT, Image::RGB_PIXEL_SIZE);

    LinuxCamera::GetInstance()->Initialize(0);
    LinuxCamera::GetInstance()->SetCameraSettings(CameraSettings());    // set default
    LinuxCamera::GetInstance()->LoadINISettings(ini);                   // load from ini

    mjpg_streamer* streamer = new mjpg_streamer(Camera::WIDTH, Camera::HEIGHT);

    BallTracker tracker = BallTracker();
    tracker.LoadINISettings(ini);
    httpd::ball_finder = &tracker.finder;

    BallFollower follower = BallFollower();

    ColorFinder* red_finder = new ColorFinder(0, 15, 55, 30, 0.3, 50.0);
    red_finder->LoadINISettings(ini, "RED");
    httpd::red_finder = red_finder;

    ColorFinder* yellow_finder = new ColorFinder(50, 15, 50, 30, 0.3, 50.0);
    yellow_finder->LoadINISettings(ini, "YELLOW");
    httpd::yellow_finder = yellow_finder;

    ColorFinder* blue_finder = new ColorFinder(230, 15, 45, 15, 0.3, 50.0);
    blue_finder->LoadINISettings(ini, "BLUE");
    httpd::blue_finder = blue_finder;

    Action::GetInstance()->LoadFile(MOTION_FILE_PATH);

    //MotionManager::GetInstance()->DEBUG_PRINT = true;

    //////////////////// Framework Initialize ////////////////////////////
    if(MotionManager::GetInstance()->Initialize(&cm730) == false)
    {
        linux_cm730.SetPortName(U2D_DEV_NAME1);
        if(MotionManager::GetInstance()->Initialize(&cm730) == false)
        {
            printf("Fail to initialize Motion Manager!\n");
            return 0;
        }
    }
    MotionManager::GetInstance()->AddModule((MotionModule*)Action::GetInstance());
    MotionManager::GetInstance()->AddModule((MotionModule*)Head::GetInstance());
    MotionManager::GetInstance()->AddModule((MotionModule*)Walking::GetInstance());
    LinuxMotionTimer::Initialize(MotionManager::GetInstance());
    /////////////////////////////////////////////////////////////////////

    Walking::GetInstance()->m_Joint.SetEnableBody(false);
    Head::GetInstance()->m_Joint.SetEnableBody(false);
    Action::GetInstance()->m_Joint.SetEnableBody(true);
    MotionManager::GetInstance()->SetEnable(true);

    cm730.WriteByte(CM730::P_LED_PANNEL, 0x01|0x02|0x04, NULL);

    LinuxActionScript::PlayMP3("../../../Data/mp3/Demonstration ready mode.mp3");
    Action::GetInstance()->Start(15);
    while(Action::GetInstance()->IsRunning()) usleep(8*1000);

    while(1)
    {
        StatusCheck::Check(cm730);

        Point2D ball_pos, red_pos, yellow_pos, blue_pos;

        LinuxCamera::GetInstance()->CaptureFrame();
        memcpy(rgb_output->m_ImageData, LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageData, LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageSize);

        if(StatusCheck::m_cur_mode == READY || StatusCheck::m_cur_mode == VISION)
        {
            ball_pos = tracker.finder.GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);
            red_pos = red_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);
            yellow_pos = yellow_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);
            blue_pos = blue_finder->GetPosition(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);

			unsigned char r, g, b;
            for(int i = 0; i < rgb_output->m_NumberOfPixels; i++)
            {
				r = 0; g = 0; b = 0;				
                if(tracker.finder.m_result->m_ImageData[i] == 1)
                {
					r = 255;
                    g = 128;
					b = 0;
                }
                if(red_finder->m_result->m_ImageData[i] == 1)
                {
					if(tracker.finder.m_result->m_ImageData[i] == 1)
					{
						r = 0;
						g = 255;
						b = 0;
					}
					else
					{
						r = 255;
						g = 0;
						b = 0;
					}
                }
                if(yellow_finder->m_result->m_ImageData[i] == 1)
                {
					if(tracker.finder.m_result->m_ImageData[i] == 1)
					{
						r = 0;
						g = 255;
						b = 0;
					}
					else
					{
						r = 255;
						g = 255;
						b = 0;
					}
                }
                if(blue_finder->m_result->m_ImageData[i] == 1)
                {
                    if(tracker.finder.m_result->m_ImageData[i] == 1)
					{
						r = 0;
						g = 255;
						b = 0;
					}
					else
					{
						r = 0;
						g = 0;
						b = 255;
					}
                }

				if(r > 0 || g > 0 || b > 0)
				{
					rgb_output->m_ImageData[i * rgb_output->m_PixelSize + 0] = r;
					rgb_output->m_ImageData[i * rgb_output->m_PixelSize + 1] = g;
					rgb_output->m_ImageData[i * rgb_output->m_PixelSize + 2] = b;
				}
            }
        }
        else if(StatusCheck::m_cur_mode == SOCCER)
        {
            tracker.Process(LinuxCamera::GetInstance()->fbuffer->m_HSVFrame);

            for(int i = 0; i < rgb_output->m_NumberOfPixels; i++)
            {
                if(tracker.finder.m_result->m_ImageData[i] == 1)
                {
                    rgb_output->m_ImageData[i*rgb_output->m_PixelSize + 0] = 255;
                    rgb_output->m_ImageData[i*rgb_output->m_PixelSize + 1] = 128;
                    rgb_output->m_ImageData[i*rgb_output->m_PixelSize + 2] = 0;
                }
            }
        }

        streamer->send_image(rgb_output);

        if(StatusCheck::m_is_started == 0)
            continue;

        switch(StatusCheck::m_cur_mode)
        {
        case READY:
            break;
        case SOCCER:
            if(Action::GetInstance()->IsRunning() == 0)
            {
                Head::GetInstance()->m_Joint.SetEnableHeadOnly(true);
                Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true);
                Action::GetInstance()->m_Joint.SetEnableBody(false);

                follower.Process(tracker.ball_position);

                if(follower.KickBall != 0)
                {
                    Head::GetInstance()->m_Joint.SetEnableHeadOnly(true);
                    Walking::GetInstance()->m_Joint.SetEnableBody(false);
                    Action::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true);

                    if(follower.KickBall == -1)
                    {
                        Action::GetInstance()->Start(12);   // RIGHT KICK
                        fprintf(stderr, "RightKick! \n");
                    }
                    else if(follower.KickBall == 1)
                    {
                        Action::GetInstance()->Start(13);   // LEFT KICK
                        fprintf(stderr, "LeftKick! \n");
                    }
                }
            }
            break;
        case MOTION:
            if(LinuxActionScript::m_is_running == 0)
                LinuxActionScript::ScriptStart(SCRIPT_FILE_PATH);
            break;
        case VISION:
            int detected_color = 0;
            detected_color |= (red_pos.X == -1)? 0 : VisionMode::RED;
            detected_color |= (yellow_pos.X == -1)? 0 : VisionMode::YELLOW;
            detected_color |= (blue_pos.X == -1)? 0 : VisionMode::BLUE;

            if(Action::GetInstance()->IsRunning() == 0)
                VisionMode::Play(detected_color);
            break;
        }
    }

    return 0;
}
