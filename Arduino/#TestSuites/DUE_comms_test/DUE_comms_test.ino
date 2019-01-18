/*
 Name:		DUE_comms_test.ino
 Created:	11/30/2018 11:45:44 AM
 Author:	MX
*/
#include <CAN_Serializer.h>
#include "Pins_dashboard.h"
Packet q;
int rst = 0, sendCounter = 0;
int sendThreshold = 20;
const int RST = 28;
unsigned long lastTime = 0;
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	Serial1.begin(9600);
	pinMode(RST, OUTPUT);
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (CAN_Serializer::receiveSerial(Serial1, &q))
	{
		CAN_Serializer::sendSerial(Serial, &q); rst = 0;
	}
	else
	{
		if (++rst >= 40)
		{
			Serial.print("RESET!");
			Serial.print(" Alive for "); Serial.print(millis() - lastTime); Serial.println(" ms");
			lastTime = millis();
			digitalWrite(RST, LOW);
			delay(200);
			digitalWrite(RST, HIGH);
			rst = 0;
		}
	}
	if (++sendCounter >= sendThreshold)
	{
		sendCounter = 0;
		sendThreshold = random(10, 20);
		dummyData(&q, BT);
		CAN_Serializer::sendSerial(Serial1, &q);
		//if (CAN_Serializer::receiveSerial(Serial, &q))
		//{
		//	CAN_Serializer::sendSerial(Serial1, &q);
		//}
	}
	delay(100);

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