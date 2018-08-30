#pragma once
#include "DisplayElement.h"
class DisplayGauge :
	public DisplayElement
{
public:
	DisplayGauge(uint16_t minVal, uint16_t maxVal);
	~DisplayGauge();
	void draw();
	bool update(uint16_t value);
private:
	uint16_t minValue, maxValue;
};

