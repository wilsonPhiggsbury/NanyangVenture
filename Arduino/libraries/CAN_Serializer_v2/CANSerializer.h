// CANSerializer.h

#ifndef _CANSERIALIZER_h
#define _CANSERIALIZER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <mcp_can.h>
class CANFrame
{
public:
	unsigned long id;
	byte length;
	byte payload[8]; // TODO: extend payload byte length, then implement conversion below through stored references of dataPoints
};
class CANSerializer
{
private:
	MCP_CAN* CAN;
	//DataPoint* dataPoints[1]; // TODO: implement convertion to raw CAN bytes only inside CANSerializer
public:
	void init(uint8_t cs);
	bool sendCanFrame(CANFrame*);
	// Populates Packet only if data is available.
	bool receiveCanFrame(CANFrame*);
};

#endif

