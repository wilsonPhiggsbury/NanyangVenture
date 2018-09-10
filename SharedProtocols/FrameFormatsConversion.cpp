#include "FrameFormats.h"

char dataPoint_shortNames[3][3] = { "FC", "CS", "SM" };
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

	// append all parameters of each data point
	uint8_t dataPoints, readValues;
	switch (ID)
	{
	case FC:
		dataPoints = NUM_FUELCELLS;
		readValues = FUELCELL_READVALUES;
		break;
	case CS:
		dataPoints = NUM_CURRENTSENSORS;
		readValues = CURRENTSENSOR_READVALUES;
		break;
	case SM:
		dataPoints = NUM_SPEEDOMETERS;
		readValues = SPEEDOMETER_READVALUES;
		break;
	}
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
