#include "DisplayElement.h"
void __cxa_pure_virtual(void) {};
DisplayElement::DisplayElement(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, Alignment xAlign, Alignment yAlign, DisplayElement* next) :screen(screen), xPos(xPos), yPos(yPos), width(width), height(height), next(next)
{
	switch (xAlign)
	{
	case alignCenter:
		this->xPos -= width / 2;
		break;
	case alignRight:
		this->xPos -= width;
		break;
	}
	switch (yAlign)
	{
	case alignCenter:
		this->yPos -= height / 2;
		break;
	case alignBtm:
		this->yPos -= height;
		break;
	}
	xPos = constrain(xPos, 0, 480);
	yPos = constrain(yPos, 0, 320);

}

DisplayElement::~DisplayElement()
{
	
}
void DisplayElement::init()
{
	drawBorder(borderStroke, borderFill);
	if (next != NULL)
		next->init();
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