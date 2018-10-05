#pragma once
#include "FrameFormats.h"
#include "DisplayElement.h"

#define MAXDISPLAYGROUPS 2
#define NUMSCREENS 3
typedef enum {
	lScreen,
	cScreen,
	rScreen
}ScreenIndex;
class DashboardScreenManager
{
private:
	ILI9488* screens[NUMSCREENS];
	DisplayElement* screenContents[NUMSCREENS][MAXDISPLAYGROUPS]; // each array element points to a linked list of screencontents

public:
	DashboardScreenManager();
	void refreshScreens(QueueItem& data); // multiplex the data to respective screens
	void refreshScreens(); // no data from CAN bus! indicate in the screens
	~DashboardScreenManager();
};
