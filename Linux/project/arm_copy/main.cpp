/*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *      Author: François Schwarzentruber
 */

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <signal.h>

#include "CM730.h"
#include "LinuxDARwIn.h"
#include "LinuxNetwork.h"

using namespace Robot;

/*handling the signals (Ctrl + C to quit etc.)*/


/*this is the function that kills the program (it is called by callback)*/
void sighandler(int sig)
{
  exit(0);
}


/*this initializes the signals (Ctrl + C to quit etc.)*/
void signalInitialize()
{
  printf("Initialization of the signals...\n");
  signal(SIGABRT, &sighandler);
  signal(SIGTERM, &sighandler);
  signal(SIGQUIT, &sighandler);
  signal(SIGINT, &sighandler);
  printf("Signals initialized\n");
}

// From JointData.h
//JointData::ID_R_SHOULDER_PITCH     = 1,
//JointData::ID_L_SHOULDER_PITCH     = 2,
//JointData::ID_R_SHOULDER_ROLL      = 3,
//JointData::ID_L_SHOULDER_ROLL      = 4,
//JointData::ID_R_ELBOW              = 5,
//JointData::ID_L_ELBOW              = 6,

/*connect to the robot (in order to make motions)*/
void CopyInitialize(CM730& cm730)
{
  printf("Try to connect to the robot...\n");
  if(cm730.Connect() == false)
  {
    printf("ERROR: impossible to connect to the robot\n");
    return;
  }
  printf("Connexion succeeded\n");

  cm730.WriteByte(JointData::ID_L_ELBOW, MX28::P_TORQUE_ENABLE, 0, 0);
  cm730.WriteByte(JointData::ID_L_SHOULDER_ROLL, MX28::P_TORQUE_ENABLE, 0, 0);
  cm730.WriteByte(JointData::ID_L_SHOULDER_PITCH, MX28::P_TORQUE_ENABLE, 0, 0);
  printf("Bras gauche debloque\n");
  cm730.WriteByte(JointData::ID_R_ELBOW, MX28::P_TORQUE_ENABLE, 1, 0);	
  cm730.WriteByte(JointData::ID_R_SHOULDER_ROLL, MX28::P_TORQUE_ENABLE, 1, 0);	
  cm730.WriteByte(JointData::ID_R_SHOULDER_PITCH, MX28::P_TORQUE_ENABLE, 1, 0);
  printf("Bras droit bloque\n");
}





/*the loop that takes the position of the left arm and reports to the right arm*/
void CopyExecute(CM730& cm730)
{
  int v;

  if(cm730.ReadWord(JointData::ID_L_ELBOW, MX28::P_PRESENT_POSITION_L, &v, 0) == CM730::SUCCESS)
  {
    cm730.WriteWord(JointData::ID_R_ELBOW, MX28::P_GOAL_POSITION_L, 4096-v, 0);
  }
  else
  {
    printf("can not read the present position of left elbow\n");
  }

  if(cm730.ReadWord(JointData::ID_L_SHOULDER_ROLL, MX28::P_PRESENT_POSITION_L, &v, 0) == CM730::SUCCESS)
  {
    cm730.WriteWord(JointData::ID_R_SHOULDER_ROLL, MX28::P_GOAL_POSITION_L, 4096-v, 0);
  }
  else
  {
    printf("can not read the present position of left shoulder (pitch)\n");
  }

  if(cm730.ReadWord(JointData::ID_L_SHOULDER_PITCH, MX28::P_PRESENT_POSITION_L, &v, 0) == CM730::SUCCESS)
  {
    cm730.WriteWord(JointData::ID_R_SHOULDER_PITCH, MX28::P_GOAL_POSITION_L, 4096-v, 0);
  }
  else
  {
    printf("can not read the present position of left elbow\n");
  }
}


/*main function*/
int main(void)
{
  printf("Demonstration...\n");
  signalInitialize();

  LinuxCM730 linux_cm730("/dev/ttyUSB0");
  CM730 cm730(&linux_cm730);

  CopyInitialize(cm730);

  while(1) {
    usleep(10000);
    CopyExecute(cm730);
  }
}
