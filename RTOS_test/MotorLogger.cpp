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
	/*voltReading += 16;
	ampReading += 16;
	if (voltReading >= 1023)
		voltReading = 0;
	if (ampReading >= 1023)
		ampReading = 0;*/

	ultoa(millis(), timeStamp, 16);
}
void MotorLogger::dumpDataInto(char* location)
{
	float highBound, finalReading;
	int address;
	unsigned int thisValue, nextValue;
	char tmp[6];


	// convert reading into voltage using lookup table
	address = V_BASE_ADDR + (V_ENTRIES) * (id) * (sizeof(unsigned int));
	highBound = V_0;
	EEPROM.get(address, nextValue);

	while (highBound <= V_N+V_STEP && nextValue < voltReading)
	{
		highBound += V_STEP;
		address += sizeof(unsigned int);
		thisValue = nextValue;
		EEPROM.get(address, nextValue);
	}

	if (highBound <= V_0)
		finalReading = V_0;
	else if (highBound >= V_N + V_STEP)
		finalReading = V_N;
	else
		finalReading = (voltReading - thisValue) * (V_STEP / (float)(nextValue - thisValue)) + (highBound - V_STEP);//map(voltReading, thisValue, nextValue, highBound-V_STEP, highBound);
	// put into tmp and ship
	dtostrf(finalReading, 4, 1, tmp);
	strcat(location, tmp);

	strcat(location, "\t");

	// convert reading into amperes using lookup table
	address = A_BASE_ADDR + (A_ENTRIES) * (id) * (sizeof(unsigned int));
	highBound = A_0;
	EEPROM.get(address, nextValue);

	while (highBound <= A_N + A_STEP && nextValue < ampReading)
	{
		highBound += A_STEP;
		address += sizeof(unsigned int);
		thisValue = nextValue;
		EEPROM.get(address, nextValue);
	}
	if (highBound <= A_0)
		finalReading = A_0;
	else if (highBound >= A_N + A_STEP)
		finalReading = A_N;
	else
		finalReading = (ampReading-thisValue) * (A_STEP / (nextValue - thisValue)) + (highBound - A_STEP);//map(ampReading, thisValue, nextValue, highBound-V_STEP, highBound);
	// put into tmp and ship
	dtostrf(finalReading, 4, 1, tmp);
	strcat(location, tmp);

}
void MotorLogger::dumpTimestampInto(char* location)
{
	strcat(location, timeStamp);
	strcat(location, "\t");
}