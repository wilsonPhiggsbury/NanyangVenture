// 
// 
// 

#include "NV10CurrentSensor.h"
// V = float
// A = float
NV10CurrentSensorClass::NV10CurrentSensorClass():DataPoint(0,0)
{
	strcpy(strHeader, "CS");
}
void NV10CurrentSensorClass::init()
{

}

void NV10CurrentSensorClass::insertData(float volts, float amps)
{
}

void NV10CurrentSensorClass::getStringHeader(char* c)
{
	strcpy(c, "CS");
}

void NV10CurrentSensorClass::packCAN(CANFrame *f)
{
	DataPoint::packCANDefault(f);
	f->length = 2 * sizeof(float);
	memcpy(f->payload, &volts, sizeof(float));
	memcpy(f->payload + sizeof(float), &amps, sizeof(float));
}

void NV10CurrentSensorClass::unpackCAN(const CANFrame *f)
{
	memcpy(&volts, f->payload, sizeof(float));
	memcpy(&amps, f->payload + sizeof(float), sizeof(float));
}

void NV10CurrentSensorClass::packString(char *str)
{
	DataPoint::packStringDefault(str);
	// V = 4.1, A = 4.1
}

void NV10CurrentSensorClass::unpackString(char * str)
{

}


