#ifndef __left_arm_h__
#define __left_arm_h__

#include "CM730.h"
#include "arm.h"

class LeftArm: public Arm
{
	public:
		LeftArm(CM730 &cm730);
		bool virtual WriteRawElbowPosition(unsigned short pos);
		bool virtual WriteRawShoulderRollPosition(unsigned short pos);
		bool virtual WriteRawShoulderPitchPosition(unsigned short pos);
		bool virtual WriteSphericalPosition(double polar_angle, double azimuth_angle);
};

#endif // __left_arm_h__

