/*
 Name:		RTOS_test.ino
 Created:	6/19/2018 1:42:24 PM
 Author:	MX
*/

#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
#include <queue.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#include "DisplayInterface.h"
#include "MotorLogger.h"
#include "HydrogenCellLogger.h"


// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only only one Task is accessing this resource at any time.
SemaphoreHandle_t fuelCellSemaphore;

// define queues
QueueHandle_t queueForLogSend = xQueueCreate(1, sizeof(QueueItem));
QueueHandle_t queueForDisplay = xQueueCreate(1, sizeof(QueueItem));
// declare globals (please keep to minimum)
bool SD_avail;
HydrogenCellLogger hydroCells[2] = { HydrogenCellLogger(&Serial1),HydrogenCellLogger(&Serial2) };
MotorLogger motors[2] = { MotorLogger(0,A0,A1),MotorLogger(1,A2,A3) };

// define tasks, types are: input, control, output
void TaskReadFuelCell(void *pvParameters);		// Input task:		Refreshes class variables for fuel cell Volts, Amps, Watts and Energy
void TaskReadMotorPower(void *pvParameters);	// Input task:		Refreshes class variables for motor Volts and Amps
void TaskQueueOutputData(void *pvParameters);	// Control task:	Controls frequency to queue data from above tasks to output tasks
void TaskLogSendData(void *pvParameters);		// Output task:		Data logged in SD card and sent through XBee. Logged and sent data should be consistent, hence they are grouped together
void TaskDisplayData(void *pvParameters);		// Output task:		Display on LCD screen

//// _______________OPTIONAL_____________
//void TaskReceiveCommands(void *pvParameters);	// Input task:		Enable real-time control of Arduino (if any)
//void TaskRefreshPeripherals(void *pvParameters);// Control task:	Updates all CAN-BUS peripherals


