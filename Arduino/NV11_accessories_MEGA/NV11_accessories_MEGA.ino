/*
 Name:		NV11_accessories_MEGA.ino
 Created:	3/8/2019 3:57:21 PM
 Author:	MX
*/
#include <CANSerializer.h>
#include "NV10AccesoriesStatus.h"
#include "Pins_lights.h"
CANSerializer Can;
NV10AccesoriesStatus dataAcc = NV10AccesoriesStatus(0x10);
bool sigOn;
unsigned long nextTime = 0;
// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(LSIG_OUTPUT, OUTPUT);
	pinMode(RSIG_OUTPUT, OUTPUT);
	pinMode(BRAKELIGHT_OUTPUT, OUTPUT);
	pinMode(RUNNINGLIGHT_OUTPUT, OUTPUT);

	Can.init(CAN_SPI_CS);
	pinMode(CAN_INTERRUPT, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(CAN_INTERRUPT), CAN_ISR, FALLING);

	digitalWrite(RUNNINGLIGHT_OUTPUT, HIGH); // turn ON lights by default
	digitalWrite(BRAKELIGHT_OUTPUT, HIGH); // turn OFF brakelights by default
}

// loop function manages signal lights blinking
void loop() {
	// relay is PULLUP by default
	// writing HIGH enables NC, disables NO
	// writing LOW enables NO, disables NC
	if (millis() > nextTime)
	{
		nextTime += 500;
		if (sigOn)
		{
			// turn off signal lights
			sigOn = false;
			digitalWrite(LSIG_OUTPUT, HIGH);
			digitalWrite(RSIG_OUTPUT, HIGH);
		}
		else
		{
			// turn on signal lights if commanded to do so
			if (dataAcc.getHazard() == STATE_EN || dataAcc.getLsig() == STATE_EN)
			{
				sigOn = true;
				digitalWrite(LSIG_OUTPUT, LOW);
			}
			if (dataAcc.getHazard() == STATE_EN || dataAcc.getRsig() == STATE_EN)
			{
				sigOn = true;
				digitalWrite(RSIG_OUTPUT, LOW);
			}
		}
	}
	delay(10);
}

void CAN_ISR()
{
	CANFrame f;
	Can.receiveCanFrame(&f);
	if (dataAcc.checkMatchCAN(&f))
	{
		dataAcc.unpackCAN(&f);
		if (dataAcc.getBrake() == STATE_EN)
			digitalWrite(BRAKELIGHT_OUTPUT, LOW); // ON
		else
			digitalWrite(BRAKELIGHT_OUTPUT, HIGH); // OFF
		if (dataAcc.getHeadlights() == STATE_EN)
			digitalWrite(RUNNINGLIGHT_OUTPUT, HIGH); // ON
		else
			digitalWrite(RUNNINGLIGHT_OUTPUT, LOW); // OFF
	}
}