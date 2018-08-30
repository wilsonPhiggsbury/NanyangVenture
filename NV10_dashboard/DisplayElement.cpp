#include "DisplayElement.h"



DisplayElement::DisplayElement()
{
}


DisplayElement::~DisplayElement()
{
}
void DisplayElement::init(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height)
{
	this->screen = screen;
	this->xPos = xPos;
	this->yPos = yPos;
	this->width = width;
	this->height = height;
	screen->begin();
	screen->fillScreen(ILI9488_BLACK);
	for(uint8_t pixels = 0; pixels < 5; pixels ++)
		screen->drawRect(xPos+pixels, yPos+pixels, width-2*pixels, height-2*pixels, ILI9488_WHITE);
}
bool DisplayElement::inRange(uint16_t value)
{
	return (value >= xPos && value < xPos+width && value >= yPos && value < yPos+height);
}