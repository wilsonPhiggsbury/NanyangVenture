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
	Packet* q;
	
public:
	DashboardScreenManager(Packet* Packet);
	void refreshScreens(void* null); // multiplex the data to respective screens
	void refreshScreens(); // no data from CAN bus! indicate in the screens
	~DashboardScreenManager();
};
