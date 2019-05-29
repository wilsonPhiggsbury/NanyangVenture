// 
// 
// 
#include "NV10CurrentSensorStats.h"
const uint32_t MS_PER_HOUR = 3600000;
const float SHUNT_100A75mV_SCALE = 0.0078125 * (100.0 / 75);
const float SHUNT_200A75mV_SCALE = 0.0078125 * (200.0 / 75);
const float VOLTAGEDIVIDER_SCALE = 1;
// parameter(CANbytes, stringChars)
// wattHour(2,4), ampPeak(1,2)
NV10CurrentSensorStatsClass::NV10CurrentSensorStatsClass(uint8_t CANId) :DataPoint("cs", CANId, 8)
{
	debug(F("DataPoint cs:\t0x0D\t8"));
}

void NV10CurrentSensorStatsClass::insertData(uint32_t voltRaw, uint32_t ampMotorRaw)
{
	timeStamp = millis();
	const float volt = voltRaw * SHUNT_100A75mV_SCALE; // conversion from Attopilot voltage pin
	const float ampMotor = ampMotorRaw * SHUNT_100A75mV_SCALE; // conversion from 50mV-200A shunt
	
	uint32_t wattMsDelta = volt * ampMotor * getTimeDiff();
	wattMs += wattMsDelta;
	ampPeak = max(ampPeak, ampMotor);
}

uint16_t NV10CurrentSensorStatsClass::getWattHours()
{
	return wattMs/MS_PER_HOUR;
}

float NV10CurrentSensorStatsClass::getAmpPeak()
{
	return ampPeak;
}

void NV10CurrentSensorStatsClass::packString(char *str)
{
	char* shiftedStr = DataPoint::packStringDefault(str);
	// wattHour = 4, aPeak = 2
	sprintf(shiftedStr, "%04d\t%02d", wattMs/MS_PER_HOUR, ampPeak);
}

void NV10CurrentSensorStatsClass::unpackString(char * str)
{
	char* ptr = strtok(str, "\t");
	ptr = strtok(NULL, "\t");
	timeStamp = strtoul(ptr, NULL, 16);

	ptr = strtok(NULL, "\t");
	wattMs = atoi(ptr) * MS_PER_HOUR;

	ptr = strtok(NULL, "\t");
	ampPeak = strtod(ptr, NULL);
}

void NV10CurrentSensorStatsClass::syncTime()
{
	getTimeDiff();
}
uint32_t NV10CurrentSensorStatsClass::getTimeDiff()
{
	uint32_t timeDiff = millis() - lastTime;
	lastTime = millis();
	return timeDiff;
}

