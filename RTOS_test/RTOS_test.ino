/*
 Name:		RTOS_test.ino
 Created:	6/19/2018 1:42:24 PM
 Author:	MX
*/

#include "HydrogenCellLogger.h"
#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
#include <queue.h>

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
const char* dataSource[] = { "Master Fuel Cell", "Slave Fuel Cell", "Motor" };
typedef struct
{
	DataSource ID;
	char data[21];
}QueueItem;


// define queues
QueueHandle_t queueForLogSend = xQueueCreate(2, sizeof(QueueItem));
QueueHandle_t queueForDisplay = xQueueCreate(1, sizeof(QueueItem));

// define tasks, types are: input, control, output
void TaskReadFuelCell(void *pvParameters);		// Input task:		Refreshes class variables for fuel cell Volts, Amps, Watts and Energy
//void TaskReadMotorPower(void *pvParameters);	// Input task:		Refreshes class variables for motor Volts and Amps
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

	// define objects for more complicated procedures
	HydrogenCellLogger hydroCells[2] = { HydrogenCellLogger(&Serial1),HydrogenCellLogger(&Serial2) };
	// Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
	// because it is sharing a resource, such as the Serial port.
	// Semaphores should only be used whilst the scheduler is running, but we can set it up here.
	if (fuelCellSemaphore == NULL)  // Check to confirm that the Serial Semaphore has not already been created.
	{
		fuelCellSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
		if ((fuelCellSemaphore) != NULL)
			xSemaphoreGive((fuelCellSemaphore));  // Make the Serial Port available for use, by "Giving" the Semaphore.
	}
	// Now set up two Tasks to run independently.
	xTaskCreate(
		TaskReadFuelCell
		, (const portCHAR *)"Fuel"
		, 200
		, hydroCells
		, 3
		, NULL);
	xTaskCreate(
		TaskQueueOutputData
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 300  // This stack size can be checked & adjusted by reading the Stack Highwater
		, hydroCells
		, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);
	xTaskCreate(
		TaskLogSendData
		, (const portCHAR *) "LogSend"
		, 150  // Stack size
		, NULL
		, 1  // Priority
		, NULL);
	xTaskCreate(
		TaskDisplayData
		, (const portCHAR *) "Display"
		, 200  // Stack size
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
	while (1)
	{
		master->readData();
		slave->readData();
		vTaskDelayUntil(&prevTick, pdMS_TO_TICKS(975));
	}
}
void TaskQueueOutputData(void *pvParameters)  // This is a Task.
{
	QueueItem outgoing;// = { FuelCell_M, "I am a piece of data" };
	HydrogenCellLogger* masterCell = ((HydrogenCellLogger*)pvParameters);
	HydrogenCellLogger* slaveCell = ((HydrogenCellLogger*)pvParameters) + 1;
	BaseType_t success;
	while(1) // A Task shall never return or exit.
	{
		success = pdPASS;
		// Arrange for outgoing fuel cell data
		outgoing.ID = FuelCell_M;
		masterCell->dumpDataInto(outgoing.data);
		success = xQueueSend(queueForLogSend, &outgoing, 100);
		outgoing.ID = FuelCell_S;
		slaveCell->dumpDataInto(outgoing.data);
		success = xQueueSend(queueForLogSend, &outgoing, 100);
		// Arrange for outgoing motor data
		// TBC...

		//// See if we can obtain or "Take" the Serial Semaphore.
		//// If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
		//if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE) // take semaphore, wait 5 ticks if fail
		//{
		//	// We were able to obtain or "Take" the semaphore and can now access the shared resource.
		//	// We want to have the Serial Port for us alone, as it takes some time to print,
		//	// so we don't want it getting stolen during the middle of a conversion.

		//	xSemaphoreGive(xSerialSemaphore); // Now free or "Give" the Serial Port for others.
		//}
		vTaskDelay(pdMS_TO_TICKS(1005));  // send one time every 1 second
	}
}

void TaskLogSendData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	QueueItem received;
	while(1)
	{
		BaseType_t success = xQueueReceive(queueForLogSend, &received, 0);
		if (success == pdPASS)
		{
			Serial.print(F("Received "));
			Serial.print(received.data);
			Serial.print(F(" from "));
			Serial.println(dataSource[received.ID]);
		}
		else
		{
			Serial.println(F("Received nothing."));
		}
		
		vTaskDelay(pdMS_TO_TICKS(300));  // poll more frequently since more data comes in at a time
	}
}
void TaskDisplayData(void *pvParameters)
{
	QueueItem received;
	while (1)
	{

		vTaskDelay(pdMS_TO_TICKS(300));
	}
}