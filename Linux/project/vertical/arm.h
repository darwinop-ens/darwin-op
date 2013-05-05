#ifndef __arm_h__
#define __arm_h__

#include "CM730.h"
#include "dxl_peripheral.h"

class Arm: public DxlPeripheral
{
	public:
		Arm(CM730 &cm730);
		bool virtual WriteRawElbowPosition(unsigned short pos) = 0;
		bool virtual WriteRawShoulderRollPosition(unsigned short pos) = 0;
		bool virtual WriteRawShoulderPitchPosition(unsigned short pos) = 0;
		bool virtual WriteSphericalPosition(double polar_angle, double azimuth_angle) = 0;
};

#endif // __arm_h__

