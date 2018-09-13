#include "FrameFormats.h"
typedef enum
{
	HEADER_FRAME,
	NORMAL_FRAME,
	SOFT_TERMINATING_FRAME,
	HARD_TERMINATING_FRAME
}TerminatorStatus;
void _QueueItem::toString(char* putHere)
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
			strcat(putHere, " ");
			strcat(putHere, data_tmp);
		}
	}
	// final string sample: CS \t timeStamp \t _ CS1V _ CS1A \t _ CS2V _ CS2A \t _ CS3V _ CS3A \0
	// remove all whitespaces in the above format
	// _ : spacebar
	// \t: tab
	// \0: null terminator
}
/*
--- About Frame IDs ---
"terminator status bits": 2 bits needed for indication of terminating frame
00 -> start of stream, carries timestamp
01 -> normal frame in middle of stream
10 -> terminate transmission for one datapoint (soft termination)
11 -> terminate transmission for one full string (hard termination)

"message type bits": 3 bits needed for message type
0 -> display FC
1 -> display CS
2 -> display SM (include throttle?)
3 -> *reserved for display payload*
4 -> buttons
5 -> *reserved*
6 -> *reserved*
7 -> *reserved*

--- About Frame Length ---
length 4 means frame contains 1 float
length 8 means frame contains 2 float
*/
void _QueueItem::toFrames(NV_CanFrames* putHere)
{
	uint8_t dataPoints = DATAPOINT_INSTANCES[ID];
	uint8_t readValues = DATAPOINT_READVALUES[ID];

	// prepare timeStamp frame
	putHere->frames->id = (ID << 2) | HEADER_FRAME;
	putHere->frames->length = sizeof(unsigned long);
	memcpy(putHere->frames->payload, &timeStamp, putHere->frames->length);
	(putHere->numFrames)++;
	// prepare payload frames
	uint8_t i,j;
	for (i = 0; i < dataPoints; i++)
	{
		uint8_t terminatorStatus;
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

bool _NV_CanFrames::toQueueItem(QueueItem* putHere)
{

	uint8_t terminatorStatus = (frames[0].id & B11);
	if (terminatorStatus != HEADER_FRAME)
	{
		Serial.println("HEADER FRAME not found.");
		Serial.println(terminatorStatus);
		return false;
	}
	// append ID
	putHere->ID = DataSource((frames[0].id >> 2) & B111);
	uint8_t dataPoints = DATAPOINT_INSTANCES[putHere->ID];
	uint8_t readValues = DATAPOINT_READVALUES[putHere->ID];
	// spot-check whether frame number is as anticipated. Two floats per frame + 1 frame for timestamp
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
void NV_CanFrames::addItem(unsigned long id, byte length, byte* payload)
{
	frames[numFrames].id = id;
	frames[numFrames].length = length;
	memcpy(frames[numFrames].payload, payload, length);
	numFrames++;
}
void NV_CanFrames::addItem(uint8_t messageType, uint8_t terminatorStatus, float payload1)
{
	addItem_(messageType, terminatorStatus, payload1, 0, false);
}
void NV_CanFrames::addItem(uint8_t messageType, uint8_t terminatorStatus, float payload1, float payload2)
{
	addItem_(messageType, terminatorStatus, payload1, payload2, true);
}
void NV_CanFrames::addItem_(uint8_t messageType, uint8_t terminatorStatus, float payload1, float payload2, bool using_payload2)
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
void NV_CanFrames::clear()
{
	numFrames = 0;
}
uint8_t NV_CanFrames::getNumFrames()
{
	return numFrames;
}