/*
 Name:		NV11_wiper_UNO.ino
 Created:	4/12/2019 8:49:29 PM
 Author:	MX
*/

#include <CANSerializer.h>
#include <NV11AccesoriesStatus.h>

#define CAN_CS 10
#define CAN_INT 2
#define WIPERRELAY_OUTPUT2 4
#define WIPERRELAY_OUTPUT1 5
CANSerializer serializer;
NV11AccesoriesStatus acc = NV11AccesoriesStatus(0x10);
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	serializer.init(CAN_CS);
	pinMode(WIPERRELAY_OUTPUT2, OUTPUT);
	pinMode(WIPERRELAY_OUTPUT1, OUTPUT);

	pinMode(CAN_INT, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(CAN_INT), CAN_ISR, FALLING);
}

// the loop function runs over and over again until power down or reset
void loop() {
	CANFrame f;
	//if (!digitalRead(CAN_INT))
	//{
	//	serializer.receiveCanFrame(&f);
	//	if (acc.checkMatchCAN(&f))
	//	{
	//		acc.unpackCAN(&f);
	//		switch (acc.getWiper())
	//		{
	//			// HH: rest
	//			// HL: rest
	//			// LH: slow
	//			// LL: fast
	//		case NV11AccesoriesStatus::wiperOff:
	//			Serial.println("Wiper Off");
	//			digitalWrite(WIPERRELAY_OUTPUT1, HIGH);
	//			digitalWrite(WIPERRELAY_OUTPUT2, HIGH);
	//			break;
	//		case NV11AccesoriesStatus::wiperSlow:
	//			Serial.println("Wiper Slow");
	//			digitalWrite(WIPERRELAY_OUTPUT1, LOW);
	//			digitalWrite(WIPERRELAY_OUTPUT2, HIGH);
	//			break;
	//		case NV11AccesoriesStatus::wiperFast:
	//			Serial.println("Wiper Fast");
	//			digitalWrite(WIPERRELAY_OUTPUT1, LOW);
	//			digitalWrite(WIPERRELAY_OUTPUT2, LOW);
	//			break;
	//		default:
	//			Serial.println("Wiper Error");
	//			break;
	//		}
	//	}
	//}

	Serial.println("Wiper OFF");
	digitalWrite(WIPERRELAY_OUTPUT2, HIGH);
	digitalWrite(WIPERRELAY_OUTPUT1, HIGH);
	delay(2000);
	Serial.println("Wiper Slow");
	digitalWrite(WIPERRELAY_OUTPUT2, LOW);
	digitalWrite(WIPERRELAY_OUTPUT1, HIGH);
	delay(2000);
	Serial.println("Wiper Fast");
	digitalWrite(WIPERRELAY_OUTPUT2, LOW);
	digitalWrite(WIPERRELAY_OUTPUT1, LOW);
	delay(2000);
	delay(100);
}
void CAN_ISR()
{
}