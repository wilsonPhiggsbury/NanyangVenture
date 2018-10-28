#include "FuelCellLogger.h"
#include <SPI.h>
#include <SD.h>

uint32_t HESFuelCell::timeStamp;
char* HESFuelCell::path;
bool HESFuelCell::SD_avail = false;
HESFuelCell::HESFuelCell(uint8_t id, HardwareSerial *port):id(id),port(port)
{
	port->begin(19200);
	port->setTimeout(500);
	updated = false;
	
	timeStamp = 0;
}
void HESFuelCell::setPath(char* thePath)
{
	SD_avail = true;
	path = thePath;
}
void HESFuelCell::logData()
{
	// read into buffer, see if payload found by using DELIMITER "\n"
	if (!port->available())
		return;
	uint8_t bytesRead = port->readBytesUntil('\n', buffer, RX_BUFFER_LEN);
	writeAsRawData(buffer);

	if (bytesRead >= 77)
	{
		updated = true;
		timeStamp = millis();
		// update respective variables
		char* readPtr = strtok(buffer, ">");	// buffer + 1 to skip the first char '>', strtok will find the 2nd one
		if (readPtr == NULL)
			return;

		readPtr = strtok(readPtr, " ");
		loggedParams[volts] = atof(readPtr);

		readPtr = strtok(NULL, " ");
		loggedParams[amps] = atof(readPtr);

		readPtr = strtok(NULL, " ");
		loggedParams[watts] = atof(readPtr);

		readPtr = strtok(NULL, " ");
		loggedParams[energy] = atof(readPtr);

		// take max stack temperature out of 4 readings
		float thisMaxTemp = 0;
		for (int i = 0; i < 4; i++)
		{
			readPtr = strtok(NULL, " ");
			thisMaxTemp = max(atof(readPtr), loggedParams[maxTemperature]);
		}
		loggedParams[maxTemperature] = thisMaxTemp;

		readPtr = strtok(NULL, " ");
		loggedParams[pressure] = atof(readPtr);

		readPtr = strtok(NULL, " ");
		loggedParams[capacitorVolts] = atof(readPtr);

		readPtr = strtok(NULL, " ");
		// skip average temperature

		readPtr = strtok(NULL, " ");
		if (strcmp(readPtr, "OP") != 0)
			loggedParams[status] = 0;	// died
		else
			loggedParams[status] = 1;	// alive
		
		//>>00.0V 00.0A 0000W 00000Wh 021.1C 028.3C 028.5C 031.6C 0.90B 59.0V 028.0C IN 00.0C 00 0000
		//  ^   * ^   * ^   * ^    *  ^    * ^    * ^    * ^    * ^   * ^   *        ^ *
		//  ".........:.........:.........:.........:.........".........:.........:.........:.........:........."
	}
	else
	{
		writeAsRawData("**");
	}
}
void HESFuelCell::dumpTimestampInto(unsigned long* location)
{
	*location = timeStamp;
}
void HESFuelCell::dumpDataInto(float location[QUEUEITEM_DATAPOINTS][QUEUEITEM_READVALUES])
{
	// expect QueueItem.payload to come in
	float* thisSlot = location[id];
	predictableCounter = predictableCounter < 60 ? predictableCounter + 5 : 0;
	for (int i = 0; i < FUELCELL_READVALUES; i++)
	{
		thisSlot[i] = predictableCounter;
	}
}
bool HESFuelCell::hasUpdated()
{
	bool tmp = updated;
	updated = false;
	return tmp;
}
void HESFuelCell::writeAsRawData(char* toWrite)
{
	if (SD_avail)
	{
		// set path to FC*RAW.txt
		// outcome: path = /LOG****/FC*raw.txt
		strcpy(path + FILENAME_HEADER_LENGTH, dataPoint_shortNames[FC]);
		itoa(id, path + FILENAME_HEADER_LENGTH + 2, 10);
		strcpy(path + FILENAME_HEADER_LENGTH + 2 + 1, "raw.txt");

		// higher priority task does not need critical section eh?
		File rawFCdata = SD.open(path, FILE_WRITE);
		rawFCdata.print(toWrite);
		rawFCdata.print("\n");
		rawFCdata.close();

		// revert path back to original
		strcpy(path + FILENAME_HEADER_LENGTH, "");
	}
}

