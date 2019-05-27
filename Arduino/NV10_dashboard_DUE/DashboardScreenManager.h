#pragma once
#include "DisplayElement.h"
#include "DisplayArrow.h"

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
	DisplayElement* dataWidgets[15]; // each array element points to a linked list of screencontents
	Packet* q;
	DisplayArrow* lSigArrow;
	DisplayArrow* rSigArrow;
	
public:
	DashboardScreenManager(Packet* Packet);
	void refreshDataWidgets(void* null); // multiplex the data to respective screens
	void refreshAnimatedWidgets();
	void refreshDataWidgets(); // no data from CAN bus! indicate in the screens
	~DashboardScreenManager();
};
