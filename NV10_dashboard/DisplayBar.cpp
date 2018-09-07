#include "DisplayBar.h"



DisplayBar::DisplayBar(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height):DisplayElement(screen, xPos, yPos, width, height)
{
	
}
void DisplayBar::update(char* value)
{
	prevValue = thisValue;
	thisValue = atof(value);
	draw();
}
void DisplayBar::draw()
{
	//Serial.print("thisVal: ");
	//Serial.print(thisValue);
	// For LEFT_TO_RIGHT facing bars, value incremented
	//Serial.print("X: ");
	//Serial.print(xPos + margin + floor(toWidthScale(prevValue)));
	//Serial.print("Y: ");
	//Serial.print(yPos + margin);
	//Serial.print("WIDTH: ");
	//Serial.print(ceil(toWidthScale(thisValue - prevValue)));
	//Serial.print("HEIGHT: ");
	//Serial.print(actualHeight);
	//Serial.println();
	if (facing == -1)
	{
		if (thisValue > prevValue)
		{
			// bigger: draw foreground color, from old value to new value
			screen->fillRect(xPos + width - margin - ceil(toWidthScale(thisValue)), yPos + margin, \
				ceil(toWidthScale(thisValue - prevValue)), actualHeight, \
				foreground);
		}
		else
		{
			// smaller: draw background color, from new value to old value
			screen->fillRect(xPos + width - margin - ceil(toWidthScale(prevValue)), yPos + margin, \
				ceil(toWidthScale(prevValue - thisValue)), actualHeight, \
				background);
		}
	}
	else if (facing == 1)
	{
		if (thisValue > prevValue)
		{
			// bigger: draw foreground color, from old value to new value
			screen->fillRect(xPos + margin + floor(toWidthScale(prevValue)), yPos + margin, \
				ceil(toWidthScale(thisValue-prevValue)), actualHeight, \
				foreground);
		}
		else
		{
			// smaller: draw background color, from new value to old value
			screen->fillRect(xPos + margin + floor(toWidthScale(thisValue)), yPos + margin,\
				ceil(toWidthScale(prevValue - thisValue)), actualHeight, \
				background);
		}

	}
}

DisplayBar::~DisplayBar()
{
}
void DisplayBar::setRange(uint16_t minVal, uint16_t maxVal)
{
	this->minVal = minVal;
	this->maxVal = maxVal;
}
void DisplayBar::setOrientation(Orientation o)
{
	switch (o)
	{
	case LEFT_TO_RIGHT:
		facing = 1;
		break;
	case RIGHT_TO_LEFT:
		facing = -1;
		break;
	}
}
void DisplayBar::refreshSettings()
{
	actualWidth = width - 2 * margin;
	actualHeight = height - 2 * margin;
	if (facing == -1)
	{
		screen->fillRect(xPos + width - margin - ceil(toWidthScale(thisValue)), yPos + margin, \
			ceil(toWidthScale(thisValue)), actualHeight, foreground);
	}
	else if (facing == 1)
	{
		screen->fillRect(xPos + margin, yPos + margin, \
			ceil(toWidthScale(thisValue)), actualHeight, foreground);

	}
}
float DisplayBar::toWidthScale(float value)
{
	return value / maxVal * actualWidth;
}