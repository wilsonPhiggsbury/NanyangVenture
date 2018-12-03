#include "DisplayElement.h"
void __cxa_pure_virtual(void) {};
DisplayElement::DisplayElement(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, Alignment xAlign, Alignment yAlign) :screen(screen), xPos(xPos), yPos(yPos), width(width), height(height)
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
void DisplayElement::init(PacketID targetID, PacketID* varID, float* value)
{
	drawBorder(borderStroke, borderFill);
	this->targetID = targetID;
	trackedID = varID;
	trackedValue = value;

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
void DisplayElement::update()
{
	if (*trackedID == targetID)
	{
		updateFloat(*trackedValue);
	}
}
void DisplayElement::wipe()
{
	screen->fillRect(xPos, yPos, width, height, background);
}

// th
/*// .h
extern "C" void __cxa_pure_virtual(void);
__extension__ typedef int __guard __attribute__((mode(__DI__)));

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release(__guard *);
extern "C" void __cxa_guard_abort(__guard *);
// .cpp
void __cxa_pure_virtual(void) {};
int __cxa_guard_acquire(__guard *g) { return !*(char *)(g); };
void __cxa_guard_release(__guard *g) { *(char *)g = 1; };
void __cxa_guard_abort(__guard *) {};*/