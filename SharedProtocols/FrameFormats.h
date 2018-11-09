#pragma once
#include <Arduino.h>
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#ifdef __AVR__

#elif defined _SAM3XA_
#endif

#define NV_CANSPEED CAN_1000KBPS
// Declare instances of every payload point
// ATTR: payload point = where the payload comes from
const uint8_t FRAME_INFO_SETS[] = {
	2,	// 2 sets of Fuel Cell data, via Serial port
	3,	// 3 sets of Current Sensor data, via Analog input
	1,	// 1 set of Speedometer data, uses Interrupt
	0,	// Unused
	1	// 1 set of Buttons
};
#define NUM_FUELCELLS 2
#define NUM_CURRENTSENSORS 3
#define NUM_SPEEDOMETERS 1
#define QUEUEITEM_DATAPOINTS 3	// max of above
// Every time we read from each payload point, how many floats are returned? (float is chosen to be the standard of return type)
const uint8_t FRAME_INFO_QUANTITY_PER_SET[] = { 
	8,	// V, A, W, Wh, T, P, V_c, St (St represented through 0 or 1)
	2,	// V, A
	1,	// km/h
	0,	// Unused
	6	// Headlights, Brake, Lsig, Rsig, Horn(?), Hazard
};
#define FUELCELL_READVALUES 8
#define CURRENTSENSOR_READVALUES 2
#define SPEEDOMETER_READVALUES 1
#define QUEUEITEM_READVALUES 8	// max of above

#define FLOAT_TO_STRING_LEN 4
#define SHORTNAME_LEN 3
const int MAX_STRING_LEN = SHORTNAME_LEN + 9 + (FLOAT_TO_STRING_LEN + 1)*(QUEUEITEM_DATAPOINTS*QUEUEITEM_READVALUES) + QUEUEITEM_DATAPOINTS;
//										^^ 9 bits for timeStamp + '\t'
// declare enum to represent every type of payload points
#define FRAMETYPES 5
typedef enum
{
	// if these ever exceed 4, need to extend the frame ID "3 bits needed for message type" below into 4 bits
	FC,
	CS,
	SM,
	N1,
	BT
}DataSource;
typedef enum {
	Headlights,
	Horn,
	Lsig,
	Rsig,
	Wiper,
	Hazard
}Peripheral;
const char frameType_shortNames[FRAMETYPES][SHORTNAME_LEN] = {"FC", "CS", "SM", "??", "BT"};
struct _NV_CanFrame;
struct _NV_CanFrames;
struct _QueueItem;
typedef _NV_CanFrame NV_CanFrame;
typedef _NV_CanFrames NV_CanFrames;
typedef _QueueItem QueueItem;
// struct to pass payload between different tasks in same microcontroller
struct _QueueItem {
	friend struct _NV_CanFrames;
	DataSource ID;
	unsigned long timeStamp;
	float data[QUEUEITEM_DATAPOINTS][QUEUEITEM_READVALUES];
	void toString(char* putHere);
	static bool toQueueItem(char * str, _QueueItem * queueItem); //		<--- *NOT YET verified if it works on AVR chips*
	void toFrames(NV_CanFrames* putHere);
};
// struct to pass payload between different microcontrollers
struct _NV_CanFrame
{
	uint32_t id;
	byte length;
	byte payload[8];
};
struct _NV_CanFrames
{
	friend struct _QueueItem;
	NV_CanFrame frames[1 + QUEUEITEM_DATAPOINTS * QUEUEITEM_READVALUES / 2 + 1 * QUEUEITEM_DATAPOINTS];
	//void setCANObj(MCP_CAN& CANObj);
	// 1 frame for timestamp, other frames for floats, +QUEUEITEM_DATAPOINTS frame for odd-number scenarios where one additional frame is needed for the lone float
	bool toQueueItem(QueueItem* putHere);
	bool addItem(unsigned long id, byte length, byte* payload);
	void addItem(DataSource messageType, uint8_t terminatorStatus, float payload1);
	void addItem(DataSource messageType, uint8_t terminatorStatus, float payload1, float payload2);
	uint8_t getNumFrames();
	void clear();
private:
	uint8_t numFrames = 0;
	//MCP_CAN* CANObj;
	void addItem_(uint8_t messageType, uint8_t terminatorStatus, float payload1, float payload2, bool using_payload2);
};
/*
--- About Frame IDs ---															ID[10:0]
"terminator status bits": 2 bits needed for indication of terminating frame.	ID[1:0]
00 -> start of stream, carries timestamp
01 -> normal frame in middle of stream
10 -> terminate transmission for one datapoint (soft termination)
11 -> terminate transmission for one full string (hard termination)

"message type bits": 3 bits needed for message type.							ID[4:2], shift left by 2
0 -> display FC
1 -> display CS
2 -> display SM (include throttle?)
3 -> *reserved for display payload*
4 -> buttons
5 -> *reserved*
6 -> *reserved*
7 -> *reserved*

--- About Frame Length ---
From NV10_dashboard:
length 4 for headlights, wiper, Lsig, Rsig
From NV10_back:
length 4 means frame contains 1 float
length 8 means frame contains 2 float

*/