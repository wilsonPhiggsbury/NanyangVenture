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
#define V_N 60
#define V_ENTRIES ((V_N-V_0)/V_STEP)+1
// (100-0)/5 + 1 = 21
#define A_STEP 2
#define A_0 0
#define A_N 48
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
	float rawToVA(char which);
	static char timeStamp[9];
	int getAddr(char which);
	static int getBaseAddr(char which);
	static int getStep(char which);

public:
	MotorLogger(int motorID, uint8_t voltPin, uint8_t ampPin);
	void populateEEPROM();
	static uint16_t getEntries(char which);
	bool updateTable(char which, uint16_t content[max(V_ENTRIES, A_ENTRIES)]);
	uint8_t id;
	void logData();
	void dumpDataInto(char * location, bool raw);
	static void dumpTimestampInto(char* location);

};