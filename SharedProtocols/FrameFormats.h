#pragma once
#include <Arduino.h>


// Declare instances of every payload point
// ATTR: payload point = where the payload comes from
const uint8_t DATAPOINT_INSTANCES[] = {
	2,	// uses Serial port
	3,	// uses Analog input
	1	// uses Interrupt
};
#define NUM_FUELCELLS 2
#define NUM_CURRENTSENSORS 3
#define NUM_SPEEDOMETERS 1
#define QUEUEITEM_DATAPOINTS 3	// max of above
// Every time we read from each payload point, how many floats are returned? (float is chosen to be the standard of return type)
const uint8_t DATAPOINT_READVALUES[] = { 
	8,// V, A, W, Wh, T, P, V_c, St (St represented through 0 or 1)
	2,// V, A
	1 // km/h. Duh.
};
#define FUELCELL_READVALUES 8
#define CURRENTSENSOR_READVALUES 2
#define SPEEDOMETER_READVALUES 1
#define QUEUEITEM_READVALUES 8	// max of above

#define FLOAT_TO_STRING_LEN 4
// declare enum to represent every type of payload points
typedef enum
{
	// if these ever exceed 4, need to extend the frame ID "3 bits needed for message type" below into 4 bits
	FC,
	CS,
	SM
}DataSource;
//						  v change this if we ever add more payload points
const char dataPoint_shortNames[3][3] = { "FC", "CS", "SM" };
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
	// 1 frame for timestamp, other frames for floats, +QUEUEITEM_DATAPOINTS frame for odd-number scenarios where one additional frame is needed for the lone float
	bool toQueueItem(QueueItem* putHere);
	void addItem(unsigned long id, byte length, byte* payload);
	void addItem(uint8_t messageType, uint8_t terminatorStatus, float payload1);
	void addItem(uint8_t messageType, uint8_t terminatorStatus, float payload1, float payload2);
	uint8_t getNumFrames();
	void clear();
private:
	uint8_t numFrames = 0;
	void addItem_(uint8_t messageType, uint8_t terminatorStatus, float payload1, float payload2, bool using_payload2);
};
/*	
	--- About Frame IDs ---
2 bits needed for indication of terminating frame
	00 -> start of stream, carries timestamp
	01 -> normal frame in middle of stream
	10 -> terminate transmission for one datapoint
	11 -> terminate transmission for one full string
3 bits needed for message type
	0 -> display FC
	1 -> display CS
	2 -> display SM (include throttle?)
	3 -> *reserved for display payload*
	4 -> buttons
	5 -> *reserved*
	6 -> *reserved*
	7 -> *reserved*

	--- About Frame Length ---
length 4 means frame contains 1 float
length 8 means frame contains 2 float
*/
