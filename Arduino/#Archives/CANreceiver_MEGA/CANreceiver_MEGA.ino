/*
Name:		ScratchPad.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#include <mcp_can.h>
#include <CAN_Serializer.h>
#include <Arduino_FreeRTOS.h>
#include <Adafruit_GFX.h>

#include <SPI.h>

void TaskRecv(void *pvParameters __attribute__((unused)));
CAN_Serializer serializer = CAN_Serializer(7);
void CAN_ISR();
void setup() {
	Serial.begin(9600);
	delay(1000);

	serializer.init();
	serializer.onlyListenFor(CS);
	attachInterrupt(digitalPinToInterrupt(20), CAN_ISR, FALLING);

	xTaskCreate(
		TaskRecv
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 500  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);

}

void loop() {

}

void TaskRecv(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	QueueItem in;
	char inStr[MAX_STRING_LEN];
	while (1)
	{
		if (serializer.recv(&in))
		{
			in.toString(inStr);
			Serial.print("RECV ");
			Serial.println(inStr);
		}
		vTaskDelay(pdMS_TO_TICKS(5));
	}
}
void CAN_ISR()
{
	serializer.recvOneFrame();
}