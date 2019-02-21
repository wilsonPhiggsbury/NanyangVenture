// 
// 
// 

#include "Speedometer.h"

Speedometer::Speedometer(uint16_t pinA, uint16_t pinB, uint16_t diameterInMM, uint16_t ticksPerRot, bool positivePhaseshiftWhenForward):pinA(pinA), pinB(pinB), diameter(diameterInMM), ticksPerRot(ticksPerRot)
{
	int pinNo = digitalPinToInterrupt(pinA);
	if (pinNo == NOT_AN_INTERRUPT)
	{
		Serial.println("Attempting to attach non-interrupt pin! Blocking...");
		while (1);
	}
	pinMode(pinA, INPUT_PULLUP);
	pinMode(pinB, INPUT_PULLUP);
	ticksToMmMultiplier = diameter * PI / ticksPerRot;
}

void Speedometer::trip()
{
	counter++;
	if (digitalRead(pinB))
		positiveIndicator = 1;
	else
		positiveIndicator = -1;
}

uint32_t Speedometer::getTotalDistTravelled()
{
	uint32_t mmTravelled = ticksToMmMultiplier * counter;
	return mmTravelled;
}

float Speedometer::calculateSpeedKmh()
{
	// get deltas
	uint32_t counterDiff = counter - lastCounter;
	lastCounter = counter;
	uint32_t timeDiff = millis() - lastTime;
	lastTime = millis();
	// calc speed
	uint32_t mmTravelled = ticksToMmMultiplier * counterDiff;
	float speedKmh = 3.6 * mmTravelled / timeDiff;
	return speedKmh * positiveIndicator;
}

uint32_t Speedometer::getTicks()
{
	return counter;
}
