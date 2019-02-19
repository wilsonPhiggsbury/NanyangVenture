// Speedometer.h

#ifndef _SPEEDOMETER_h
#define _SPEEDOMETER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include "WProgram.h"
#endif

class Speedometer
{
public:
	Speedometer(uint16_t pinA, uint16_t pinB, uint16_t diameterInMM, uint16_t ticksPerRot, bool positivePhaseshiftWhenForward);
	void trip();
	uint32_t getTotalDistTravelled();
	float getSpeedKmh();
	uint32_t getTicks();

private:
	uint32_t counter = 0, lastCounter = 0, lastTime = 0;
	uint16_t diameter, ticksPerRot;
	float ticksToMmMultiplier;
	int8_t positiveIndicator;
	uint8_t pinA, pinB;
};

#endif
