#include "DisplayArrow.h"


void DisplayArrow::refreshSettings()
{
	wipe();
}

void DisplayArrow::draw()
{
	const uint8_t triangleHeight = height, triangleWidth = width/3, rectangleHeight = height/3*2, rectangleWidth = width/3*2;
	uint16_t y = yPos + height / 2;
	switch (orientation)
	{
	case left:
		screen->fillTriangle(xPos, y, xPos + triangleWidth, y - (triangleHeight / 2), xPos + triangleWidth, y + (triangleHeight / 2), foreground);
		screen->fillRect(xPos + triangleWidth, y - rectangleHeight / 2, rectangleWidth, rectangleHeight, foreground);
		break;
	case right:
		screen->fillRect(xPos, y - rectangleHeight / 2, rectangleWidth, rectangleHeight, foreground);
		screen->fillTriangle(xPos + rectangleWidth + triangleWidth, y, xPos + rectangleWidth, y - (triangleHeight / 2), xPos + rectangleWidth, y + (triangleHeight / 2), foreground);
		break;
	}
}

DisplayArrow::DisplayArrow(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, Orientation o) :DisplayElement(screen, xPos, yPos, width, height, alignLeft, alignCenter)
{
	orientation = o;
}


DisplayArrow::~DisplayArrow()
{
}

void DisplayArrow::updateFloat(float value)
{
	if (value == STATE_EN)
	{
		draw();
	}
	else
	{
		refreshSettings();
	}
}

void DisplayArrow::updateNull()
{
	uint16_t f = foreground, b = background;
	setColors(ILI9488_RED, ILI9488_BLACK);
	draw();
	setColors(f, b);
}
