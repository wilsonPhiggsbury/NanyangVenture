#include "DashboardScreenManager.h"
#include "DisplayBar.h"
#include "DisplayText.h"
#include "DisplayGauge.h"
#include "Wiring_Dashboard.h"

static const int screenWidth = 480;
static const int screenHeight = 320;
DashboardScreenManager::DashboardScreenManager(QueueItem* queueItem):q(queueItem)
{
	DataSource* trackIDaddr = &(queueItem->ID);
	// initialize screens
	ILI9488* leftScreen = new ILI9488(LEFT_LCD_CS, LCD_DC, LCD_RST);
	leftScreen->begin();
	leftScreen->setRotation(3);
	leftScreen->fillScreen(ILI9488_BLACK);
	leftScreen->setTextColor(ILI9488_WHITE);
	ILI9488* centerScreen = new ILI9488(CENTER_LCD_CS, LCD_DC);
	centerScreen->begin();
	centerScreen->setRotation(1);
	centerScreen->fillScreen(ILI9488_BLACK);
	centerScreen->setTextColor(ILI9488_WHITE);
	ILI9488* rightScreen = new ILI9488(RIGHT_LCD_CS, LCD_DC);
	rightScreen->begin();
	rightScreen->setRotation(3);
	rightScreen->fillScreen(ILI9488_BLACK);
	rightScreen->setTextColor(ILI9488_WHITE);
	// ---------------------------- LEFT SCREEN -----------------------------
	// initialize widgets
	DisplayText* capOutAmp_txt = new DisplayText(leftScreen, 0, 0, 90, 75, alignLeft, alignTop);
	DisplayText* capInAmp_txt = new DisplayText(leftScreen, screenWidth, 0, 90, 75, alignRight, alignTop);
	DisplayText* motorAmp_txt = new DisplayText(leftScreen, 0, screenHeight, 90, 75, alignLeft, alignBtm);
	DisplayText* motorVolt_txt = new DisplayText(leftScreen, screenWidth, screenHeight, 90, 75, alignRight, alignBtm);
	DisplayBar* capOutAmp_bar = new DisplayBar(leftScreen, 480/2-10, 100, 220, 35, DisplayBar::RIGHT_TO_LEFT);
	DisplayBar* capInAmp_bar = new DisplayBar(leftScreen, 480/2+10, 100, 220, 35, DisplayBar::LEFT_TO_RIGHT);
	DisplayBar* motorAmp_bar = new DisplayBar(leftScreen, 480/2-10, 180, 220, 70, DisplayBar::RIGHT_TO_LEFT);
	DisplayBar* motorVolt_bar = new DisplayBar(leftScreen, 480/2+10, 180, 220, 70, DisplayBar::LEFT_TO_RIGHT);
	// listen to data on a pointer
	capInAmp_txt->init(CS, trackIDaddr, &(queueItem->data[0][1]));
	capInAmp_bar->init(CS, trackIDaddr, &(queueItem->data[0][1]));
	capOutAmp_txt->init(CS, trackIDaddr, &(queueItem->data[1][1]));
	capOutAmp_bar->init(CS, trackIDaddr, &(queueItem->data[1][1]));
	motorVolt_txt->init(CS, trackIDaddr, &(queueItem->data[2][0]));
	motorVolt_bar->init(CS, trackIDaddr, &(queueItem->data[2][0]));
	motorAmp_txt->init(CS, trackIDaddr, &(queueItem->data[2][1]));
	motorAmp_bar->init(CS, trackIDaddr, &(queueItem->data[2][1]));
	// customize each widget
	capOutAmp_txt->setMargin(0);
	capInAmp_txt->setMargin(0);
	motorAmp_txt->setMargin(0);
	motorVolt_txt->setMargin(2);
	capOutAmp_bar->setRange(0, 10);
	capInAmp_bar->setRange(0, 10);
	motorAmp_bar->setRange(0, 40);
	motorVolt_bar->setRange(45, 60);
	capOutAmp_bar->setColors(ILI9488_RED, ILI9488_MAROON);
	capInAmp_bar->setColors(ILI9488_DARKGREY, ILI9488_DARKGREY);
	motorAmp_bar->setColors(ILI9488_RED, ILI9488_MAROON);
	motorVolt_bar->setColors(ILI9488_CYAN, ILI9488_DARKCYAN);

	// ---------------------------- CENTER SCREEN -----------------------------
	// initialize widgets
	DisplayText* speedDisplay = new DisplayText(centerScreen, screenWidth/2, screenHeight/2, 200, 200, alignCenter, alignCenter);
	// listen to data on a pointer
	speedDisplay->init(SM, trackIDaddr, &(queueItem->data[0][0]));
	// customize each widget
	speedDisplay->setColors(ILI9488_WHITE, ILI9488_DARKGREEN);

	// ---------------------------- RIGHT SCREEN -----------------------------
	// initialize widgets
	DisplayText* stackTemperature_txt = new DisplayText(rightScreen, 0, 0, 140, 80, alignLeft, alignTop);
	DisplayText* status_txt = new DisplayText(rightScreen, screenWidth / 2, 0, 200, 80, alignCenter, alignTop);
	DisplayText* pressure_txt = new DisplayText(rightScreen, screenWidth, 0, 140, 80, alignRight, alignTop);
	DisplayText* energy_txt = new DisplayText(rightScreen, screenWidth / 2, screenHeight / 2 - 40 + 20, 400, 80, alignCenter, alignCenter);
	DisplayBar* energy_bar = new DisplayBar(rightScreen, screenWidth / 2, screenHeight / 2 + 40 + 20, 400, 80, alignCenter, alignCenter);
	// listen to data on a pointer
	stackTemperature_txt->init(FC, trackIDaddr, &(queueItem->data[0][4]));
	status_txt->init(FC, trackIDaddr, &(queueItem->data[0][7]));
	pressure_txt->init(FC, trackIDaddr, &(queueItem->data[0][5]));
	energy_txt->init(FC, trackIDaddr, &(queueItem->data[0][3]));
	energy_bar->init(FC, trackIDaddr, &(queueItem->data[0][3]));
	energy_bar->setRange(0, 100);
	// customize each widget's skin
	stackTemperature_txt->setColors(ILI9488_WHITE, ILI9488_BLUE);
	status_txt->setColors(ILI9488_WHITE, ILI9488_BLUE);
	pressure_txt->setColors(ILI9488_WHITE, ILI9488_BLUE);
	energy_txt->setColors(ILI9488_WHITE, ILI9488_BLUE);
	energy_bar->setColors(ILI9488_YELLOW, ILI9488_BLACK);

	// ---------------------------- tie up references for updating later -----------------------------
	allWidgets[0] = capInAmp_txt;
	allWidgets[1] = capInAmp_bar;
	allWidgets[2] = capOutAmp_txt;
	allWidgets[3] = capOutAmp_bar;
	allWidgets[4] = motorAmp_txt;
	allWidgets[5] = motorAmp_bar;
	allWidgets[6] = motorVolt_txt;
	allWidgets[7] = motorVolt_bar;

	allWidgets[8] = speedDisplay;

	allWidgets[9] = stackTemperature_txt;
	allWidgets[10] = pressure_txt;
	allWidgets[11] = status_txt;
	allWidgets[12] = energy_txt;
	allWidgets[13] = energy_bar;


}
void DashboardScreenManager::refreshScreens()
{
	for (int i = 0; i < 14; i++)
	{
		DisplayText* box;
		if (q->ID == FC)
		{
			switch (i)
			{
			case 9: // FC temp should be < 60
				box = (DisplayText*)allWidgets[i];
				if (q->data[0][4] >= 60)
					box->setColors(ILI9488_WHITE, ILI9488_RED);
				else
					box->setColors(ILI9488_WHITE, ILI9488_BLUE);
				allWidgets[i]->update();
				break;
			case 11: // FC status should be = 1.0 (only two possible values)
				box = (DisplayText*)allWidgets[i];
				if (q->data[0][7] == 1.0)
				{
					box->setColors(ILI9488_GREEN, ILI9488_DARKGREEN);
					box->update("ON");
				}
				else
				{
					box->setColors(ILI9488_RED, ILI9488_MAROON);
					box->update("OFF");
				}
				break;
			case 10: // FC pressure should be > 0.5
				box = (DisplayText*)allWidgets[i];
				if (q->data[0][5] <= 0.5)
					box->setColors(ILI9488_WHITE, ILI9488_RED);
				else
					box->setColors(ILI9488_WHITE, ILI9488_BLUE);
				allWidgets[i]->update();
				break;
			default:
				allWidgets[i]->update();
				break;
			}
		}
		else if(q->ID == CS)
		{
			switch (i)
			{
			case 4: // motor amp should be < 25 
				box = (DisplayText*)allWidgets[i];
				if (q->data[2][1] >= 25)
					box->setColors(ILI9488_WHITE, ILI9488_RED);
				else
					box->setColors(ILI9488_WHITE, ILI9488_BLACK);
				break;
			case 6: // motor volt should be > 45 
				box = (DisplayText*)allWidgets[i];
				if (q->data[2][0] <= 45)
					box->setColors(ILI9488_WHITE, ILI9488_RED);
				else
					box->setColors(ILI9488_WHITE, ILI9488_BLACK);
				break;
			}
			allWidgets[i]->update();
		}
		else
		{
			allWidgets[i]->update();
		}
	}
}
void DashboardScreenManager::refreshScreens(int status)
{
	if (status == 0)
	{
		DisplayText* speedDisplay = (DisplayText*)allWidgets[8];
		speedDisplay->update("---");
	}
}

DashboardScreenManager::~DashboardScreenManager()
{
}
