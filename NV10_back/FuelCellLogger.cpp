#include "FuelCellLogger.h"
#include <SD.h>

const char DELIMITER[] = ">>";
const size_t DELIMITER_LEN = strlen(DELIMITER);
char HESFuelCell::timeStamp[9];
HESFuelCell::HESFuelCell(uint8_t id, HardwareSerial *port):id(id),port(port)
{
	port->begin(19200);
	updated = false;
	strcpy(volts, "0.00");
	strcpy(amps, "0.00");
	strcpy(watts, "0000");
	strcpy(energy, "00000");
	strcpy(capacitorVolts, "00.0");
	strcpy(status, "XX");
	strcpy(timeStamp, "0");
}

void HESFuelCell::logData()
{
	// read into buffer, see if data found by using DELIMITER ">>"
	if(!port->available())
		return;
	while (port->available())
	{
		if (bufferPointer >= RX_BUFFER_LEN-1)
		{
			// buffer overflow
			break;
		}
		buffer[bufferPointer] = port->read();
		// check if there was partial data before this. If yes, lookout for newline indicators!
		if (hadPartialData && (buffer[bufferPointer] == '>' || buffer[bufferPointer] == '\r' || buffer[bufferPointer] == '\n'))
		{
			// if newline confirmed, discard the old partial data. Otherwise, append.
			buffer[0] = buffer[bufferPointer];
			bufferPointer = 1;
			hadPartialData = false;
		}
		bufferPointer++;
	}
	buffer[bufferPointer] = '\0';
	debug("Read");
	//debug("---");
	//debug(buffer);
	//debug("***");
	// search buffer for DELIMITER, *found points to start of the DELIMITER (or NULL if not found)
	char *found = strstr(buffer, DELIMITER);
	if (found == NULL)
	{
		// DELIMITER nowhere to be found in current buffer. Clear it.
		// Log the whole row of raw data before discarding
		writeRawdata(buffer);
		// then discard by resetting the pointer
		bufferPointer = 0;
		debug("THROW");
	}
	else if (strlen(found) < DELIMITER_LEN + 75) // 75 = (4+2) + (4+2) + (4+2) + (5+37) + (4+9) + (2)
	{
		// Log the part about to be discarded
		writeRawdata(buffer, found);
		// Partial valid data came in. Migrate wanted parts to the front of buffer array.
		uint8_t moveCounter = 0;
		do
		{
			buffer[moveCounter] = *(found+moveCounter);
		} while (buffer[moveCounter++] != '\0');
		bufferPointer = moveCounter-1;
		hadPartialData = true;
		debug("SHIFT");
		// We will continue reading them next time we fire this function.
	}
	else
	{
		writeRawdata(buffer);
		// update timestamp
		ultoa(millis(), timeStamp, 16);
		updated = true;
		// update respective variables
		strncpy(volts, found + DELIMITER_LEN + 0, 4);
		strncpy(amps, found + DELIMITER_LEN + 6, 4);
		strncpy(watts, found + DELIMITER_LEN + 12, 4);
		strncpy(energy, found + DELIMITER_LEN + 18, 5);
		strncpy(capacitorVolts, found + DELIMITER_LEN + 60, 4);
		strncpy(status, found + DELIMITER_LEN + 73, 2);
		if (hadPartialData)
			debug("SALVAGE");
		//>>00.0V 00.0A 0000W 00000Wh 021.1C 028.3C 028.5C 031.6C 0.90B 59.0V 028.0C IN 00.0C 00 0000
		//  ^   * ^   * ^   * ^    *                                    ^   *        ^ *
		//volts[4] = amps[4] = watts[4] = energy[5] = capacitorVolts[4] = status[2] = '\0';

		// clear buffer string by resetting pointer
		bufferPointer = 0;
	}
}
void HESFuelCell::dumpTimestampInto(char* location)
{
	strcat(location, timeStamp);//	8
	//						SUM =	8
}
void HESFuelCell::dumpDataInto(char* location)
{
	strcat(location, volts);//			4
	strcat(location, "\t");//			1
	strcat(location, amps);//			4
	strcat(location, "\t");//			1
	strcat(location, watts);//			4
	strcat(location, "\t");//			1
	strcat(location, energy);//			5
	strcat(location, "\t");//			1
	strcat(location, capacitorVolts);//	4
	strcat(location, "\t");//			1
	strcat(location, status);//			2
	//									1 (for '\0')
	//							SUM =  29
}
bool HESFuelCell::hasUpdated()
{
	bool tmp = updated;
	updated = false;
	return tmp;
}
void HESFuelCell::writeRawdata(char* toWrite)
{
	if (SD_avail)
	{
		if (id == 0)
			strcpy(path + FILENAME_HEADER_LENGTH, MASTER_FUELCELL_RAW_FILENAME);
		else if (id == 1)
			strcpy(path + FILENAME_HEADER_LENGTH, SLAVE_FUELCELL_RAW_FILENAME);
		else
			return;

		File rawFCdata = SD.open(path, FILE_WRITE);
		rawFCdata.print(toWrite);
		rawFCdata.close();
		strcpy(path + FILENAME_HEADER_LENGTH, "");
	}
}
void HESFuelCell::writeRawdata(char* toWrite, char* writeUntilHere)
{
	char tmp = (*writeUntilHere);
	(*writeUntilHere) = '\0';
	writeRawdata(toWrite);
	(*writeUntilHere) = tmp;
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
>>59.5V 00.0A 0000W 00000Wh 022.7C 029.6C 030.4C 033.5C 0.91B 53.9V 027.9C SD 41.4C 20 0000 */