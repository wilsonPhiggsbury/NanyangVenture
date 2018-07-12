// HydrogenCellLogger.h
#include "Wiring.h"
#ifndef _HYDROGENCELLLOGGER_h
#define _HYDROGENCELLLOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


class HydrogenCellLogger
{
 private:
	 HardwareSerial *port;
	 char buffer[100];
	 uint8_t bufferPointer = 0;
	 char amps[5];
	 char volts[5];
	 char watts[5];
	 char energy[6];
	 char capacitorVolts[5];
	 char status[3];
	 bool updated;

	 static char timeStamp[9];
	 

 public:
	 HydrogenCellLogger(HardwareSerial *port);
	 void init();
	 void readData();
	 static void dumpTimestampInto(char * location);
	 void dumpDataInto(char * location);
	 bool hasUpdated();
	 void debugPrint();
};


#endif

