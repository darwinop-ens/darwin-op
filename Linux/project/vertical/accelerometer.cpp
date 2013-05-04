#include "accelerometer.h"
#include <math.h>

Accelerometer::Accelerometer(CM730 *cm730):
	DxlPeripheral(cm730)
{
}

bool Accelerometer::ReadRawValues(unsigned short &ax, unsigned short &ay, unsigned short &az)
{
	unsigned char table[CM730::MAXNUM_ADDRESS];
	int result;

	for(int i=0; i<CM730::MAXNUM_ADDRESS; i++)
		table[i] = 0;

	result = sub_controller->ReadTable(CM730::ID_CM, CM730::P_ACCEL_X_L, CM730::P_ACCEL_Z_H, table, 0);

	if (result == CM730::SUCCESS)
	{
		ax = ((unsigned short)table[CM730::P_ACCEL_X_H] << 8) | table[CM730::P_ACCEL_X_L];
		ay = ((unsigned short)table[CM730::P_ACCEL_Y_H] << 8) | table[CM730::P_ACCEL_Y_L];
		az = ((unsigned short)table[CM730::P_ACCEL_Z_H] << 8) | table[CM730::P_ACCEL_Z_L];
		return true;
	}
	else
	{
		ax = 0;
		ay = 0;
		az = 0;
		return false;
	}
}

bool Accelerometer::ReadCartesianValues(double &ax, double &ay, double &az)
{
	unsigned short rawx, rawy, rawz;
	bool result;
	result = ReadRawValues(rawx, rawy, rawz);
	ax = (rawx - 512.0) / 128.0;
	ay = (rawy - 512.0) / 128.0;
	az = (rawz - 512.0) / 128.0;
	return result;
}

bool Accelerometer::ReadSphericalValues(double &magnitude, double &polar_angle, double &azimuth_angle)
{
	double ax, ay, az, horizontal_magnitude;
	bool result;

	result = ReadCartesianValues(ax, ay, az);

	horizontal_magnitude = ax*ax + ay*ay;
	magnitude = sqrt(horizontal_magnitude + az*az);
	horizontal_magnitude = sqrt(horizontal_magnitude);

	if (magnitude > 0.0)
	{
		polar_angle = atan2(horizontal_magnitude,az);
		if (horizontal_magnitude > 0.0)
		{
			azimuth_angle = atan2(ay,ax);
		}
		else
		{
			azimuth_angle = 0.0;
		}
		return result;
	}
	else
	{
		polar_angle = 0.0;
		azimuth_angle = 0.0;
		return false;
	}
}

