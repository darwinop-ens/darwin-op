/*
 *   MX28.cpp
 *
 *  
 *
 */
#include "MX28.h"

using namespace Robot;

const int MX28::MIN_VALUE = 0;

const int MX28::CENTER_VALUE = 2048;
const int MX28::MAX_VALUE = 4095;
const double MX28::MIN_ANGLE = -180.0; // degree
const double MX28::MAX_ANGLE = 180.0; // degree
const double MX28::RATIO_VALUE2ANGLE = 0.088; // 360 / 4096
const double MX28::RATIO_ANGLE2VALUE = 11.378; // 4096 / 360

const int MX28::PARAM_BYTES = 7;
