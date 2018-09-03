#pragma once
#include "DisplayElement.h"
#include "ILI9488.h"
class DisplayBar :
	public DisplayElement
{
public:
	DisplayBar(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
	void draw();
	~DisplayBar();
private:

	void refreshSettings();
};

