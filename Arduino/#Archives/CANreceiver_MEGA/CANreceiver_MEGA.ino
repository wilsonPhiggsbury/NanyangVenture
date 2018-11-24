/*
Name:		ScratchPad.ino
Created:	6/24/2018 11:37:49 PM
Author:	MX
*/
#include <mcp_can.h>

#include <Arduino_FreeRTOS.h>
#include <Adafruit_GFX.h>

#include <SPI.h>

void TaskRecv(void *pvParameters __attribute__((unused)));
MCP_CAN CAN0 = MCP_CAN(7);
volatile int CAN_incoming = 0;
void CAN_ISR();
void setup() {
	Serial.begin(9600);
	delay(1000);
	if (CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
	{
		Serial.println("CAN receiver MEGA initialized.");
		// Code for hardware masking of certain unimportant bits. Modified from example sketch.
		// For some library issues:
		//		init_Mask must be called on indexes [0, 1]
		//		init_Filt must be called on indexes [0, 5]
		// Missing out any single line will cause some weird behaviour, hence the duplicate definitions.

		CAN0.init_Mask(0, 0, 0x003C0000);                // Init id mask: XXOO OOXX (X = ignore these bits, O = control these bits)
		CAN0.init_Filt(0, 0, 0x00100000);                // Init filter:  XX01 00XX (Only accept button frames)
		CAN0.init_Filt(1, 0, 0x00100000);                // Init filter:  Duplicate definition same as above

		CAN0.init_Mask(1, 0, 0x003F0000);                // Init id mask: XXOO OOOO
		CAN0.init_Filt(2, 0, 0x00100000);                // Init filter:  XX01 0000 <--- these are actually redundancies
		CAN0.init_Filt(3, 0, 0x00110000);                // Init filter:  XX01 0001 <--- must fill in for compulsory's sake
		CAN0.init_Filt(4, 0, 0x00120000);                // Init filter:  XX01 0010 <--- they are explicit commands to accept termination bits
		CAN0.init_Filt(5, 0, 0x00130000);                // Init filter:  XX01 0011 <--- could've done without, but oh well... let's move on
	}
	else
	{
		Serial.println("CAN error.");
	}
	attachInterrupt(digitalPinToInterrupt(20), CAN_ISR, FALLING);

	CAN0.setMode(MCP_NORMAL);
	xTaskCreate(
		TaskRecv
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 150  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);

}

void loop() {
	
}

void TaskRecv(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	unsigned long ID;
	byte msgLength = 8;
	byte inBuffer[8];
	while (1)
	{
		if (CAN_incoming)
		{
			CAN0.readMsgBuf(&ID, &msgLength, inBuffer);
			//Serial.print("Received data from source ");

			Serial.print(ID, HEX);
			Serial.print("\t");
			for (int i = 0; i < msgLength; i++)
			{
				if (isPrintable(inBuffer[i]))
				{
					Serial.print((char)inBuffer[i]);
				}
				else
				{
					Serial.print("[");
					Serial.print((uint8_t)inBuffer[i]);
					Serial.print("]");
				}
			}
			Serial.println();
			CAN_incoming = false;
		}
		else if (CAN_incoming == -1)
		{
			Serial.println("CAN receive error.");
			//vTaskDelay(50);
		}
		vTaskDelay(pdMS_TO_TICKS(5));

	}
}
void CAN_ISR()
{
	if (CAN0.checkError() == CAN_OK)
	{
		CAN_incoming = 1;
	}
	else
	{
		CAN_incoming = -1;
	}
}