/*
 Name:		NV10_dashboard.ino
 Created:	8/15/2018 1:35:26 PM
 Author:	MX
*/


//#include <Arduino_FreeRTOS.h>
//#include <queue.h>
#include <FreeRTOS_ARM.h>
#include <SPI.h>

#include <ILI9488.h>
#include "Wiring_Dashboard.h"
// dependent header files
#include "FrameFormats.h"
// ----------------------

#include "DisplayContainer.h"
#include "DisplayBar.h"
#include "DisplayText.h"
#include "DisplayGauge.h"
//#include "Bitmaps.h"

void TaskRefreshScreen(void* pvParameters);
void TaskReadSerial(void* pvParameters);
//QueueHandle_t queueForDisplay = xQueueCreate(1, sizeof(QueueItem));
void setup() {
	Serial.begin(9600);
	Serial3.begin(9600);
	Serial3.setTimeout(100);
	delay(100);
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
			CS
			Current

		Car Speed
		Acceleration
		Handbrake light
		Cylinder Pressure
	*/
	//xTaskCreate(
	//	TaskRefreshScreen
	//	, (const portCHAR *)"Refresh"  // A name just for humans
	//	, 1000 // This stack size can be checked & adjusted by reading the Stack Highwater
	//	, NULL // Any pointer to pass in
	//	, 3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	//	, NULL);

	xTaskCreate(
		TaskReadSerial
		, (const portCHAR *)"ReadS3"  // A name just for humans
		, 1000  // This stack size can be checked & adjusted by reading the Stack Highwater
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
	//centerLCD.drawRGBBitmap(77, 0, image, 150, 150);
	//centerLCD.setAddrWindow(120, 80, 240, 160);
	//centerLCD.pushColors(image, 9600, true);
	//centerLCD.fillRect(240, 80, 120, 80, ILI9488_RED);
	/*
	// voltage is an indication of capacitor charge, goes right
	DisplayBar capVolt = DisplayBar(&centerLCD, 240+0, 160+30, 240, 80);
	// current flowing from Fuel Cell into Capacitor, goes right
	DisplayBar capCurrent_charge = DisplayBar(&centerLCD, 240+0, 160-30-80, 240, 80);
	// current drawn from capacitor, goes left
	DisplayBar capCurrent_discharge = DisplayBar(&centerLCD, 240-240, 160-30-80, 240, 80);
	// current drawn from capacitor + FC, goes left
	DisplayBar motorCurrent = DisplayBar(&centerLCD, 240-240, 160+30, 240, 80);

	// dummy
	DisplayText t = DisplayText(&centerLCD, 0, 0, 100, 40);

	t.setMargin(12);
	t.setColors(ILI9488_WHITE, ILI9488_BLACK);
	capVolt.setMargin(6);
	capCurrent_charge.setMargin(6);
	capCurrent_discharge.setMargin(6);
	motorCurrent.setMargin(6);

	capVolt.setRange(45, 60);
	capVolt.setOrientation(DisplayBar::LEFT_TO_RIGHT);
	capVolt.setColors(ILI9488_CYAN, ILI9488_BLACK);
	capCurrent_charge.setRange(0, 20);
	capCurrent_charge.setOrientation(DisplayBar::LEFT_TO_RIGHT);
	capCurrent_charge.setColors(ILI9488_DARKCYAN, ILI9488_BLACK);
	capCurrent_discharge.setRange(0, 20);
	capCurrent_discharge.setOrientation(DisplayBar::RIGHT_TO_LEFT);
	capCurrent_discharge.setColors(ILI9488_MAROON, ILI9488_BLACK);
	motorCurrent.setRange(0, 20);
	motorCurrent.setOrientation(DisplayBar::RIGHT_TO_LEFT);
	motorCurrent.setColors(ILI9488_RED, ILI9488_BLACK);

	while (1)
	{
	char randStr[4];
	itoa(random(45, 60), randStr, 10);
	capVolt.update(randStr);
	itoa(random(0, 20), randStr, 10);
	capCurrent_charge.update(randStr);
	itoa(random(0, 20), randStr, 10);
	capCurrent_discharge.update(randStr);
	itoa(random(0, 20), randStr, 10);
	motorCurrent.update(randStr);
	vTaskDelay(pdMS_TO_TICKS(300));

	}
	*/
	QueueItem received;
	char data[MAX_STRING_LEN];
	//ILI9488 centerLCD = ILI9488(CENTER_LCD_CS, CENTER_LCD_DC, CENTER_LCD_RST);
	//ILI9488 leftLCD = ILI9488(LEFT_LCD_CS, LEFT_LCD_DC, LEFT_LCD_RST);
	//pinMode(CENTER_LCD_LED, OUTPUT);
	//digitalWrite(CENTER_LCD_LED, HIGH);
	//centerLCD.begin();
	//leftLCD.begin();
	//centerLCD.setRotation(3);
	//leftLCD.setRotation(1);
	//centerLCD.fillScreen(ILI9488_PURPLE);
	//centerLCD.setTextColor(ILI9488_WHITE);
	//leftLCD.fillScreen(ILI9488_PURPLE);
	//leftLCD.setTextColor(ILI9488_WHITE);
	//DisplayText speedDisplay = DisplayText(&centerLCD, (480 - 200) / 2, (320 - 200) / 2, 200, 200);
	//DisplayText speedDisplay2 = DisplayText(&leftLCD, (480 - 200) / 2, (320 - 200) / 2, 200, 200);
	//speedDisplay.init();
	//speedDisplay.setColors(ILI9488_WHITE, ILI9488_PURPLE);
	//speedDisplay2.init();
	//speedDisplay2.setColors(ILI9488_WHITE, ILI9488_PURPLE);
	TickType_t delay = pdMS_TO_TICKS(200);
	uint32_t lastTick = 0; Serial.print("I AM READY!!");
	while (1)
	{
		//uint8_t rNum = random(0, 99);
		//char tmp[4];
		//itoa(rNum, tmp, 10);
		//lastTick = millis();
		//speedDisplay.update(tmp);
		//speedDisplay2.update(tmp);
		//Serial.println(millis() - lastTick);
		//BaseType_t success = xQueueReceive(queueForDisplay, &received, 0);
		if (false)
		{
			//sprintf(data, "%4.1f");
			received.toString(data);
			Serial.println(data);
			//Serial.println(data);
		}
		else
		{
			Serial.println("NUFFINK");
		}
		if (false)
		{
			//char payload[3 + 9 + (FLOAT_TO_STRING_LEN + 1)*(QUEUEITEM_DATAPOINTS*QUEUEITEM_READVALUES) + QUEUEITEM_DATAPOINTS];
			//switch (received.ID)
			//{
			//case FC:

			//	break;
			//case CS:

			//	break;
			//case SM:
			//	dtostrf(received.data[0][0], 4, 1, payload);
			//	Serial.print("Payload: ");
			//	Serial.println(payload);
			//	//speedDisplay.update(payload);
			//	break;
			//}
		}
		else
		{
			//Serial.println("NO!");
			//char errorVal[] = "NO!!";
			//speedDisplay.update(errorVal);
		}
		vTaskDelay(delay);
	}
}
void TaskReadSerial(void* pvParameters)
{
	Serial.println("Hi from Serial.");
	QueueItem outgoing;
	char payload[MAX_STRING_LEN];
	TickType_t delay = pdMS_TO_TICKS(200);
	Serial.println("Ready to receive.");
	while (1)
	{

		if (Serial3.available())
		{
			int bytesRead = Serial3.readBytesUntil('\n', payload, MAX_STRING_LEN);
			if(bytesRead>0)
				payload[bytesRead-1] = '\0';
			QueueItem::toQueueItem(payload, &outgoing);
			//xQueueSend(queueForDisplay, &outgoing, 100);

			// DEBUG printing to find out println actually prints "\r\n"
			int counter = 0;
			while (counter < bytesRead)
			{
				char tmp = payload[counter++];
				if (isPrintable(tmp))
					Serial.print(tmp);
				else
				{
					Serial.print("<");
					Serial.print((uint8_t)tmp);
					Serial.print(">");
				}
			}
			Serial.print("[");
			Serial.print(Serial.available());
			Serial.print("]");
			Serial.println("__");
		}
		vTaskDelay(delay);
	}
}