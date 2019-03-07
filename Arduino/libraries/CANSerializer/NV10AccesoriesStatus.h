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
	uint8_t& lsig = data.Byte[0];
	uint8_t& rsig = data.Byte[1];
	uint8_t& hazard = data.Byte[2];
	uint8_t& headlights = data.Byte[3];
	uint8_t& brake = data.Byte[4];
	uint8_t& wiper = data.Byte[5];
	
public:
	NV10AccesoriesStatus(uint8_t CANId);

	void setLsig(uint8_t status);
	void setRsig(uint8_t status);
	void setWiper(uint8_t status);
	void setHazard(uint8_t status);
	void setHeadlights(uint8_t status);
	void setBrake(uint8_t status);
	uint8_t getLsig();
	uint8_t getRsig();
	uint8_t getWiper();
	uint8_t getHazard();
	uint8_t getHeadlights();
	uint8_t getBrake();
	void insertData(uint8_t lsig, uint8_t rsig, uint8_t wiper, uint8_t hazard, uint8_t headlights, uint8_t brake);
	void packString(char*);
	void unpackString(char * str);
};

#endif

