// HydrogenCellLogger.h

#ifndef _HYDROGENCELLLOGGER_h
#define _HYDROGENCELLLOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class HydrogenCellLogger
{
 protected:
	 HardwareSerial *port;
	 char buffer[100];
	 uint8_t tmpCounter = 0;
	 char amps[5];
	 char volts[5];
	 char watts[5];
	 char energy[6];

	 static char timeStamp[9];
	 

 public:
	 HydrogenCellLogger(HardwareSerial *port);
	 void init();
	 void readData();
	 static void dumpTimestampInto(char * location);
	 void dumpDataInto(char * location);
	 void debugPrint();
};


#endif

