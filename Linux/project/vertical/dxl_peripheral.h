#ifndef __dxl_peripheral_h__
#define __dxl_peripheral_h__

#include "CM730.h"

using namespace Robot;

class DxlPeripheral
{
	protected:
		CM730 &sub_controller;
	public:
		DxlPeripheral(CM730 &cm730);
};

#endif // __dxl_peripheral_h__

