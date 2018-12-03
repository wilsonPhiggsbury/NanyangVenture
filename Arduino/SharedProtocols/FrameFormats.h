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
#define NUM_DATASETS 3	// max of above
// Every time we read from each payload point, how many floats are returned? (float is chosen to be the standard of return type)
const uint8_t FRAME_INFO_SUBSETS[] = { 
	8,	// V, A, W, Wh, T, P, V_c, St (St represented through 0 or 1)
	2,	// V, A
	1,	// km/h
	0,	// Unused
	6	// Headlights, Brake, Lsig, Rsig, Horn(?), Hazard
};
#define FUELCELL_READVALUES 8
#define CURRENTSENSOR_READVALUES 2
#define SPEEDOMETER_READVALUES 1
#define NUM_DATASUBSETS 8	// max of above

#define FLOAT_TO_STRING_LEN 4
#define SHORTNAME_LEN 3
const int MAX_STRING_LEN = SHORTNAME_LEN + 9 + (FLOAT_TO_STRING_LEN + 1)*(NUM_DATASETS*NUM_DATASUBSETS) + NUM_DATASETS;
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
}PacketID;
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
typedef _NV_CanFrames Frames;
typedef _QueueItem Packet;
// struct to pass payload between different tasks in same microcontroller
struct _QueueItem {
	friend struct _NV_CanFrames;
	PacketID ID;
	unsigned long timeStamp;
	float data[NUM_DATASETS][NUM_DATASUBSETS];
	void toString(char* putHere);
	static bool toPacket(char * str, _QueueItem * queueItem); //		<--- *NOT YET verified if it works on AVR chips*
	void toFrames(Frames* putHere);
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
	NV_CanFrame frames[1 + NUM_DATASETS * NUM_DATASUBSETS / 2 + 1 * NUM_DATASETS];
	//void setCANObj(MCP_CAN& CANObj);
	// 1 frame for timestamp, other frames for floats, +QUEUEITEM_DATAPOINTS frame for odd-number scenarios where one additional frame is needed for the lone float
	bool toPacket(Packet* putHere);
	bool addItem(unsigned long id, byte length, byte* payload);
	void addItem(PacketID messageType, uint8_t terminatorStatus, float payload1);
	void addItem(PacketID messageType, uint8_t terminatorStatus, float payload1, float payload2);
	uint8_t getNumFrames();
	void clear();
private:
	uint8_t numFrames = 0;
	//MCP_CAN* CANObj;
	void addItem_(uint8_t messageType, uint8_t terminatorStatus, float payload1, float payload2, bool using_payload2);
};