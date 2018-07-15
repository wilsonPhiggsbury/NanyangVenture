// AttopilotCurrentSensor.h

#ifndef _MOTORLOGGER_h
#define _MOTORLOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#endif
#include "Behaviour.h"
#include <EEPROM.h>

// range of volt & amp readings expected
#define V_STEP 5
#define V_0 0
#define V_N 60
#define V_ENTRIES ((V_N-V_0)/V_STEP)+1

#define A_STEP 2
#define A_0 0
#define A_N 40
#define A_ENTRIES ((A_N-A_0)/A_STEP)+1



// LOOKUP TABLE declared below
const uint16_t V_TABLE[NUM_MOTORS][V_ENTRIES] PROGMEM = {
	{
		// L.wheel
		0,
		62,
		128,
		193,
		259,
		325,
		390,
		455,
		520,
		586,
		651,
		717,
		782
	},
	{
		// R.wheel
		0,
		62,
		128,
		193,
		259,
		326,
		391,
		456,
		522,
		588,
		653,
		719,
		785
	},
	{
		// Capacitor
		0,
		62,
		128,
		193,
		257,
		323,
		388,
		453,
		519,
		584,
		648,
		714,
		779
	}
};
const uint16_t A_TABLE[NUM_MOTORS][A_ENTRIES] PROGMEM = {
	{
		// L.wheel
		0,
		15,
		29,
		44,
		59,
		74,
		89,
		104,
		118,
		133,
		148,
		161,
		178,
		193,
		208,
		223,
		237,
		252,
		267,
		282,
		297
	},
	{
		// R.wheel
		1,
		17,
		32,
		47,
		62,
		77,
		93,
		108,
		123,
		138,
		153,
		169,
		184,
		199,
		215,
		230,
		245,
		260,
		276,
		291,
		306
	},
	{
		// Capacitor
		0,
		5,
		12,
		19,
		26,
		32,
		39,
		46,
		53,
		60,
		67,
		74,
		81,
		87,
		94,
		101,
		108,
		115,
		122,
		129,
		135
	}
};

class AttopilotCurrentSensor
{
private:
	uint8_t voltPin;
	uint8_t ampPin;
	uint16_t voltReading = 0;
	uint16_t ampReading = 0;
	uint16_t ampPeak = 0;
	// As opposed to conventional Wh, this variable takes milliseconds instead of hours as time frame due to calculation efficiency.
	// Don't worry, will convert to float when sending.
	uint32_t totalEnergy = 0; 
	float rawToVA(char which, float reading);
	unsigned long timeStamp;

public:
	AttopilotCurrentSensor(int motorID, uint8_t voltPin, uint8_t ampPin);
	uint8_t id;
	void logData();
	void dumpTimestampInto(char* location);
	void dumpVoltReadingInto(char * location);
	void dumpAmpReadingInto(char * location);
	void dumpAmpPeakInto(char * location);
	void dumpTotalEnergyInto(char * location);

};