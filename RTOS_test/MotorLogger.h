// MotorLogger.h

#ifndef _MOTORLOGGER_h
#define _MOTORLOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define SAMPLE_RATE 16
#define TABLE_ENTRIES 1024/SAMPLE_RATE
#define V_BASE_ADDR 0
#define A_BASE_ADDR 2048
// storage space of volts/amps lookup table = 4*sizeof(float)*ENTRIES 
// 4: motors, 4KB: size of EEPROM
#endif



class MotorLogger
{
private:
	float lookupTable[TABLE_ENTRIES];
	uint8_t id;
	uint8_t voltPin;
	uint8_t ampPin;
	uint8_t voltReading;
	uint8_t ampReading;
protected:

public:

	MotorLogger(int motorID, uint8_t voltPin, uint8_t ampPin);

	void logData();

	void dumpDataInto(char * location);

};