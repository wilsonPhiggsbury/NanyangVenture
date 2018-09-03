#pragma once

#include <Arduino.h>>
#include "ILI9488.h"""
class DisplayElement
{
public:
	DisplayElement();
	DisplayElement(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
	~DisplayElement();
	void init(ILI9488 * screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
	bool inRange(uint16_t value);
	virtual void update(char* value) = 0;
	virtual void draw() = 0;

	DisplayElement* next;
protected:
	ILI9488* screen;
	uint16_t xPos, yPos;
	uint16_t margin;
	uint16_t width, height;


};

