// 
// 
// 

#include "MotorLogger.h"
#include <Arduino_FreeRTOS.h>

char MotorLogger::timeStamp[9];
MotorLogger::MotorLogger(int motorID, uint8_t voltPin, uint8_t ampPin):voltPin(voltPin),ampPin(ampPin),id(motorID)
{
	
}
void MotorLogger::populateEEPROM()
{
	// populate EEPROM for testing purposes
	int address = V_BASE_ADDR + (V_ENTRIES) * (id) * (sizeof(unsigned int));
	//Serial.print("V_ENTRIES: ");Serial.println(V_ENTRIES);
	//Serial.print("ID: ");Serial.print(id);Serial.print(" Address: ");Serial.println(address);
	for (int i = 0; i < V_ENTRIES; i++)
	{
		Serial.print(i);
		unsigned int content = ((i * 1024.0) / V_ENTRIES);
		int offset = sizeof(unsigned int)*i;
		EEPROM.put(address + offset, content);
	}
	address = A_BASE_ADDR + (A_ENTRIES) * (id) * (sizeof(unsigned int));
	//Serial.print("A_ENTRIES: ");Serial.println(A_ENTRIES);
	//Serial.print("ID: ");Serial.print(id);Serial.print(" Address: ");Serial.println(address);
	for (int i = 0; i < A_ENTRIES; i++)
	{
		Serial.print(i);
		unsigned int content = ((i * 1024.0) / A_ENTRIES);
		int offset = sizeof(unsigned int)*i;
		EEPROM.put(address + offset, content);
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
void MotorLogger::dumpDataInto(char* location)
{
	char tmp[6];
	float finalReading;

	// convert analog reading into VOLTS using lookup table
	finalReading = rawToVA(voltReading, V_0, V_N, V_STEP, V_BASE_ADDR);
	// put into tmp
	dtostrf(finalReading, 4, 1, tmp);

	strcat(location, tmp);
	strcat(location, "\t");

	// convert analog reading into AMPS using lookup table
	finalReading = rawToVA(ampReading, A_0, A_N, A_STEP, A_BASE_ADDR);
	// put into tmp and ship
	dtostrf(finalReading, 4, 1, tmp);
	strcat(location, tmp);

}
float MotorLogger::rawToVA(uint16_t reading, float first, float last, float step, int baseAddr)
{
	float highBound, finalReading;
	int address;
	unsigned int thisValue, nextValue;


	// convert reading into voltage using lookup table
	int entries = ((last - first)/step) + 1;
	address = baseAddr + (entries) * (id) * (sizeof(unsigned int));
	highBound = first;
	EEPROM.get(address, nextValue);

	while (highBound <= last + step && nextValue < reading)
	{
		highBound += step;
		address += sizeof(unsigned int);
		thisValue = nextValue;
		EEPROM.get(address, nextValue);
	}

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