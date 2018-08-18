/*
 Name:		LookupTable.ino
 Created:	8/17/2018 10:08:21 AM
 Author:	MX
*/
#include "Wiring.h"
#include "CurrentSensorLogger.h"
AttopilotCurrentSensor motors[NUM_CURRENTSENSORS] = {
	AttopilotCurrentSensor(0,L_WHEEL_VPIN,L_WHEEL_APIN),
	AttopilotCurrentSensor(1,R_WHEEL_VPIN,R_WHEEL_APIN),
	AttopilotCurrentSensor(2,SUPERCAP_VPIN,SUPERCAP_APIN)
};
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	delay(500);
}

// the loop function runs over and over again until power down or reset
void loop() {
	int v, a;

	Serial.print(F("L. Wheel:\t"));
	analogReadVA(L_WHEEL_VPIN, L_WHEEL_APIN, &v, &a);
	printVA(v, a);

	Serial.print(F("R. Wheel:\t"));
	analogReadVA(R_WHEEL_VPIN, R_WHEEL_APIN, &v, &a);
	printVA(v, a);

	Serial.print(F("SuperCap:\t"));
	analogReadVA(SUPERCAP_VPIN, SUPERCAP_APIN, &v, &a);
	printVA(v, a);

}
void analogReadVA(uint8_t vpin, uint8_t apin, int* vDest, int* aDest)
{
	*vDest = analogRead(vpin);
	*aDest = analogRead(apin);
}
void printVA(int& vDest, int& aDest)
{
	Serial.print(vDest);
	Serial.print("\t");
	Serial.print(aDest);
	Serial.println();
}