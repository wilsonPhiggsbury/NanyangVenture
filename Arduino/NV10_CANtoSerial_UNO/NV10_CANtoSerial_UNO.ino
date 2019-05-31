/*
Name:		CANtoSerial_UNO.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#include <CANSerializer.h>
#include <NV10FuelCell.h>
#include <NV10CurrentSensor.h>
#include <NV10CurrentSensorStats.h>
#include <NV10AccesoriesStatus.h>
#include <NV11DataSpeedo.h>

#define CAN_SPI_CS 4
#define CAN_INTERRUPT 3
// This sketch is meant for ATmega328P on breadboard, see "files for Arduino IDE" for the "ATmega328 on a breadboard (8 MHz internal clock)" board option
// Also useable on a normal Arduino UNO if the board option not found
// extended fuse:	0x05
// low fuse:		0xE2
// high fuse:		0xDA

NV10FuelCell dataFC = NV10FuelCell(0x13);
NV10CurrentSensor dataCS = NV10CurrentSensor(0x11);
NV10CurrentSensorStats dataCSStats = NV10CurrentSensorStats(0x12);
NV11DataSpeedo dataSpeedo = NV11DataSpeedo(0x0A);
NV10AccesoriesStatus dataAcc = NV10AccesoriesStatus(0x10);

// dashboard UNO will only send button commands, will only receive all other logging stats
DataPoint dpRecv[] = { dataFC, dataCS, dataCSStats, dataSpeedo };
DataPoint dpSend[] = { dataAcc };

CANSerializer serializer;
CANFrame f;
char str[100];

void CAN_ISR();
void setup() {
	Serial.begin(9600);
	delay(100);
	if (!serializer.init(CAN_SPI_CS))
	{
		delay(1000);
		asm volatile ("  jmp 0");
	}
	pinMode(CAN_INTERRUPT, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(CAN_INTERRUPT), CAN_ISR, FALLING);
}

void loop() {
	// any CAN to send?
	uint8_t bytesRead = Serial.readBytesUntil('\n', str, 100);
	if (bytesRead > 0)
	{
		for (int i = 0; i < sizeof(dpSend) / sizeof(dpSend[0]); i++)
		{
			if (dpSend[i].checkMatchString(str))
			{
				dpSend[i].unpackString(str);
				dpSend[i].packCAN(&f);
				serializer.sendCanFrame(&f);
				break;
			}
		}
	}
	// any CAN to receive?
	if (serializer.receiveCanFrame(&f))
	{
		for (int i = 0; i < sizeof(dpRecv) / sizeof(dpRecv[0]); i++)
		{
			if (dpRecv[i].checkMatchString(str))
			{
				dpRecv[i].unpackCAN(&f);
				dpRecv[i].packString(str);
				Serial.println(str);
				break;
			}
		}
	}
	delay(10);
}
void CAN_ISR()
{
	// pulse one frame in
	//serializer.recvCAN_OneFrame();
}