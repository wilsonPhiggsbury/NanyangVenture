#include "DisplayText.h"

const int textWidthPerSize = 6;
const int textHeightPerSize = 8;

DisplayText::DisplayText(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height):DisplayElement(screen, xPos, yPos, width, height)
{
	// use superclass constructor to assign all basic values
	// fill these with rubbish before calling refreshSettings(), which acts as the real initializing function
	refreshSettings();
}
void DisplayText::draw()
{
	screen->setTextSize(textSize);
	screen->setCursor(cursorX, cursorY);
	screen->setTextColor(foreground, background); // set BG color so that BG can wipe out the old text
	screen->println(text);
}
void DisplayText::update(char* value)
{
	text = value;
	textWidth = strlen(text);
	if (prevTextWidth != textWidth)
	{
		refreshSettings();
	}
	prevTextWidth = textWidth;
	draw();
}
//void DisplayText::setTextLength(uint8_t length)
//{
//	textWidth = length;
//	refreshSettings();
//}
void DisplayText::refreshSettings()
{
	// clear text box
	screen->setTextColor(foreground, background);
	screen->setTextSize(textSize);
	screen->setCursor(cursorX, cursorY);
	for (int i = 0; i < prevTextWidth; i++)
		screen->print(" ");

	// a char of text size 1 is 6px * 8px (H * V), which is ratio 3 / 4
	if (width > height * 3 / 4)
	{
		// take text size using height, margin excluded 
		textSize = (height - margin) / textHeightPerSize / textWidth / textHeight;
		//Serial.println("Height < Width, take height");
	}
	else
	{
		// take text size using width, margin excluded 
		textSize = (width - margin) / textWidthPerSize / textWidth / textHeight;
		//Serial.println("Width < Height, take width.");
	}
	//Serial.print("Text size: ");
	//Serial.println(textSize);

	cursorX = xPos + width / 2 - textWidth * textSize * textWidthPerSize / 2;
	cursorY = yPos + height / 2 - textHeight * textSize * textHeightPerSize / 2;
}

DisplayText::~DisplayText()
{
}
