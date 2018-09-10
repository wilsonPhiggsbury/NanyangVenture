/*
Name:		ScratchPad.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#include <mcp_can.h>
#include <Arduino_FreeRTOS.h>
#include <Adafruit_GFX.h>

#include <SPI.h>

void TaskQueueOutputData(void *pvParameters __attribute__((unused)));
MCP_CAN CAN0 = MCP_CAN(48);
volatile int CAN_incoming = 0;
void CAN_ISR();
void setup() {
	Serial.begin(9600);
	delay(1000);
	if (CAN0.begin(CAN_500KBPS) == CAN_OK)
	{
		Serial.println("CAN receiver MEGA initialized.");
	}
	else
	{
		Serial.println("CAN error.");
	}
	attachInterrupt(digitalPinToInterrupt(19), CAN_ISR, FALLING);

	xTaskCreate(
		TaskQueueOutputData
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 400  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);

}

void loop() {

}

void TaskQueueOutputData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	uint8_t msgLength = 8;
	byte inBuffer[8];
	while (1) // A Task shall never return or exit.
	{
		if (CAN_incoming == 1)
		{
			CAN0.readMsgBuf(&msgLength, inBuffer);
			Serial.print("Received data from source ");
			Serial.println(CAN0.getCanId());
			for (int i = 0; i < msgLength; i++)
			{
				Serial.print((char)inBuffer[i]);
			}
			Serial.println();
			CAN_incoming = false;
		}
		else if (CAN_incoming == -1)
		{
			Serial.println("CAN receive error.");
			vTaskDelay(50);
		}
	}
}
void CAN_ISR()
{
	if (CAN0.checkError() == CAN_OK)
	{
		CAN_incoming = 1;
	}
	else
	{
		CAN_incoming = -1;
	}
}