// NV10CurrentSensorStats.h

#ifndef _NV11BMS_h
#define _NV11BMS_h

#include <DataPoint.h>
class NV11BMS:public DataPoint
{
 protected:
	 uint16_t& volt = data.UInt[0];
	 uint16_t& amp = data.UInt[2];
	 uint16_t& temperature = data.UInt[4];

 public:
	NV11BMS(uint8_t CANId);
	void insertData(uint32_t volt, uint32_t amp);
	uint16_t getVolt();
	uint16_t getAmp();
	uint16_t getTemperature();
	void packString(char*);
	void unpackString(char * str);
};

#endif

