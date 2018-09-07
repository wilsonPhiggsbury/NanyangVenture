// 
// 
// 

#include "CurrentSensorLogger.h"


AttopilotCurrentSensor::AttopilotCurrentSensor(int motorID, uint8_t voltPin, uint8_t ampPin):voltPin(voltPin),ampPin(ampPin),id(motorID)
{
	// id is only for accessing lookup table
}
void AttopilotCurrentSensor::logData()
{
	voltReading = analogRead(voltPin);
	ampReading = analogRead(ampPin);

	ampPeak = max(ampReading, ampPeak);
	/*if (timeStamp != 0)
	{
		float usedEnergy = rawToVA('V', voltReading) * rawToVA('A', ampReading);
		usedEnergy *= (millis() - timeStamp);
		totalEnergy +=  usedEnergy;
	}*/
	timeStamp = millis();
}

void AttopilotCurrentSensor::dumpTimestampInto(char* location)
{
	char tmp[9];
	ultoa(timeStamp, tmp, 16);
	strcat(location, tmp);
}
void AttopilotCurrentSensor::dumpDataInto(char* location)
{
	float finalReading;
	char tmp[6];

	dumpAttributeInto(location, voltReading, 'V');
	dumpAttributeInto(location, ampReading, 'A');
	dumpAttributeInto(location, ampPeak, 'A');
}
void AttopilotCurrentSensor::dumpAttributeInto(char* location, uint16_t attribute, char conversionType)
{
	float finalReading;
	char tmp[6];

	strcat(location, "\t");
	finalReading = rawToVA(conversionType, attribute);
	dtostrf(finalReading, 4, 1, tmp);
	strcat(location, tmp);
}
//void AttopilotCurrentSensor::dumpTotalEnergyInto(char *location)
//{
//	//float finalReading = rawToVA('A', ampPeak);
//	// TODO: there is place for optimization by postponing calculate raw first until truly dumping data
//	char tmp[6];
//	float whr = totalEnergy / 3600000;
//	dtostrf(whr, 6, 1, tmp);
//	strcat(location, tmp);
//}
float AttopilotCurrentSensor::rawToVA(char which, float reading)
{
	float first, last, step, maxIndex;
	uint16_t thisValue, nextValue;
	const uint16_t* TABLE;
	switch (which)
	{
	case 'V':
		first = V_0;
		last = V_N;
		step = V_STEP;
		maxIndex = V_ENTRIES;
		TABLE = V_TABLE[id]; // point to this segment of the array
		break;
	case 'A':
		first = A_0;
		last = A_N;
		step = A_STEP;
		maxIndex = A_ENTRIES;
		TABLE = A_TABLE[id]; // point to this segment of the array
		break;
	}
	// Convert reading into volt/amp using lookup table
	//	Find two values in lookup table so that this reading is "sandwiched" between them. Linear map between those two values to obtain real volt/amp value.
	int i = 0;
	nextValue = pgm_read_word(TABLE + i);
	if (reading < nextValue)
		return first;
	for (i = 1; i<maxIndex; i++)
	{
		thisValue = nextValue; // TABLE[i]
		nextValue = pgm_read_word(TABLE + i); // TABLE[i+1]
		if (reading < nextValue)
		{
			//reading = map(reading, thisValue, nextValue, (i - 1)*step + first, (i)*step + first);
			// Map function truncates decimals. Use below implementation.
			reading = ((reading - thisValue) * (step) / (nextValue - thisValue)) + ((i - 1)*step + first);
			return reading;
		}
	}
	return last;
}