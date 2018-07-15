// DisplayInterface.h

#ifndef _DISPLAYINTERFACE_h
#define _DISPLAYINTERFACE_h
#endif

#include "Behaviour.h"

#include <Adafruit_GFX.h>
#include <LiquidCrystal_I2C.h>
#include <TFT_ILI9163C.h>

static const char* header[] = { "FC", "MT" };
class DisplayLCD
{
private:
	LiquidCrystal_I2C& screen;

public:
	DisplayLCD(LiquidCrystal_I2C & screenPtr);
	void printData(QueueItem& received);

};
