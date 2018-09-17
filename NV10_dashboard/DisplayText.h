#pragma once
#include "DisplayElement.h"
class DisplayText :
	public DisplayElement
{
public:
	DisplayText(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
	void update(char* value);
	~DisplayText();

private:
	uint8_t textSize = 2;
	uint16_t cursorX = 0;
	uint16_t cursorY = 0;
	char* text;
	uint8_t textWidth = 1;
	uint8_t textHeight = 1;
	uint8_t prevTextWidth = 0;
	uint8_t prevTextRows = 0;
	void draw();
	void refreshSettings();
};

