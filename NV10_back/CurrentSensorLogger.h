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
const uint16_t V_TABLE[NUM_CURRENTSENSORS][V_ENTRIES] PROGMEM = {
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
const uint16_t A_TABLE[NUM_CURRENTSENSORS][A_ENTRIES] PROGMEM = {
	{
		// L.wheel
		0,
		15,
		30,
		45,
		60,
		75,
		90,
		105,
		120,
		135,
		151,
		165,
		181,
		195,
		211,
		225,
		242,
		256,
		271,
		286,
		301
	},
	{
		// R.wheel
		2,
		17,
		33,
		48,
		64,
		80,
		95,
		110,
		125,
		141,
		156,
		172,
		187,
		202,
		218,
		233,
		250,
		264,
		280,
		296,
		311
	},
	{
		// Capacitor
		0,
		6,
		13,
		19,
		27,
		33,
		40,
		47,
		54,
		61,
		68,
		75,
		82,
		89,
		96,
		103,
		110,
		117,
		123,
		130,
		137
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
	float totalEnergy = 0; 
	float rawToVA(char which, float reading);
	unsigned long timeStamp;
	void dumpAttributeInto(char * location, uint16_t attribute, char conversionType);

public:
	AttopilotCurrentSensor(int motorID, uint8_t voltPin, uint8_t ampPin);
	uint8_t id;
	void logData();
	void dumpTimestampInto(char* location);
	void dumpDataInto(char * location);
	//void dumpTotalEnergyInto(char * location);

};