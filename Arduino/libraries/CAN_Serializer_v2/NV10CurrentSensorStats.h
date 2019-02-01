// NV10CurrentSensorStats.h

#ifndef _NV10CURRENTSENSORSTATS_h
#define _NV10CURRENTSENSORSTATS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <NV10CurrentSensor.h>
class NV10CurrentSensorStatsClass:public NV10CurrentSensorClass
{
 protected:
	 float wattHours, ampPeak;

 public:
	 void insertData(float volts, float amps);
};

typedef NV10CurrentSensorStatsClass NV10CurrentSensorStats;

#endif

