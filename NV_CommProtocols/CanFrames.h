#pragma once
#include "FrameFormats.h"
#include <mcp_can.h>

class QueueItem;
// struct to pass payload between different microcontrollers
struct Frame
{
	uint32_t id;
	byte length;
	byte payload[8];
};
class CanFrames
{
public:
	CanFrames();
	~CanFrames();
	void setCANObj(MCP_CAN * CANObj);
	struct Frame frames[1 + QUEUEITEM_DATAPOINTS * QUEUEITEM_READVALUES / 2 + 1 * QUEUEITEM_DATAPOINTS];
	// 1 frame for timestamp, other frames for floats, +QUEUEITEM_DATAPOINTS frame for odd-number scenarios where one additional frame is needed for the lone float
	bool toQueueItem(QueueItem* putHere);
	bool addItem(unsigned long id, byte length, byte* payload);
	void addItem(enum DataSource messageType, uint8_t terminatorStatus, float payload1);
	void addItem(enum DataSource messageType, uint8_t terminatorStatus, float payload1, float payload2);
	uint8_t getNumFrames();
	void clear();
private:
	uint8_t numFrames = 0;
	MCP_CAN* CANObj;
	void addItem_(uint8_t messageType, uint8_t terminatorStatus, float payload1, float payload2, bool using_payload2);
};