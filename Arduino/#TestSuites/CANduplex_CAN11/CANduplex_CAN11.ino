/*
Name:		ScratchPad.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#define CAN_CS 7
#define CAN_INT 20
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <CAN_Serializer.h>
#include <SPI.h>

void TaskGenerate(void *pvParameters __attribute__((unused)));
void TaskCAN(void *pvParameters __attribute__((unused)));
QueueHandle_t queueForCAN = xQueueCreate(1, sizeof(Packet));
CAN_Serializer serializer = CAN_Serializer();
bool CAN_incoming = false;
void CAN_ISR();
void setup() {
	Serial.begin(9600);
	delay(1000);
	if (!serializer.init(CAN_CS))
	{
		Serial.print("CAN FAIL");
		while (1);
	}
	Serial.println("CAN Full Duplex terminal COM11.");
	attachInterrupt(digitalPinToInterrupt(CAN_INT), CAN_ISR, FALLING);

	xTaskCreate(
		TaskGenerate
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 600  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);
	xTaskCreate(
		TaskCAN
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 1200  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);

}

void loop() {

}
void TaskGenerate(void *pvParameters __attribute__((unused)))
{
	Packet out;
	while (1)
	{
		dummyData(&out, SM);
		xQueueSend(queueForCAN, &out, 100);
		vTaskDelay(50);
		dummyData(&out, CS);
		xQueueSend(queueForCAN, &out, 100);
		vTaskDelay(50);
		dummyData(&out, CS);
		xQueueSend(queueForCAN, &out, 100);
		vTaskDelay(50);
	}
}

void TaskCAN(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	Packet out, in;

	while (1) // A Task shall never return or exit.
	{
		BaseType_t success = xQueueReceive(queueForCAN, &out, 0);
		if (success)
		{
			if (!serializer.sendCAN(&out))
			{
				Serial.println(F("Dropped frame!"));
			}
			else
			{
				Serial.print("SENT ");
				printQ(&out);
			}
		}
		bool received = serializer.recvCAN(&in);
		if (received)
		{
			//Serial.print("RECV ");
			printQ(&in);
		}
		serializer.sendCAN_OneFrame();
		vTaskDelay(5);   // send payload per 5ms
	}
}
void CAN_ISR()
{
	serializer.recvCAN_OneFrame();
}

void dummyData(Packet* q, PacketID id) {
	q->ID = id;
	int i, j;
	i = FRAME_INFO_SETS[id];
	j = FRAME_INFO_SUBSETS[id];
	for (int _i = 0; _i < i; _i++)
	{
		for (int _j = 0; _j < j; _j++)
		{
			q->data[_i][_j] = random(0, 10);
		}
	}
	switch (id)
	{
	case FC:
		q->data[0][7] = random(0, 1);
		q->data[0][3] = random(0, 100);
		break;
	case CS:
		q->data[2][1] = random(0, 40);
		q->data[2][0] = random(45, 60);
		break;
	case SM:

		break;
	}

}
void printQ(Packet *q)
{
	char str[MAX_STRING_LEN];
	q->toString(str);
	Serial.println(str);
}