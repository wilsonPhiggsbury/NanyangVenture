#pragma once
#include <ILI9488.h>
#include "DisplayElement.h"
class DisplayContainer
{
public:
	DisplayContainer(ILI9488* parentScreen, int x, int y);
	void addDisplayElement(DisplayElement * child);
	void draw();
private:
	int xPos, yPos;
	ILI9488* screen;
	DisplayElement* firstChild;
	

};

