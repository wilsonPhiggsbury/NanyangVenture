/*
    Name:       DUE_button_test_raw.ino
    Created:	29/1/2019 7:18:06 PM
    Author:     DESKTOP-GV1MS6E\MX
*/


void setup()
{
	Serial.begin(9600);
	for (int i = 22; i < 42; i += 1)
	{
		pinMode(i, INPUT_PULLUP);
	}

}

void loop()
{

	for (int i = 22; i < 42; i += 1)
	{
		bool st = digitalRead(i);
		if (st == LOW)
		{
			Serial.print(i);
			Serial.print('\t');
		}
	}
	delay(100);
}
