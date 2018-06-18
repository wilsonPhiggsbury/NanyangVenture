// HydrogenCellLogger.h

#ifndef _HYDROGENCELLLOGGER_h
#define _HYDROGENCELLLOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Timer.h"

class HydrogenCellLogger
{
 protected:
	 HardwareSerial * port;
	 float amps;
	 float volts;
	 float watts;
	 float energy;
	 Timer *eventTimer;

 public:
	HydrogenCellLogger(HardwareSerial port, Timer *event);
	void init();
	float getAmps();
	float getVolts();
	float getWatts();
	float getEnergy();
	void writeToSD();
	
};

static void logData();
#endif

