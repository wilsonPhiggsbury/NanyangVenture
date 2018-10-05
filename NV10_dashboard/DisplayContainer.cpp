#include "DisplayContainer.h"



DisplayContainer::DisplayContainer(ILI9488* parentScreen, int x, int y):screen(parentScreen),xPos(x),yPos(y)
{

}

void DisplayContainer::addDisplayElement(DisplayElement * child)
{
	if (firstChild == NULL)
		firstChild = child;
	else
	{
		DisplayElement* thisChild = firstChild;
		while (thisChild->next != NULL)
			thisChild = thisChild->next;
		thisChild->next = child;
	}
}

void DisplayContainer::draw()
{
	DisplayElement* thisChild = firstChild;
	while (thisChild != NULL)
	{
		//thisChild->update("RANDOMSTUFF");
		thisChild = thisChild->next;
	}
}