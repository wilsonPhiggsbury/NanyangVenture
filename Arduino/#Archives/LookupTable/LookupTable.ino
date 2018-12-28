/*
 Name:		LookupTable.ino
 Created:	8/17/2018 10:08:21 AM
 Author:	MX
*/
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	delay(500);
}

// the loop function runs over and over again until power down or reset
void loop() {
	
	int capOut, motor;
	analogReadVA(A3, A5, &capOut, &motor);
	printVA(capOut, motor);
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