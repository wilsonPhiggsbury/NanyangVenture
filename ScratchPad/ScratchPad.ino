/*
 Name:		ScratchPad.ino
 Created:	6/24/2018 11:37:49 PM
 Author:	MX
*/
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include <Arduino_FreeRTOS.h>
#include <Adafruit_GFX.h>

#include <SPI.h>

void TaskQueueOutputData(void *pvParameters __attribute__((unused)));
MCP_CAN CAN0 = MCP_CAN(7);
bool CAN_incoming = false;
void CAN_ISR();
void setup() {
	if (CAN0.begin(CAN_1000KBPS) == CAN_OK)
	{
		CAN0.mcp2515_modifyRegister()
	}
	attachInterrupt(digitalPinToInterrupt(20), CAN_ISR, FALLING);
	Serial.begin(9600);
	delay(1000);

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
	uint8_t msgLength;

	while (1) // A Task shall never return or exit.
	{
		if (CAN_incoming)
		{
			CAN0.readMsgBuf(&msgLength, )
			CAN_incoming = false;
		}
	}
}
void CAN_ISR()
{
	CAN_incoming = true;
}