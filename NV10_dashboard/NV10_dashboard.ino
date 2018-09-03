/*
 Name:		NV10_dashboard.ino
 Created:	8/15/2018 1:35:26 PM
 Author:	MX
*/


// the setup function runs once when you press reset or power the board
#include <FreeRTOS_ARM.h>
#include <ILI9488.h>
#include "DisplayContainer.h"
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
	Serial.println("DONE INIT");
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
		, 250  // This stack size can be checked & adjusted by reading the Stack Highwater
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
	ILI9488 testScreen = ILI9488(10, 7, 8);
	testScreen.begin();
	testScreen.setRotation(1);
	testScreen.fillScreen(ILI9488_BLACK);

	DisplayText testElement = DisplayText();
	DisplayText testElement2 = DisplayText();
	testElement.init(&testScreen, 0, 0, 240, 50);
	testElement2.init(&testScreen, 240, 0, 240, 50);
	while (1)
	{
		testElement2.update("12.4");
		testElement.update("50.0");
		testElement.draw();
		testElement2.draw();
		vTaskDelay(pdMS_TO_TICKS(1000));
		testElement.update("XXOX");
		testElement2.update("OOXO");
		testElement.draw();
		testElement2.draw();
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
void TaskReadCAN(void* pvParameters)
{

	while (1)
	{

	}
}