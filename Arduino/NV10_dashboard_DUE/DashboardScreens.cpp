// 
// 
// 

#include "DashboardScreens.h"
DashboardScreens::DashboardScreens()
{

}
void DashboardScreens::dashboardInit()
{
	pinMode(LCD_OUTPUT_BACKLIGHT, OUTPUT);
	digitalWrite(LCD_OUTPUT_BACKLIGHT, HIGH);
	centerScreen.begin();
	//leftScreen.begin();
	//rightScreen.begin();

	centerScreen.setRotation(1);
	//leftScreen.setRotation(3);
	//rightScreen.setRotation(3);

	centerScreen.fillScreen(ILI9488_BLACK);
	//leftScreen.fillScreen(ILI9488_BLACK);
	//rightScreen.fillScreen(ILI9488_BLACK);

	speedTxt.setColors(ILI9488_WHITE, ILI9488_DARKGREEN);
	lSigArrow.setColors(ILI9488_GREEN, ILI9488_BLACK);
	rSigArrow.setColors(ILI9488_GREEN, ILI9488_BLACK);
	status_txt.setColors(ILI9488_WHITE, ILI9488_BLUE);
	//capOutAmp_bar.setColors(ILI9488_RED, ILI9488_MAROON);
	//capInAmp_bar.setColors(ILI9488_CYAN, ILI9488_DARKCYAN);
	motorAmp_txt.setColors(ILI9488_RED, ILI9488_MAROON);
	motorAmp_bar.setColors(ILI9488_RED, ILI9488_MAROON);
	motorAmp_txt.drawBorder(0, ILI9488_BLACK);
	motorAmp_bar.drawBorder(0, ILI9488_BLACK);

	motorVolt_txt.setColors(ILI9488_CYAN, ILI9488_DARKCYAN);
	motorVolt_bar.setColors(ILI9488_CYAN, ILI9488_DARKCYAN);
	motorVolt_txt.drawBorder(0, ILI9488_BLACK);
	motorVolt_bar.drawBorder(0, ILI9488_BLACK);

	motorAmp_bar.setRange(0, 40);
	motorVolt_bar.setRange(35, 55);
	//stackTemperature_txt.setColors(ILI9488_WHITE, ILI9488_BLUE);
	//status_txt.setColors(ILI9488_WHITE, ILI9488_BLUE);
	//pressure_txt.setColors(ILI9488_WHITE, ILI9488_BLUE);
	//energy_txt.setColors(ILI9488_WHITE, ILI9488_BLUE);
	//energy_bar.setColors(ILI9488_YELLOW, ILI9488_BLACK);
}
void DashboardScreens::dashboardNextFrame()
{
	if (fcTimeoutCounter < fcTimeout)
	{
		fcTimeoutCounter++;
	}
	else if (fcTimeoutCounter == fcTimeout)
	{
		fcTimeoutCounter++;
		// deactivate fc widgets
		status_txt.setColors(ILI9488_WHITE, ILI9488_BLUE);
		status_txt.updateText("--");
	}
}
void DashboardScreens::dashboardNextValuesFC(int watts, float pressure, int temperature, const char * status)
{
	//energy_bar.updateFloat(watts);
	//energy_txt.updateFloat(watts);
	//pressure_txt.updateFloat(pressure);
	//stackTemperature_txt.updateFloat(temperature);
	if (fcTimeoutCounter > fcTimeout)
	{
		// reactivate FC widgets if previously disabled
		// ...
	}
	fcTimeoutCounter = 0; // reset FC timeout everytime FC data arrives
	if (!strcmp(status, "OP"))
	{
		status_txt.setColors(ILI9488_GREEN, ILI9488_DARKGREEN);
	}
	else if (!strcmp(status, "IN"))
	{
		status_txt.setColors(ILI9488_YELLOW, ILI9488_OLIVE);
	}
	else if (!strcmp(status, "SD"))
	{
		status_txt.setColors(ILI9488_RED, ILI9488_MAROON);
	}
	else
	{
		status_txt.setColors(ILI9488_WHITE, ILI9488_BLUE);
	}
	status_txt.updateText(status);
}
void DashboardScreens::dashboardNextValuesCS(int volts, int ampCapIn, int ampCapOut, int ampMotor)
{
	motorVolt_bar.updateFloat(volts);
	//capInAmp_bar.updateFloat(ampCapIn);
	//capOutAmp_bar.updateFloat(ampCapOut);
	motorAmp_bar.updateFloat(ampMotor);

	motorVolt_txt.updateFloat(volts);
	//capInAmp_txt.updateFloat(ampCapIn);
	//capOutAmp_txt.updateFloat(ampCapOut);
	motorAmp_txt.updateFloat(ampMotor);
}

void DashboardScreens::dashboardNextValuesCS(int volts, int ampMotor)
{
	dashboardNextValuesCS(volts, 0, 0, ampMotor);
}

void DashboardScreens::dashboardNextValuesSpeed(int speedKmh)
{
	speedTxt.updateFloat(speedKmh);
}

void DashboardScreens::dashboardToggleSig(int lsig, int rsig)
{
	if (lsig)
		lSigArrow.activate();
	else
		lSigArrow.wipe();


	if (rsig)
		rSigArrow.activate();
	else
		rSigArrow.wipe();
}
