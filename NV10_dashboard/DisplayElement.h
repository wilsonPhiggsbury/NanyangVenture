#pragma once

#include <Arduino.h>>
#include "ILI9488.h"""
class DisplayElement
{
public:
	DisplayElement();
	DisplayElement(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
	~DisplayElement();
	bool inRange(uint16_t value);
	virtual void setMargin(uint8_t margin);
	virtual void setColors(uint16_t foreground, uint16_t background);
	virtual void update(char* value) = 0;
	virtual void refreshSettings() = 0;

	DisplayElement* next;
protected:
	ILI9488* screen;
	uint16_t xPos, yPos;
	uint16_t margin;
	uint16_t width, height;
	uint8_t borderStroke;
	uint16_t borderFill;
	uint16_t foreground, background;
	void wipe();
	void drawBorder(uint8_t stroke, uint16_t color);
	virtual void draw() = 0;

};

