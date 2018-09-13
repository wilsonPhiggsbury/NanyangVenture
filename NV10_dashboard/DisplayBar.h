#pragma once
#include "DisplayElement.h"
#include "ILI9488.h"


class DisplayBar :
	public DisplayElement
{
public:
	typedef enum {
		LEFT_TO_RIGHT,RIGHT_TO_LEFT,TOP_TO_BOTTOM,BOTTOM_TO_TOP
	}Orientation;
	DisplayBar(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height);
	void update(char * value);
	void draw();
	void setRange(uint16_t minVal, uint16_t maxVal);
	void setOrientation(Orientation o);
	~DisplayBar();
private:
	uint16_t minVal = 0;
	uint16_t maxVal = 100;
	uint16_t actualWidth, actualHeight;
	float prevValue = 0;
	float thisValue = 0;
	int8_t facing = 1;
	void refreshSettings();
	float toWidthScale(float value);
};

