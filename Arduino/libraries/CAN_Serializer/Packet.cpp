#include "Frames.h"
#include "Packet.h"

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
void Packet::toString(char* putHere)
{
	// assume what pointer's passed in is not properly null-terminated
	*putHere = '\0';
	// append frame name
	strcat(putHere, frameType_shortNames[ID]);
	strcat(putHere, "\t");
	// append time stamp
	char timeStamp_tmp[9];
	ultoa(timeStamp, timeStamp_tmp, 16);
	strcat(putHere, timeStamp_tmp);

	// append all parameters of each payload point
	uint8_t dataPoints = FRAME_INFO_SETS[ID];
	uint8_t readValues = FRAME_INFO_SUBSETS[ID];

	for (uint8_t i = 0; i < dataPoints; i++)
	{
		strcat(putHere, "\t");
		for (uint8_t j = 0; j < readValues; j++)
		{
			char data_tmp[5];
#ifdef __AVR__
			dtostrf(data[i][j], FLOAT_TO_STRING_LEN, 1, data_tmp);
#elif defined _SAM3XA_
			sprintf(data_tmp, "%4.1f", data[i][j]);
#endif
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
bool Packet::toPacket(char* str, Packet* Packet)
{
	char* cur = strtok(str, "\t");
	int i, j = 0;
	// linear search for Packet ID, stop at correct ID. Return fail if overshoot.
	while(j<FRAMETYPES && strcmp(frameType_shortNames[j],cur))
	{
		j++;
	}
	if (j >= FRAMETYPES)
	{
		return false;
	}
	Packet->ID = (PacketID)j;
	uint8_t sets = FRAME_INFO_SETS[j];
	uint8_t subsets = FRAME_INFO_SUBSETS[j];

	cur = strtok(NULL, "\t");
	Packet->timeStamp = strtoul(cur, NULL, 16);

	for (i = 0; i < sets; i++)
	{
		for (j = 0; j < subsets; j++)
		{
			cur = strtok(NULL, " ");
			// for some reason the strtok below is not working, commented out and patched a HOTFIX to offset the pointer
			if (i != 0 && j == 0)
				cur++;
#ifdef __AVR__
			Packet->data[i][j] = atof(cur);
#elif defined _SAM3XA_
			Packet->data[i][j] = strtof(cur, NULL);
#endif
		}
		//cur = strtok(NULL, "\t");
	}
	return true;
}
void Packet::toFrames(Frames* putHere)
{
	uint8_t dataPoints = FRAME_INFO_SETS[ID];
	uint8_t readValues = FRAME_INFO_SUBSETS[ID];

	// prepare timeStamp frame
	//putHere->frames->id = (ID << 2) | HEADER_FRAME;
	//putHere->frames->length = sizeof(unsigned long);
	//memcpy(putHere->frames->payload, &timeStamp, putHere->frames->length);
	//(putHere->numFrames)++;
	putHere->addItem((ID << 2) | HEADER_FRAME, sizeof(unsigned long), (byte*)&timeStamp);
	// prepare payload frames
	uint8_t i,j;
	for (i = 0; i < dataPoints; i++)
	{
		TerminatorStatus terminatorStatus;
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
}