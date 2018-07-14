// 
// 
// 

#include "MotorLogger.h"
#include <Arduino_FreeRTOS.h>


char MotorLogger::timeStamp[9];
MotorLogger::MotorLogger(int motorID, uint8_t voltPin, uint8_t ampPin):voltPin(voltPin),ampPin(ampPin),id(motorID)
{
	
}

int MotorLogger::getStep(char which)
{
	switch (which)
	{
	case 'V':
		return V_STEP;
	case 'A':
		return A_STEP;
	default:
		debug(F("Attempted getBaseAddr() with invalid attribute."));
		return -1;
	}
}
uint16_t MotorLogger::getEntries(char which)
{
	switch (which)
	{
	case 'V':
		return V_ENTRIES;
	case 'A':
		return A_ENTRIES;
	default:
		debug(F("Attempted getEntries() with invalid attribute."));
		return 65535;
	}
}
void MotorLogger::logData()
{
	voltReading = analogRead(voltPin);
	ampReading = analogRead(ampPin);
	ampPeak = max(ampReading, ampPeak);
	//voltReading += 16;
	//ampReading += 16;
	//if (voltReading >= 1023)
	//	voltReading = 0;
	//if (ampReading >= 1023)
	//	ampReading = 0;

	ultoa(millis(), timeStamp, 16);
}
void MotorLogger::dumpDataInto(char* location, bool raw)
{
	char tmp[6];
	float finalReading;

	// convert analog reading into VOLTS using lookup table
	debug_("V ID: ");debug(id);
	if (raw)
		finalReading = voltReading;
	else
		finalReading = rawToVA('V', voltReading);
	// put into tmp, length 4 (dot inclusive) with 1 decimal place
	dtostrf(finalReading, 4, 1, tmp);
	strcat(location, tmp);
	strcat(location, "\t");

	// convert analog reading into AMPS using lookup table
	debug_("A ID: ");debug(id);
	if (raw)
		finalReading = ampReading;
	else
		finalReading = rawToVA('A', ampReading);
	// put into tmp and ship
	dtostrf(finalReading, 4, 1, tmp);
	strcat(location, tmp);

}
void MotorLogger::dumpAmpPeakInto(char *location)
{
	float finalReading = rawToVA('A', ampPeak);
	char tmp[6];
	dtostrf(finalReading, 4, 1, tmp);
	strcat(location, tmp);
}
float MotorLogger::rawToVA(char which, float reading)
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
	debug_(which);debug_(" reading: ");debug(reading);
	// convert reading into voltage using lookup table
	int i = 0;
	nextValue = pgm_read_word(TABLE + i);
	//debug_(which);debug_("-");debug_(i);debug_(":  ");debug(nextValue);
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
void MotorLogger::dumpTimestampInto(char* location)
{
	strcat(location, timeStamp);
}
uint16_t MotorLogger::getAmpPeak()
{
	return ampPeak;
}