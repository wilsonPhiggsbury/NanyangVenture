/*
 Name:		NV10_dashboard.ino
 Created:	8/15/2018 1:35:26 PM
 Author:	MX
*/


// the setup function runs once when you press reset or power the board
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <mcp_can.h>
#include <ILI9488.h>

#include "Wiring_Dashboard.h"
#include "Behaviour_Dashboard.h"
// dependent header files
#include "CAN_ID_protocol.h"
// ----------------------

#include "DisplayContainer.h"
#include "DisplayBar.h""
#include "DisplayText.h"
#include "DisplayGauge.h"
//#include "Bitmaps.h"

const int screenLED = 9;

void TaskRefreshScreen(void* pvParameters);
void TaskReadCAN(void* pvParameters);
TaskHandle_t ReadCAN;
QueueHandle_t queueForDisplay = xQueueCreate(1, sizeof(QueueItem));
MCP_CAN CANObj = MCP_CAN(CAN_CS_PIN);
volatile bool incoming = false;
void setup() {
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
		, 250  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL // Any pointer to pass in
		, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);
	xTaskCreate(
		TaskReadCAN
		, (const portCHAR *)"ReadCAN"  // A name just for humans
		, 500  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL // Any pointer to pass in
		, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, &ReadCAN);

	vTaskStartScheduler();
	while (1)
	{
		Serial.println(F("NV10_DASHBOARD IS SLEEPING BECAUSE CAN_INIT FAIL..."));
		delay(2000);
	}
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
void CAN_incoming()
{
	incoming = true;
}
void TaskRefreshScreen(void* pvParameters)
{
	QueueItem received;
	ILI9488 leftScreen = ILI9488(10, 7, 8);
	leftScreen.begin();
	leftScreen.setRotation(1);
	leftScreen.fillScreen(ILI9488_BLACK);
	//leftScreen.drawRGBBitmap(77, 0, image, 150, 150);
	//leftScreen.setAddrWindow(120, 80, 240, 160);
	//leftScreen.pushColors(image, 9600, true);
	//leftScreen.fillRect(240, 80, 120, 80, ILI9488_RED);
	Serial.println("Screen1 initialized.");
	/*
	// voltage is an indication of capacitor charge, goes right
	DisplayBar capVolt = DisplayBar(&leftScreen, 240+0, 160+30, 240, 80);
	// current flowing from Fuel Cell into Capacitor, goes right
	DisplayBar capCurrent_charge = DisplayBar(&leftScreen, 240+0, 160-30-80, 240, 80);
	// current drawn from capacitor, goes left
	DisplayBar capCurrent_discharge = DisplayBar(&leftScreen, 240-240, 160-30-80, 240, 80);
	// current drawn from capacitor + FC, goes left
	DisplayBar motorCurrent = DisplayBar(&leftScreen, 240-240, 160+30, 240, 80);

	// dummy
	DisplayText t = DisplayText(&leftScreen, 0, 0, 100, 40);

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
	TickType_t delay = pdMS_TO_TICKS(300);
	while (1)
	{
		BaseType_t success = xQueueReceive(queueForDisplay, &received, 0);
		if (success == pdPASS)
		{

		}
		vTaskDelay(delay);
	}
}
void TaskReadCAN(void* pvParameters)
{
	

	byte buf[8];
	uint32_t id;
	byte len;

	QueueItem outgoing;
	uint8_t counter = 0;
	while (1)
	{
		Serial.println("ALIVE");
		if (incoming)
		{
			incoming = false;
			if (CANObj.checkError() != CAN_OK)
			{
				Serial.println("Data spoilt...");
			}
			byte status = CANObj.readMsgBuf(&len, buf);
			if (status != CAN_OK)
				Serial.println("No data...");
			// Populate outgoing data string. id's first bit is 1 means end of msg
			while (status == CAN_OK && CANObj.getCanId() & 1 != 1)
			{
				strcpy(outgoing.data + counter, "\t");
				counter += 1;
				memcpy(outgoing.data + counter, buf, len);
				counter += len;
				CANObj.readMsgBuf(&len, buf);
			}
			strcpy(outgoing.data + counter, "\0");

			// Populate outgoing data type.
			if (id & 2)
				outgoing.ID = CS;
			else
				outgoing.ID = FC;
			Serial.print("Recv ID:");
			Serial.println(id);
			Serial.print("Content:\n");
			Serial.println(outgoing.data);
			//xQueueSend(queueForDisplay, &outgoing, 100);
			counter = 0;
		}
		
		vTaskDelay(pdMS_TO_TICKS(150));
	}
}