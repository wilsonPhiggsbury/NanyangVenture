/*
Name:		CANtoSerial_UNO.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include <Arduino_FreeRTOS.h>
#include <Adafruit_GFX.h>
#include "FrameFormats.h"

#include <SPI.h>

// This sketch is meant for ATmega328P on breadboard, see "files for Arduino IDE" for the "ATmega328 on a breadboard (8 MHz internal clock)" board option
// Also useable on a normal Arduino UNO if the board option not found
// extended fuse:	0x05
// low fuse:		0xE2
// high fuse:		0xDA

void TaskReceiveCAN(void *pvParameters __attribute__((unused)));
MCP_CAN CANObj = MCP_CAN(4);
volatile int CAN_incoming = 0;
void CAN_ISR();
void setup() {
	Serial.begin(9600);
	delay(100);
	if (CANObj.begin(NV_CANSPEED) == CAN_OK)
	{
		Serial.print("Y");
	}
	else
	{
		Serial.print("N");
	}
	attachInterrupt(digitalPinToInterrupt(3), CAN_ISR, FALLING);

	xTaskCreate(
		TaskReceiveCAN
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 800  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);

}

void loop() {

}

void TaskReceiveCAN(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	unsigned long id;
	byte len;
	byte inBuffer[8];

	QueueItem incoming;
	NV_CanFrames frames;
	while (1) // A Task shall never return or exit.
	{
		if (CAN_incoming == 1)
		{
			// ------------------------------ covert CAN frames into String for Serial -----------------------------------
			CANObj.readMsgBufID(&id, &len, inBuffer);
			bool isLastFrame = frames.addItem(id, len, inBuffer);
			if (isLastFrame)
			{
				bool convertSuccess = frames.toQueueItem(&incoming);
				if (convertSuccess)
				{
					// print data
					char payload[MAX_STRING_LEN];
					incoming.toString(payload);
					Serial.println(payload);
				}
				frames.clear();
			}

			CAN_incoming = false;
		}
		else if (CAN_incoming == -1)
		{
			Serial.println("~");
			vTaskDelay(50);
		}
	}
}
void CAN_ISR()
{
	if (CANObj.checkError() == CAN_OK)
	{
		CAN_incoming = 1;
	}
	else
	{
		CAN_incoming = -1;
	}
}