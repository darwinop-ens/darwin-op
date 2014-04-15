#include "CM730.h"
#include "arm.h"

Arm::Arm(CM730 &cm730, unsigned char shoulder_pitch_id, unsigned char shoulder_roll_id, unsigned char elbow_id):
	ShoulderPitch(cm730, shoulder_pitch_id),
	ShoulderRoll(cm730, shoulder_roll_id),
	Elbow(cm730, elbow_id)
{
}

bool Arm::ReadRawPosition(unsigned short &shoulder_pitch, unsigned short &shoulder_roll, unsigned short &elbow)
{
	bool result;
	result  = ShoulderPitch.ReadRawPosition(shoulder_pitch);
	result &= ShoulderRoll.ReadRawPosition(shoulder_roll);
	result &= Elbow.ReadRawPosition(elbow);
	return result;
}

bool Arm::WriteRawPosition(unsigned short shoulder_pitch, unsigned short shoulder_roll, unsigned short elbow)
{
	bool result;
	result  = ShoulderPitch.WriteRawPosition(shoulder_pitch);
	result &= ShoulderRoll.WriteRawPosition(shoulder_roll);
	result &= Elbow.WriteRawPosition(elbow);
	return result;
}
