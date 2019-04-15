// 
// 
// 
#include "NV11Commands.h"

NV11Commands::NV11Commands(uint8_t CANId) :DataPoint("CM", CANId, 1)
{
}

void NV11Commands::insertData(uint8_t horn)
{
	this->horn = horn;
}

void NV11Commands::clearActivationHistory()
{
	horn = notActivated;
}

void NV11Commands::activateHorn()
{
	horn = activated;
}

uint8_t NV11Commands::getHorn()
{
	return horn;
}

void NV11Commands::packString(char *str)
{
	char* shiftedStr = DataPoint::packStringDefault(str);
	// horn = 1
	sprintf(shiftedStr, "%d", horn);
}

void NV11Commands::unpackString(char * str)
{
	char* ptr = strtok(str, "\t");
	ptr = strtok(NULL, "\t");
	timeStamp = strtoul(ptr, NULL, 16);

	ptr = strtok(NULL, "\t");
	horn = atoi(ptr);
}
