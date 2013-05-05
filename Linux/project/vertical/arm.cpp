#include "CM730.h"
#include "arm.h"

Arm::Arm(CM730 &cm730, unsigned char shoulder_pitch_id, unsigned char shoulder_roll_id, unsigned char elbow_id):
	ShoulderPitch(cm730, shoulder_pitch_id),
	ShoulderRoll(cm730, shoulder_roll_id),
	Elbow(cm730, elbow_id)
{
}

