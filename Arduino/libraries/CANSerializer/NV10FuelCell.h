// NV10FuelCell.h

#ifndef _NV10FUELCELL_h
#define _NV10FUELCELL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <DataPoint.h>
class NV10FuelCellClass:public DataPoint
{
 protected:
	 float& pressure = data.Float[0];
	 uint16_t& watts = data.UInt[2];
	 uint8_t& temperature = data.Byte[6];
	 uint8_t& status = data.Byte[7];

	 char statusTxt[3];

 public:
	 NV10FuelCellClass(uint8_t CANId);

	float getPressure();
	uint16_t getWatts();
	uint8_t getTemperature();
	const char* getStatus();

	void unpackCAN(const CANFrame*);
	void insertData(char* str);
	void packString(char*);
	void unpackString(char * str);
};

typedef NV10FuelCellClass NV10FuelCell;

#endif

