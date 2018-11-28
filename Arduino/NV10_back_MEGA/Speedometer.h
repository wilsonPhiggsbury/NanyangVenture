// Speedometer.h

#ifndef _SPEEDOMETER_h
#define _SPEEDOMETER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <Constants.h>
class Speedometer
{
public:
	Speedometer(uint8_t id, uint16_t diameter);
	void storeWheelInterval();
	static void dumpTimestampInto(unsigned long * location);
	void dumpDataInto(float location[NUM_DATASETS][NUM_DATASUBSETS]);
	float calculateSpeed_MS();
private:
	float predictableCounter = 0;
	uint16_t wheelDiameter; // in millimeters
	uint16_t timeDiff = 0;
	unsigned long lastTime = 0;
	static unsigned long timeStamp;
	void processData();
	uint8_t id;
	float loggedParams[SPEEDOMETER_READVALUES] = {};
	typedef enum {
		kmh
	}LoggedParams;
};

#endif

