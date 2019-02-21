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
	 uint16_t& volt = data.UInt[0];
	 uint16_t& ampCapIn = data.UInt[1];
	 uint16_t& ampCapOut = data.UInt[2];
	 uint16_t& ampMotor = data.UInt[3];

 public:
	NV10CurrentSensorClass(uint8_t CANId);

	void insertData(uint32_t voltRaw, uint32_t ampCInRaw, uint32_t ampCOutRaw, uint32_t ampMotorRaw);

	void packString(char*);
	void unpackString(char * str);

	uint16_t getVolt();
	uint16_t getAmpCapIn();
	uint16_t getAmpCapOut();
	uint16_t getAmpMotor();
};

typedef NV10CurrentSensorClass NV10CurrentSensor;

#endif

