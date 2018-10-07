#pragma once
#include "FrameFormats.h"

class NV_CanFrames;
// struct to pass payload between different tasks in same microcontroller
class QueueItem {
public:
	QueueItem();
	~QueueItem();
	DataSource ID;
	unsigned long timeStamp;
	float data[QUEUEITEM_DATAPOINTS][QUEUEITEM_READVALUES];
	void toString(char* putHere);
	static bool toQueueItem(char * str, QueueItem * queueItem); //		<--- *NOT YET verified if it works on AVR chips*
	void toFrames(NV_CanFrames* putHere);
};