// AttopilotCurrentSensor.h

#ifndef _MOTORLOGGER_h
#define _MOTORLOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#endif

#include <Constants.h>

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
const uint16_t V_TABLE[][V_ENTRIES] PROGMEM = {
	{
		// CAP_IN
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
	},
	{
		// CAP_OUT *not tested*
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
	},
	{
		// Motor
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
};
const uint16_t A_TABLE[][A_ENTRIES] PROGMEM = {
	{
		// CAP_IN
		0.0,
		5.5,
		12.0,
		18.5,
		26.0,
		32.0,
		39.5,
		46.0,
		53.5,
		60.0,
		66.0,
		73.5,
		80.0,
		88.0,
		94.5,
		100.0,
		107.0,
		114.0,
		120.0,
		127.0,
		134.5
	},
	{
		// CAP_OUT
		0.0,
		5.5,
		12.0,
		18.5,
		26.0,
		32.0,
		39.5,
		46.0,
		53.5,
		60.0,
		66.0,
		73.5,
		80.0,
		88.0,
		94.5,
		100.0,
		107.0,
		114.0,
		120.0,
		127.0,
		134.5
	},
	{
		// Motor
		0.0,
		4.0,
		10.0,
		17.0,
		24.0,
		31.0,
		38.0,
		46.0,
		52.5,
		60.0,
		67.0,
		73.5,
		81.0,
		88.0,
		95.0,
		103.0,
		110.0,
		116.5,
		123.5,
		131.0,
		138.0
	}
};

class AttopilotCurrentSensor
{
private:
	float predictableCounter = 0;
	static uint32_t timeStamp;

	uint8_t id;
	uint8_t voltPin;
	uint8_t ampPin;

	float loggedParams[CURRENTSENSOR_READVALUES];
	typedef enum {
		volt,
		amp
	}LoggedParameters;

	//uint16_t ampPeak = 0;
	// As opposed to conventional Wh, this variable takes milliseconds instead of hours as time frame due to calculation efficiency.
	// Don't worry, will convert to float when sending.
	//float totalEnergy = 0; 
	void processData();
	float rawToVA(LoggedParameters which, float reading);


public:
	AttopilotCurrentSensor(int motorID, uint8_t voltPin, uint8_t ampPin);
	void logData();
	static void dumpTimestampInto(unsigned long* location);
	void dumpDataInto(float location[NUM_DATASETS][NUM_DATASUBSETS]);
	//void dumpTotalEnergyInto(char * location);

};