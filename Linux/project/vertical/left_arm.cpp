#include "CM730.h"
#include "JointData.h"
#include "left_arm.h"

// Joint positions

// bend elbow = 1282
// mid elbow = 2048
// straighten elbow = 3072

// shoulder pitch top = 0
// shoulder pitch front = 1024
// shoulder pitch bottom = 2048
// shoulder pitch back = 3072

// shoulder roll abduction (normal to the body) = 1484
// shoulder roll adduction (along the body) = 2508

LeftArm::LeftArm(CM730 &cm730):
	Arm(cm730, JointData::ID_L_SHOULDER_PITCH, JointData::ID_L_SHOULDER_ROLL, JointData::ID_L_ELBOW)
{
}

bool LeftArm::WriteSphericalPosition(double polar_angle, double azimuth_angle)
{
	if(polar_angle>M_PI_2)
	{
		ShoulderRoll.WriteRawPosition(3532.0 - polar_angle*1024.0/M_PI_2);
		ShoulderPitch.WriteRawPosition(azimuth_angle*1024.0/M_PI_2);
		Elbow.WriteRawPosition(3072);
		return true;
	}
	else
	{
		return false;
	}
}

