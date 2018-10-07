
#include "QueueItem.h"
#include "NV_CanFrames.h"



QueueItem::QueueItem()
{
}


QueueItem::~QueueItem()
{
}

void QueueItem::toString(char* putHere)
{
	// assume what pointer's passed in is not properly null-terminated
	*putHere = '\0';
	// append frame name
	strcat(putHere, dataPoint_shortNames[ID]);
	strcat(putHere, "\t");
	// append time stamp
	char timeStamp_tmp[9];
	ultoa(timeStamp, timeStamp_tmp, 16);
	strcat(putHere, timeStamp_tmp);

	// append all parameters of each payload point
	uint8_t dataPoints = DATAPOINT_INSTANCES[ID];
	uint8_t readValues = DATAPOINT_READVALUES[ID];

	for (uint8_t i = 0; i < dataPoints; i++)
	{
		strcat(putHere, "\t");
		for (uint8_t j = 0; j < readValues; j++)
		{
			char data_tmp[5];
			dtostrf(data[i][j], FLOAT_TO_STRING_LEN, 1, data_tmp);
			//#ifdef __AVR__
			//#elif defined _SAM3XA_
			//			sprintf(data_tmp, "%4.1f", data[i][j]);
			//#endif
			strcat(putHere, data_tmp);
			strcat(putHere, " ");
		}
	}
	// final string sample: CS \t timeStamp \t CS1V _ CS1A _ \t CS2V _ CS2A _ \t CS3V _ CS3A _ \0
	// remove all whitespaces in the above format
	// _ : spacebar
	// \t: tab
	// \0: null terminator
}
bool QueueItem::toQueueItem(char* str, QueueItem* queueItem)
{
	char* cur = strtok(str, "\t");
	int i, j = 0;
	while (j < DATAPOINTS && strcmp(dataPoint_shortNames[j], cur))
	{
		j++;
	}
	if (j >= DATAPOINTS)
	{
		return false;
	}
	queueItem->ID = (DataSource)j;
	uint8_t dataPoints = DATAPOINT_INSTANCES[j];
	uint8_t readValues = DATAPOINT_READVALUES[j];

	cur = strtok(NULL, "\t");
	queueItem->timeStamp = strtoul(cur, NULL, 16);

	for (i = 0; i < dataPoints; i++)
	{
		for (j = 0; j < readValues; j++)
		{
			cur = strtok(NULL, " ");
			// for some reason the strtok below is not working, commented out and patched a HOTFIX to offset the pointer
			if (i != 0 && j == 0)
				cur++;
#ifdef __AVR__
			queueItem->data[i][j] = atof(cur);
#elif defined _SAM3XA_
			queueItem->data[i][j] = strtof(cur, NULL);
#endif
		}
		//cur = strtok(NULL, "\t");
	}
	return true;
}
void QueueItem::toFrames(NV_CanFrames* putHere)
{
	uint8_t dataPoints = DATAPOINT_INSTANCES[ID];
	uint8_t readValues = DATAPOINT_READVALUES[ID];

	// prepare timeStamp frame
	unsigned long frameID = (this->ID << 2) | HEADER_FRAME;
	byte frameLength = sizeof(unsigned long);
	byte* framePayload = (byte*)&timeStamp;
	putHere->addItem(frameID, frameLength, framePayload);
	// prepare payload frames
	uint8_t i, j;
	for (i = 0; i < dataPoints; i++)
	{
		enum TerminatorStatus terminatorStatus;
		for (j = 0; j < readValues - 1; j += 2)
		{
			if (j == readValues - 2)
			{
				if (i == dataPoints - 1)
					terminatorStatus = HARD_TERMINATING_FRAME; // B*whatever* is binary representation of numbers
				else
					terminatorStatus = SOFT_TERMINATING_FRAME;
			}
			else
			{
				terminatorStatus = NORMAL_FRAME;
			}
			putHere->addItem(ID, terminatorStatus, data[i][j], data[i][j + 1]);
		}
		if (j == readValues - 1)
		{
			terminatorStatus = HARD_TERMINATING_FRAME;
			putHere->addItem(ID, terminatorStatus, data[i][j]);
		}
	}
	//putHere->numFrames = 1 + dataPoints * ceil(readValues / 2.0);
}
