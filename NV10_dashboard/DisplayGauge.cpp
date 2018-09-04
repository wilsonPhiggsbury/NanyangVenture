#include "DisplayGauge.h"



DisplayGauge::DisplayGauge(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height):DisplayElement(screen, xPos, yPos, width, height)
{
}

DisplayGauge::~DisplayGauge()
{
}
void DisplayGauge::setRange(uint16_t minVal, uint16_t maxVal)
{
	minValue = minVal;
	maxValue = maxVal;
}
void DisplayGauge::draw()
{

}

bool DisplayGauge::update(uint16_t value)
{
	
}
