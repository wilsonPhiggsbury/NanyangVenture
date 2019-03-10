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
// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(LSIG_OUTPUT, OUTPUT);
	pinMode(RSIG_OUTPUT, OUTPUT);
	pinMode(BRAKELIGHT_OUTPUT, OUTPUT);
	pinMode(RUNNINGLIGHT_OUTPUT, OUTPUT);

	pinMode(CAN_INTERRUPT, INPUT_PULLUP);
	Can.init(CAN_SPI_CS);
	attachInterrupt(CAN_INTERRUPT, CAN_ISR, FALLING);
	
}

// loop function manages signal lights blinking
void loop() {
	if (sigOn)
	{
		sigOn = false;
		digitalWrite(LSIG_OUTPUT, LOW);
		digitalWrite(RSIG_OUTPUT, LOW);
	}
	else
	{
		if (dataAcc.getHazard() == STATE_EN || dataAcc.getLsig() == STATE_EN)
		{
			sigOn = true;
			digitalWrite(LSIG_OUTPUT, HIGH);
		}
		if (dataAcc.getHazard() == STATE_EN || dataAcc.getRsig() == STATE_EN)
		{
			sigOn = true;
			digitalWrite(RSIG_OUTPUT, HIGH);
		}
	}
	delay(500);
}

void CAN_ISR()
{
	CANFrame f;
	Can.receiveCanFrame(&f);
	if (dataAcc.checkMatchCAN(&f))
	{
		dataAcc.unpackCAN(&f);
		if (dataAcc.getBrake() == STATE_EN)
			digitalWrite(BRAKELIGHT_OUTPUT, HIGH);
		else
			digitalWrite(BRAKELIGHT_OUTPUT, LOW);
		if (dataAcc.getHeadlights() == STATE_EN)
			digitalWrite(RUNNINGLIGHT_OUTPUT, HIGH);
		else
			digitalWrite(RUNNINGLIGHT_OUTPUT, LOW);
	}
}