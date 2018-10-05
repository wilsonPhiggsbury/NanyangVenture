#include "DashboardScreenManager.h"
#include "DisplayBar.h"
#include "DisplayText.h"
#include "DisplayGauge.h"
#include "Wiring_Dashboard.h"

static const int screenWidth = 480;
static const int screenHeight = 320;
DashboardScreenManager::DashboardScreenManager()
{
	// initialize screens
	ILI9488 leftScreen = ILI9488(LEFT_LCD_CS, LEFT_LCD_DC, LEFT_LCD_RST);
	leftScreen.begin();
	leftScreen.setRotation(1);
	leftScreen.fillScreen(ILI9488_BLACK);
	leftScreen.setTextColor(ILI9488_WHITE);
	ILI9488 centerScreen = ILI9488(CENTER_LCD_CS, CENTER_LCD_DC, CENTER_LCD_RST);
	centerScreen.begin();
	centerScreen.setRotation(1);
	centerScreen.fillScreen(ILI9488_BLACK);
	centerScreen.setTextColor(ILI9488_WHITE);
	ILI9488 rightScreen = ILI9488(RIGHT_LCD_CS, RIGHT_LCD_DC, RIGHT_LCD_RST);
	rightScreen.begin();
	rightScreen.setRotation(1);
	rightScreen.fillScreen(ILI9488_BLACK);
	rightScreen.setTextColor(ILI9488_WHITE);
	// tie up references for future use
	screens[lScreen] = &leftScreen;
	screens[cScreen] = &centerScreen;
	screens[rScreen] = &rightScreen;

	// initialize widgets
	DisplayText capOutAmp_txt = DisplayText(&leftScreen, 0, 0, 90, 30, alignLeft, alignTop, NULL);
	DisplayText capInAmp_txt = DisplayText(&leftScreen, 0, 0, 90, 30, alignRight, alignTop, &capOutAmp_txt);
	DisplayText motorAmp_txt = DisplayText(&leftScreen, 0, 0, 90, 30, alignLeft, alignBtm, &capInAmp_txt);
	DisplayText motorVolt_txt = DisplayText(&leftScreen, 0, 0, 90, 30, alignRight, alignBtm, &motorAmp_txt);
	DisplayBar capOutAmp_bar = DisplayBar(&leftScreen, 480/2-10, 120, 220, 35, DisplayBar::RIGHT_TO_LEFT, NULL);
	DisplayBar capInAmp_bar = DisplayBar(&leftScreen, 480/2+10, 120, 220, 35, DisplayBar::LEFT_TO_RIGHT, &capOutAmp_bar);
	DisplayBar motorAmp_bar = DisplayBar(&leftScreen, 480/2-10, 200, 220, 35, DisplayBar::RIGHT_TO_LEFT, &capInAmp_bar);
	DisplayBar motorVolt_bar = DisplayBar(&leftScreen, 480/2+10, 200, 220, 35, DisplayBar::LEFT_TO_RIGHT, &motorAmp_bar);
	capOutAmp_bar.setRange(0, 20);
	capInAmp_bar.setRange(0, 20);
	motorAmp_bar.setRange(0, 20);
	motorVolt_bar.setRange(40, 60);
	// tie up references for future use
	screenContents[lScreen][0] = &motorVolt_txt;
	screenContents[lScreen][1] = &motorVolt_bar;
	screenContents[lScreen][0]->init();
	screenContents[lScreen][1]->init();

	// initialize widgets
	DisplayText speedDisplay = DisplayText(&centerScreen, screenWidth/2, screenHeight/2, 200, 200, alignCenter, alignCenter, NULL);
	speedDisplay.setColors(ILI9488_WHITE, ILI9488_PURPLE);
	// tie up references for future use
	screenContents[cScreen][0] = &speedDisplay;
	screenContents[cScreen][0]->init();

	// initialize widgets
	DisplayText stackTemperature_txt = DisplayText(&rightScreen, 0, 0, 140, 50, alignLeft, alignTop, NULL);
	DisplayText pressure_txt = DisplayText(&rightScreen, screenWidth, 0, 140, 50, alignRight, alignTop, &stackTemperature_txt);
	DisplayText status_txt = DisplayText(&rightScreen, screenWidth / 2, 0, 200, 50, alignCenter, alignTop, &pressure_txt);
	DisplayText energy_txt = DisplayText(&rightScreen, screenWidth / 2, screenHeight / 2 - 25, 400, 50, alignCenter, alignCenter, &status_txt);
	DisplayBar energy_bar = DisplayBar(&rightScreen, screenWidth / 2, screenHeight / 2, 400, 50, DisplayBar::LEFT_TO_RIGHT, NULL);
	energy_bar.setRange(0, 100);
	// tie up references for future use
	screenContents[rScreen][0] = &energy_txt;
	screenContents[rScreen][1] = &energy_bar;


}
void DashboardScreenManager::refreshScreens(QueueItem& info)
{
	DisplayText* txt;
	DisplayBar* bar;
	switch (info.ID)
	{
	case FC:
		// FC data goes to right screen: temp, state, pressure, Wh
		bar = (DisplayBar*)screenContents[rScreen][1];
		bar->update(info.data[0][3]);
		txt = (DisplayText*)screenContents[rScreen][0];	// energy
		txt->update(info.data[0][3]);
		txt = (DisplayText*)txt->next;	// status
		txt->update(info.data[0][7]);
		txt = (DisplayText*)txt->next;	// pressure
		txt->update(info.data[0][5]);
		txt = (DisplayText*)txt->next;	// temperature
		txt->update(info.data[0][4]);
		break;
	case CS:
		/* CS data goes to left screen: CapOut  >  CapIn 
											^		v
										Motor	<  Volt 
		*/
		txt = (DisplayText*)screenContents[lScreen][0];
		txt->update(info.data[2][0]);	// motor volt
		txt = (DisplayText*)txt->next;
		txt->update(info.data[2][1]);	// motor amp
		txt = (DisplayText*)txt->next;
		txt->update(info.data[0][1]);	// capIn amp
		txt = (DisplayText*)txt->next;
		txt->update(info.data[1][1]);	// capOut amp

		bar = (DisplayBar*)screenContents[lScreen][1];
		bar->update(info.data[2][0]);	// motor volt
		bar = (DisplayBar*)bar->next;
		bar->update(info.data[2][1]);	// motor amp
		bar = (DisplayBar*)bar->next;
		bar->update(info.data[0][1]);	// capIn amp
		bar = (DisplayBar*)bar->next;
		bar->update(info.data[1][1]);	// capOut amp

		break;
	case SM:
		// SM data goes to center screen
		txt = (DisplayText*)screenContents[cScreen][0];
		txt->update(info.data[0][0]);
		break;
	}
}
void DashboardScreenManager::refreshScreens()
{
	//speedDisplay->update("---");
}

DashboardScreenManager::~DashboardScreenManager()
{
}
