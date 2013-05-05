#ifndef __accelerometer_h__
#define __accelerometer_h__

#include "CM730.h"
#include "dxl_peripheral.h"

class Accelerometer: public DxlPeripheral
{
	public:
		Accelerometer(CM730 &cm730);
		bool ReadRawValues(unsigned short &ax, unsigned short &ay, unsigned short &az);
		bool ReadCartesianValues(double &ax, double &ay, double &az);
		bool ReadSphericalValues(double &magnitude, double &polar_angle, double &azimuth_angle);
};

#endif // __accelerometer_h__

