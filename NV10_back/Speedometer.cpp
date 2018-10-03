// 
// 
// 

#include "Speedometer.h"
#include "Behaviour.h"

unsigned long Speedometer::timeStamp;
Speedometer::Speedometer(uint8_t id, uint16_t diameter_in_mm):id(id),wheelDiameter(diameter_in_mm)
{
}
void Speedometer::storeWheelInterval()
{
	// this is called by an ISR. It can happen anytime in the program.
	timeDiff = millis() - lastTime;
	lastTime = millis();
}
void Speedometer::dumpTimestampInto(unsigned long* location)
{
	*location = timeStamp;
}
void Speedometer::dumpDataInto(float location[QUEUEITEM_DATAPOINTS][QUEUEITEM_READVALUES])
{
	processData();
	// expect QueueItem.payload to come in
	float* thisSlot = location[id];
	//loggedParams[0] = random(1, 100) / 10.0;
	for (int i = 0; i < SPEEDOMETER_READVALUES; i++)
	{
		thisSlot[i] = loggedParams[i];
	}
}
void Speedometer::processData()
{
	loggedParams[kmh] = calculateSpeed_MS() * 3.6;
}
float Speedometer::calculateSpeed_MS()
{
	// since storeWheelInterval() will only occur if speedometer is tripped, there may be cases where it will not trip when the car brakes too fast
	// need obtain a separate timeDiff to check against the benchmarked value
	timeStamp = millis();
	if (timeStamp - lastTime > SPEEDOMETER_REFRESH_INTERVAL)
		return 0;
	else
		return PI * wheelDiameter / timeDiff;
	// wheel perimeter / time = PI * diameter (mm) / timeDiff (ms)
}