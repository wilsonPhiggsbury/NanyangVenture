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
#include "Globals.h"
class HESFuelCell
{
 private:
	 static uint32_t timeStamp;

	 uint8_t id;
	 HardwareSerial *port;
	 char buffer[RX_BUFFER_LEN];
	 uint8_t bufferPointer = 0;
	 float loggedParams[FUELCELL_READVALUES];
	 bool updated;
	 void writeAsRawData(char* toWrite);

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
	 void logData();
	 static void dumpTimestampInto(uint32_t * location);
	 void dumpDataInto(float location[QUEUEITEM_DATAPOINTS][QUEUEITEM_READVALUES]);
	 bool hasUpdated();
};


#endif

