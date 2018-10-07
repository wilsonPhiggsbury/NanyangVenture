#include "CanFrames.h"
#include "QueueItem.h"


CanFrames::CanFrames()
{
}


CanFrames::~CanFrames()
{
}

void CanFrames::setCANObj(MCP_CAN* CANObj)
{
	this->CANObj = CANObj;
}
bool CanFrames::toQueueItem(QueueItem* putHere)
{

	uint8_t terminatorStatus = (frames[0].id & B11);
	if (terminatorStatus != HEADER_FRAME)
	{
		Serial.println("HEADER FRAME not found.");
		Serial.println(terminatorStatus);
		return false;
	}
	// append ID
	putHere->ID = (enum DataSource)((frames[0].id >> 2) & B111);
	uint8_t dataPoints = DATAPOINT_INSTANCES[putHere->ID];
	uint8_t readValues = DATAPOINT_READVALUES[putHere->ID];
	// spot-check whether frame number is as anticipated. 1/2 frame per float + 1 frame for timestamp
	if (numFrames != dataPoints * ceil(readValues / 2.0) + 1)
	{
		Serial.print("NUMFRAMES incorrect. Expecting: ");
		Serial.print(dataPoints * ceil(readValues / 2.0) + 1);
		Serial.print(" Received: ");
		Serial.print(numFrames);
		numFrames = 0;
		return false;
	}
	numFrames = 0;
	// append timestamp
	char timeStamp_tmp[9];
	memcpy(&(putHere->timeStamp), frames[0].payload, frames[0].length);
	// append payload
	int frameCounter = 1;
	for (int i = 0; i < dataPoints; i++)
	{

		for (int j = 0; j < readValues; j++)
		{
			// spot-check frame termination to ensure it follows convention specified by "message type bits" (bits 2~5)
			uint8_t terminatorStatus = (frames[frameCounter].id & B11);

			if (j >= readValues - 2)
			{
				if (i == dataPoints - 1) {
					if (terminatorStatus != HARD_TERMINATING_FRAME)
					{
						return false;
					}
				}
				else {
					if (terminatorStatus != SOFT_TERMINATING_FRAME)
					{
						return false;
					}
				}
			}
			else
			{
				if (terminatorStatus != NORMAL_FRAME)
				{
					return false;
				}
			}
			// frame termination structure is normal, copy out payload contents
			float* copyDest = putHere->data[i] + j;
			byte* copySource;
			if (j % 2 == 0) {
				// even number (incl 0)
				copySource = frames[frameCounter].payload;
			}
			else {
				copySource = frames[frameCounter].payload + sizeof(float);
				frameCounter++;
			}
			memcpy(copyDest, copySource, sizeof(float));
		}
	}
	return true;
}
bool CanFrames::addItem(unsigned long id, byte length, byte* payload)
{
	frames[numFrames].id = id;
	frames[numFrames].length = length;
	memcpy(frames[numFrames].payload, payload, length);
	numFrames++;
	// return true for HARD_TERMINATING_FRAME
	return (id & HARD_TERMINATING_FRAME) == HARD_TERMINATING_FRAME;
}
void CanFrames::addItem(enum DataSource messageType, uint8_t terminatorStatus, float payload1)
{
	addItem_(messageType, terminatorStatus, payload1, 0, false);
}
void CanFrames::addItem(enum DataSource messageType, uint8_t terminatorStatus, float payload1, float payload2)
{
	addItem_(messageType, terminatorStatus, payload1, payload2, true);
}
void CanFrames::addItem_(uint8_t messageType, uint8_t terminatorStatus, float payload1, float payload2, bool using_payload2)
{
	frames[numFrames].id = (messageType << 2) | terminatorStatus;

	// copy payload1 into upper half of payload byte array
	memcpy(frames[numFrames].payload, &payload1, sizeof(float));
	if (using_payload2)
	{
		// copy payload2 into lower half of payload byte array
		memcpy(frames[numFrames].payload + sizeof(float), &payload2, sizeof(float));
		frames[numFrames].length = 2 * sizeof(float);
		// WARNING: only works on platforms with sizeof(byte) = 2*sizeof(float)
	}
	else
	{
		frames[numFrames].length = 1 * sizeof(float);
	}
	numFrames++;
}
void CanFrames::clear()
{
	numFrames = 0;
}
uint8_t CanFrames::getNumFrames()
{
	return numFrames;
}