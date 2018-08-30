#include "DisplayGauge.h"



DisplayGauge::DisplayGauge(uint16_t minVal, uint16_t maxVal)
{
	minValue = minVal;
	maxValue = maxVal;
}


DisplayGauge::~DisplayGauge()
{
}
void DisplayGauge::draw()
{
}

bool DisplayGauge::update(uint16_t value)
{
	if (!inRange(value))return false;
}
