#pragma once
#include "DisplayElement.h"
class DisplayGauge :
	public DisplayElement
{
public:
	DisplayGauge(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
	void update(char * value);
	~DisplayGauge();
	void draw();
	bool update(uint16_t value);
	void setRange(uint16_t minVal, uint16_t maxVal);
private:
	uint16_t minVal, maxVal;
	uint16_t radInner = 10, radOuter = 20;

	void refreshSettings();
	float prevValue = 0;
	float thisValue = 0;
};

