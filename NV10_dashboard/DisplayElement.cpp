#include "DisplayElement.h"

DisplayElement::DisplayElement(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height):screen(screen),xPos(xPos),yPos(yPos),width(width),height(height)
{
	margin = 12;
	borderStroke = 3;
	borderFill = ILI9488_WHITE;
	for (uint8_t pixels = 0; pixels < borderStroke; pixels++)
		screen->drawRect(xPos + pixels, yPos + pixels, width - 2 * pixels, height - 2 * pixels, borderFill);

	foreground = ILI9488_WHITE;
	background = ILI9488_BLACK;
}

DisplayElement::~DisplayElement()
{
	
}
void DisplayElement::drawBorder(uint8_t stroke, uint16_t fill)
{
	for (uint8_t pixels = 0; pixels < borderStroke; pixels++)
		screen->drawRect(xPos + pixels, yPos + pixels, width - 2 * pixels, height - 2 * pixels, ILI9488_BLACK);

	borderStroke = stroke;
	borderFill = fill;

	for (uint8_t pixels = 0; pixels < borderStroke; pixels++)
		screen->drawRect(xPos + pixels, yPos + pixels, width - 2 * pixels, height - 2 * pixels, borderFill);
}
void DisplayElement::setMargin(uint8_t margin)
{
	this->margin = margin;
	refreshSettings();
}
void DisplayElement::setColors(uint16_t foreground, uint16_t background)
{
	bool bgChanged = this->background != background;
	this->foreground = foreground;
	this->background = background;
	refreshSettings();
	if (bgChanged)
	{
		wipe();
		drawBorder(borderStroke, borderFill);
	}
}
void DisplayElement::wipe()
{
	screen->fillRect(xPos, yPos, width, height, background);
}