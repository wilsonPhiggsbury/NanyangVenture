#pragma once
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
	DisplayElement* allWidgets[14]; // each array element points to a linked list of screencontents
	QueueItem* q;
	
public:
	DashboardScreenManager(QueueItem* queueItem);
	void refreshScreens(int status); // multiplex the data to respective screens
	void refreshScreens();
	~DashboardScreenManager();
};
