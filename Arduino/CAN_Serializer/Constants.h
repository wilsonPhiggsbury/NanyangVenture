#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <Arduino.h>
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

#endif