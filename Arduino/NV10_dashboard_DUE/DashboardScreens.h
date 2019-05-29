// DashboardScreens.h

#ifndef _DASHBOARDSCREENS_h
#define _DASHBOARDSCREENS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ILI9488.h>
#include "TextWidget.h"
#include "BarWidget.h"
#include "ArrowWidget.h"
#include "Pins_dashboard.h"

const uint16_t SCREENWIDTH = 480;
const uint16_t SCREENHEIGHT = 240;

ILI9488 centerScreen = ILI9488(LCDCENTER_SPI_CS, LCD_OUTPUT_DC, LCD_OUTPUT_RST);
ILI9488 leftScreen = ILI9488(LCDLEFT_SPI_CS, LCD_OUTPUT_DC);
ILI9488 rightScreen = ILI9488(LCDRIGHT_SPI_CS, LCD_OUTPUT_DC);

// center screen x offset: +25, -5
TextWidget speedTxt = TextWidget(&centerScreen, SCREENWIDTH / 2 + 25, SCREENHEIGHT / 2, 150, 150, alignCenter, alignBtm);
ArrowWidget lSigArrow = ArrowWidget(&centerScreen, 15 + 25, SCREENHEIGHT / 2, 100, 80, LEFT_TO_RIGHT);
ArrowWidget rSigArrow = ArrowWidget(&centerScreen, SCREENWIDTH - 95 - 5, SCREENHEIGHT / 2, 100, 80, RIGHT_TO_LEFT);
TextWidget status_txt = TextWidget(&centerScreen, SCREENWIDTH / 2 + 25, SCREENHEIGHT / 2 + 25, 200, 80, alignCenter, alignTop);
// left screen
TextWidget capOutAmp_txt = TextWidget(&leftScreen, 0, 0, 90, 75, alignLeft, alignTop);
TextWidget capInAmp_txt = TextWidget(&leftScreen, SCREENWIDTH, 0, 90, 75, alignRight, alignTop);
TextWidget motorAmp_txt = TextWidget(&leftScreen, 0, SCREENHEIGHT, 90, 75, alignLeft, alignBtm);
TextWidget motorVolt_txt = TextWidget(&leftScreen, SCREENWIDTH, SCREENHEIGHT, 90, 75, alignRight, alignBtm);
BarWidget capOutAmp_bar = BarWidget(&leftScreen, 480 / 2 - 10, 100, 220, 35, RIGHT_TO_LEFT);
BarWidget capInAmp_bar = BarWidget(&leftScreen, 480 / 2 + 10, 100, 220, 35, LEFT_TO_RIGHT);
BarWidget motorAmp_bar = BarWidget(&leftScreen, 480 / 2 - 10, 160, 220, 90, RIGHT_TO_LEFT);
BarWidget motorVolt_bar = BarWidget(&leftScreen, 480 / 2 + 10, 160, 220, 90, LEFT_TO_RIGHT); // original y=180, h=70
// right screen
TextWidget stackTemperature_txt = TextWidget(&rightScreen, 0, 0, 140, 80, alignLeft, alignTop);
TextWidget status_txt = TextWidget(&rightScreen, SCREENWIDTH / 2, 0, 200, 80, alignCenter, alignTop);
TextWidget pressure_txt = TextWidget(&rightScreen, SCREENWIDTH, 0, 140, 80, alignRight, alignTop);
TextWidget energy_txt = TextWidget(&rightScreen, SCREENWIDTH / 2, SCREENHEIGHT / 2 - 40 + 20, 400, 80, alignCenter, alignCenter);
BarWidget energy_bar = BarWidget(&rightScreen, SCREENWIDTH / 2, SCREENHEIGHT / 2 + 40 + 20, 400, 80, alignCenter, alignCenter);

void dashboardInit();
void dashboardNextFrame();
void dashboardNextValuesFC(int watts, float pressure, int temperature, const char* status);
void dashboardNextValuesCS(int volts, int ampCapIn, int ampCapOut, int ampMotor);
void dashboardNextValuesSpeed(int speedKmh);

#endif

