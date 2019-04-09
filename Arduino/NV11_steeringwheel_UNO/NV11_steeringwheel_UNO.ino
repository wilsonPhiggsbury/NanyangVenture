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
	pinMode(WIPER_INPUT1, INPUT_PULLUP);
	pinMode(WIPER_INPUT2, INPUT_PULLUP);
	pinMode(FOURWSS_INPUT1, INPUT_PULLUP);
	pinMode(FOURWSS_INPUT2, INPUT_PULLUP);
	pinMode(HAZARD_INPUT, INPUT_PULLUP);
	pinMode(HEADLIGHT_INPUT, INPUT_PULLUP);

	pinMode(HORN_INPUT, INPUT_PULLUP);
	pinMode(LSIG_INPUT, INPUT_PULLUP);
	pinMode(RSIG_INPUT, INPUT_PULLUP);
	pinMode(REGEN_INPUT, INPUT_PULLUP);
	pinMode(SPARE1_INPUT, INPUT_PULLUP);
	pinMode(SPARE2_INPUT, INPUT_PULLUP);
	pinMode(SPARE3_INPUT, INPUT_PULLUP);

	if (!serializer.init(CAN_SPI_CS))
		Serial.println("CAN init fail");
	else
		Serial.println("CAN init success");
	attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), CAN_ISR, FALLING);
}

// the loop function runs over and over again until power down or reset
void loop() {
	CANFrame f;

	// ... code to populate dataAcc (eg: dataAcc.setLsig(STATE_EN))
	if (!digitalRead(HEADLIGHT_INPUT))
		dataAcc.setHeadlights(NV11AccesoriesStatus::enable);
	else
		dataAcc.setHeadlights(NV11AccesoriesStatus::disable);

	if (!digitalRead(HAZARD_INPUT))
		dataAcc.setHazard(NV11AccesoriesStatus::enable);
	else
		dataAcc.setHazard(NV11AccesoriesStatus::disable);

	if (!digitalRead(LSIG_INPUT))
		dataAcc.setLsig(NV11AccesoriesStatus::enable);
	else
		dataAcc.setLsig(NV11AccesoriesStatus::disable);

	if (!digitalRead(RSIG_INPUT))
		dataAcc.setRsig(NV11AccesoriesStatus::enable);
	else
		dataAcc.setRsig(NV11AccesoriesStatus::disable);

	if (!digitalRead(REGEN_INPUT))
		dataAcc.setRegen(NV11AccesoriesStatus::enable);
	else
		dataAcc.setRegen(NV11AccesoriesStatus::disable);

	if (!digitalRead(FOURWSS_INPUT1))
		dataAcc.setFourWS(NV11AccesoriesStatus::fourWScounterPhase); // left
	else if (!digitalRead(FOURWSS_INPUT2))
		dataAcc.setFourWS(NV11AccesoriesStatus::fourWSinPhase); // right
	else
		dataAcc.setFourWS(NV11AccesoriesStatus::fourWSfrontOnly); // center

	if (!digitalRead(WIPER_INPUT1))
		dataAcc.setWiper(NV11AccesoriesStatus::wiperOff); // left
	else if (!digitalRead(WIPER_INPUT2))
		dataAcc.setWiper(NV11AccesoriesStatus::wiperFast); // right
	else
		dataAcc.setWiper(NV11AccesoriesStatus::wiperSlow); // center

	if (dataAcc.dataHasChanged())
	{
		dataAcc.packCAN(&f);
		serializer.sendCanFrame(&f);
	}
	delay(100);
}
void CAN_ISR()
{

}
//void printButtons()
//{
//
//	Serial.println("--------ACTIVE LOW BUTTONS--------");
//	for (int i = 2; i <= 9; i++)
//	{
//		if (!digitalRead(i))
//		{
//			Serial.print(i);
//			Serial.print('\t');
//		}
//	}
//	for (int i = A0; i <= A5; i++)
//	{
//		if (!digitalRead(i))
//		{
//			Serial.print('A');
//			Serial.print(i-A0);
//			Serial.print('\t');
//		}
//	}
//	Serial.println();
//}


	//// ... code to populate dataCommands (eg: dataCommands.activateHorn())
	//dataCommands.clearActivationHistory();
	//if (!digitalRead(HORN_INPUT))
	//{
	//	dataCommands.activateHorn();
	//}
	//
	//if (dataCommands.getHorn() == NV11AccesoriesStatus::enable)
	//{
	//	dataCommands.packCAN(&f);
	//	serializer.sendCanFrame(&f);
	//}