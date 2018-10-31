#include "DisplayBar.h"



DisplayBar::DisplayBar(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, Orientation orientation) :\
DisplayElement(screen, xPos, yPos, width, height, orientation == LEFT_TO_RIGHT ? alignLeft : alignRight, alignCenter)
{
	setOrientation(orientation);
}
DisplayBar::DisplayBar(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, Alignment xAlign, Alignment yAlign) :\
DisplayElement(screen, xPos, yPos, width, height, xAlign, yAlign)
{
	setOrientation(LEFT_TO_RIGHT);
}
void DisplayBar::updateFloat(float value)
{
	prevValue = thisValue;
	thisValue = constrain(value, minVal, maxVal);
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
	float thisWidth = toWidthScale(thisValue);
	float prevWidth = toWidthScale(prevValue);
	float diff = abs(thisWidth - prevWidth);
	if (facing == -1)
	{
		if (thisValue > prevValue)
		{
			// bigger: draw foreground color, from old value to new value
			screen->fillRect(xPos + width - margin - ceil(thisWidth), yPos + margin, \
				ceil(diff), actualHeight, \
				foreground);
		}
		else if (prevValue > thisValue)
		{
			// smaller: draw background color, from new value to old value
			screen->fillRect(xPos + width - margin - ceil(prevWidth), yPos + margin, \
				ceil(diff), actualHeight, \
				background);
		}
	}
	else if (facing == 1)
	{
		if (thisValue > prevValue)
		{
			// bigger: draw foreground color, from old value to new value
			screen->fillRect(xPos + margin + floor(prevWidth), yPos + margin, \
				ceil(diff), actualHeight, \
				foreground);
		}
		else if (prevValue > thisValue)
		{
			// smaller: draw background color, from new value to old value
			screen->fillRect(xPos + margin + floor(thisWidth), yPos + margin,\
				ceil(diff), actualHeight, \
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
	// synchronize history to avoid jumps 
	thisValue = prevValue = minVal;
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
	return map(value, minVal, maxVal, 0, actualWidth);// (value - minVal) / (maxVal - minVal) * actualWidth + minVal;
}