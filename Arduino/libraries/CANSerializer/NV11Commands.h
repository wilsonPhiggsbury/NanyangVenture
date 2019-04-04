// NV10CurrentSensorStats.h

#ifndef _NV11COMMANDS_h
#define _NV11COMMANDS_h

#include <DataPoint.h>
class NV11Commands:public DataPoint
{
 protected:
	 uint8_t& horn = data.Byte[0];
	 enum eActivateStats {
		 notActivated,
		 activated
	 };

 public:
	NV11Commands(uint8_t CANId);

	void insertData(uint8_t horn);
	// call this to deactivate all states. You should use this before activating the states one by one again.
	void clearActivationHistory();
	void activateHorn();
	uint8_t getHorn();
	void packString(char*);
	void unpackString(char * str);
};

#endif

