/*
 Name:		Sketch1.ino
 Created:	6/18/2018 11:41:26 AM
 Author:	MX
*/

#include "HydrogenCellLogger.h"
#include "src\LocalHeader1.h"

Timer t;
HydrogenCellLogger *hydrocell_master;



// methods below here
//

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	while (!Serial);
	Serial.println("Serial port opened.");
	delay(1000);
	hydrocell_master = new HydrogenCellLogger(Serial1, &t);
	hydrocell_master->init();
}

// the loop function runs over and over again until power down or reset
void loop() {
	t.update();
	Serial.println("Hi from main.ino!");
	if (Serial1)Serial.println("Serial1 is open!");
}
