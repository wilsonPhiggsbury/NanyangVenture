// NV10CurrentSensorStats.h

#ifndef _DATAPOINTTEMPLATE_h
#define _DATAPOINTTEMPLATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <DataPoint.h>
class NV11DataSpeedo:public DataPoint
{
 protected:
	 float& speed = data.Float[0];

 public:
	NV11DataSpeedo();
	// implement parent class
	void insertData(float speed);
	float getSpeed();
	void packString(char*);
	void unpackString(char * str);
};
#endif

