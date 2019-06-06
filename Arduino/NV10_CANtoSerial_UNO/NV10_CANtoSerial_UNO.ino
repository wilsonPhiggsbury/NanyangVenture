/*
Name:		CANtoSerial_UNO.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#include <CANSerializer.h>
#include <NV10AccesoriesStatus.h>
#include <NV10CurrentSensor.h>
#include <NV10CurrentSensorStats.h>
#include <NV10FuelCell.h>
#include <NV11DataSpeedo.h>

// seedstudio CAN shield v1.2
#define CAN_CS 4
#define CAN_INT 3
NV10CurrentSensor dataCS;
NV10CurrentSensorStats dataCSStats;
NV10AccesoriesStatus dataAccessory;
NV10FuelCell dataFC;
NV11DataSpeedo dataSpeedo;

DataPoint* dpRecv[] = { &dataFC, &dataCS, &dataCSStats, &dataSpeedo };
DataPoint* dpSend[] = { &dataAccessory };

CANSerializer serializer;
void setup()
{
	Serial.begin(9600);
	pinMode(CAN_INT, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(CAN_INT), CAN_ISR, FALLING);
	if (!serializer.init(CAN_CS, 1000))
	{
		delay(1000);
		asm volatile ("  jmp 0");
	}
}

void loop()
{
	CANFrame f;
	char str[100];
	if (Serial.available())
	{
		int bytesRead = Serial.readBytesUntil('\n', str, 100);
		str[bytesRead - 1] = '\0';
		for (int i = 0; i < sizeof(dpSend) / sizeof(dpSend[0]); i++)
		{
			if (dpSend[i]->checkMatchString(str))
			{
				dpSend[i]->unpackString(str);
				dpSend[i]->packCAN(&f);
				serializer.sendCanFrame(&f);
			}
		}
	}
	if (serializer.receiveCanFrame(&f))
	{
		for (int i = 0; i < sizeof(dpRecv) / sizeof(dpRecv[0]); i++)
		{
			if(dpRecv[i]->checkMatchCAN(&f))
			{
				dpRecv[i]->unpackCAN(&f);
				dpRecv[i]->packString(str);
				Serial.println(str);
			}
		}
	}
}
void CAN_ISR()
{

}