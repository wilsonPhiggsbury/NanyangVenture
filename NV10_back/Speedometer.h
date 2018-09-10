// Speedometer.h

#ifndef _SPEEDOMETER_h
#define _SPEEDOMETER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Speedometer
{
public:
	Speedometer(uint16_t diameter);
	void storeWheelInterval();
	float calculateSpeed_MS();
	float calculateSpeed_KMH();
private:
	uint16_t wheelDiameter; // in millimeters
	uint16_t timeDiff = 0;
	unsigned long lastTime = 0;
};

#endif

