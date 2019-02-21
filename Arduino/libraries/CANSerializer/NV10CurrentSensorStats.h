// NV10CurrentSensorStats.h

#ifndef _NV10CURRENTSENSORSTATS_h
#define _NV10CURRENTSENSORSTATS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <NV10CurrentSensor.h>
class NV10CurrentSensorStatsClass:public DataPoint
{
 protected:
	 uint32_t& wattMs = data.Long[0];
	 float& ampPeak = data.Float[1];

 public:
	NV10CurrentSensorStatsClass(uint8_t CANId);

	void insertData(uint32_t voltRaw, uint32_t ampMotorRaw);
	uint16_t getWattHours();
	float getAmpPeak();
	void packString(char*);
	void unpackString(char * str);
	void syncTime();
private:
	uint32_t lastTime;
	uint32_t getTimeDiff();
};

typedef NV10CurrentSensorStatsClass NV10CurrentSensorStats;

#endif

