#pragma once
#include <Arduino.h>
// Declare instances of every data point
// data point = where the data comes from
#define NUM_FUELCELLS 2				// uses Serial port
#define NUM_CURRENTSENSORS 3		// uses Analog input
#define NUM_SPEEDOMETERS 1			// uses Interrupt
#define QUEUEITEM_DATAPOINTS max(max(NUM_FUELCELLS, NUM_CURRENTSENSORS), NUM_SPEEDOMETERS)
// Every time we read from each data point, how many floats are returned? (float is chosen to be the standard of return type)
#define FUELCELL_READVALUES 8		// V, A, W, Wh, T, P, V_c, St (St represented through 0 or 1)
#define CURRENTSENSOR_READVALUES 2	// V, A
#define SPEEDOMETER_READVALUES 1	// km/h. Duh.
#define QUEUEITEM_READVALUES max(max(FUELCELL_READVALUES, CURRENTSENSOR_READVALUES), SPEEDOMETER_READVALUES)

#define FLOAT_TO_STRING_LEN 4

// declare enum to represent every type of data points
typedef enum
{
	FC,
	CS,
	SM
}DataSource;
//						  v change this if we ever add more data points
extern char dataPoint_shortNames[3][3];
// struct to pass data between different tasks in same microcontroller
typedef struct {
	DataSource ID;
	unsigned long timeStamp;
	float data[QUEUEITEM_DATAPOINTS][QUEUEITEM_READVALUES];
	void toString(char* putHere);
}QueueItem;

// struct to pass data between different microcontrollers
typedef struct
{
	uint32_t id;
	byte length;
	byte data;
}NV_CanFrame;