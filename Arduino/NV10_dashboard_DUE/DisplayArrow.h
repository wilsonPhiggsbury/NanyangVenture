#pragma once
#include "DisplayElement.h"
class DisplayArrow :
	public DisplayElement
{
public:
	typedef enum _orientation{
		left,
		right
	}Orientation;
	DisplayArrow(ILI9488 * screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, Orientation o);
	~DisplayArrow();
	void updateFloat(float value);
	void updateNull();
private:
	Orientation orientation;
	void refreshSettings();
	void draw();
};

