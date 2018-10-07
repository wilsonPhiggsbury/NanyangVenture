#pragma once
#include "FrameFormats.h"
const uint8_t DATAPOINT_INSTANCES[] = {
	2,	// uses Serial port
	3,	// uses Analog input
	1	// uses Interrupt
};
const uint8_t DATAPOINT_READVALUES[] = {
	8,// V, A, W, Wh, T, P, V_c, St (St represented through 0 or 1)
	2,// V, A
	1 // km/h. Duh.
};
enum DataSource
{
	// if these ever exceed 4, need to extend the frame ID "3 bits needed for message type" below into 4 bits
	FC,
	CS,
	SM
};
enum TerminatorStatus
{
	HEADER_FRAME,
	NORMAL_FRAME,
	SOFT_TERMINATING_FRAME,
	HARD_TERMINATING_FRAME
};
const char dataPoint_shortNames[DATAPOINTS][SHORTNAME_LEN] = { "FC", "CS", "SM" };
const int MAX_STRING_LEN = SHORTNAME_LEN + 9 + (FLOAT_TO_STRING_LEN + 1)*(QUEUEITEM_DATAPOINTS*QUEUEITEM_READVALUES) + QUEUEITEM_DATAPOINTS;
//typedef enum
//{
//	// if these ever exceed 4, need to extend the frame ID "3 bits needed for message type" below into 4 bits
//	FC,
//	CS,
//	SM
//}DataSource;
//typedef enum
//{
//	HEADER_FRAME,
//	NORMAL_FRAME,
//	SOFT_TERMINATING_FRAME,
//	HARD_TERMINATING_FRAME
//}TerminatorStatus;