/*
 Name:		NV10_dashboard2_DUE.ino
 Created:	3/21/2019 8:01:28 PM
 Author:	MX
*/
#include "DashboardScreens.h"
#include "ArrowWidget.h"
#include "BarWidget.h"
#include "TextWidget.h"

#include "Pins_dashboard.h"
#include <FreeRTOS_ARM.h>

#include <NV10FuelCell.h>
#include <NV10CurrentSensor.h>
#include <NV10CurrentSensorStats.h>
#include <NV10AccesoriesStatus.h>
#include <NV11DataSpeedo.h>

NV10FuelCell dataFC = NV10FuelCell(0x13);
NV10CurrentSensor dataCS = NV10CurrentSensor(0x11);
NV10CurrentSensorStats dataCSStats = NV10CurrentSensorStats(0x12);
NV11DataSpeedo dataSpeedo = NV11DataSpeedo(0x0A);
NV10AccesoriesStatus dataAcc = NV10AccesoriesStatus(0x10);

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

	dashboardInit();
	// I tried putting attachinterrupt in the for loop above but failed. Lambda functions complain.
	// So here, have some wall text.
	attachInterrupt(digitalPinToInterrupt(BTN_HAZARD), [] {
		dataAcc.toggleHazard();
	}, FALLING);
	attachInterrupt(digitalPinToInterrupt(BTN_WIPER), [] {
		dataAcc.toggleWiper();
	}, FALLING);
	//attachInterrupt(digitalPinToInterrupt(BTN_HORN), [] {
	//	
	//}, FALLING);
	attachInterrupt(digitalPinToInterrupt(BTN_HEADLIGHT), [] {
		dataAcc.toggleHeadlights();
	}, FALLING);
	attachInterrupt(digitalPinToInterrupt(BTN_LSIG), [] {
		dataAcc.toggleLsig();
	}, FALLING);
	attachInterrupt(digitalPinToInterrupt(BTN_RSIG), [] {
		dataAcc.toggleRsig();
	}, FALLING);

	const unsigned int pins[] = { BTN_HAZARD, BTN_WIPER, BTN_HORN, BTN_HEADLIGHT, BTN_LSIG, BTN_RSIG };
	setDebounce(pins, sizeof(pins) / sizeof(pins[0]));
}
void loop()
{
	char s[100];
	// output dashboard display based on incoming CAN strings
	uint8_t bytesRead = Serial1.readBytesUntil('\n', s, 100);
	if (bytesRead > 0)
	{
		s[bytesRead - 1] = '\0';
		if (dataFC.checkMatchString(s))
		{
			dataFC.unpackString(s);
			dashboardNextValuesFC(dataFC.getWatts(), dataFC.getPressure(), dataFC.getTemperature(), dataFC.getStatus());
		}
		else if (dataCS.checkMatchString(s))
		{
			dataCS.unpackString(s);
			dashboardNextValuesCS(dataCS.getVolt(), dataCS.getAmpCapIn(), dataCS.getAmpCapOut(), dataCS.getAmpMotor());
		}
		else if (dataSpeedo.checkMatchString(s))
		{
			dataSpeedo.unpackString(s);
			dashboardNextValuesSpeed(dataSpeedo.getSpeed());
		}
	}
	// output CAN strings based on buttons inputs (already handled by interrupts)
	if (dataAcc.dataRequiresBroadcast())
	{
		dataAcc.packString(s);
		Serial1.println(s);
	}

	dashboardNextFrame();
	// wait until 100ms elapsed
	static unsigned long lastTime;
	while (millis() - lastTime < 100);
	lastTime = millis();
}