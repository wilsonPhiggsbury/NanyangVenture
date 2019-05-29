// 
// ADS1115 full scale range (FSR) table
//	GAIN	FSR(mV)	LSB size(microV)
//	0.5		6144	187.5
//	1		4096	125
//	2		2048	62.5
//	4		1024	31.25
//	8		512		15.625
//	16		256		7.8125 <-- we are using this when we setGain(GAIN_SIXTEEN), for highest resolution. 
//  Keeping FSR small increases resolution, but it must be larger than shunt mV or the ADC may be damaged.
// 
// 75mV 200A shunt: 200A maps to - 75mV / 0.0078125mV per raw unit = 9600 raw unit
// 75mV 200A shunt: 1A maps to - 9600 raw unit / 200 amp = 48 raw units per amp
// Hence, amp = raw * ADC scale
// where ADC scale = 0.0078125 * shuntA / shuntmV
const float SHUNT_100A75mV_SCALE = 0.0078125 * (100.0 / 75);
const float SHUNT_200A75mV_SCALE = 0.0078125 * (200.0 / 75);
const float VOLTAGEDIVIDER_SCALE = 1;
#include "NV10CurrentSensor.h"
// parameter(CANbytes, stringChars)
// volts(1,2), ampCapIn(1,2), ampCapOut(1,2), ampMotor(1,2)
NV10CurrentSensorClass::NV10CurrentSensorClass(uint8_t CANId):DataPoint("CS", CANId, 8)
{
	debug(F("DataPoint CS:\t0x0C\t8"));
}

void NV10CurrentSensorClass::insertData(uint32_t voltRaw, uint32_t ampCInRaw, uint32_t ampCOutRaw, uint32_t ampMotorRaw)
{
	timeStamp = millis();
	volt = voltRaw * VOLTAGEDIVIDER_SCALE; // conversion from voltage divider
	ampCapIn = ampCInRaw * SHUNT_100A75mV_SCALE; // conversion from 75mV-100A shunt
	ampCapOut = ampCOutRaw * SHUNT_100A75mV_SCALE; // conversion from 75mV-100A shunt
	ampMotor = ampMotorRaw * SHUNT_100A75mV_SCALE; // conversion from 75mV-100A shunt
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
	ampCapIn = atoi(ptr);

	ptr = strtok(NULL, "\t");
	ampCapOut = atoi(ptr);

	ptr = strtok(NULL, "\t");
	ampMotor = atoi(ptr);
}

uint16_t NV10CurrentSensorClass::getVolt()
{
	return volt;
}

uint16_t NV10CurrentSensorClass::getAmpCapIn()
{
	return ampCapIn;
}

uint16_t NV10CurrentSensorClass::getAmpCapOut()
{
	return ampCapOut;
}

uint16_t NV10CurrentSensorClass::getAmpMotor()
{
	return ampMotor;
}


