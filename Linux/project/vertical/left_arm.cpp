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

LeftArm::LeftArm(CM730 *cm730):
	Arm(cm730)
{
}

bool LeftArm::WriteRawElbowPosition(unsigned short pos)
{
	int result;
	result = sub_controller->WriteWord(JointData::ID_L_ELBOW, MX28::P_GOAL_POSITION_L, pos & 0x0FFF, 0);
	return (result == CM730::SUCCESS);
}

bool LeftArm::WriteRawShoulderRollPosition(unsigned short pos)
{
	int result;
	result = sub_controller->WriteWord(JointData::ID_L_SHOULDER_ROLL, MX28::P_GOAL_POSITION_L, pos & 0x0FFF, 0);
	return (result == CM730::SUCCESS);
}

bool LeftArm::WriteRawShoulderPitchPosition(unsigned short pos)
{
	int result;
	result = sub_controller->WriteWord(JointData::ID_L_SHOULDER_PITCH, MX28::P_GOAL_POSITION_L, pos & 0x0FFF, 0);
	return (result == CM730::SUCCESS);
}

bool LeftArm::WriteSphericalPosition(double polar_angle, double azimuth_angle)
{
	double x, y, z;
	double body_magnitude, roll, pitch;

	x = sin(polar_angle)*cos(azimuth_angle);
	y = sin(polar_angle)*sin(azimuth_angle);
	z = cos(polar_angle);

	if(x>0)
	{
		body_magnitude = sqrt(y*y + z*z);
		roll=atan2(body_magnitude,x);
		if(body_magnitude > 0.0)
		{
			pitch = atan2(y,z);
		}
		else
		{
			pitch = 0.0;
		}			
		WriteRawShoulderRollPosition(1484.0 + roll*1024.0/M_PI_2);
		WriteRawShoulderPitchPosition(pitch*1024.0/M_PI_2);
		WriteRawElbowPosition(3072);
		return true;
	}
	else
	{
		return false;
	}
}

