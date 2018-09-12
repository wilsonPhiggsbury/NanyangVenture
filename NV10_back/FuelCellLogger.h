// HESFuelCell.h
#ifndef _HYDROGENCELLLOGGER_h
#define _HYDROGENCELLLOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define RX_BUFFER_LEN 100

#include "FrameFormats.h"

class HESFuelCell
{
 private:
	 static uint32_t timeStamp;

	 static bool SD_avail;
	 static char* path;
	 static const uint8_t FILENAME_HEADER_LENGTH = 10;

	 uint8_t id;
	 HardwareSerial *port;
	 char buffer[RX_BUFFER_LEN];
	 uint8_t bufferPointer = 0;
	 bool updated;
	 void writeAsRawData(char* toWrite);

	 float loggedParams[FUELCELL_READVALUES] = {};
	 typedef enum {
		 volts,
		 amps,
		 watts,
		 energy,
		 maxTemperature,
		 pressure,
		 capacitorVolts,
		 status
	 }LoggedParams;
	 

 public: 
	 HESFuelCell(uint8_t id, HardwareSerial *port);
	 static void setPath(char* thePath);
	 void logData();
	 static void dumpTimestampInto(unsigned long * location);
	 void dumpDataInto(float location[QUEUEITEM_DATAPOINTS][QUEUEITEM_READVALUES]);
	 bool hasUpdated();
};


#endif

