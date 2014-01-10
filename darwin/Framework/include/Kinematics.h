/*
 *   Kinematics.h
 *
 *   Author: ROBOTIS
 *
 */

#ifndef _KINEMATICS_H_
#define _KINEMATICS_H_

#include "Matrix.h"
#include "JointData.h"

namespace Robot
{
	class Kinematics
	{
	private:
		static Kinematics* m_UniqueInstance;
        Kinematics();

	protected:

	public:
		static const double CAMERA_DISTANCE; //mm
		static const double EYE_TILT_OFFSET_ANGLE; //degree
		static const double LEG_SIDE_OFFSET; //mm
		static const double THIGH_LENGTH; //mm
		static const double CALF_LENGTH; //mm
		static const double ANKLE_LENGTH; //mm
		static const double LEG_LENGTH; //mm (THIGH_LENGTH + CALF_LENGTH + ANKLE_LENGTH)

		~Kinematics();

		static Kinematics* GetInstance()			{ return m_UniqueInstance; }
	};
}

#endif
