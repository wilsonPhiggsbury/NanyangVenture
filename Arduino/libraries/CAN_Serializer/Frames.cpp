#include "Frames.h"
#include "QueueItem.h"
/*
--- About Frame IDs ---															ID[10:0]
"terminator status bits": 2 bits needed for indication of terminating frame.	ID[1:0]
00 -> start of stream, carries timestamp
01 -> normal frame in middle of stream
10 -> terminate transmission for one datapoint (soft termination)
11 -> terminate transmission for one full string (hard termination)

"message type bits": 4 bits needed for message type.							ID[4:2], shift left by 2
0 -> display FC
1 -> display CS
2 -> display SM (include throttle?)
3 -> *reserved for display payload*
4 -> buttons
5 -> *reserved*
6 -> *reserved*
7 -> *reserved*

--- About Frame Length ---
From NV10_dashboard:
length 4 for headlights, wiper, Lsig, Rsig
From NV10_back:
length 4 means frame contains 1 float
length 8 means frame contains 2 float

*/
bool Frames::toPacket(Packet* putHere)
{

	uint8_t terminatorStatus = (frames[0].id & B11);
	if (terminatorStatus != HEADER_FRAME)
	{
		debug_(F("Wrong termination ID of "));
		debug_(terminatorStatus);
		debug(F(", expected HEADER frame."));
		this->clear();
		return false;
	}
	// append ID
	putHere->ID = PacketID((frames[0].id >> 2) & B1111);
	uint8_t dataPoints = FRAME_INFO_SETS[putHere->ID];
	uint8_t readValues = FRAME_INFO_SUBSETS[putHere->ID];
	// spot-check whether frame number is as anticipated. 1/2 frame per float + 1 frame for timestamp
	if (numFrames != dataPoints * ceil(readValues / 2.0) + 1)
	{
		debug_(F("NUMFRAMES incorrect. Expecting: "));
		debug_(dataPoints * ceil(readValues / 2.0) + 1);
		debug_(F(" Received: "));
		debug(numFrames);
		this->clear();
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
						debug_(F("Wrong termination ID of "));
						debug_(terminatorStatus);
						debug(F(", expected HARD terminating frame."));
						this->clear();
						return false;
					}
				}
				else {
					if (terminatorStatus != SOFT_TERMINATING_FRAME)
					{
						debug_(F("Wrong termination ID of "));
						debug_(terminatorStatus);
						debug(F(", expected SOFT terminating frame."));
						this->clear();
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
bool Frames::addItem(unsigned long id, byte length, byte* payload)
{
	frames[numFrames].id = id;
	frames[numFrames].length = length;
	memcpy(frames[numFrames].payload, payload, length);
	numFrames++;
	// return true for HARD_TERMINATING_FRAME
	return (id & HARD_TERMINATING_FRAME) == HARD_TERMINATING_FRAME;
}
void Frames::addItem(PacketID messageType, uint8_t terminatorStatus, float payload1)
{
	addItem_(messageType, terminatorStatus, payload1, 0, false);
}
void Frames::addItem(PacketID messageType, uint8_t terminatorStatus, float payload1, float payload2)
{
	addItem_(messageType, terminatorStatus, payload1, payload2, true);
}
void Frames::addItem_(uint8_t messageType, uint8_t terminatorStatus, float payload1, float payload2, bool using_payload2)
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
void Frames::clear()
{
	numFrames = 0;
}
uint8_t Frames::getNumFrames()
{
	return numFrames;
}