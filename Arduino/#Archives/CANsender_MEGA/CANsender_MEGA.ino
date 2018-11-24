/*
Name:		ScratchPad.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include <Arduino_FreeRTOS.h>

#include <SPI.h>

unsigned long id = 0xFF;
void TaskSend(void *pvParameters __attribute__((unused)));
MCP_CAN CAN0 = MCP_CAN(4);
bool CAN_incoming = false;
void CAN_ISR();
void setup() {
	Serial.begin(9600);
	delay(1000);
	if (CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
	{
		Serial.println("CAN sender MEGA initialized.");
		CAN0.setMode(MCP_NORMAL);
	}
	else
	{
		Serial.println("CAN INIT fail");
		while (1);
	}
	attachInterrupt(digitalPinToInterrupt(3), CAN_ISR, FALLING);

	xTaskCreate(
		TaskSend
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 400  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);

}

void loop() {

}

void TaskSend(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	byte msgLength = 8;
	byte outBuffer[8];
	char data[8] = "MEGA_YO";

	while (1) // A Task shall never return or exit.
	{
		// send payload:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'payload' = array of payload bytes to 
		for (int i = 0;i < 8;i++)
		{
			outBuffer[i] = data[i];
		}
		byte sndStat = CAN0.sendMsgBuf(id++, 0, 8, outBuffer);
		if (id > 0x1E0)id = 0xF0;
		if (sndStat == CAN_OK) {
			Serial.print("ID: ");
			Serial.println(id, HEX);
		}
		else {
			Serial.println("Error Sending Message...");
		}
		vTaskDelay(pdMS_TO_TICKS(5));   // send payload per 2000ms
	}
}
void CAN_ISR()
{
	if (CAN0.checkError() == CAN_OK)
	{
		CAN_incoming = true;
	}
}
