#include "DisplayDrawing.h"



DisplayDrawing::DisplayDrawing(ILI9488* screen, uint16_t xPos, uint16_t yPos, uint16_t width, uint16_t height, Alignment xAlign, Alignment yAlign) :DisplayElement(screen, xPos, yPos, width, height, xAlign, yAlign)
{
}


DisplayDrawing::~DisplayDrawing()
{
}
