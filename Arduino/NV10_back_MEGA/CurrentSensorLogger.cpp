// 
// 
// 

#include "CurrentSensorLogger.h"

uint32_t AttopilotCurrentSensor::timeStamp;
AttopilotCurrentSensor::AttopilotCurrentSensor(int motorID, uint8_t voltPin, uint8_t ampPin):voltPin(voltPin),ampPin(ampPin),id(motorID)
{
	// id is only for accessing lookup table
	ADS.setGain(GAIN_SIXTEEN);
	ADS.begin();
}
/// <summary>
/// Logs the Current Sensor analog data. 
/// </summary>
void AttopilotCurrentSensor::logData()
{
	if (id == 2)
	{
		loggedParams[volt] = 0;
		loggedParams[amp] = ADS.readADC_Differential_0_1();
	}
	else
	{
		loggedParams[volt] = analogRead(voltPin);
		loggedParams[amp] = analogRead(ampPin);
	}

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
void AttopilotCurrentSensor::dumpDataInto(float location[NUM_DATASETS][NUM_DATASUBSETS])
{
	// only convert to Volt/Amp during send, don't convert during logging as logging happens mroe frequently
	processData();
	float* thisSlot = location[id];
	for (int i = 0; i < CURRENTSENSOR_READVALUES; i++)
	{
		thisSlot[i] = loggedParams[i];
	}
}
void AttopilotCurrentSensor::processData()
{
	if (id == 2) // HOTFIX for motor shunt
	{
		loggedParams[volt] = 0;
		loggedParams[amp] /= 256.0;
	}
	else
	{
		loggedParams[volt] = rawToVA(volt, loggedParams[volt]);
		loggedParams[amp] = rawToVA(amp, loggedParams[amp]);
	}
}
/// <summary>
/// Converts raw analog readings [0-1023] to Volts/Amps, using a lookup table.
/// </summary>
/// <param name="which">An enum to specify V or A</param>
/// <param name="reading"></param>
/// <returns></returns>
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