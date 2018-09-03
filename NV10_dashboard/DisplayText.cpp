#include "DisplayText.h"



DisplayText::DisplayText()
{
	
}
void DisplayText::init(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height)
{
	margin = 12;
	DisplayElement::init(screen, xPos, yPos, width, height);
	Serial.println("Initializing text element...");
	// a char of text size 1 is 6px * 8px (H * V), which is ratio 3 / 4
	if (width > height * 3 / 4)
	{
		// take text size using height, 1/3 margin included 
		textSize = (height - margin) / 8;
		Serial.println("Height < Width, take height");
	}
	else
	{
		// take text size using width, 1/3 margin included 
		textSize = (width - margin) / 6;
		Serial.println("Width < Height, take width.");
	}
	Serial.print("Text size: ");
	Serial.println(textSize);

	cursorX = xPos + width / 2 - textSize * 6 / 2;
	cursorY = yPos + height / 2 - textSize * 8 / 2;

	screen->setTextColor(ILI9488_WHITE, ILI9488_BLACK); // set white as font color, black as font BG so that BG can wipe out the old text
	screen->setTextSize(textSize);
}
void DisplayText::draw()
{
	screen->setCursor(cursorX - (text.length()-1)*textSize*6/2, cursorY);
	screen->print(text);
	//screen->print(textSize);
	//screen->print(" ");
	//screen->print(cursorX);
	//screen->print(" ");
	//screen->print(cursorY);
	//screen->print(" ");
}
void DisplayText::update(char* value)
{
	text = value;
}
DisplayText::~DisplayText()
{
}
