/*
 Name:		DUE_comms_test.ino
 Created:	11/30/2018 11:45:44 AM
 Author:	MX
*/
#include <CAN_Serializer.h>
Packet q;
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	Serial1.begin(9600);
}

// the loop function runs over and over again until power down or reset
void loop() {
	char str[MAX_STRING_LEN];
	size_t readBytes;
	if (CAN_Serializer::recvSerial(Serial1, &q))
	{
		CAN_Serializer::sendSerial(Serial, &q);
	}
	if (CAN_Serializer::recvSerial(Serial, &q))
	{
		CAN_Serializer::sendSerial(Serial1, &q);
	}
	delay(100);

}
