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

#include "abstracct


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



/*main function*/
int main(void)
{
  printf("Demonstration...\n");
  signalInitialize();

  LinuxCM730 linux_cm730("/dev/ttyUSB0");
  CM730* cm730 = new CM730(&linux_cm730);

  AbstractBehavior behavior = new BehaviorArmCopy(cm730);

  while(1) {
    usleep(10000);
    CopyExecute(cm730);
  }
}
