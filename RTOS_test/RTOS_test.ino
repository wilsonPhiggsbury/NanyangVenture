/*
 Name:		RTOS_test.ino
 Created:	6/19/2018 1:42:24 PM
 Author:	MX
*/

#include <EEPROM.h>
#include "MotorLogger.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HydrogenCellLogger.h"
#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
#include <queue.h>

#define IDLEN 2
#define DATALEN 48

// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only only one Task is accessing this resource at any time.
SemaphoreHandle_t fuelCellSemaphore;
// Declare queue data structures: ID of enum type and data of String type
typedef enum
{
	FuelCell_M,
	FuelCell_S,
	Motor
}DataSource;
const char* dataSource[] = { "FM", "FS", "M1" };
typedef struct
{
	DataSource ID;
	char data[DATALEN];
}QueueItem;


// define queues
QueueHandle_t queueForLogSend = xQueueCreate(2, sizeof(QueueItem));
QueueHandle_t queueForDisplay = xQueueCreate(2, sizeof(QueueItem));
// declare globals (please keep to minimum)
bool SD_avail;

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
	delay(1000);
	// do handshake, block if not acquired...
	// TBC...

	// wipe the SD and recreate all files
	// TBC...
	SD_avail = SD.begin(4);

	// define objects for more complicated procedures
	HydrogenCellLogger hydroCells[2] = { HydrogenCellLogger(&Serial1),HydrogenCellLogger(&Serial2) };

	// Now set up two Tasks to run independently.
	xTaskCreate(
		TaskReadFuelCell
		, (const portCHAR *)"Fuel"
		, 200
		, hydroCells
		, 3
		, NULL);
	xTaskCreate(
		TaskReadMotorPower
		, (const portCHAR *)"Motor"
		, 200
		, NULL
		, 3
		, NULL);
	xTaskCreate(
		TaskQueueOutputData
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 200  // This stack size can be checked & adjusted by reading the Stack Highwater
		, hydroCells
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
	HydrogenCellLogger* master = (HydrogenCellLogger*)pvParameters;
	HydrogenCellLogger* slave = ((HydrogenCellLogger*)pvParameters)+1;
	TickType_t prevTick = xTaskGetTickCount();
	TickType_t delay = pdMS_TO_TICKS(975);
	while (1)
	{
		master->readData();
		slave->readData();
		vTaskDelayUntil(&prevTick, delay);
	}
}
void TaskReadMotorPower(void* pvParameters)
{
	while (1)
	{
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
	
}
void TaskQueueOutputData(void *pvParameters)  // This is a Task.
{
	QueueItem outgoing;
	HydrogenCellLogger* masterCell = ((HydrogenCellLogger*)pvParameters);
	HydrogenCellLogger* slaveCell = ((HydrogenCellLogger*)pvParameters) + 1;
	TickType_t delay = pdMS_TO_TICKS(1005);
	BaseType_t success;
	while(1) // A Task shall never return or exit.
	{
		success = pdPASS;
		// Arrange for outgoing fuel cell data
		outgoing.ID = FuelCell_M;
		masterCell->dumpDataInto(outgoing.data);
		success &= xQueueSend(queueForLogSend, &outgoing, 100);
		success &= xQueueSend(queueForDisplay, &outgoing, 100);
		outgoing.ID = FuelCell_S;
		slaveCell->dumpDataInto(outgoing.data);
		success &= xQueueSend(queueForLogSend, &outgoing, 100);
		success &= xQueueSend(queueForDisplay, &outgoing, 100);
		// Arrange for outgoing motor data
		// TBC...


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
			fileName = dataSource[received.ID];
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
		else
		{
			/*Serial.println(F("Received nothing."));*/
		}
		
		vTaskDelay(delay);  // poll more frequently since more data comes in at a time
	}
}
void TaskDisplayData(void *pvParameters)
{
	LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
	lcd.begin(20, 4); // sixteen characters across - 2 lines
	lcd.setBacklight(255);
	QueueItem received;
	char toPrint[4][20];
	toPrint[0][19] = toPrint[1][19] = toPrint[2][19] = toPrint[3][19] = '\0';
	TickType_t delay = pdMS_TO_TICKS(480);// delay 480 ms, shorter than reading/queueing tasks since this task has lower priority
	while (1)
	{
		BaseType_t success;
		while (xQueueReceive(queueForDisplay, &received, 0) == pdPASS)
		{
			strncpy(toPrint[received.ID], dataSource[received.ID], 2); // truncate heading to 2 letters
			strcpy(toPrint[received.ID]+2, ":  ");
			strncpy(toPrint[received.ID]+2+3, received.data, 9);
			strncpy(toPrint[received.ID] + 2 + 3 + 9, "\0", 1);

			lcd.setCursor(0,received.ID);
			lcd.print(toPrint[received.ID]);
		}

		vTaskDelay(delay);
	}
}