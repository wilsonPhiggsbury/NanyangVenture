#include "Constants.h"
class NV_CanFrames;
// struct to pass payload between different tasks in same microcontroller
class QueueItem {
	friend class NV_CanFrames;
	friend class CAN_Serializer;
public:
	DataSource ID;
	unsigned long timeStamp;
	float data[NUM_DATASETS][NUM_DATASUBSETS];
	void toString(char* putHere);
	static bool toQueueItem(char * str, QueueItem * queueItem); //		<--- *NOT YET verified if it works on AVR chips*
private:
	void toFrames(NV_CanFrames* putHere);
};