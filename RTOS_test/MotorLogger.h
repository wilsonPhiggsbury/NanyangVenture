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

#define SAMPLE_RATE 16
#define TABLE_ENTRIES 1024/SAMPLE_RATE
#define V_BASE_ADDR 0
#define A_BASE_ADDR 2048
// storage space of volts/amps lookup table = 4*sizeof(float)*ENTRIES 
// 4: motors, 4KB: size of EEPROM


class MotorLogger
{
private:
	float lookupTable[TABLE_ENTRIES];
	uint8_t voltPin;
	uint8_t ampPin;
	uint8_t voltReading;
	uint8_t ampReading;
	static char timeStamp[9];

public:
	MotorLogger(int motorID, uint8_t voltPin, uint8_t ampPin);
	uint8_t id;
	void logData();
	void dumpDataInto(char * location);
	static void dumpTimestampInto(char* location);

};