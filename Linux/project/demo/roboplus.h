#include "LinuxNetwork.h"
#include "LinuxMotionTimer.h"
#include "CM730.h"

//void roboplus_init(Robot::LinuxServer &server, bool &connected);

void roboplus_exec(Robot::CM730 &cm730, Robot::LinuxServer &server, Robot::LinuxSocket &new_sock, bool &connected, Robot::LinuxMotionTimer &motion_timer);

//void roboplus_close(void);
