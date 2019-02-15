// NV10CurrentSensorStats.h

#ifndef _DATAPOINTTEMPLATE_h
#define _DATAPOINTTEMPLATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define STATE_EN 1
#define STATE_DS 0
#include <DataPoint.h>
class NV10AccesoriesStatus:public DataPoint
{
 protected:
	int8_t lsig = 0;
	int8_t rsig = 0;
	int8_t wiper = 0;
	int8_t hazard = 0;
	int8_t headlights = 0;
	int8_t brake = 0;
	
public:
	NV10AccesoriesStatus(uint8_t CANId);

	void setLsig(int8_t status);
	void setRsig(int8_t status);
	void setWiper(int8_t status);
	void setHazard(int8_t status);
	void setHeadlights(int8_t status);
	void setBrake(int8_t status);
	int8_t getLsig();
	int8_t getRsig();
	int8_t getWiper();
	int8_t getHazard();
	int8_t getHeadlights();
	int8_t getBrake();
	void insertData(int8_t lsig, int8_t rsig, int8_t wiper, int8_t hazard, int8_t headlights, int8_t brake);
	void packCAN(CANFrame*);
	void unpackCAN(const CANFrame*);
	void packString(char*);
	void unpackString(char * str);
};

#endif

