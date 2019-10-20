/*
    Name:       TEST2.ino
    Created:	21/2/2019 3:53:08 PM
    Author:     DESKTOP-GV1MS6E\MX
*/
#include <CANSerializer.h>
#include <NV10AccesoriesStatus.h>
#include <NV10CurrentSensor.h>
#include <NV10CurrentSensorStats.h>
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

NV10CurrentSensor dataCS;
NV10CurrentSensorStats dataCSStats;
NV10AccesoriesStatus dataAccessory;
NV10FuelCell dataFC;
NV11DataSpeedo dataSpeedo;

CANSerializer serializer;
void setup()
{
	Serial.begin(9600);
	pinMode(CAN_INT, INPUT_PULLUP);
	if (!serializer.init(CAN_CS, CANSPEED))
		Serial.println("CAN FAIL");
	else
		Serial.println("CAN init");
}

void loop()
{
	CANFrame f;
	char str[100];
	if (!digitalRead(CAN_INT))
	{
		if (!serializer.receiveCanFrame(&f))
			Serial.println("FALSE ALARM!");
		if (dataCS.checkMatchCAN(&f))
		{
			dataCS.unpackCAN(&f);
			dataCS.packString(str);
			Serial.println(str);
		}
		else if (dataAccessory.checkMatchCAN(&f))
		{
			//Serial.print("Received acc: ");
			dataAccessory.unpackCAN(&f);
			dataAccessory.packString(str);
			Serial.println(str);
		}
		else if (dataFC.checkMatchCAN(&f))
		{
			dataFC.unpackCAN(&f);
			dataFC.packString(str);
			Serial.println(str);
		}
		else if (dataCSStats.checkMatchCAN(&f))
		{
			dataCSStats.unpackCAN(&f);
			dataCSStats.packString(str);
			Serial.println(str);
		}
		else if (dataSpeedo.checkMatchCAN(&f))
		{
			dataSpeedo.unpackCAN(&f);
			dataSpeedo.packString(str);
			Serial.println(str);
		}
		else
		{
			Serial.print("Recevived unknown packet of ID 0x"); Serial.println(f.id, HEX);
		}
	}
}
