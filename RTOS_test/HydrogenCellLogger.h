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
	 char tmp[100];
	 uint8_t tmpCounter = 0;
	 char amps[5];
	 char volts[5];
	 char watts[5];
	 char energy[6];
	 

 public:
	 HydrogenCellLogger(HardwareSerial *port);
	 void init();
	 void readData();
	 void send();
	 float getAmps();
	 float getVolts();
	 float getWatts();
	 float getEnergy();
};


#endif

