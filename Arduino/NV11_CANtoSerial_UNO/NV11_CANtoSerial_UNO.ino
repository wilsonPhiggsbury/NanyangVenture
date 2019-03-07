/*
    Name:       NV11_CANtoSerial_UNO.ino
    Created:	8/3/2019 12:06:26 AM
    Author:     DESKTOP-GV1MS6E\MX
*/
#define CAN_CS 4
#include <CANSerializer.h>
#include <NV10AccesoriesStatus.h>
#include <NV11DataSpeedo.h>
CANSerializer serializer;
NV11DataSpeedo dataSpeedo = NV11DataSpeedo(0x0A);
NV10AccesoriesStatus dataAccesories = NV10AccesoriesStatus(0x10);
DataPoint* canListenList[] = { &dataSpeedo, &dataAccesories };
DataPoint* serialListenList[] = { &dataSpeedo, &dataAccesories };
void setup()
{
	Serial.begin(9600);
	Serial.setTimeout(500);
	while (!Serial);
	serializer.init(CAN_CS);
}

void loop()
{
	CANFrame f;
	char toSerialPort[100];
	char fromSerialPort[100];
	if (serializer.receiveCanFrame(&f))
	{
		for (int i = 0; i < sizeof(canListenList) / sizeof(canListenList[0]); i++)
		{
			DataPoint& dataPoint = *canListenList[i];
			if (dataPoint.checkMatchCAN(&f))
			{
				dataPoint.unpackCAN(&f);
				dataPoint.packString(toSerialPort);
				Serial.println(toSerialPort);
				break;
			}
		}
	}
	if (Serial.available())
	{
		int bytesRead = Serial.readBytesUntil('\n', fromSerialPort, 100); 
		fromSerialPort[bytesRead-1] = '\0'; // "\r\n" should be the terminator, replace '\r' with '\0'
		for (int i = 0; i < sizeof(serialListenList) / sizeof(serialListenList[0]); i++)
		{
			DataPoint& dataPoint = *serialListenList[i];
			if (dataPoint.checkMatchString(fromSerialPort))
			{
				dataPoint.unpackString(fromSerialPort);
				dataPoint.packCAN(&f);
				serializer.sendCanFrame(&f);
				break;
			}
		}
	}
	delayMicroseconds(1000);
}