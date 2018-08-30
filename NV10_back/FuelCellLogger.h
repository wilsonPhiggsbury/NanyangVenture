// HESFuelCell.h
#ifndef _HYDROGENCELLLOGGER_h
#define _HYDROGENCELLLOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define RX_BUFFER_LEN 100
#include "Globals.h"
class HESFuelCell
{
 private:
	 static uint32_t timeStamp;

	 uint8_t id;
	 HardwareSerial *port;
	 char buffer[RX_BUFFER_LEN];
	 bool hadPartialData  = false;
	 uint8_t bufferPointer = 0;
	 char amps[5];
	 char volts[5];
	 char watts[5];
	 char energy[6];
	 char maxTemperature[6];
	 char pressure[5];
	 char capacitorVolts[5];
	 char status[3];
	 bool updated;
	 void writeAsRawData(char* toWrite);

	 void debugPrint(char* buffer, int howMuch);
	 

 public: 
	 HESFuelCell(uint8_t id, HardwareSerial *port);
	 void logData();
	 static void dumpTimestampInto(char * location);
	 void dumpDataInto(char * location);
	 bool hasUpdated();
};


#endif

