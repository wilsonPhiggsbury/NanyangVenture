#pragma once

#include <Arduino.h>>
#include "ILI9488.h"
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
	uint16_t xPos, yPos, width, height;
	uint16_t margin = 12;
	uint8_t borderStroke = 3;
	uint16_t borderFill = ILI9488_WHITE;
	uint16_t foreground = ILI9488_WHITE;
	uint16_t background = ILI9488_BLACK;
	void wipe();
	void drawBorder(uint8_t stroke, uint16_t color);
	virtual void refreshSettings() = 0;
	virtual void draw() = 0;
public:
	DisplayElement();
	DisplayElement(ILI9488 * screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, Alignment xAlign, Alignment yAlign, DisplayElement * next);
	~DisplayElement();
	void init();
	virtual void setMargin(uint8_t margin);
	virtual void setColors(uint16_t foreground, uint16_t background);
	virtual void update(float value) = 0;

	DisplayElement* next = NULL;

};

