#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define DEBUG 0
#if DEBUG
#define debug_(str) Serial.print(str)
#define debug(str)  Serial.println(str)
#else
#define debug_(...)
#define debug(...)
#endif

#include <Arduino.h>
#define NV_CANSPEED CAN_1000KBPS
// Declare instances of every payload point
// ATTR: payload point = where the payload comes from
const uint8_t FRAME_INFO_SETS[] = {
	1,	// 2 sets of Fuel Cell data, via Serial port
	3,	// 3 sets of Current Sensor data, via Analog input
	1,	// 1 set of Speedometer data, uses Interrupt
	1,	// 1 set of brake light data
	1	// 1 set of Buttons
};
#define NUM_FUELCELLS 1
#define NUM_CURRENTSENSORS 3
#define NUM_SPEEDOMETERS 1
#define NUM_DATASETS 3	// max of above
// Every time we read from each payload point, how many floats are returned? (float is chosen to be the standard of return type)
const uint8_t FRAME_INFO_SUBSETS[] = { 
	4,	// Wh, T, P, St (St represented through 0 or 1)
	2,	// V, A
	1,	// km/h
	1,	// bool for brakelight on/off
	6	// Headlights, Brake, Lsig, Rsig, Horn(?), Hazard
};
#define FUELCELL_READVALUES 4
#define CURRENTSENSOR_READVALUES 2
#define SPEEDOMETER_READVALUES 1
#define NUM_DATASUBSETS 6	// max of above

#define FLOAT_TO_STRING_LEN 4
#define SHORTNAME_LEN 3
const int MAX_STRING_LEN = SHORTNAME_LEN + 9 + (FLOAT_TO_STRING_LEN + 1)*(NUM_DATASETS*NUM_DATASUBSETS) + NUM_DATASETS;
//										^^ 9 bits for timeStamp + '\t'
// declare enum to represent every type of payload points
#define FRAMETYPES 5
/// <summary>
/// Enum for data points. Depreceated in favor of newer implementation of CANSerializer (previously named CAN_Serializer)
/// </summary>
typedef enum
{
	// if these ever exceed 4, need to extend the frame ID "3 bits needed for message type" below into 4 bits
	FC,
	CS,
	SM,
	BK,
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
#define STATE_EN 1
#define STATE_DS 0
#define NUMSTATES 6
extern uint8_t peripheralStates[NUMSTATES];
const char frameType_shortNames[FRAMETYPES][SHORTNAME_LEN] = {"FC", "CS", "SM", "BK", "BT"};

#endif