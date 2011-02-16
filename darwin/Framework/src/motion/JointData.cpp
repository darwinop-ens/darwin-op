/*
 *   JointData.cpp
 *
 *   Author: ROBOTIS
 *
 */

#include "RX28M.h"
#include "JointData.h"

using namespace Robot;

JointData::JointData()
{
	for(int i=0; i<NUMBER_OF_JOINTS; i++)
	{
		m_Enable[i] = true;
		m_Value[i] = RX28M::CENTER_VALUE;
		m_Angle[i] = 0.0;
		m_CWSlope[i] = SLOPE_DEFAULT;
		m_CCWSlope[i] = SLOPE_DEFAULT;
	}
}

JointData::~JointData()
{
}

void JointData::SetEnable(int id, bool enable)
{
	m_Enable[id] = enable;
}

void JointData::SetEnableHeadOnly(bool enable)
{
    if(enable) SetEnableBody(false);
	m_Enable[ID_HEAD_PAN] = enable;
	m_Enable[ID_HEAD_TILT] = enable;
}

void JointData::SetEnableRightArmOnly(bool enable)
{
    if(enable) SetEnableBody(false);
	m_Enable[ID_R_SHOULDER_PITCH] = enable;
	m_Enable[ID_R_SHOULDER_ROLL] = enable;
	m_Enable[ID_R_ELBOW] = enable;
}

void JointData::SetEnableLeftArmOnly(bool enable)
{
    if(enable) SetEnableBody(false);
	m_Enable[ID_L_SHOULDER_PITCH] = enable;
	m_Enable[ID_L_SHOULDER_ROLL] = enable;
	m_Enable[ID_L_ELBOW] = enable;
}

void JointData::SetEnableRightLegOnly(bool enable)
{
    if(enable) SetEnableBody(false);
	m_Enable[ID_R_HIP_YAW] = enable;
	m_Enable[ID_R_HIP_ROLL] = enable;
	m_Enable[ID_R_HIP_PITCH] = enable;
	m_Enable[ID_R_KNEE] = enable;
	m_Enable[ID_R_ANKLE_PITCH] = enable;
	m_Enable[ID_R_ANKLE_ROLL] = enable;
}

void JointData::SetEnableLeftLegOnly(bool enable)
{
    if(enable) SetEnableBody(false);
	m_Enable[ID_L_HIP_YAW] = enable;
	m_Enable[ID_L_HIP_ROLL] = enable;
	m_Enable[ID_L_HIP_PITCH] = enable;
	m_Enable[ID_L_KNEE] = enable;
	m_Enable[ID_L_ANKLE_PITCH] = enable;
	m_Enable[ID_L_ANKLE_ROLL] = enable;
}

void JointData::SetEnableUpperBodyWithoutHead(bool enable)
{
    if(enable) SetEnableBody(false);
    m_Enable[ID_R_SHOULDER_PITCH] = enable;
    m_Enable[ID_R_SHOULDER_ROLL] = enable;
    m_Enable[ID_R_ELBOW] = enable;
    m_Enable[ID_L_SHOULDER_PITCH] = enable;
    m_Enable[ID_L_SHOULDER_ROLL] = enable;
    m_Enable[ID_L_ELBOW] = enable;
}

void JointData::SetEnableLowerBody(bool enable)
{
    if(enable) SetEnableBody(false);
    m_Enable[ID_R_HIP_YAW] = enable;
    m_Enable[ID_R_HIP_ROLL] = enable;
    m_Enable[ID_R_HIP_PITCH] = enable;
    m_Enable[ID_R_KNEE] = enable;
    m_Enable[ID_R_ANKLE_PITCH] = enable;
    m_Enable[ID_R_ANKLE_ROLL] = enable;
    m_Enable[ID_L_HIP_YAW] = enable;
    m_Enable[ID_L_HIP_ROLL] = enable;
    m_Enable[ID_L_HIP_PITCH] = enable;
    m_Enable[ID_L_KNEE] = enable;
    m_Enable[ID_L_ANKLE_PITCH] = enable;
    m_Enable[ID_L_ANKLE_ROLL] = enable;
}

void JointData::SetEnableBodyWithoutHead(bool enable)
{
    if(enable) SetEnableBody(false);
    m_Enable[ID_R_SHOULDER_PITCH] = enable;
    m_Enable[ID_R_SHOULDER_ROLL] = enable;
    m_Enable[ID_R_ELBOW] = enable;
    m_Enable[ID_L_SHOULDER_PITCH] = enable;
    m_Enable[ID_L_SHOULDER_ROLL] = enable;
    m_Enable[ID_L_ELBOW] = enable;
    m_Enable[ID_R_HIP_YAW] = enable;
    m_Enable[ID_R_HIP_ROLL] = enable;
    m_Enable[ID_R_HIP_PITCH] = enable;
    m_Enable[ID_R_KNEE] = enable;
    m_Enable[ID_R_ANKLE_PITCH] = enable;
    m_Enable[ID_R_ANKLE_ROLL] = enable;
    m_Enable[ID_L_HIP_YAW] = enable;
    m_Enable[ID_L_HIP_ROLL] = enable;
    m_Enable[ID_L_HIP_PITCH] = enable;
    m_Enable[ID_L_KNEE] = enable;
    m_Enable[ID_L_ANKLE_PITCH] = enable;
    m_Enable[ID_L_ANKLE_ROLL] = enable;
}

void JointData::SetEnableBody(bool enable)
{
    for(int id = 1; id < NUMBER_OF_JOINTS; id++)
        m_Enable[id] = enable;
}

bool JointData::GetEnable(int id)
{
	return m_Enable[id];
}

void JointData::SetValue(int id, int value)
{
	if(value < RX28M::MIN_VALUE)
		value = RX28M::MIN_VALUE;
	else if(value >= RX28M::MAX_VALUE)
		value = RX28M::MAX_VALUE;

	m_Value[id] = value;
	m_Angle[id] = (double)(value - RX28M::CENTER_VALUE) * RX28M::RATIO_VALUE2ANGLE;
}

int JointData::GetValue(int id)
{
	return m_Value[id];
}

void JointData::SetAngle(int id, double angle)
{
	if(angle < RX28M::MIN_ANGLE)
		angle = RX28M::MIN_ANGLE;
	else if(angle > RX28M::MAX_ANGLE)
		angle = RX28M::MAX_ANGLE;

	m_Angle[id] = angle;
	m_Value[id] = (int)(angle * RX28M::RATIO_ANGLE2VALUE) + RX28M::CENTER_VALUE;
}

double JointData::GetAngle(int id)
{
	return m_Angle[id];
}

void JointData::SetRadian(int id, double radian)
{
	SetAngle(id, radian * (180.0 / 3.141592));
}

double JointData::GetRadian(int id)
{
	return GetAngle(id) * (180.0 / 3.141592); 
}

void JointData::SetSlope(int id, int cwSlope, int ccwSlope)
{
	SetCWSlope(id, cwSlope);
	SetCCWSlope(id, ccwSlope);
}

void JointData::SetCWSlope(int id, int cwSlope)
{
	m_CWSlope[id] = cwSlope;
}

int JointData::GetCWSlope(int id)
{
	return m_CWSlope[id];
}

void JointData::SetCCWSlope(int id, int ccwSlope)
{
	m_CCWSlope[id] = ccwSlope;
}

int JointData::GetCCWSlope(int id)
{
	return m_CCWSlope[id];
}
