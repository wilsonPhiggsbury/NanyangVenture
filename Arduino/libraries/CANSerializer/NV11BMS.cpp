// 
// 
// 
#include "NV11BMS.h"

NV11BMS::NV11BMS(uint8_t CANId):DataPoint("BM", CANId, 6)
{
}

void NV11BMS::insertData(uint32_t volt, uint32_t amp)
{
	this->volt = volt;
	this->amp = amp;
}

uint16_t NV11BMS::getVolt()
{
	return volt;
}
uint16_t NV11BMS::getAmp()
{
	return amp;
}
uint16_t NV11BMS::getTemperature()
{
	return temperature;
}

void NV11BMS::packString(char *str)
{
	char* shiftedStr = DataPoint::packStringDefault(str);

	sprintf(shiftedStr, "%d\t%d\t%d", volt, amp, temperature);
}

void NV11BMS::unpackString(char * str)
{
	char* ptr = strtok(str, "\t");
	ptr = strtok(NULL, "\t");
	timeStamp = strtoul(ptr, NULL, 16);

	ptr = strtok(NULL, "\t");
	volt = atoi(ptr);

	ptr = strtok(NULL, "\t");
	amp = atoi(ptr);

	ptr = strtok(NULL, "\t");
	temperature = atoi(ptr);
}
