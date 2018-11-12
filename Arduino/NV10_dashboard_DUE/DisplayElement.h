#pragma once

#include <Arduino.h>
#include "ILI9488.h"
#include "FrameFormats.h"
extern "C" void __cxa_pure_virtual(void);
typedef enum {
	alignLeft,
	alignCenter,
	alignRight,
	alignTop = alignLeft,
	alignBtm = alignRight
}Alignment;

class DisplayElement
{
protected:
	ILI9488* screen;
	DataSource targetSource;
	DataSource* trackedSource;
	float* trackedValue;
	int16_t xPos, yPos;
	uint16_t width, height;
	uint16_t margin = 8;
	uint8_t borderStroke = 3;
	uint16_t borderFill = ILI9488_WHITE;
	uint16_t foreground = ILI9488_WHITE;
	uint16_t background = ILI9488_BLACK;
	bool stuck = false;
	void wipe();
	void drawBorder(uint8_t stroke, uint16_t color);
	virtual void refreshSettings() = 0;
	virtual void draw() = 0;
public:
	DisplayElement(ILI9488 * screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, Alignment xAlign, Alignment yAlign);
	~DisplayElement();
	void init(DataSource listeningTo, DataSource* varSource, float* value);
	virtual void setMargin(uint8_t margin);
	virtual void setColors(uint16_t foreground, uint16_t background);
	virtual void updateFloat(float value) = 0;
	virtual void update();
	virtual void updateNull() = 0;

};

