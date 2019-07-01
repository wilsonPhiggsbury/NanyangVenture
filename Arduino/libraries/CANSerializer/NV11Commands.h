// NV10CurrentSensorStats.h

#ifndef _NV11COMMANDS_h
#define _NV11COMMANDS_h

#include <DataPoint.h>
class NV11Commands:public DataPoint
{
 protected:
	 uint8_t& horn = data.Byte[0];
	 uint8_t& lapCounter = data.Byte[1];
	 uint8_t& lapCounterReset = data.Byte[2];
	 uint8_t& shutdownPi = data.Byte[3];
	 enum eActivateStats {
		 notTriggered,
		 triggered
	 };

 public:
	NV11Commands();

	void insertData(uint8_t horn);
	// call this to deactivate all states. You should use this before activating the states one by one again.
	void clearActivationHistory();
	void triggerHorn();
	void triggerLaps();
	void triggerLapsReset();
	void triggerShutdownRPi();
	uint8_t getHorn();
	uint8_t getLapTrig();
	void packString(char*);
	void unpackString(char * str);
	// commands only require broadcast when any of it is triggered
	bool dataRequiresBroadcast();
};

#endif

