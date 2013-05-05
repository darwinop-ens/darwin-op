#ifndef __joint_h__
#define __joint_h__

#include "CM730.h"
#include "dxl_peripheral.h"

class Joint: public DxlPeripheral
{
	private:
		unsigned char m_id;
	public:
		Joint(CM730 &cm730, unsigned char id);
		bool WriteTorqueEnable(bool enable);
		bool WriteRawPosition(unsigned short pos);
		bool ReadTorqueEnable(bool &enable);
		bool ReadRawPosition(unsigned short &pos);
};

#endif // __joint_h__
