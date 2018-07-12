// 
// 
// 

#include "MotorLogger.h"
#include <Arduino_FreeRTOS.h>

#define DEBUG 0
#if DEBUG == 1
#define debug(...) Serial.println(__VA_ARGS__)
#define debug_(...) Serial.print(__VA_ARGS__)
#else
#define debug(...)
#define debug_(...)
#endif

char MotorLogger::timeStamp[9];
MotorLogger::MotorLogger(int motorID, uint8_t voltPin, uint8_t ampPin):voltPin(voltPin),ampPin(ampPin),id(motorID)
{
	
}
void MotorLogger::populateEEPROM()
{
	// populate EEPROM for testing purposes
	const float READING_V_RATIO = 1023 / 5;
	int address = getAddr('V');
	// lookup table for volts
	unsigned int voltLookupTable[V_ENTRIES] = {
		READING_V_RATIO * 0,
		READING_V_RATIO * 0.32,
		READING_V_RATIO * 0.64,
		READING_V_RATIO * 0.96, 
		READING_V_RATIO * 1.28,
		READING_V_RATIO * 1.60,
		READING_V_RATIO * 1.92,
		READING_V_RATIO * 2.24, 
		READING_V_RATIO * 2.56,
		READING_V_RATIO * 2.88,
		READING_V_RATIO * 3.20,
		READING_V_RATIO * 3.52,
		READING_V_RATIO * 3.84
	};
	debug_("V_ENTRIES: ");debug(V_ENTRIES);debug_("ID: ");debug_(id);debug_(" Address: ");debug(address);
	for (int i = 0; i < V_ENTRIES; i++)
	{
		unsigned int content;
		content = voltLookupTable[i];
		//content = ((i * 1024.0) / V_ENTRIES);
		int offset = sizeof(unsigned int)*i;
		EEPROM.put(address + offset, content);
		debug_(address + offset);debug_(" ");debug(content);
	}
	// lookup table for amps
	unsigned int ampLookupTable[A_ENTRIES] = {
		READING_V_RATIO * 0,
		READING_V_RATIO * 0.13,
		READING_V_RATIO * 0.27,
		READING_V_RATIO * 0.40,
		READING_V_RATIO * 0.53,
		READING_V_RATIO * 0.67,
		READING_V_RATIO * 0.80,
		READING_V_RATIO * 0.93,
		READING_V_RATIO * 1.03,
		READING_V_RATIO * 1.20,
		READING_V_RATIO * 1.33,
		READING_V_RATIO * 1.47,
		READING_V_RATIO * 1.60,
		READING_V_RATIO * 1.73,
		READING_V_RATIO * 1.87,
		READING_V_RATIO * 2.00,
		READING_V_RATIO * 2.13,
		READING_V_RATIO * 2.27,
		READING_V_RATIO * 2.40,
		READING_V_RATIO * 2.53,
		READING_V_RATIO * 2.67,
		READING_V_RATIO * 2.80,
		READING_V_RATIO * 2.93,
		READING_V_RATIO * 3.07,
		READING_V_RATIO * 3.20
	};
	address = getAddr('A');
	debug_("A_ENTRIES: ");debug(A_ENTRIES);debug_("ID: ");debug_(id);debug_(" Address: ");debug(address);
	for (int i = 0; i < A_ENTRIES; i++)
	{
		unsigned int content;
		content = ampLookupTable[i];
		//content = ((i * 1024.0) / A_ENTRIES);
		int offset = sizeof(unsigned int)*i;
		EEPROM.put(address + offset, content);
		debug_(address + offset);debug_(" ");debug(content);
	}
}

bool MotorLogger::updateTable(char which, unsigned int content[max(V_ENTRIES, A_ENTRIES)])
{
	unsigned int entries = MotorLogger::getEntries(which);
	
	for (int i = 0; i < entries; i++)
	{
		int offset = i * sizeof(unsigned int);
		int address = getAddr(which) + offset;
		EEPROM.put(address, content[i]);
		debug_(address + offset);debug_(" ");debug(content[i]);
	}
}
int MotorLogger::getAddr(char which)
{
	unsigned int entries = getEntries(which);
	int base = getBaseAddr(which);
	return base + (entries) * (id) * (sizeof(unsigned int));
}
int MotorLogger::getBaseAddr(char which)
{
	switch (which)
	{
	case 'V':
		return V_BASE_ADDR;
	case 'A':
		return A_BASE_ADDR;
	default:
		debug(F("Attempted getBaseAddr() with invalid attribute."));
		return -1;
	}
}
void MotorLogger::logData()
{
	voltReading = analogRead(voltPin);
	ampReading = analogRead(ampPin);
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
	if(!raw)finalReading = rawToVA('V');
	// put into tmp, length 4 (dot inclusive) with 1 decimal place
	dtostrf(finalReading, 4, 1, tmp);
	strcat(location, tmp);
	strcat(location, "\t");

	// convert analog reading into AMPS using lookup table
	if(!raw)finalReading = rawToVA('A');
	// put into tmp and ship
	dtostrf(finalReading, 4, 1, tmp);
	strcat(location, tmp);

}
float MotorLogger::rawToVA(char which)
{
	float first, last, step, highBound;
	int entries, address, baseAddress;
	unsigned int reading, thisValue, nextValue;

	switch (which)
	{
	case 'V':
		first = V_0;
		last = V_N;
		step = V_STEP;
		baseAddress = V_BASE_ADDR;
		reading = voltReading;
		break;
	case 'A':
		first = A_0;
		last = A_N;
		step = A_STEP;
		baseAddress = A_BASE_ADDR;
		reading = ampReading;
		break;
	}
	// convert reading into voltage using lookup table
	entries = ((last - first)/step) + 1;
	address = baseAddress + (entries) * (id) * (sizeof(unsigned int));
	highBound = first;
	EEPROM.get(address, nextValue);

	while (highBound <= last + step && nextValue < reading)
	{
		highBound += step;
		address += sizeof(unsigned int);
		thisValue = nextValue;
		EEPROM.get(address, nextValue);
	}

	float finalReading;
	if (highBound <= first)
		finalReading = first;
	else if (highBound >= last + step)
		finalReading = last;
	else
		finalReading = (reading - thisValue) * (step / (float)(nextValue - thisValue)) + (highBound - step);//map(reading, thisValue, nextValue, highBound-step, highBound);

	return finalReading;
}
void MotorLogger::dumpTimestampInto(char* location)
{
	strcat(location, timeStamp);
	strcat(location, "\t");
}
unsigned int MotorLogger::getEntries(char which)
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