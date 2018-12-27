/*
 Name:		DUE_comms_test.ino
 Created:	11/30/2018 11:45:44 AM
 Author:	MX
*/
#include <CAN_Serializer.h>
#include "Pins_dashboard.h"
Packet q;
int rst = 0;
const int RST = 17;// 28;
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
		rst++;
		if (rst >= 20)
		{
			Serial.println("RESET!");
			digitalWrite(RST, LOW);
			delay(200);
			digitalWrite(RST, HIGH);
			rst = 0;
		}
	}
	if (CAN_Serializer::receiveSerial(Serial, &q))
	{
		CAN_Serializer::sendSerial(Serial1, &q);
	}
	delay(100);

}
