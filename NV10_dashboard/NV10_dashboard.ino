/*
 Name:		NV10_dashboard.ino
 Created:	8/15/2018 1:35:26 PM
 Author:	MX
*/


// the setup function runs once when you press reset or power the board
#include <FreeRTOS_ARM.h>
#include <ILI9488.h>
#include "DisplayContainer.h"
#include "DisplayBar.h""
#include "DisplayText.h"
#include "DisplayGauge.h"

void TaskRefreshScreen(void* pvParameters);
void TaskReadCAN(void* pvParameters);
const int screenLED = 9;
void setup() {
	pinMode(screenLED, OUTPUT);
	digitalWrite(screenLED, HIGH);
	Serial.begin(9600);
	delay(1000);
	// 480 x 320 pixels
	/*Dashboard Info
		Fuel cell info x2
			Voltage
			Current
			Temperature(stack)
			Pressure
			Status(SS, IN, OP, SD, etc.)
		Capacitor & Motors x3
			Voltage
			Motor
			Current

		Car Speed
		Acceleration
		Handbrake light
		Cylinder Pressure
	*/
	xTaskCreate(
		TaskRefreshScreen
		, (const portCHAR *)"Refresh"  // A name just for humans
		, 500  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL // Any pointer to pass in
		, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);

	vTaskStartScheduler();
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
void TaskRefreshScreen(void* pvParameters)
{
	ILI9488 leftScreen = ILI9488(10, 7, 8);
	leftScreen.begin();
	leftScreen.setRotation(3);
	leftScreen.fillScreen(ILI9488_BLACK);

	DisplayText testElement1 = DisplayText(&leftScreen, 0, 0, 50, 320);
	DisplayText testElement2 = DisplayText(&leftScreen, 50, 0, 190, 50);
	DisplayBar testElement3 = DisplayBar(&leftScreen, 240, 100, 240, 50);
	DisplayText testElement4 = DisplayText(&leftScreen, 290, 0, 190, 50);


	testElement1.setMargin(6);
	testElement2.setMargin(12);
	testElement3.setMargin(6);
	testElement4.setMargin(10);

	testElement3.setRange(0,100);
	testElement3.setOrientation(DisplayBar::RIGHT_TO_LEFT);

	while (1)
	{
		char randStr[4];
		itoa(random(0,100), randStr, 10);
		testElement1.setColors(ILI9488_WHITE, ILI9488_RED);
		testElement3.setColors(ILI9488_CYAN, ILI9488_BLACK);
		testElement1.update("SD");
		testElement2.update("----");
		testElement3.update(randStr);
		testElement4.update("DEAD");
		vTaskDelay(pdMS_TO_TICKS(300));
		itoa(random(0, 100), randStr, 10);
		testElement1.setColors(ILI9488_BLACK, ILI9488_GREEN);
		testElement3.setColors(ILI9488_WHITE, ILI9488_BLACK);
		testElement1.update("OP");
		testElement2.update("55.2");
		testElement3.update(randStr);
		testElement4.update("ALIVE");
		vTaskDelay(pdMS_TO_TICKS(300));

		itoa(random(0, 100), randStr, 10);
		testElement3.update(randStr);
		vTaskDelay(pdMS_TO_TICKS(300));

	}
}
void TaskReadCAN(void* pvParameters)
{

	while (1)
	{

	}
}