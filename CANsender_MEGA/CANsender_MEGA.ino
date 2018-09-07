/*
Name:		ScratchPad.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include <Arduino_FreeRTOS.h>

#include <SPI.h>

void QueueOutputData(void *pvParameters __attribute__((unused)));
MCP_CAN CAN0 = MCP_CAN(48);
bool CAN_incoming = false;
void CAN_ISR();
void setup() {
	Serial.begin(9600);
	delay(1000);
	if (CAN0.begin(CAN_250KBPS) == CAN_OK)
	{
		Serial.println("CAN sender MEGA initialized.");
	}
	else
	{
		Serial.println("CAN INIT fail");
		while (1);
	}
	attachInterrupt(digitalPinToInterrupt(19), CAN_ISR, FALLING);

	xTaskCreate(
		QueueOutputData
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 400  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);

}

void loop() {

}

void QueueOutputData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	uint8_t msgLength = 8;
	byte outBuffer[8];
	char data[8] = "MEGA_YO";

	while (1) // A Task shall never return or exit.
	{
		// send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to 
		for (int i = 0;i < 8;i++)
		{
			outBuffer[i] = data[i];
		}
		byte sndStat = CAN0.sendMsgBuf(0x004, 0, 8, outBuffer);
		if (sndStat == CAN_OK) {
			Serial.println("Message Sent Successfully!");
		}
		else {
			Serial.println("Error Sending Message...");
		}
		delay(2000);   // send data per 2000ms
	}
}
void CAN_ISR()
{
	if (CAN0.checkError() == CAN_OK)
	{
		CAN_incoming = true;
	}
}
