// 
// 
// 
#include "DataPointTemplate.h"
// ## Please configure your 2-char String header, CAN ID and total parameter bytes
DataPointTemplate::DataPointTemplate() :DataPoint("XX", 0xFF, 8)
{
}

void DataPointTemplate::insertData(uint32_t param1, uint32_t param2)
{
	this->param1 = param1;
	this->param2 = param2;
}

void DataPointTemplate::insertData(uint8_t param1, float param2, float param3, float param4)
{
	//## param 1 has expected range [0, 255]
	//## param 2 has expected range [0, 1] with 1 decimal precision
	//## param 3 has expected range [-100, 100] with 1 decimal precision
	//## param 4 is represented by float (yields maximum precision system can provide)
	this->param1 = param1; // passing in uint8_t, directly assign
	this->param2 = param2*10; // passing in float, convert to internal representation (param2*10 must be in range [0, 255])
	this->param3 = (param3 + 100) * 10; // passing in float, convert to internal representation ((param3+100)*10) must be in range [0, 65535])
	this->param4 = (uint32_t)param4; // passing in float, directly convert 4-byte data into 4-byte data

	// these can be implemented as seperate setter functions as well
}
//## getter function must tally
uint16_t DataPointTemplate::getParam1()
{
	return param1;
}
//## getter function must tally
float DataPointTemplate::getParam2()
{
	return param2 / 10.0;
}
//## getter function must tally
float DataPointTemplate::getParam3()
{
	return (param3 / 10.0) - 100;
}
//## getter function must tally
float DataPointTemplate::getParam4()
{
	return (float)param4;
}
void DataPointTemplate::packString(char *str)
{
	// do not touch this line
	char* shiftedStr = DataPoint::packStringDefault(str);

	//## how do you want to visualize your data as string?
#ifdef __AVR__
	char floatStringBuffer[8];
	dtostrf(param2 / 10.0, 3, 1, floatStringBuffer);
	sprintf(shiftedStr, "%d\t%s", param1, floatStringBuffer); // note: sprintf("%f") does not work on AVR. This is a workaround.
#elif defined _SAM3XA_
	sprintf(shiftedStr, "%d\t%4.1f", param1, param2);
#endif
}
void DataPointTemplate::unpackString(char * str)
{
	// do not touch these lines
	char* ptr = strtok(str, "\t");
	ptr = strtok(NULL, "\t");
	timeStamp = strtoul(ptr, NULL, 16);

	//## how do you undo your packing, and restore the params from the packed string?
	ptr = strtok(NULL, "\t");
	param1 = atoi(ptr);

	ptr = strtok(NULL, "\t");
	param2 = strtod(ptr, NULL);

}
