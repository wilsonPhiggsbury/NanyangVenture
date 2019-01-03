// 
// 
// 

#include "CurrentSensorLogger.h"

uint32_t AttopilotCurrentSensor::timeStamp;
AttopilotCurrentSensor::AttopilotCurrentSensor(int motorID, uint8_t voltPin, uint8_t ampPin):voltPin(voltPin),ampPin(ampPin),id(motorID)
{
	// id is only for accessing lookup table
}
void AttopilotCurrentSensor::logData()
{
	loggedParams[volt] = analogRead(voltPin);
	loggedParams[amp] = analogRead(ampPin);

	//ampPeak = max(ampReading, ampPeak);
	/*if (timeStamp != 0)
	{
		float usedEnergy = rawToVA('V', voltReading) * rawToVA('A', ampReading);
		usedEnergy *= (millis() - timeStamp);
		totalEnergy +=  usedEnergy;
	}*/
	timeStamp = millis();
}

void AttopilotCurrentSensor::dumpTimestampInto(unsigned long* location)
{
	*location = timeStamp;
}
void AttopilotCurrentSensor::dumpDataInto(float location[QUEUEITEM_DATAPOINTS][QUEUEITEM_READVALUES])
{
	// only convert to Volt/Amp during send, don't convert during logging as logging happens mroe frequently
	processData();
	float* thisSlot = location[id];
	//predictableCounter = predictableCounter < 40 ? predictableCounter + 2 : 0;
	for (int i = 0; i < CURRENTSENSOR_READVALUES; i++)
	{
		thisSlot[i] = loggedParams[i];
	}
}
void AttopilotCurrentSensor::processData()
{
	for (int i = 0; i < CURRENTSENSOR_READVALUES; i++)
	{
		loggedParams[i] = rawToVA((LoggedParameters)i, loggedParams[i]);
	}
}
float AttopilotCurrentSensor::rawToVA(LoggedParameters which, float reading)
{
	float first, last, step, maxIndex;
	uint16_t thisValue, nextValue;
	const uint16_t* TABLE;
	switch (which)
	{
	case volt:
		first = V_0;
		last = V_N;
		step = V_STEP;
		maxIndex = V_ENTRIES;
		TABLE = V_TABLE[id]; // point to this segment of the array
		break;
	case amp:
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