/*
 Name:		MultitaskLights.ino
 Created:	11/1/2018 1:31:02 AM
 Author:	MX
*/
#include <Adafruit_NeoPixel.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
// the setup function runs once when you press reset or power the board
Adafruit_NeoPixel lstrip = Adafruit_NeoPixel(7, 12, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rstrip = Adafruit_NeoPixel(7, 13, NEO_GRB + NEO_KHZ800);
QueueHandle_t lsigCommand, rsigCommand;
bool lsigOn = false, rsigOn = false;
void TaskToggleHeadlights(void* pvParameters);
void TaskToggleLsig(void* pvParameters);
void TaskToggleRsig(void* pvParameters);
void TaskFlashSignals(void* pvParameters);
void setup() {
	Serial.begin(9600);
	lsigCommand = xQueueCreate(1, sizeof(int));
	rsigCommand = xQueueCreate(1, sizeof(int));
	xTaskCreate(
		TaskToggleHeadlights
		, (const portCHAR *)"HEAD"
		, 80
		, NULL
		, 1
		, NULL);
	xTaskCreate(
		TaskToggleLsig
		, (const portCHAR *)"LTOG"
		, 80
		, NULL
		, 1
		, NULL);
	xTaskCreate(
		TaskToggleRsig
		, (const portCHAR *)"RTOG"
		, 80
		, NULL
		, 1
		, NULL);
	xTaskCreate(
		TaskFlashSignals
		, (const portCHAR *)"FLASH"
		, 200
		, NULL
		, 2
		, NULL);

}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
void TaskToggleHeadlights(void* pvParameters)
{
	bool lightsOn = false;
	int headlightPin = 2;
	pinMode(headlightPin, INPUT_PULLUP);//12
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	while (1)
	{
		if (digitalRead(headlightPin) == LOW)
		{
			digitalWrite(8, !lightsOn);
			digitalWrite(9, !lightsOn);
			digitalWrite(10, !lightsOn);
			digitalWrite(11, !lightsOn);
			lightsOn = !lightsOn;
			Serial.print("HEADLIGHTS is now ");
			Serial.println(lightsOn ? "ON" : "OFF");
			vTaskDelay(pdMS_TO_TICKS(300));
		}
		vTaskDelay(pdMS_TO_TICKS(20));
	}
}
void TaskToggleLsig(void* pvParameters)
{
	int lsigBtn = 3;
	pinMode(lsigBtn, INPUT_PULLUP);
	while (1)
	{
		if (digitalRead(lsigBtn) == LOW)
		{
			lsigOn = !lsigOn;
			Serial.print("Lsig is now ");
			Serial.println(lsigOn ? "ON" : "OFF");
			vTaskDelay(pdMS_TO_TICKS(300));
		}
		vTaskDelay(pdMS_TO_TICKS(20));
	}

}
void TaskToggleRsig(void* pvParameters)
{
	int rSigBtn = 4;
	pinMode(rSigBtn, INPUT_PULLUP);
	while (1)
	{
		if (digitalRead(rSigBtn) == LOW)
		{
			rsigOn = !rsigOn;
			Serial.print("Rsig is now ");
			Serial.println(rsigOn ? "ON" : "OFF");
			vTaskDelay(pdMS_TO_TICKS(300));
		}
		vTaskDelay(pdMS_TO_TICKS(20));
	}

}
void TaskFlashSignals(void* pvParameters)
{
	lstrip.begin();
	rstrip.begin();

	while (1)
	{
		for (int i = 0; i < 7; i++)
		{
			if (lsigOn)
			{
				
				lstrip.setPixelColor(i, 255, 165, 0);
				lstrip.setBrightness(64);
				lstrip.show();
			}
			if (rsigOn)
			{
				rstrip.setPixelColor(i, 255, 165, 0);
				rstrip.setBrightness(64);
				rstrip.show();
			}
		}
		if (lsigOn)
		{
			Serial.println("ILLUMINATING Lsig");
		}
		if (rsigOn)
		{
			Serial.println("ILLUMINATING Rsig");
		}
		vTaskDelay(pdMS_TO_TICKS(500));
		for (int i = 0; i < 7; i++)
		{
			lstrip.setPixelColor(i, 0, 0, 0);
			lstrip.show();
			rstrip.setPixelColor(i, 0, 0, 0);
			rstrip.show();
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}