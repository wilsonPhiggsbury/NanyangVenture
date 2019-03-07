/*
    Name:       NV11_CANtoSerial_UNO.ino
    Created:	8/3/2019 12:06:26 AM
    Author:     DESKTOP-GV1MS6E\MX
*/

#include <CANSerializer.h>
#include <NV10AccesoriesStatus.h>
CANSerializer serializer;
NV10AccesoriesStatus dataAccesories = NV10AccesoriesStatus(0x010);
void setup()
{
	Serial.begin(9600);
	if (!serializer.init(7))
		Serial.println("CAN init fail");

}

void loop()
{
	CANFrame f;
	char s[100];
	dataAccesories.insertData(1, 1, 0, 0, 0, 0);
	dataAccesories.packCAN(&f);
	serializer.sendCanFrame(&f);
	dataAccesories.packString(s);
	Serial.println(s);
	delay(1000);

	dataAccesories.insertData(0, 0, 1, 1, 1, 1);
	dataAccesories.packCAN(&f);
	serializer.sendCanFrame(&f);
	dataAccesories.packString(s);
	Serial.println(s);
	delay(1000);
}
