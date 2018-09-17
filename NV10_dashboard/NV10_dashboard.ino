/*
 Name:		NV10_dashboard.ino
 Created:	8/15/2018 1:35:26 PM
 Author:	MX
*/


#include <Arduino_FreeRTOS.h>
#include <queue.h>
//#include <FreeRTOS_ARM.h>
#include <SPI.h>

#include <mcp_can.h>
#include <ILI9488.h>

#include "Wiring_Dashboard.h"
// dependent header files
#include "FrameFormats.h"
// ----------------------

#include "DisplayContainer.h"
#include "DisplayBar.h""
#include "DisplayText.h"
#include "DisplayGauge.h"
//#include "Bitmaps.h"

void TaskRefreshScreen(void* pvParameters);
void TaskReadCAN(void* pvParameters);
TaskHandle_t ReadCAN;
QueueHandle_t queueForDisplay = xQueueCreate(1, sizeof(QueueItem));
MCP_CAN CANObj = MCP_CAN(CAN_CS_PIN);
volatile bool incoming = false;
void setup() {
	Serial.begin(9600);
	delay(100);
	attachInterrupt(digitalPinToInterrupt(CAN_INTERRUPT_PIN), CAN_incoming, FALLING);
	if (CANObj.begin(CAN_1000KBPS) != CAN_OK)
	{
		Serial.println(F("NV10_dashboard CAN init fail!"));
		while (1);
	}
	else
	{
		Serial.println(F("NV10_dashboard CAN init success!"));
	}
	pinMode(CENTER_LCD_LED, OUTPUT);
	digitalWrite(CENTER_LCD_LED, HIGH);
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
	xTaskCreate(
		TaskRefreshScreen
		, (const portCHAR *)"Refresh"  // A name just for humans
		, 700  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL // Any pointer to pass in
		, 3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);
	xTaskCreate(
		TaskReadCAN
		, (const portCHAR *)"ReadCAN"  // A name just for humans
		, 800  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL // Any pointer to pass in
		, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, &ReadCAN);

	//vTaskStartScheduler();
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
void CAN_incoming()
{
	if (CANObj.checkError() == CAN_OK)
		incoming = true;
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
	//ILI9488 centerLCD = ILI9488(CENTER_LCD_CS, CENTER_LCD_DC, CENTER_LCD_RST);
	//centerLCD.begin();
	//centerLCD.setRotation(1);
	//centerLCD.fillScreen(ILI9488_PURPLE);
	//centerLCD.setTextColor(ILI9488_WHITE);
	//DisplayText speedDisplay = DisplayText(&centerLCD, (480-200)/2, (320-200)/2, 200, 200);
	//speedDisplay.init();
	//speedDisplay.setColors(ILI9488_WHITE, ILI9488_PURPLE);
	TickType_t delay = pdMS_TO_TICKS(1200);
	while (1)
	{
		BaseType_t success = xQueueReceive(queueForDisplay, &received, 100);
		if (success == pdPASS)
		{
			char payload[3 + 9 + (FLOAT_TO_STRING_LEN + 1)*(QUEUEITEM_DATAPOINTS*QUEUEITEM_READVALUES) + QUEUEITEM_DATAPOINTS];
			switch (received.ID)
			{
			case FC:

				break;
			case CS:

				break;
			case SM:
				dtostrf(received.data[0][0], 4, 1, payload);
				Serial.print("Payload: ");
				Serial.println(payload);
				//speedDisplay.update(payload);
				break;
			}
		}
		else
		{
			Serial.println("NO!");
			//char errorVal[] = "NO!!";
			//speedDisplay.update(errorVal);
		}
		vTaskDelay(delay);
	}
}
void TaskReadCAN(void* pvParameters)
{
	

	byte buf[8];
	unsigned long id;
	byte len;

	QueueItem outgoing;
	uint8_t counter = 0;
	NV_CanFrames framesCollection;
	while (1)
	{
		if (incoming)
		{
			incoming = false;

			CANObj.readMsgBufID(&id, &len, buf);
			framesCollection.addItem(id, len, buf);

			if ((id & B11) == B11)
			{
				bool convertSuccess = framesCollection.toQueueItem(&outgoing);
				if (convertSuccess)
				{
					// print data
					//char payload[3 + 9 + (FLOAT_TO_STRING_LEN + 1)*(QUEUEITEM_DATAPOINTS*QUEUEITEM_READVALUES) + QUEUEITEM_DATAPOINTS];
					//outgoing.toString(payload);
					//Serial.println(payload);
					xQueueSend(queueForDisplay, &outgoing, 100);
				}
				else
				{
					//Serial.println("Conversion error...");
				}
				framesCollection.clear();
			}
			//else
			//{
			//	//Serial.print("Saved ");
			//	//Serial.println(id);
			//}

			//if ((id & B11) == B11) {
			//	// print raw frames
			//	int i = 0;
			//	do
			//	{
			//		Serial.print("Frame ");
			//		Serial.print(i);
			//		Serial.print(": ");
			//		Serial.print(framesCollection.frames[i].id);
			//		Serial.print(" ");
			//		Serial.print(framesCollection.frames[i].length);
			//		Serial.print("\n");
			//		for (int j = 0;j < framesCollection.frames[i].length;j++)
			//			Serial.println(framesCollection.frames[i].payload[j], 16);
			//		i++;
			//	}while ((framesCollection.frames[i-1].id & B11) != B11);
			//	framesCollection.clear();
			//}
		}
		//vTaskDelay(pdMS_TO_TICKS(150));
	}
}