/*
 Name:		NV11_steeringwheel_UNO.ino
 Created:	4/3/2019 1:28:11 AM
 Author:	MX
*/
#include <CANSerializer.h>
#include <NV11AccesoriesStatus.h>
#include <NV11Commands.h>
#include "Pins_steeringwheel.h"
CANSerializer serializer;
NV11AccesoriesStatus dataAcc(0x10);
NV11Commands dataCommands(0x11);
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	if (!serializer.init(CAN_SPI_CS))
		Serial.println("CAN init fail");
	else
		Serial.println("CAN init success");

	for (int i = 2; i <= 9; i++)
	{
		pinMode(i, INPUT_PULLUP);
	}
}

// the loop function runs over and over again until power down or reset
void loop() {
	Serial.println("--------ACTIVE LOW BUTTONS--------");
	for (int i = 2; i <= 9; i++)
	{
		if (!digitalRead(i))
		{
			Serial.print(i);
			Serial.print('\t');
		}
	}
	Serial.println();
	delay(1000);

	//CANFrame f;
	//// ... code to populate dataAcc (eg: dataAcc.setLsig(STATE_EN))
	//dataCommands.clearActivationHistory();
	//// ... code to populate dataCommands (eg: dataCommands.activateHorn())
	//dataAcc.packCAN(&f);
	//serializer.sendCanFrame(&f);
	//dataCommands.packCAN(&f);
	//serializer.sendCanFrame(&f);
}
