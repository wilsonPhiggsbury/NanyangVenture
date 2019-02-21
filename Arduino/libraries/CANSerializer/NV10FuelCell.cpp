// 
// 
// 

#include "NV10FuelCell.h"
// parameter(CANbytes, stringChars)
// Watts(2,4), pressure(4,4), temperature(1,2), status(1,2)
NV10FuelCellClass::NV10FuelCellClass(uint8_t CANId):DataPoint(CANId, 8)
{
	strcpy(strHeader, "FC");
}

float NV10FuelCellClass::getPressure()
{
	return pressure;
}

uint16_t NV10FuelCellClass::getWatts()
{
	return watts;
}

uint8_t NV10FuelCellClass::getTemperature()
{
	return temperature;
}

const char * NV10FuelCellClass::getStatus()
{
	return statusTxt;
}

void NV10FuelCellClass::insertData(char* str)
{
	// slice up the string
	if (strlen(str) >= 77)
	{
		char* readPtr = strtok(str, ">");	// str + 1 to skip the first char '>', strtok will find the 2nd one
		if (readPtr == NULL)
			return;

		//updated = true;
		timeStamp = millis();
		// update respective variables

		readPtr = strtok(readPtr, " ");
		//loggedParams[volts] = atof(readPtr);

		readPtr = strtok(NULL, " ");
		//loggedParams[amps] = atof(readPtr);

		readPtr = strtok(NULL, " ");
		//loggedParams[watts] = atof(readPtr);
		watts = atof(readPtr);
		readPtr = strtok(NULL, " ");
		//loggedParams[energy] = atof(readPtr);

		// take max stack temperature out of 4 readings
		float thisMaxTemp = 0;
		for (int i = 0; i < 4; i++)
		{
			readPtr = strtok(NULL, " ");
			thisMaxTemp = max(atof(readPtr), thisMaxTemp);
		}
		//loggedParams[maxTemperature] = thisMaxTemp;
		temperature = thisMaxTemp;
		readPtr = strtok(NULL, " ");
		//loggedParams[pressure] = atof(readPtr);
		pressure = atof(readPtr);
		readPtr = strtok(NULL, " ");
		//loggedParams[capacitorVolts] = atof(readPtr);

		readPtr = strtok(NULL, " ");
		// skip average temperature

		readPtr = strtok(NULL, " ");
		strcpy(statusTxt, readPtr);
		if (strcmp(readPtr, "OP") == 0)
			status = 1;	// alive
		else if (strcmp(readPtr, "SD") == 0)
			status = 0;	// SHUTDOWN! 
		else
			status = 255; // initializing

		//>>00.0V 00.0A 0000W 00000Wh 021.1C 028.3C 028.5C 031.6C 0.90B 59.0V 028.0C IN 00.0C 00 0000
		//  ^   * ^   * ^   * ^    *  ^    * ^    * ^    * ^    * ^   * ^   *        ^ *
		//  ".........:.........:.........:.........:.........".........:.........:.........:.........:........."
	}
	// TODO: remember to terminate the string in the main code!
}

void NV10FuelCellClass::unpackCAN(const CANFrame* f)
{
	DataPoint::unpackCAN(f);
	switch (status)
	{
	case 1:
		strcpy(statusTxt, "OP");
		break;
	case 0:
		strcpy(statusTxt, "SD");
		break;
	case 255:
		strcpy(statusTxt, "IN");
		break;
	default:
		strcpy(statusTxt, "??");
		break;
	}
}

void NV10FuelCellClass::packString(char * str)
{
	char* shiftedStr = DataPoint::packStringDefault(str);
	// p = 4.2, t = 2, w = 4, st = 2
#ifdef __AVR__
	char tmp[8];
	dtostrf(pressure, 3, 1, tmp);
	sprintf(shiftedStr, "%04d\t%s\t%2d\t%s", watts, tmp, temperature, statusTxt);

#elif defined _SAM3XA_
	sprintf(shiftedStr, "%04d\t%4.1f\t%2d\t%s", watts, pressure, temperature, statusTxt);
#endif
}

void NV10FuelCellClass::unpackString(char * str)
{
	char* ptr = strtok(str, "\t");
	ptr = strtok(NULL, "\t");
	timeStamp = strtoul(ptr, NULL, 16);

	ptr = strtok(NULL, "\t");
	watts = atoi(ptr);

	ptr = strtok(NULL, "\t");
	pressure = strtod(ptr, NULL);

	ptr = strtok(NULL, "\t");
	temperature = atoi(ptr);

	ptr = strtok(NULL, "\t");
	strcpy(statusTxt, ptr);

}

