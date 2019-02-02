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
	float volt, ampCapIn, ampCapOut, ampMotor;

 public:
	NV10CurrentSensorClass(uint8_t CANId);

	void insertData(uint32_t voltRaw, uint32_t ampCInRaw, uint32_t ampCOutRaw, uint32_t ampMotorRaw);

	void packCAN(CANFrame*);
	void unpackCAN(const CANFrame*);
	void packString(char*);
	void unpackString(char * str);
};

typedef NV10CurrentSensorClass NV10CurrentSensor;

#endif

