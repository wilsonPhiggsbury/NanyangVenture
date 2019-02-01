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
	 float pressure;
	 uint16_t watts;
	 uint8_t temperature, status;
	 char statusTxt[3];

 public:
	 NV10FuelCellClass();
	void init();

	float getPressure();
	uint16_t getWatts();
	uint8_t getTemperature();
	const char* getStatus();


	void insertData(char* str);
	virtual void getStringHeader(char* c);
	void packCAN(CANFrame*);
	void unpackCAN(const CANFrame*);
	void packString(char*);
	void unpackString(char * str);
};

typedef NV10FuelCellClass NV10FuelCell;

#endif

