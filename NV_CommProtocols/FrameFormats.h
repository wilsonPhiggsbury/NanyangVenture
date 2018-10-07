#pragma once
#include <Arduino.h>
#include "dtostrf.h"
#ifdef __AVR__

#elif defined _SAM3XA_

#endif

// Declare instances of every payload point
// ATTR: payload point = where the payload comes from
extern const uint8_t DATAPOINT_INSTANCES[];
#define NUM_FUELCELLS 2
#define NUM_CURRENTSENSORS 3
#define NUM_SPEEDOMETERS 1
#define QUEUEITEM_DATAPOINTS 3	// max of above
// Every time we read from each payload point, how many floats are returned? (float is chosen to be the standard of return type)
extern const uint8_t DATAPOINT_READVALUES[];
#define FUELCELL_READVALUES 8
#define CURRENTSENSOR_READVALUES 2
#define SPEEDOMETER_READVALUES 1
#define QUEUEITEM_READVALUES 8	// max of above

#define FLOAT_TO_STRING_LEN 4
#define SHORTNAME_LEN 3
extern const int MAX_STRING_LEN;
//										^^ 9 bits for timeStamp + '\t'
// declare enum to represent every type of payload points
#define DATAPOINTS 3
extern const char dataPoint_shortNames[DATAPOINTS][SHORTNAME_LEN];
extern enum DataSource;
extern enum TerminatorStatus;
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