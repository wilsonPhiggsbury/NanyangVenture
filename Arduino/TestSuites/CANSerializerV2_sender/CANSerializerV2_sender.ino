/*
    Name:       TEST1.ino
    Created:	21/2/2019 3:53:02 PM
    Author:     DESKTOP-GV1MS6E\MX
*/

#include <CANSerializer.h>
#include <NV10AccesoriesStatus.h>
#include <NV10CurrentSensor.h>
#include <NV10FuelCell.h>
#include <NV11DataSpeedo.h>

//#define SEEDSTUDIO_CAN_SHIELD
// seedstudio CAN shield v1.2
#ifdef SEEDSTUDIO_CAN_SHIELD
#define CAN_CS 10
#define CAN_INT 2
#define CANSPEED 500
#else
#define CAN_CS 4
#define CAN_INT 3
#define CANSPEED 1000
#endif

char dummyFCTexts[][100] = {
	">>52.3V 04.8A 0000W 00100Wh 021.1C 028.3C 038.5C 031.6C 0.90B 59.0V 028.0C IN 00.0C 00 0000",
	">>45.3V 40.0A 0000W 09235Wh 021.1C 028.3C 038.5C 031.6C 0.90B 59.0V 028.0C OP 00.0C 00 0000",
	">>21.8V 51.1A 0000W 03211Wh 021.1C 028.3C 038.5C 031.6C 0.90B 59.0V 028.0C SD 00.0C 00 0000",
	">>55.5V 00.8A 0000W 01230Wh 021.1C 028.3C 038.5C 031.6C 0.90B 59.0V 028.0C UN 00.0C 00 0000"
};

NV10CurrentSensor dataCS;
NV10AccesoriesStatus dataAccessory;
NV10FuelCell dataFC;
NV11DataSpeedo dataSpeedo;

CANSerializer serializer = CANSerializer();
void setup()
{
	Serial.begin(9600);
	if (!serializer.init(CAN_CS, CANSPEED))
	{
		Serial.println("CAN init fail");
	}
}

void loop()
{
	CANFrame f;
	char str[50];

	dataCS.insertData(random(35, 60), random(0, 40), random(0, 40), random(0, 40));
	dataCS.packCAN(&f);
	serializer.sendCanFrame(&f);
	dataCS.packString(str);
	Serial.println(str);
	readSerial();
	delay(1000);

	dataAccessory.insertData(random(0, 2), random(0, 2), random(0, 2), random(0, 2), random(0, 2), random(0, 3));
	dataAccessory.packCAN(&f);
	serializer.sendCanFrame(&f);
	dataAccessory.packString(str);
	Serial.println(str);
	readSerial();
	delay(1000);

	dataSpeedo.insertData(random(0, 40));
	dataSpeedo.packCAN(&f);
	serializer.sendCanFrame(&f);
	dataSpeedo.packString(str);
	Serial.println(str);
	readSerial();
	delay(1000);

	static uint8_t a = 0;
	dataFC.insertData(dummyFCTexts[a++]);
	dataFC.packCAN(&f);
	serializer.sendCanFrame(&f);
	dataFC.packString(str);
	Serial.println(str);
	readSerial();
	delay(2000);
}
void readSerial()
{
	CANFrame f;
	char str[100];
	if (Serial.available())
	{
		int bytesRead = Serial.readBytesUntil('\n', str, 100);
		str[bytesRead - 1] = '\0';

		if (dataCS.checkMatchString(str))
		{
			dataCS.unpackString(str);
			dataCS.packCAN(&f);
			serializer.sendCanFrame(&f);
		}
		else if (dataAccessory.checkMatchString(str))
		{
			dataAccessory.unpackString(str);
			dataAccessory.packCAN(&f);
			serializer.sendCanFrame(&f);
		}
		else if (dataFC.checkMatchString(str))
		{
			dataFC.unpackString(str);
			dataFC.packCAN(&f);
			serializer.sendCanFrame(&f);
		}
	}
}