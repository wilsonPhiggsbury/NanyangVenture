#pragma once
#include "DisplayElement.h"
class DisplayGauge :
	public DisplayElement
{
public:
	DisplayGauge(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
	~DisplayGauge();
	void setRange(uint16_t minVal, uint16_t maxVal);
	void draw();
	bool update(uint16_t value);
private:
	uint16_t minValue, maxValue;
	void refreshSettings();
};

