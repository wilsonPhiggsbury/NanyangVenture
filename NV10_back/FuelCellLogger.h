// HESFuelCell.h
#ifndef _HYDROGENCELLLOGGER_h
#define _HYDROGENCELLLOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define RX_BUFFER_LEN 200
#include "Globals.h"
class HESFuelCell
{
 private:
	 static char timeStamp[9];

	 uint8_t id;
	 HardwareSerial *port;
	 char buffer[RX_BUFFER_LEN];
	 bool hadPartialData  = false;
	 uint8_t bufferPointer = 0;
	 char amps[5];
	 char volts[5];
	 char watts[5];
	 char energy[6];
	 char capacitorVolts[5];
	 char status[3];
	 bool updated;
	 void writeRawdata(char* toWrite);
	 void writeRawdata(char* toWrite, char* writeUntilHere);
	 

 public: 
	 HESFuelCell(uint8_t id, HardwareSerial *port);
	 void logData();
	 static void dumpTimestampInto(char * location);
	 void dumpDataInto(char * location);
	 bool hasUpdated();
};


#endif

