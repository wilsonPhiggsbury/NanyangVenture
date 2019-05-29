/*
Name:		CANtoSerial_UNO.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#include <CAN_Serializer.h>
#define CAN_SPI_CS 4
#define CAN_INTERRUPT 3
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
	if (!serializer.init(CAN_SPI_CS))
	{
		delay(1000);
		asm volatile ("  jmp 0");
	}
	pinMode(CAN_INTERRUPT, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(CAN_INTERRUPT), CAN_ISR, FALLING);
}

void loop() {
	// anything to send?
	if (serializer.receiveSerial(Serial, &out))
	{
		serializer.sendCanPacket(&out);
	}
	// anything to receive?
	if (serializer.receiveCanPacket(&in))
	{
		serializer.sendSerial(Serial, &in);
	}
	delay(10);
}
void CAN_ISR()
{
	// pulse one frame in
	//serializer.recvCAN_OneFrame();
}