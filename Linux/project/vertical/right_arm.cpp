#include "CM730.h"
#include "JointData.h"
#include "right_arm.h"

// Joint positions

// bend elbow = 2812
// mid elbow = 2048
// straighten elbow = 1024

// shoulder pitch top = 0
// shoulder pitch front = 3072
// shoulder pitch bottom = 2048
// shoulder pitch back = 1024

// shoulder roll abduction (normal to the body) = 2600
// shoulder roll adduction (along the body) = 1576

RightArm::RightArm(CM730 &cm730):
	Arm(cm730, JointData::ID_R_SHOULDER_PITCH, JointData::ID_R_SHOULDER_ROLL, JointData::ID_R_ELBOW)
{
}

bool RightArm::WriteSphericalPosition(double polar_angle, double azimuth_angle)
{
	double x, y, z;
	double body_magnitude, roll, pitch;

	x = sin(polar_angle)*cos(azimuth_angle);
	y = sin(polar_angle)*sin(azimuth_angle);
	z = cos(polar_angle);

	if(x<0)
	{
		body_magnitude = sqrt(y*y + z*z);
		roll=atan2(body_magnitude,-x);
		if(body_magnitude > 0.0)
		{
			pitch = atan2(-y,z);
		}
		else
		{
			pitch = 0.0;
		}			
		ShoulderRoll.WriteRawPosition(2600.0 - roll*1024.0/M_PI_2);
		ShoulderPitch.WriteRawPosition(pitch*1024.0/M_PI_2);
		Elbow.WriteRawPosition(1024);
		return true;
	}
	else
	{
		return false;
	}
}

