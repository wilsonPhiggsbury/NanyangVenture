// MotorLogger.h

#ifndef _MOTORLOGGER_h
#define _MOTORLOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#endif
#include <EEPROM.h>

#define V_BASE_ADDR 0
#define A_BASE_ADDR 2048
// n = (Vn-V0)/V_Resolution + 1
// n = (An-A0)/A_Resolution + 1
// mem required = sizeof(float) * n
#define V_STEP 5
#define V_0 0
#define V_N 95
#define V_ENTRIES ((V_N-V_0)/V_STEP)+1
// (100-0)/5 + 1 = 21
#define A_STEP 2
#define A_0 0
#define A_N 98
#define A_ENTRIES ((A_N-A_0)/A_STEP)+1
// (50-0)/1 + 1 = 51

// storage space of volts/amps lookup table = 4*sizeof(float)*ENTRIES 
// 4: motors, 4KB: size of EEPROM


class MotorLogger
{
private:
	uint8_t voltPin;
	uint8_t ampPin;
	uint16_t voltReading = 0;
	uint16_t ampReading = 0;
	static char timeStamp[9];
	float rawToVA(uint16_t reading, float first, float last, float step, int baseAddr);

public:
	MotorLogger(int motorID, uint8_t voltPin, uint8_t ampPin);
	void populateEEPROM();
	uint8_t id;
	void logData();
	void dumpDataInto(char * location);
	static void dumpTimestampInto(char* location);

};