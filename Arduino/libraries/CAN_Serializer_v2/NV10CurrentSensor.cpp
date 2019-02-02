// 
// 
// 
const float ATTOPILOT180A_VOLTAGE_SCALE = 50 / (3.3 / (5 * 1024));
const float ATTOPILOT180A_CURRENT_SCALE = 180 / (3.3 / (5 * 1024));
const float SHUNT200A_CURRENT_SCALE = 1/32.0;
const float SHUNT50A_CURRENT_SCALE = 1/256.0;
#include "NV10CurrentSensor.h"
// parameter(CANbytes, stringChars)
// volts(1,2), ampCapIn(1,2), ampCapOut(1,2), ampMotor(1,2)
NV10CurrentSensorClass::NV10CurrentSensorClass(uint8_t CANId):DataPoint(CANId, 1+1+1+1)
{
	strcpy(strHeader, "CS");
}

void NV10CurrentSensorClass::insertData(uint32_t voltRaw, uint32_t ampCInRaw, uint32_t ampCOutRaw, uint32_t ampMotorRaw)
{
	timeStamp = millis();
	volt = voltRaw * ATTOPILOT180A_VOLTAGE_SCALE; // conversion from Attopilot voltage pin
	ampCapIn = ampCInRaw * ATTOPILOT180A_CURRENT_SCALE;// conversion from Attopilot current pin
	ampCapOut = ampCOutRaw * ATTOPILOT180A_CURRENT_SCALE;// conversion from Attopilot current pin
	ampMotor = ampMotorRaw * SHUNT200A_CURRENT_SCALE; // conversion from 50mV-200A shunt
}

void NV10CurrentSensorClass::packCAN(CANFrame *f)
{
	DataPoint::packCANDefault(f);
	memcpy(f->payload, &volt, sizeof(uint8_t));
	memcpy(f->payload + sizeof(uint8_t), &ampCapIn, sizeof(uint8_t));
	memcpy(f->payload + 2*sizeof(uint8_t), &ampCapOut, sizeof(uint8_t));
	memcpy(f->payload + 3*sizeof(uint8_t), &ampMotor, sizeof(uint8_t));
}

void NV10CurrentSensorClass::unpackCAN(const CANFrame *f)
{
	memcpy(&volt, f->payload, sizeof(uint8_t));
	memcpy(&ampCapIn, f->payload + sizeof(uint8_t), sizeof(uint8_t));
	memcpy(&ampCapOut, f->payload + 2 * sizeof(uint8_t), sizeof(uint8_t));
	memcpy(&ampMotor, f->payload + 3 * sizeof(uint8_t), sizeof(uint8_t));
}

void NV10CurrentSensorClass::packString(char *str)
{
	char* shiftedStr = DataPoint::packStringDefault(str);
	// v = 2, aCin = 2, aCout = 2, aMotor = 2
	sprintf(shiftedStr, "%02d\t%02d\t%02d\t%02d", volt, ampCapIn, ampCapOut, ampMotor);
}

void NV10CurrentSensorClass::unpackString(char * str)
{
	char* ptr = strtok(str, "\t");
	ptr = strtok(NULL, "\t");
	timeStamp = strtoul(ptr, NULL, 16);

	ptr = strtok(NULL, "\t");
	volt = atoi(ptr);

	ptr = strtok(NULL, "\t");
	ampCapIn = strtod(ptr, NULL);

	ptr = strtok(NULL, "\t");
	ampCapOut = atoi(ptr);

	ptr = strtok(NULL, "\t");
	ampMotor = atoi(ptr);
}


