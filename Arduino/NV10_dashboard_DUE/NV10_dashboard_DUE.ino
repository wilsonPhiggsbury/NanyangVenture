/*
 Name:		NV10_dashboard2_DUE.ino
 Created:	3/21/2019 8:01:28 PM
 Author:	MX
*/
#include "ArrowWidget.h"
#include "BarWidget.h"
#include "TextWidget.h"

#include "Pins_dashboard.h"
#include <FreeRTOS_ARM.h>

#include <NV10AccesoriesStatus.h>

NV10AccesoriesStatus acc = NV10AccesoriesStatus(0x10);
ILI9488 centerScreen = ILI9488(LCDCENTER_SPI_CS, LCD_OUTPUT_DC, LCD_OUTPUT_RST);
// center screen x offset: +25, -5
//TextWidget t = TextWidget(&centerScreen, 475, 0, 200, 50, alignRight, alignTop);
//BarWidget b = BarWidget(&centerScreen, 475, 200, 200, 40, RIGHT_TO_LEFT);
//void setup() {
//	pinMode(LCD_OUTPUT_BACKLIGHT, OUTPUT);
//	digitalWrite(LCD_OUTPUT_BACKLIGHT, HIGH);
//
//	centerScreen.begin();
//	centerScreen.setRotation(1);
//
//	t.init();
//	t.setColors(ILI9488_WHITE, ILI9488_BLACK);
//	t.setMargin(2);
//
//	b.init();
//	b.setRange(0, 10);
//	b.setColors(ILI9488_CYAN, ILI9488_BLACK);
//}
//
//// the loop function runs over and over again until power down or reset
//void loop() {
//	t.setMargin(0);
//	t.updateFloat(12.0);
//	delay(1000);
//	t.updateFloat(12.1);
//	delay(1000);
//	t.updateText("FHAHA");
//	delay(1000);
//	t.updateText("FHAHAHA");
//	delay(1000);
//	t.updateText("FHAHAH");
//	delay(1000);
//
//	delay(1000);
//
//	b.updateFloat(0.0);
//	delay(1000);
//	b.updateFloat(2.5);
//	delay(1000);
//	b.updateFloat(5.0);
//	delay(1000);
//	b.updateFloat(7.5);
//	delay(1000);
//	b.updateFloat(10.0);
//	delay(1000);
//
//	delay(1000);
//}

void setup()
{
	Serial1.begin(9600);
	Serial1.setTimeout(500);
	// I tried putting attachinterrupt in the for loop above but failed. Lambda functions complain.
	// So here, have some wall text.
	attachInterrupt(digitalPinToInterrupt(BTN_HAZARD), [] {
		acc.toggleHazard();
	}, FALLING);
	attachInterrupt(digitalPinToInterrupt(BTN_WIPER), [] {
		acc.toggleWiper();
	}, FALLING);
	//attachInterrupt(digitalPinToInterrupt(BTN_HORN), [] {
	//	
	//}, FALLING);
	attachInterrupt(digitalPinToInterrupt(BTN_HEADLIGHT), [] {
		acc.toggleHeadlights();
	}, FALLING);
	attachInterrupt(digitalPinToInterrupt(BTN_LSIG), [] {
		acc.toggleLsig();
	}, FALLING);
	attachInterrupt(digitalPinToInterrupt(BTN_RSIG), [] {
		acc.toggleRsig();
	}, FALLING);
}
void loop()
{
	char s[100];
	// output dashboard display based on incoming CAN strings
	uint8_t bytesRead = Serial1.readBytesUntil('\n', s, 100);
	if (bytesRead > 0)
	{
		s[bytesRead - 1] = '\0';
		if (acc.checkMatchString(s))
		{
			acc.unpackString(s);
			dashboardNextValues();
		}
	}
	// output CAN strings based on buttons inputs (already handled by interrupts)
	if (acc.dataRequiresBroadcast())
	{
		acc.packString(s);
		Serial1.println(s);
	}

	dashboardNextFrame();
	// wait until 100ms elapsed
	static unsigned long lastTime;
	while (millis() - lastTime < 100);
	lastTime = millis();
}
void dashboardNextFrame()
{

}
void dashboardNextValues()
{

}