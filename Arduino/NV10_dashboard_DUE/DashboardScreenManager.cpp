#include "DashboardScreenManager.h"
#include "DisplayBar.h"
#include "DisplayText.h"
#include "DisplayGauge.h"
#include "Pins_Dashboard.h"

static const int screenWidth = 480;
static const int screenHeight = 320;
DashboardScreenManager::DashboardScreenManager(Packet* Packet)
{
	q = Packet;
	PacketID* trackIDaddr = &(Packet->ID);
	// initialize screens
	ILI9488* leftScreen = new ILI9488(LCDLEFT_SPI_CS, LCD_OUTPUT_DC, LCD_OUTPUT_RST);
	ILI9488* centerScreen = new ILI9488(LCDCENTER_SPI_CS, LCD_OUTPUT_DC);
	ILI9488* rightScreen = new ILI9488(LCDRIGHT_SPI_CS, LCD_OUTPUT_DC);
	leftScreen->begin();
	centerScreen->begin();
	rightScreen->begin();

	leftScreen->setRotation(3);
	leftScreen->fillScreen(ILI9488_BLACK);
	leftScreen->setTextColor(ILI9488_WHITE);
	centerScreen->setRotation(1);
	centerScreen->fillScreen(ILI9488_BLACK);
	centerScreen->setTextColor(ILI9488_WHITE);
	rightScreen->setRotation(3);
	rightScreen->fillScreen(ILI9488_BLACK);
	rightScreen->setTextColor(ILI9488_WHITE);
	// ---------------------------- LEFT SCREEN -----------------------------
	// initialize widgets
	//DisplayText* capOutAmp_txt = new DisplayText(leftScreen, 0, 0, 90, 75, alignLeft, alignTop);
	//DisplayText* capInAmp_txt = new DisplayText(leftScreen, screenWidth, 0, 90, 75, alignRight, alignTop);
	//DisplayText* motorAmp_txt = new DisplayText(leftScreen, 0, screenHeight, 90, 75, alignLeft, alignBtm);
	//DisplayText* motorVolt_txt = new DisplayText(leftScreen, screenWidth, screenHeight, 90, 75, alignRight, alignBtm);
	//DisplayBar* capOutAmp_bar = new DisplayBar(leftScreen, 480/2-10, 100, 220, 35, DisplayBar::RIGHT_TO_LEFT);
	//DisplayBar* capInAmp_bar = new DisplayBar(leftScreen, 480/2+10, 100, 220, 35, DisplayBar::LEFT_TO_RIGHT);
	//DisplayBar* motorAmp_bar = new DisplayBar(leftScreen, 480/2-10, 160, 220, 90, DisplayBar::RIGHT_TO_LEFT);
	//DisplayBar* motorVolt_bar = new DisplayBar(leftScreen, 480/2+10, 140, 220, 120, DisplayBar::LEFT_TO_RIGHT); // original y=180, h=70
	// listen to data on a pointer
	//capInAmp_txt->init(CS, trackIDaddr, &(Packet->data[0][1]));
	//capInAmp_bar->init(CS, trackIDaddr, &(Packet->data[0][1]));
	//capOutAmp_txt->init(CS, trackIDaddr, &(Packet->data[1][1]));
	//capOutAmp_bar->init(CS, trackIDaddr, &(Packet->data[1][1]));
	//motorVolt_bar->init(CS, trackIDaddr, &(Packet->data[2][0]));
	//motorAmp_txt->init(CS, trackIDaddr, &(Packet->data[2][1]));
	//motorAmp_bar->init(CS, trackIDaddr, &(Packet->data[2][1]));
	// customize each widget
	//capOutAmp_txt->setMargin(0);
	//capInAmp_txt->setMargin(0);
	//motorAmp_txt->setMargin(0);
	//motorVolt_txt->setMargin(2);
	//capOutAmp_bar->setRange(0, 10);
	//capInAmp_bar->setRange(0, 10);
	//motorAmp_bar->setRange(0, 40);
	//motorVolt_bar->setRange(45, 60);
	//capOutAmp_bar->setColors(ILI9488_RED, ILI9488_MAROON);
	//capInAmp_bar->setColors(ILI9488_CYAN, ILI9488_DARKCYAN);
	//motorAmp_bar->setColors(ILI9488_RED, ILI9488_MAROON);
	//motorVolt_bar->setColors(ILI9488_CYAN, ILI9488_DARKCYAN);

	// ---------------------------- CENTER SCREEN -----------------------------
	// initialize widgets
	DisplayText* speedDisplay = new DisplayText(centerScreen, screenWidth/2 + 20, screenHeight/2, 150, 150, alignCenter, alignBtm);
	lSigArrow = new DisplayArrow(centerScreen, 40, screenHeight / 2 - 40, 100, 80, DisplayArrow::left);
	rSigArrow = new DisplayArrow(centerScreen, screenWidth-100, screenHeight / 2 - 40, 100, 80, DisplayArrow::right);
	// listen to data on a pointer
	speedDisplay->init(SM, trackIDaddr, &(Packet->data[0][0]));
	lSigArrow->init(BT, trackIDaddr, &(Packet->data[0][Lsig]));
	rSigArrow->init(BT, trackIDaddr, &(Packet->data[0][Rsig]));
	// customize each widget
	speedDisplay->setColors(ILI9488_WHITE, ILI9488_DARKGREEN);
	lSigArrow->setColors(ILI9488_GREEN, ILI9488_BLACK);
	rSigArrow->setColors(ILI9488_GREEN, ILI9488_BLACK);

	// ---------------------------- RIGHT SCREEN -----------------------------
	// initialize widgets
	DisplayText* stackTemperature_txt = new DisplayText(rightScreen, 0, 0, 140, 80, alignLeft, alignTop);
	DisplayText* status_txt = new DisplayText(centerScreen, screenWidth / 2, screenHeight, 200, 80, alignCenter, alignBtm);
	//DisplayText* pressure_txt = new DisplayText(rightScreen, screenWidth, 0, 140, 80, alignRight, alignTop);
	//DisplayText* energy_txt = new DisplayText(rightScreen, screenWidth / 2, screenHeight / 2 - 40 + 20, 400, 80, alignCenter, alignCenter);
	//DisplayBar* energy_bar = new DisplayBar(rightScreen, screenWidth / 2, screenHeight / 2 + 40 + 20, 400, 80, alignCenter, alignCenter);
	DisplayBar* volt_bar = new DisplayBar(centerScreen, screenWidth / 2, screenHeight / 2 + 40, 200, 75, DisplayBar::LEFT_TO_RIGHT);
	DisplayBar* amp_bar = new DisplayBar(rightScreen, screenWidth / 2, screenHeight / 2 + 40, 200, 60, DisplayBar::RIGHT_TO_LEFT);
	DisplayText* motorVolt_txt = new DisplayText(centerScreen, screenWidth / 2, screenHeight/2 + 40, 180, 75, alignRight, alignCenter);
	DisplayText* motorAmp_txt = new DisplayText(rightScreen, screenWidth / 2, screenHeight/2 + 110, 90, 75, alignRight, alignCenter);
	// listen to data on a pointer
	stackTemperature_txt->init(FC, trackIDaddr, &(Packet->data[0][1]));
	status_txt->init(FC, trackIDaddr, &(Packet->data[0][3]));
	//pressure_txt->init(FC, trackIDaddr, &(Packet->data[0][2]));
	//energy_txt->init(FC, trackIDaddr, &(Packet->data[0][0]));
	//energy_bar->init(FC, trackIDaddr, &(Packet->data[0][0]));
	//energy_bar->setRange(0, 100);
	volt_bar->init(FC, trackIDaddr, &(Packet->data[0][0]));
	amp_bar->init(FC, trackIDaddr, &(Packet->data[0][2]));
	motorVolt_txt->init(FC, trackIDaddr, &(Packet->data[0][0]));
	motorAmp_txt->init(CS, trackIDaddr, &(Packet->data[0][2]));
	volt_bar->setRange(35, 60);
	amp_bar->setRange(0, 40);
	motorVolt_txt->setMargin(2);
	motorAmp_txt->setMargin(2);
	// customize each widget
	stackTemperature_txt->setColors(ILI9488_WHITE, ILI9488_BLUE);
	status_txt->setColors(ILI9488_WHITE, ILI9488_BLUE);
	//pressure_txt->setColors(ILI9488_WHITE, ILI9488_BLUE);
	//energy_txt->setColors(ILI9488_WHITE, ILI9488_BLUE);
	//energy_bar->setColors(ILI9488_YELLOW, ILI9488_BLACK);
	volt_bar->setColors(ILI9488_CYAN, ILI9488_BLACK);
	amp_bar->setColors(ILI9488_YELLOW, ILI9488_BLACK);
	motorVolt_txt->setColors(ILI9488_WHITE, ILI9488_DARKCYAN);
	motorAmp_txt->setColors(ILI9488_WHITE, ILI9488_MAROON);
	// ---------------------------- tie up references for updating later -----------------------------
	//dataWidgets[0] = NULL;// capInAmp_txt;
	//dataWidgets[1] = NULL;// capInAmp_bar;
	//dataWidgets[2] = capOutAmp_txt;
	//dataWidgets[3] = capOutAmp_bar;
	//dataWidgets[4] = motorAmp_txt;
	//dataWidgets[5] = motorAmp_bar;
	//dataWidgets[6] = motorVolt_txt;
	//dataWidgets[7] = motorVolt_bar;

	dataWidgets[8] = speedDisplay;

	dataWidgets[9] = stackTemperature_txt;
	dataWidgets[10] = status_txt;
	dataWidgets[11] = volt_bar;
	dataWidgets[12] = motorVolt_txt;
	dataWidgets[13] = amp_bar;
	dataWidgets[14] = motorAmp_txt;
}
void DashboardScreenManager::refreshDataWidgets()
{
	for (int i = 8; i < 15; i++)
	{
		DisplayText* box;
		DisplayBar* bar;
		if (q->ID  == FC)
		{
			switch (i)
			{
			case 9: // FC temp should be < 60
				box = (DisplayText*)dataWidgets[i];
				if (q->data[0][1] >= 60)
					box->setColors(ILI9488_WHITE, ILI9488_RED);
				else
					box->setColors(ILI9488_WHITE, ILI9488_BLUE);
				dataWidgets[i]->update();
				break;
			case 10: // FC status should be = 1.0 (only 3 possible values)
				box = (DisplayText*)dataWidgets[i];
				if (q->data[0][3] == 1.0)
				{
					box->setColors(ILI9488_GREEN, ILI9488_DARKGREEN);
					box->update("ON");
					debug("FC ON");
				}
				else if (q->data[0][3] == 2.0)
				{
					box->setColors(ILI9488_ORANGE, ILI9488_YELLOW);
					box->update("INIT");
					debug("FC INIT");
				}
				else
				{
					box->setColors(ILI9488_RED, ILI9488_MAROON);
					box->update("OFF");
					debug("FC OFF");
				}
				break;
			case 11: // volt bar
				bar = (DisplayBar*)dataWidgets[i];
				if (q->data[0][0] >= 60 || q->data[0][0] <= 45)
				{
					bar->setColors(ILI9488_CYAN, ILI9488_RED);
				}
				else
				{
					bar->setColors(ILI9488_CYAN, ILI9488_BLACK);
				}
				bar->update();
				break;
			case 12: // volt text
				box = (DisplayText*)dataWidgets[i];
				if (q->data[0][0] >= 60 || q->data[0][0] <= 45)
					box->setColors(ILI9488_WHITE, ILI9488_RED);
				else
					box->setColors(ILI9488_WHITE, ILI9488_DARKCYAN);
				dataWidgets[i]->update();
				break;
			case 13: // amp bar
				bar = (DisplayBar*)dataWidgets[i];
				if (q->data[0][2] > 30)
				{
					bar->setColors(ILI9488_RED, ILI9488_BLACK);
				}
				else
				{
					bar->setColors(ILI9488_YELLOW, ILI9488_BLACK);
				}
				bar->update();
				break;
			case 14: // amp text
				box = (DisplayText*)dataWidgets[i];
				if (q->data[0][2] > 30)
					box->setColors(ILI9488_WHITE, ILI9488_RED);
				else
					box->setColors(ILI9488_WHITE, ILI9488_MAROON);
				dataWidgets[i]->update();
				break;
			default:
				dataWidgets[i]->update();
				break;
			}
		}
		else if (q->ID == CS)
		{
			switch (i)
			{
			case 4: // motor amp should be < 40 
				box = (DisplayText*)dataWidgets[i];
				if (q->data[2][1] >= 40)
					box->setColors(ILI9488_WHITE, ILI9488_RED);
				else
					box->setColors(ILI9488_WHITE, ILI9488_BLACK);
				break;
			case 6: // motor volt should be > 45 
				box = (DisplayText*)dataWidgets[i];
				if (q->data[2][0] <= 45)
					box->setColors(ILI9488_WHITE, ILI9488_RED);
				else
					box->setColors(ILI9488_WHITE, ILI9488_BLACK);
				break;
			}
			dataWidgets[i]->update();
		}
		else
		{
			dataWidgets[i]->update();
		}
	}
	lSigArrow->update();
	rSigArrow->update();
}
void DashboardScreenManager::refreshDataWidgets(void* null)
{
	for (int i = 8; i < 15; i++)
	{
		dataWidgets[i]->updateNull();
	}
}
void DashboardScreenManager::refreshAnimatedWidgets()
{
}

DashboardScreenManager::~DashboardScreenManager()
{
}
