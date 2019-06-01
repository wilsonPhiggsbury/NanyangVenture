// 
// 
// 

#include "DashboardScreens.h"

void dashboardInit()
{
	pinMode(LCD_OUTPUT_BACKLIGHT, OUTPUT);
	digitalWrite(LCD_OUTPUT_BACKLIGHT, HIGH);
	centerScreen.begin();
	leftScreen.begin();
	rightScreen.begin();

	centerScreen.setRotation(1);
	leftScreen.setRotation(3);
	rightScreen.setRotation(3);

	capOutAmp_bar.setColors(ILI9488_RED, ILI9488_MAROON);
	capInAmp_bar.setColors(ILI9488_CYAN, ILI9488_DARKCYAN);
	motorAmp_bar.setColors(ILI9488_RED, ILI9488_MAROON);
	motorVolt_bar.setColors(ILI9488_CYAN, ILI9488_DARKCYAN);

	stackTemperature_txt.setColors(ILI9488_WHITE, ILI9488_BLUE);
	status_txt.setColors(ILI9488_WHITE, ILI9488_BLUE);
	pressure_txt.setColors(ILI9488_WHITE, ILI9488_BLUE);
	energy_txt.setColors(ILI9488_WHITE, ILI9488_BLUE);
	energy_bar.setColors(ILI9488_YELLOW, ILI9488_BLACK);
}
void dashboardNextFrame()
{

}
void dashboardNextValuesFC(int watts, float pressure, int temperature, const char * status)
{
	energy_bar.updateFloat(watts);
	energy_txt.updateFloat(watts);
	pressure_txt.updateFloat(pressure);
	stackTemperature_txt.updateFloat(temperature);
	status_txt.updateText(status);
}
void dashboardNextValuesCS(int volts, int ampCapIn, int ampCapOut, int ampMotor)
{
	motorVolt_bar.updateFloat(volts);
	capInAmp_bar.updateFloat(ampCapIn);
	capOutAmp_bar.updateFloat(ampCapOut);
	motorAmp_bar.updateFloat(ampMotor);

	motorVolt_txt.updateFloat(volts);
	capInAmp_txt.updateFloat(ampCapIn);
	capOutAmp_txt.updateFloat(ampCapOut);
	motorAmp_txt.updateFloat(ampMotor);
}

void dashboardNextValuesSpeed(int speedKmh)
{
	speedTxt.updateFloat(speedKmh);
}