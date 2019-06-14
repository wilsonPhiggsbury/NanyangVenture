/*
 Name:		NV10_dashboard2_DUE.ino
 Created:	3/21/2019 8:01:28 PM
 Author:	MX
*/

/*
WARNING

	DO NOT ENABLE debug(...) in this script. Verified to malfunction.
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

NV10FuelCell dataFC;
NV10CurrentSensor dataCS;
NV10CurrentSensorStats dataCSStats;
NV11DataSpeedo dataSpeedo;
NV10AccesoriesStatus dataAcc;

DashboardScreens d;
HardwareSerial& CANSerialPort = Serial1;
HardwareSerial& debugSerialPort = Serial;
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
void setDebounce(const unsigned int pins[], uint8_t numPins, uint16_t waitTimeMultiplier = 500);
void setup()
{
	debugSerialPort.begin(9600);
	CANSerialPort.begin(9600);
	CANSerialPort.setTimeout(500);

	pinMode(CAN_OUTPUT_RST, OUTPUT);
	digitalWrite(CAN_OUTPUT_RST, LOW);
	delay(100);
	digitalWrite(CAN_OUTPUT_RST, HIGH);

	d.dashboardInit();
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

	dataAcc.insertData(0, 0, 0, 0, 0, 0);
}
void loop()
{
	char s[100];
	// output dashboard display based on incoming CAN strings
	uint8_t bytesRead = CANSerialPort.readBytesUntil('\n', s, 100);
	if (bytesRead > 0)
	{
		s[bytesRead - 1] = '\0';
		debugSerialPort.print("<R> ");
		debugSerialPort.println(s);
		if (dataFC.checkMatchString(s))
		{
			dataFC.unpackString(s);
			d.dashboardNextValuesFC(dataFC.getWatts(), dataFC.getPressure(), dataFC.getTemperature(), dataFC.getStatus());
		}
		else if (dataCS.checkMatchString(s))
		{
			dataCS.unpackString(s);
			d.dashboardNextValuesCS(dataCS.getVolt(), dataCS.getAmpCapIn(), dataCS.getAmpCapOut(), dataCS.getAmpMotor());
		}
		else if (dataSpeedo.checkMatchString(s))
		{
			dataSpeedo.unpackString(s);
			d.dashboardNextValuesSpeed(dataSpeedo.getSpeed());
		}
		else if (dataAcc.checkMatchString(s))
		{
			dataAcc.unpackString(s);
		}
	}
	// output CAN strings based on buttons inputs (already handled by interrupts)
	if (dataAcc.dataRequiresBroadcast())
	{
		d.dashboardToggleSig(dataAcc.getLsig(), dataAcc.getRsig());
		dataAcc.packString(s);
		CANSerialPort.println(s);
		debugSerialPort.print("<S> ");
		debugSerialPort.println(s);
	}

	d.dashboardNextFrame();

	static uint8_t canRstCounter = 0;
	if (canRstCounter++ > 30)
	{
		canRstCounter = 0;
		digitalWrite(CAN_OUTPUT_RST, LOW);
		delay(100);
		digitalWrite(CAN_OUTPUT_RST, HIGH);
	}
	// wait until 100ms elapsed
	static unsigned long lastTime;
	while (millis() - lastTime < 100);
	lastTime = millis();
}