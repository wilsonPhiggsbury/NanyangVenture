/*
 Name:		CAN_sender.ino
 Created:	11/5/2018 10:18:01 PM
 Author:	MX
*/
#include "FrameFormats.h"
#include <Arduino_FreeRTOS.h>
#define NUMSTATES 6
QueueItem q;
MCP_CAN CANObj = MCP_CAN(48);
void TaskSimulateButtons(void* pvParameters);
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	q.ID = BT;
	q.timeStamp = 1000;
	pinMode(LED_BUILTIN, OUTPUT);
	for (int i = 0; i < NUMSTATES; i++)
	{
		q.data[0][i] = 0;
	}
	if (CANObj.begin(NV_CANSPEED) != CAN_OK)
	{
		Serial.println(F("NV10_back CAN init fail!"));
	}
	xTaskCreate(TaskSimulateButtons, "SIM", 800, NULL, 1, NULL);
}

// the loop function runs over and over again until power down or reset
void loop() {
	
}
void TaskSimulateButtons(void* pvParameters)
{
	NV_CanFrames out;
	while (1)
	{
		q.ID = BT;
		Serial.println("ON");
		digitalWrite(LED_BUILTIN, HIGH);
		sendButtonCommands(Headlights, true);
		vTaskDelay(pdMS_TO_TICKS(800));
		vTaskDelay(pdMS_TO_TICKS(1300));
		Serial.println("OFF");
		digitalWrite(LED_BUILTIN, LOW);
		sendButtonCommands(Headlights, false);
		vTaskDelay(pdMS_TO_TICKS(3000));

		q.ID = SM;
		Serial.println("++");
		digitalWrite(LED_BUILTIN, HIGH);
		for (int i = 0; i < 1; i++)
		{
			dummyData(&q, FC);
			vTaskDelay(pdMS_TO_TICKS(5));
		}
		vTaskDelay(pdMS_TO_TICKS(5));
		Serial.println("--");
		digitalWrite(LED_BUILTIN, LOW);
		for (int i = 0; i < 2; i++)
		{
			dummyData(&q, CS);
			vTaskDelay(pdMS_TO_TICKS(5));
		}
		vTaskDelay(pdMS_TO_TICKS(3000));
	}

}
void sendButtonCommands(Peripheral p, bool set)
{
	NV_CanFrames out;
	q.data[0][p] = set ? 1 : 0;
	q.toFrames(&out);
	sendCAN(out);
}
void sendCAN(NV_CanFrames& out)
{
	for (uint8_t i = 0; i < out.getNumFrames(); i++)
	{
		NV_CanFrame& thisFrame = out.frames[i];
		byte status = CANObj.sendMsgBuf(thisFrame.id, 0, thisFrame.length, thisFrame.payload);

		if (status != CAN_OK)
		{
			// handle sending error
			Serial.print("Unsent ID:");
			Serial.println(q.ID);
		}
		else
		{
		}
		vTaskDelay(pdMS_TO_TICKS(5));
	}
	out.clear();
}
void dummyData(QueueItem* q, DataSource id) {
	q->ID = id;
	int i, j;
	switch (id)
	{
	case FC:
		i = 2; j = 8;
		break;
	case CS:
		i = 3; j = 2;
		break;
	case SM:
		i = 1; j = 1;
		break;
	}
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

	NV_CanFrames out;
	q->toFrames(&out);
	sendCAN(out);
}