// the setup function runs once when you press reset or power the board
void setup() {
	// initialize serial communication at 9600 bits per second:
	Serial.begin(9600);
	delay(100);
	//wipeEEPROM();
	//motors[0].populateEEPROM();
	//motors[1].populateEEPROM();
	//printEEPROM();

	// do handshake, block if not acquired...
	// TBC...

	// wipe the SD and recreate all files
	// TBC...
	SD_avail = SD.begin(SD_SPI_CS);
	if (!SD_avail)
	{
		Serial.println(F("SD card not present!"));
	}
	// define objects for more complicated procedures
	
	
	// Now set up two Tasks to run independently.
	xTaskCreate(
		TaskReadFuelCell
		, (const portCHAR *)"Fuel"
		, 150
		, hydroCells
		, 3
		, NULL);
	xTaskCreate(
		TaskReadMotorPower
		, (const portCHAR *)"Motor"
		, 150
		, motors
		, 3
		, NULL);
	xTaskCreate(
		TaskQueueOutputData
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 200  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);
	xTaskCreate(
		TaskLogSendData
		, (const portCHAR *) "LogSend"
		, 300  // Stack size
		, NULL
		, 1  // Priority
		, NULL);
	xTaskCreate(
		TaskDisplayData
		, (const portCHAR *) "Display"
		, 300  // Stack size
		, NULL
		, 1  // Priority
		, NULL);

	// Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}

void loop()
{
	// Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void TaskReadFuelCell(void *pvParameters)
{
	// Obtain fuel cell object references from parameter passed in
	HydrogenCellLogger* masterCell = (HydrogenCellLogger*)pvParameters;
	HydrogenCellLogger* slaveCell = ((HydrogenCellLogger*)pvParameters) + 1;
	TickType_t prevTick = xTaskGetTickCount();
	TickType_t delay = pdMS_TO_TICKS(975);
	while (1)
	{
		masterCell->readData();
		slaveCell->readData();
		vTaskDelayUntil(&prevTick, delay);
	}
}
void TaskReadMotorPower(void* pvParameters)
{
	MotorLogger* m1 = ((MotorLogger*)pvParameters);
	MotorLogger* m2 = ((MotorLogger*)pvParameters)+1;
	/*MotorLogger* m3 = ((MotorLogger*)pvParameters)+2;*/
	TickType_t delay = pdMS_TO_TICKS(975);
	while (1)
	{
		m1->logData();
		m2->logData();
		vTaskDelay(delay);
	}
	
}
void TaskQueueOutputData(void *pvParameters)  // This is a Task.
{
	QueueItem outgoing;
	/*HydrogenCellLogger* masterCell = (HydrogenCellLogger*)pvParameters;
	HydrogenCellLogger* slaveCell = ((HydrogenCellLogger*)pvParameters) + 1;*/
	//MotorLogger* motor1 = ((Loggers*)pvParameters)->motors;
	/*MotorLogger* motor2 = ((Loggers*)pvParameters)->motors + 1;
	MotorLogger* motor3 = ((Loggers*)pvParameters)->motors + 2;*/
	TickType_t delay = pdMS_TO_TICKS(1005);
	BaseType_t success;

	while(1) // A Task shall never return or exit.
	{
		success = pdPASS;
		// Arrange for outgoing fuel cell data
		outgoing.ID = FuelCell;
		outgoing.data[0] = '\0';
		/* ------------------DATA FORMAT------------------
						FM				FS
			millis		V	A	W	Wh	V	A	W	Wh
		--------------------------------------------------*/
		HydrogenCellLogger::dumpTimestampInto(outgoing.data);
		if (hydroCells[0].hasUpdated())
		{
			hydroCells[0].dumpDataInto(outgoing.data);
			strcat(outgoing.data, "\t");
			hydroCells[1].dumpDataInto(outgoing.data);
		}
		else
		{
			strcat(outgoing.data, "------");
		}
		success &= xQueueSend(queueForLogSend, &outgoing, 100);
		success &= xQueueSend(queueForDisplay, &outgoing, 100);
		

		// Arrange for outgoing motor data
		outgoing.ID = Motor;
		outgoing.data[0] = '\0';
		/* ------------------DATA FORMAT------------------
						M1		M2		M3		M4
			millis		V	A	V	A	V	A	V	A	
		--------------------------------------------------*/
		MotorLogger::dumpTimestampInto(outgoing.data);
		motors[0].dumpDataInto(outgoing.data);
		strcat(outgoing.data, "\t");
		motors[1].dumpDataInto(outgoing.data);
		success &= xQueueSend(queueForLogSend, &outgoing, 100);
		success &= xQueueSend(queueForDisplay, &outgoing, 100);

		vTaskDelay(delay);  // send one time every 1 second
	}
}

void TaskLogSendData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	QueueItem received;
	TickType_t delay = pdMS_TO_TICKS(300); // delay 300 ms, shorter than reading/queueing tasks since this task has lower priority
	
	while(1)
	{
		BaseType_t success = xQueueReceive(queueForLogSend, &received, 0);
		const char* fileName;
		if (success == pdPASS)
		{
			switch (received.ID)
			{
			case FuelCell:
				fileName = "FC";
				break;
			case Motor:
				fileName = "MT";
				break;
			}
			Serial.print(fileName);
			Serial.print('\t');
			Serial.println(received.data);
			// -------------- SD store -------------
			if (SD_avail)
			{
				File writtenFile = SD.open(fileName, FILE_WRITE);
				writtenFile.println(received.data);
				writtenFile.close();
			}
		}

		vTaskDelay(delay);  // poll more frequently since more data comes in at a time
	}
}
void TaskDisplayData(void *pvParameters)
{
	QueueItem received;
	LiquidCrystal_I2C lcdScreen = LiquidCrystal_I2C(LCD1_I2C_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
	TFT_ILI9163C tftScreen = TFT_ILI9163C(TFT1_SPI_CS, TFT1_SPI_RS, TFT1_SPI_RST);
	DisplayLCD lcdManager = DisplayLCD(lcdScreen);
	DisplayTFT tftManager = DisplayTFT(tftScreen);

	TickType_t delay = pdMS_TO_TICKS(480);// delay 480 ms, shorter than reading/queueing tasks since this task has lower priority

	while (1)
	{
		BaseType_t success;
		while (xQueueReceive(queueForDisplay, &received, 0) == pdPASS)
		{
			lcdManager.printData(received);
			tftManager.printData(received);
		}

		vTaskDelay(delay);
	}
}

void printEEPROM()
{
	for (int address = 0; address < EEPROM.length(); address += sizeof(unsigned int))
	{
		unsigned int tis;
		EEPROM.get(address, tis);
		if (tis == 65535)continue;
		Serial.print(address);
		Serial.print(" ");
		Serial.println(tis);
	}
}
void wipeEEPROM()
{
	for (int address = 0; address < EEPROM.length(); address += 1)
	{
		uint16_t tis;
		EEPROM.write(address, 255);
	}
	Serial.println("EEPROM wiped.");
}