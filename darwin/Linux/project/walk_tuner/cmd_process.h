#ifndef _DXL_MANAGER_CMD_PROCESS_H_
#define _DXL_MANAGER_CMD_PROCESS_H_


#include "LinuxDARwIn.h"


#define PROGRAM_VERSION		"v1.00"
#define SCREEN_COL			35
#define SCREEN_ROW			25

// Position of Column
#define CMD_COL			2
#define PARAM_COL		27

// Position of Row
#define WALKING_MODE_ROW			0
#define X_OFFSET_ROW				1
#define Y_OFFSET_ROW				2
#define Z_OFFSET_ROW				3
#define ROLL_OFFSET_ROW				4
#define PITCH_OFFSET_ROW			5
#define YAW_OFFSET_ROW				6
#define HIP_PITCH_OFFSET_ROW		7
#define AUTO_BALANCE_ROW			8
#define PERIOD_TIME_ROW				9
#define DSP_RATIO_ROW				10
#define STEP_FORWARDBACK_ROW		11
#define STEP_RIGHTLEFT_ROW			12
#define STEP_DIRECTION_ROW			13
#define TURNING_AIM_ROW				14
#define FOOT_HEIGHT_ROW				15
#define SWING_RIGHTLEFT_ROW			16
#define SWING_TOPDOWN_ROW			17
#define PELVIS_OFFSET_ROW			18
#define ARM_SWING_GAIN_ROW			19
#define BAL_KNEE_GAIN_ROW			20
#define BAL_ANKLE_PITCH_GAIN_ROW	21
#define BAL_HIP_ROLL_GAIN_ROW		22
#define BAL_ANKLE_ROLL_GAIN_ROW		23
#define CMD_ROW						24


int _getch();
bool AskSave();


// Move cursor
void GoToCursor(int col, int row);
void MoveUpCursor();
void MoveDownCursor();
void MoveLeftCursor();
void MoveRightCursor();

// Disp & Drawing
void DrawIntro(Robot::CM730 *cm730);
void DrawEnding();
void DrawScreen();
void ClearCmd();
void PrintCmd(const char *message);

// Edit value
void IncreaseValue(bool large);
void DecreaseValue(bool large);

// Command process
void BeginCommandMode();
void EndCommandMode();
void HelpCmd();
void SaveCmd();
void MonitorCmd();


#endif
