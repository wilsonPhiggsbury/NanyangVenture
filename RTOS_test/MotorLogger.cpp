// 
// 
// 

#include "MotorLogger.h"


#include <EEPROM.h>

MotorLogger::MotorLogger(int motorID, uint8_t voltPin, uint8_t ampPin):voltPin(voltPin),ampPin(ampPin),id(motorID)
{
	// get lookup table from EEPROM
	/*int address = BASE_ADDR + motorID * TABLE_ENTRIES * sizeof(float);
	for (int i = 0; i < TABLE_ENTRIES; i++)
	{
		EEPROM.get(address + sizeof(float)*i, lookupTable[i]);
	}*/
}
void MotorLogger::logData()
{
	voltReading = analogRead(voltPin);
	ampReading = analogRead(ampPin);
}
void MotorLogger::dumpDataInto(char* location)
{
	float lo, hi, finalReading;
	int address;
	// convert reading into voltage using lookup table
	address = V_BASE_ADDR + (id * TABLE_ENTRIES + voltReading / SAMPLE_RATE) * sizeof(float);
	EEPROM.get(address, lo);
	EEPROM.get(address + sizeof(float), hi);
	finalReading = lo + hi * (voltReading%SAMPLE_RATE);
	Serial.println(dtostrf(finalReading, 4, 1, location));

	// convert reading into amperes using lookup table
	address = A_BASE_ADDR + (id * TABLE_ENTRIES + ampReading / SAMPLE_RATE) * sizeof(float);
	EEPROM.get(address, lo);
	EEPROM.get(address + sizeof(float), hi);
	finalReading = lo + hi * (ampReading%SAMPLE_RATE);
	Serial.println(dtostrf(finalReading, 4, 1, location));

}