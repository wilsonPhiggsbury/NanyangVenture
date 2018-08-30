#pragma once
#include "DisplayElement.h"
class DisplayText :
	public DisplayElement
{
public:
	DisplayText();
	void init(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
	void draw();
	void update(char* value);
	~DisplayText();

private:
	uint8_t textSize;
	uint16_t cursorX, cursorY;
	String text;
};

