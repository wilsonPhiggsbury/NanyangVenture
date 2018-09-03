#pragma once
#include "DisplayElement.h"
class DisplayText :
	public DisplayElement
{
public:
	DisplayText(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
	void update(char* value);
	void setTextLength(uint8_t l);
	~DisplayText();

private:
	uint8_t textSize;
	uint16_t cursorX, cursorY;
	String text;
	uint8_t textCols, textRows;
	void draw();
	void refreshSettings();
};

