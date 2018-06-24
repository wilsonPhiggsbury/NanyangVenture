/*
 Name:		ScratchPad.ino
 Created:	6/24/2018 11:37:49 PM
 Author:	MX
*/
#include <Arduino_FreeRTOS.h>
#include <queue.h>

void TaskQueueOutputData(void *pvParameters);
void TaskLogSendData(void *pvParameters);
QueueHandle_t queueForLogSend;
void setup() {
	Serial.begin(9600);
	delay(1000);

	xTaskCreate(
		TaskQueueOutputData
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 400  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);

	xTaskCreate(
		TaskLogSendData
		, (const portCHAR *) "Dequeue"
		, 256  // Stack size
		, NULL
		, 1  // Priority
		, NULL);

	queueForLogSend = xQueueCreate(1, sizeof(int));
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
void TaskQueueOutputData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	while (1) // A Task shall never return or exit.
	{
		int data = 100;
		BaseType_t success = xQueueSend(queueForLogSend, &data, portMAX_DELAY);
		if (success != pdPASS)
			Serial.println(F("Fail sending data."));
		else
			Serial.println(F("Success sending data."));
		//// See if we can obtain or "Take" the Serial Semaphore.
		//// If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
		//if (xSemaphoreTake(xSerialSemaphore, (TickType_t)5) == pdTRUE) // take semaphore, wait 5 ticks if fail
		//{
		//	// We were able to obtain or "Take" the semaphore and can now access the shared resource.
		//	// We want to have the Serial Port for us alone, as it takes some time to print,
		//	// so we don't want it getting stolen during the middle of a conversion.

		//	xSemaphoreGive(xSerialSemaphore); // Now free or "Give" the Serial Port for others.
		//}
		vTaskDelay(pdMS_TO_TICKS(300));  // one tick delay (15ms) in between reads for stability
	}
}

void TaskLogSendData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	int received;
	while (1)
	{
		BaseType_t success = xQueueReceive(queueForLogSend, &received, 0);
		if (success == pdPASS)
		{
			Serial.print(F("Received:"));
			Serial.println(received);
		}
		else
		{
			Serial.println(F("Got nothing."));
		}

		vTaskDelay(pdMS_TO_TICKS(300));  // one tick delay (15ms) in between reads for stability
	}
}