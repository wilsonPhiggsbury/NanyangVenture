// NV10CurrentSensor.h

#ifndef _NV10CURRENTSENSOR_h
#define _NV10CURRENTSENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <DataPoint.h>
class NV10CurrentSensorClass:public DataPoint
{
 protected:
	 float volts, amps;

 public:
	 NV10CurrentSensorClass();
	 void init();

	virtual void insertData(float volts, float amps);
	virtual void getStringHeader(char * c);
	void packCAN(CANFrame*);
	void unpackCAN(const CANFrame*);
	void packString(char*);
	void unpackString(char * str);
};

typedef NV10CurrentSensorClass NV10CurrentSensor;

#endif

