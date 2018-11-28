/*
Name:		CANtoSerial_UNO.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#include <CAN_Serializer.h>

// This sketch is meant for ATmega328P on breadboard, see "files for Arduino IDE" for the "ATmega328 on a breadboard (8 MHz internal clock)" board option
// Also useable on a normal Arduino UNO if the board option not found
// extended fuse:	0x05
// low fuse:		0xE2
// high fuse:		0xDA

CAN_Serializer serializer = CAN_Serializer(7);//4
QueueItem out, in;
char str[MAX_STRING_LEN];

void CAN_ISR();
void setup() {
	Serial.begin(9600);
	delay(100);
	serializer.init();
	//serializer.onlyListenFor(BT);
	attachInterrupt(digitalPinToInterrupt(20), CAN_ISR, FALLING);//3
}

void loop() {
	// anything to send?
	if (Serial.available())
	{
		int bytesRead = Serial.readBytesUntil('\n', str, MAX_STRING_LEN);
		if (bytesRead > 0)
			str[bytesRead - 1] = '\0';
		// --------------------------- covert to intermediate representation "QueueItem" --------------------------------
		bool convertSuccess = QueueItem::toQueueItem(str, &out);
		if (convertSuccess)
		{
			if (!serializer.send(&out))
			{
				Serial.println(F("!!"));
			}
		}
		else
		{
			//Serial.println(F("Can't convert to queueitem."));
		}
	}
	// anything to receive?
	bool received = serializer.recv(&in);
	if (received)
	{
		in.toString(str);
		Serial.println(str);
	}
	// pulse one frame out
	serializer.sendOneFrame();
}
void CAN_ISR()
{
	// pulse one frame in
	serializer.recvOneFrame();
}