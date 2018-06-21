/*
 Name:		RTOS_test.ino
 Created:	6/19/2018 1:42:24 PM
 Author:	MX
*/

#include "HydrogenCellLogger.h"
#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).

// Declare a mutex Semaphore Handle which we will use to manage the Serial Port.
// It will be used to ensure only only one Task is accessing this resource at any time.
SemaphoreHandle_t xSerialSemaphore;

// define two Tasks for DigitalRead & AnalogRead
void TaskReadFuelCell(void *pvParameters);
void TaskReadMotorPower(void *pvParameters);

// the setup function runs once when you press reset or power the board
void setup() {

	// initialize serial communication at 9600 bits per second:
	Serial.begin(9600);
	delay(1000);

	// Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
	// because it is sharing a resource, such as the Serial port.
	// Semaphores should only be used whilst the scheduler is running, but we can set it up here.
	if (xSerialSemaphore == NULL)  // Check to confirm that the Serial Semaphore has not already been created.
	{
		xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
		if ((xSerialSemaphore) != NULL)
			xSemaphoreGive((xSerialSemaphore));  // Make the Serial Port available for use, by "Giving" the Semaphore.
	}
	
	// Now set up two Tasks to run independently.
	xTaskCreate(
		TaskReadFuelCell
		, (const portCHAR *)"ReadFuelCell"  // A name just for humans
		, 256  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);
	

	xTaskCreate(
		TaskReadMotorPower
		, (const portCHAR *) "ReadMotorPower"
		, 256  // Stack size
		, NULL
		, 1  // Priority
		, NULL);

	//xTaskCreate(
	//	TaskReadMotorPower
	//	, (const portCHAR *) "ReadMotorPower"
	//	, 128  // Stack size
	//	, NULL
	//	, 1  // Priority
	//	, NULL);

	// Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}

void loop()
{
	// Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskReadFuelCell(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	HydrogenCellLogger cell1 = HydrogenCellLogger(&Serial1);
	while(1) // A Task shall never return or exit.
	{
		cell1.readData();
		if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE) // take semaphore, wait 5 ticks if fail
		{
			// We were able to obtain or "Take" the semaphore and can now access the shared resource.
			// We want to have the Serial Port for us alone, as it takes some time to print,
			// so we don't want it getting stolen during the middle of a conversion.

			xSemaphoreGive(xSerialSemaphore); // Now free or "Give" the Serial Port for others.
		}

		vTaskDelay(pdMS_TO_TICKS(300));  // one tick delay (15ms) in between reads for stability
	}
}

void TaskReadMotorPower(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	HydrogenCellLogger cell2 = HydrogenCellLogger(&Serial2);
	while(1)
	{
		cell2.readData();
		// See if we can obtain or "Take" the Serial Semaphore.
		// If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
		if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE)
		{
			// We were able to obtain or "Take" the semaphore and can now access the shared resource.
			// We want to have the Serial Port for us alone, as it takes some time to print,
			// so we don't want it getting stolen during the middle of a conversion.
			xSemaphoreGive(xSerialSemaphore); // Now free or "Give" the Serial Port for others.
		}
		vTaskDelay(pdMS_TO_TICKS(300));  // one tick delay (15ms) in between reads for stability
	}
}