#include "Constants.h"
class QueueItem;
typedef enum
{
	HEADER_FRAME,
	NORMAL_FRAME,
	SOFT_TERMINATING_FRAME,
	HARD_TERMINATING_FRAME
}TerminatorStatus;
// struct to pass payload between different microcontrollers
struct NV_CanFrame
{
	uint32_t id;
	byte length;
	byte payload[8];
};
class NV_CanFrames
{
	friend class QueueItem;
	friend class CAN_Serializer;
	NV_CanFrame frames[1 + NUM_DATASETS * NUM_DATASUBSETS / 2 + 1 * NUM_DATASETS];
	//void setCANObj(MCP_CAN& CANObj);
	// 1 frame for timestamp, other frames for floats, +QUEUEITEM_DATAPOINTS frame for odd-number scenarios where one additional frame is needed for the lone float
	bool toQueueItem(QueueItem* putHere);
	bool addItem(unsigned long id, byte length, byte* payload);
	void addItem(DataSource messageType, uint8_t terminatorStatus, float payload1);
	void addItem(DataSource messageType, uint8_t terminatorStatus, float payload1, float payload2);
	uint8_t getNumFrames();
	void clear();
private:
	uint8_t numFrames = 0;
	//MCP_CAN* CANObj;
	void addItem_(uint8_t messageType, uint8_t terminatorStatus, float payload1, float payload2, bool using_payload2);
};