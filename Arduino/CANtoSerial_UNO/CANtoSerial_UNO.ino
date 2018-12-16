/*
Name:		CANtoSerial_UNO.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#include <CAN_Serializer.h>
#define CAN_CS_PIN 4
#define CAN_INT_PIN 3
// This sketch is meant for ATmega328P on breadboard, see "files for Arduino IDE" for the "ATmega328 on a breadboard (8 MHz internal clock)" board option
// Also useable on a normal Arduino UNO if the board option not found
// extended fuse:	0x05
// low fuse:		0xE2
// high fuse:		0xDA

CAN_Serializer serializer;
Packet out, in;
char str[MAX_STRING_LEN];

void CAN_ISR();
void setup() {
	Serial.begin(9600);
	delay(100);
	serializer.init(CAN_CS_PIN);
	pinMode(CAN_INT_PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), CAN_ISR, FALLING);
}

void loop() {
	// anything to send?
	if (serializer.recvSerial(Serial, &out))
	{
		serializer.sendCAN(&out);
	}
	// anything to receive?
	if (serializer.recvCAN(&in))
	{
		serializer.sendSerial(Serial, &in);
	}
	// pulse one frame out
	serializer.sendCAN_OneFrame();
}
void CAN_ISR()
{
	// pulse one frame in
	serializer.recvCAN_OneFrame();
}