// 
// 
// 

#include "MotorLogger.h"



MotorLogger::MotorLogger(int motorID, uint8_t voltPin, uint8_t ampPin):voltPin(voltPin),ampPin(ampPin),id(motorID)
{
	// populate EEPROM for testing purposes
	/*int address = V_BASE_ADDR + motorID * TABLE_ENTRIES * sizeof(float);
	int address2 = A_BASE_ADDR + motorID * TABLE_ENTRIES * sizeof(float);
	for (int i = 0; i < TABLE_ENTRIES; i++)
	{
		EEPROM.put(address + sizeof(float)*i, (float)i);
		EEPROM.put(address2 + sizeof(float)*i, (float)i);
	}*/
}
void MotorLogger::logData()
{
	voltReading = analogRead(voltPin);
	ampReading = analogRead(ampPin);
	ultoa(millis(), timeStamp, 16);
}
void MotorLogger::dumpDataInto(char* location)
{
	float lo, hi, finalReading;
	int address;
	char tmp[6];


	// convert reading into voltage using lookup table
	address = V_BASE_ADDR + (id * TABLE_ENTRIES + voltReading / SAMPLE_RATE) * sizeof(float);
	EEPROM.get(address, lo);
	EEPROM.get(address + sizeof(float), hi);
	finalReading = lo + (hi - lo) * (voltReading%SAMPLE_RATE);
	dtostrf(finalReading, 4, 1, tmp);
	strcat(location, tmp);

	strcat(location, "\t");

	// convert reading into amperes using lookup table
	address = A_BASE_ADDR + (id * TABLE_ENTRIES + ampReading / SAMPLE_RATE) * sizeof(float);
	EEPROM.get(address, lo);
	EEPROM.get(address + sizeof(float), hi);
	finalReading = lo + (hi-lo) * (ampReading%SAMPLE_RATE);
	dtostrf(finalReading, 4, 1, tmp);
	strcat(location, tmp);

}
void MotorLogger::dumpTimestampInto(char* location)
{
	strcat(location, timeStamp);
	strcat(location, "\t");
}