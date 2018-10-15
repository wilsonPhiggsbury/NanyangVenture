#pragma once
#include "DisplayElement.h"
class DisplayText :
	public DisplayElement
{
public:
	DisplayText(ILI9488 * screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, Alignment xAlign, Alignment yAlign, DisplayElement * next);
	void update(float value, char * addOn);
	void update(float value);
	void update(char * value);
	~DisplayText();

private:
	uint8_t textSize = 2;
	int16_t cursorX = 0;
	int16_t cursorY = 0;
	char* text;
	uint8_t textWidth = 1;
	uint8_t textHeight = 1;
	uint8_t prevTextWidth = 0;
	uint8_t prevTextHeight = 0;
	void draw();
	void refreshSettings();
};

