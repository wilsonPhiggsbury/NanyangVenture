#include "DisplayGauge.h"



DisplayGauge::DisplayGauge(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height):DisplayElement(screen, xPos, yPos, width, height)
{
}
void DisplayGauge::update(char* value)
{
	prevValue = thisValue;
	thisValue = strtof(value, NULL);
	draw();
}
void DisplayGauge::draw()
{

}
DisplayGauge::~DisplayGauge()
{
}
void DisplayGauge::setRange(uint16_t minVal, uint16_t maxVal)
{
	this->minVal = minVal;
	this->maxVal = maxVal;
}

bool DisplayGauge::update(uint16_t value)
{
	
}
