// NV10CurrentSensorStats.h

#ifndef _DATAPOINTTEMPLATE_h
#define _DATAPOINTTEMPLATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <DataPoint.h>
class DataPointTemplate:public DataPoint
{
 protected:
	 uint32_t param1 = 0;
	 float param2 = 0; // params can be higher resolution than their packed variants (this float is packed as uint16_t in .cpp file)

 public:
	DataPointTemplate(uint8_t CANId);

	void insertData(uint32_t param1, uint32_t param2);
	uint16_t getParam1();
	float getParam2();
	void packCAN(CANFrame*);
	void unpackCAN(const CANFrame*);
	void packString(char*);
	void unpackString(char * str);
};

#endif
