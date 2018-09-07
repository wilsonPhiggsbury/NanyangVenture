// 
// 
// 

#include "Speedometer.h"

Speedometer::Speedometer(uint16_t diameter_in_mm):wheelDiameter(diameter_in_mm)
{
}
void Speedometer::storeWheelInterval()
{
	timeDiff = millis() - lastTime;
	lastTime = millis();
}
float Speedometer::calculateSpeed_MS()
{
	// since storeWheelInterval() will only occur if speedometer is tripped, there may be cases where it will not trip when the car brakes too fast
	// need obtain a separate timeDiff to check against the benchmarked value
	if (millis() - lastTime > SPEEDOMETER_REFRESH_INTERVAL)
		return 0;
	else
		return PI * wheelDiameter / timeDiff;
}
float Speedometer::calculateSpeed_KMH()
{
	float km_h = calculateSpeed_MS()*3.6;
	return km_h;
}