/* H182_v1.3
Starting Up
System Init. >>00.0V 00.0A 0000W 00000Wh 000.0C 000.0C 000.0C 000.0C 0.00B 00.0V 000.0C IN 00.0C 00 0000
>>00.0V 00.0A 0000W 00000Wh 021.1C 028.3C 028.5C 031.6C 0.90B 59.0V 028.0C IN 00.0C 00 0000
>>49.6V 00.0A 0000W 00000Wh 021.2C 028.4C 028.5C 031.7C 0.91B 53.4V 028.4C IN 00.0C 00 0000
>>47.5V 00.0A 0000W 00000Wh 021.3C 028.5C 028.6C 031.7C 0.91B 53.3V 028.0C IN 00.0C 00 0000
>>47.9V 00.0A 0000W 00000Wh 021.4C 028.6C 028.6C 031.8C 0.90B 53.5V 027.7C IN 00.0C 00 0000
>>48.1V 00.0A 0000W 00000Wh 021.4C 028.7C 028.7C 031.8C 0.91B 52.9V 028.2C SS 00.0C 20 0200
>>48.0V 00.0A 0000W 00000Wh 021.4C 028.8C 028.8C 031.8C 0.91B 53.1V 028.8C SS 00.0C 20 0200
>>47.6V 00.0A 0000W 00000Wh 021.4C 028.8C 028.7C 031.8C 0.91B 53.1V 028.2C SS 00.0C 20 0200
>>46.9V 00.0A 0000W 00000Wh 021.4C 028.7C 028.8C 031.8C 0.73B 53.0V 028.3C SS 00.0C 20 0200
>>52.3V 00.0A 0000W 00000Wh 021.3C 028.7C 028.7C 031.8C 0.60B 53.1V 028.8C SS 00.0C 20 0200
>>69.0V 00.0A 0000W 00000Wh 021.3C 028.7C 028.7C 031.7C 0.58B 52.7V 028.4C SS 00.0C 20 0200
>>69.0V 00.0A 0000W 00000Wh 021.3C 028.6C 028.7C 031.7C 0.58B 52.6V 027.8C SS 00.0C 20 0200
>>56.8V 00.0A 0000W 00000Wh 021.3C 028.6C 028.7C 031.8C 0.58B 52.9V 028.2C SS 00.0C 20 0200
>>58.1V 00.0A 0000W 00000Wh 021.3C 028.6C 028.7C 031.8C 0.59B 52.2V 027.8C SS 00.0C 20 0200
>>58.5V 00.0A 0000W 00000Wh 021.3C 028.6C 028.8C 031.9C 0.59B 52.9V 028.0C SS 00.0C 20 0200
>>54.6V 00.5A 0030W 00000Wh 021.3C 028.7C 028.8C 032.0C 0.84B 53.0V 028.2C OP 00.0C 20 0200
>>57.3V 00.9A 0054W 00000Wh 021.4C 028.7C 028.9C 032.1C 0.83B 52.9V 028.1C OP 41.2C 20 0206
>>51.0V 02.2A 0114W 00000Wh 021.5C 028.8C 029.0C 032.2C 0.83B 52.9V 028.3C OP 41.5C 20 0214
>>53.5V 02.4A 0129W 00000Wh 021.6C 028.8C 029.1C 032.3C 0.84B 52.9V 028.4C OP 41.5C 20 0216
>>53.6V 02.2A 0119W 00000Wh 021.7C 028.9C 029.2C 032.4C 0.85B 52.9V 028.1C OP 41.5C 20 0214
>>53.7V 02.1A 0112W 00000Wh 021.8C 028.9C 029.3C 032.5C 0.84B 52.9V 028.2C OP 41.5C 20 0214
>>53.7V 01.9A 0104W 00000Wh 021.8C 029.0C 029.4C 032.6C 0.84B 52.9V 028.2C OP 41.4C 20 0213
>>53.8V 01.8A 0101W 00000Wh 021.9C 029.0C 029.5C 032.7C 0.84B 53.6V 028.2C OP 41.4C 20 0212
>>51.7V 02.0A 0105W 00000Wh 022.0C 029.1C 029.6C 032.8C 0.85B 53.6V 028.2C OP 41.4C 20 0213
>>54.0V 02.1A 0115W 00000Wh 022.0C 029.2C 029.7C 032.9C 0.84B 53.6V 028.1C OP 41.5C 20 0214
>>54.1V 02.0A 0109W 00000Wh 022.2C 029.2C 029.8C 033.0C 0.85B 53.6V 028.2C OP 41.4C 20 0213
>>54.1V 01.9A 0103W 00000Wh 022.3C 029.3C 029.9C 033.1C 0.84B 53.6V 028.3C OP 41.4C 20 0212
H182_v1.3
Shutting down...
>>54.2V 01.8A 0098W 00000Wh 022.4C 029.4C 030.0C 033.2C 0.58B 54.0V 028.3C SD 41.4C 20 0000
>>57.1V 00.0A 0000W 00000Wh 022.5C 029.4C 030.1C 033.3C 0.59B 53.8V 028.4C SD 41.4C 20 0000
>>58.9V 00.0A 0000W 00000Wh 022.5C 029.5C 030.3C 033.4C 0.58B 54.0V 028.7C SD 41.4C 20 0000
>>59.3V 00.0A 0000W 00000Wh 022.6C 029.6C 030.4C 033.4C 0.90B 53.9V 028.4C SD 41.4C 20 0000
>>59.5V 00.0A 0000W 00000Wh 022.7C 029.6C 030.4C 033.5C 0.91B 53.9V 027.9C SD 41.4C 20 0000 
*/