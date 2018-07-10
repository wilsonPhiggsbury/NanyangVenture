// DisplayInterface.h

#ifndef _DISPLAYINTERFACE_h
#define _DISPLAYINTERFACE_h

#include "Wiring.h"

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
class DisplayTFT
{
private:
	TFT_ILI9163C& screen;

public:
	DisplayTFT(TFT_ILI9163C& screenPtr);
	void printData(QueueItem& received);
};

#endif

char * parseFuelCellData(char * dest, char * source);
