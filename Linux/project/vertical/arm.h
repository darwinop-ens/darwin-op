#ifndef __arm_h__
#define __arm_h__

#include "CM730.h"
#include "joint.h"

class Arm
{
	public:
		Arm(CM730 &cm730, unsigned char shoulder_pitch_id, unsigned char shoulder_roll_id, unsigned char elbow_id);
		Joint ShoulderPitch;
		Joint ShoulderRoll;
		Joint Elbow;
		bool virtual WriteSphericalPosition(double polar_angle, double azimuth_angle) = 0;
};

#endif // __arm_h__

