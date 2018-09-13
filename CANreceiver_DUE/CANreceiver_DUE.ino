/*
Name:		ScratchPad.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include <FreeRTOS_ARM.h>
#include <Adafruit_GFX.h>

#include <SPI.h>

void QueueOutputData(void *pvParameters __attribute__((unused)));
MCP_CAN CANObj = MCP_CAN(4);
volatile int CAN_incoming = 0;
void CAN_ISR();
void setup() {
	Serial.begin(9600);
	delay(1000);
	if (CANObj.begin(CAN_250KBPS) == CAN_OK)
	{
		Serial.println("CAN receiver DUE initialized.");
	}
	else
	{
		Serial.println("CAN error.");
	}
	attachInterrupt(digitalPinToInterrupt(3), CAN_ISR, FALLING);

	xTaskCreate(
		QueueOutputData
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 400  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);
	vTaskStartScheduler();
}

void loop() {

}

void QueueOutputData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	uint8_t msgLength = 8;
	byte inBuffer[8];
	while (1) // A Task shall never return or exit.
	{
		if (CAN_incoming == 1)
		{
			CANObj.readMsgBuf(&msgLength, inBuffer);
			Serial.print("Received data from source ");
			Serial.println(CANObj.getCanId());
			Serial.println("CONTENT:");
			for (int i = 0; i < msgLength; i++)
			{
				if (isPrintable(inBuffer[i]))
					Serial.print((char)inBuffer[i]);
				else
					Serial.print((int)inBuffer[i]);
			}
			Serial.println();
		}
		else if (CAN_incoming == -1)
		{
			Serial.println("CAN receive error.");
		}
		else
		{
			Serial.println("Alive.");
		}
		CAN_incoming = 0;
		vTaskDelay(pdMS_TO_TICKS(500));
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