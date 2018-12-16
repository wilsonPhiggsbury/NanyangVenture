/*
 Name:		MultitaskLights.ino
 Created:	11/1/2018 1:31:02 AM
 Author:	MX
*/
#include <CAN_Serializer.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "Pins_front.h"
Adafruit_NeoPixel lstrip = Adafruit_NeoPixel(7, LSIG_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rstrip = Adafruit_NeoPixel(7, RSIG_PIN, NEO_GRB + NEO_KHZ800);
CAN_Serializer serializer;

/*
peripheralStates:
	Headlights
	Horn
	Lsig
	Rsig
	Wiper
	Hazard
*/
/* -------------------------------------------------------
 SERIAL PLOTTER WITH BUTTONS TO CHECK STABILITY

 NOTE:
	This is a slave device. Its state is purely set to match the dashboard's commands.

 ------------------------------------------------------- */
void TaskToggle(void* pvParameters);
void TaskBlink(void* pvParameters);
void TaskMoveWiper(void* pvParameters);
void TaskCAN(void* pvParameters);
QueueHandle_t queueForCAN = xQueueCreate(1, sizeof(Packet));
TaskHandle_t taskBlink, taskMoveWiper, taskToggle;
void setup() {
	Serial.begin(9600);

	serializer.init(CAN_CS_PIN);
	//serializer.onlyListenFor(BT);
	attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), CAN_ISR, FALLING);
	
	xTaskCreate(
		TaskToggle
		, (const portCHAR *)"HEAD"
		, 300 // -25
		, NULL
		, 2
		, &taskToggle);
	xTaskCreate(
		TaskCAN
		, (const portCHAR *)"CAN la"
		, 1200 // -25
		, NULL
		, 1
		, NULL);
	xTaskCreate(
		TaskBlink
		, (const portCHAR *)"SIG"
		, 150 // -25
		, NULL
		, 3
		, &taskBlink);
	//xTaskCreate(
	//	TaskMoveWiper
	//	, (const portCHAR *)"WIPE"
	//	, 150 // -25
	//	, NULL
	//	, 3
	//	, &taskMoveWiper);
	
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
void TaskToggle(void* pvParameters)
{
	Packet incoming;
	pinMode(HORN_PIN, OUTPUT);
	pinMode(HEADLIGHTS_PIN, OUTPUT);
	while (1)
	{
		if (peripheralStates[Horn] == STATE_EN)
		{
			debug(F("HORN ON"));
			digitalWrite(HORN_PIN, HIGH);
		}
		else if (peripheralStates[Horn] == STATE_DS)
		{
			debug(F("HORN OFF"));
			digitalWrite(HORN_PIN, LOW);
		}

		if (peripheralStates[Headlights] == STATE_EN)
		{
			debug(F("LIGHTS ON"));
			digitalWrite(HEADLIGHTS_PIN, HIGH);
		}
		else if (peripheralStates[Headlights] == STATE_DS)
		{
			debug(F("LIGHTS OFF"));
			digitalWrite(HEADLIGHTS_PIN, LOW);
		}
		vTaskSuspend(taskToggle);
	}
}
void TaskBlink(void* pvParameters)
{
	bool lsigOn = false, rsigOn = false;
	//pinMode(LSIG_PIN, OUTPUT);
	//pinMode(RSIG_PIN, OUTPUT);
	lstrip.begin();
	rstrip.begin();
	lstrip.setBrightness(64);
	rstrip.setBrightness(64);
	pinMode(LED_BUILTIN, OUTPUT);
	while (1)
	{
		for (int i = 0; i < 7; i++)
		{
			if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Lsig] == STATE_EN)
			{
				if (lsigOn)
				{
					lsigOn = false;
					lstrip.setPixelColor(i, 0, 0, 0);
					lstrip.show();
					digitalWrite(LED_BUILTIN, LOW);
				}
				else
				{
					lsigOn = true;
					lstrip.setPixelColor(i, 255, 165, 0);
					lstrip.show();
					digitalWrite(LED_BUILTIN, HIGH);
				}
			}
			else
			{
				lsigOn = false;
				lstrip.setPixelColor(i, 0, 0, 0);
				lstrip.show();
				digitalWrite(LED_BUILTIN, LOW);
			}

			if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Rsig] == STATE_EN)
			{
				//debug("RSIG ON");
				if (rsigOn)
				{
					rsigOn = false;
					rstrip.setPixelColor(i, 0, 0, 0);
					rstrip.show();
				}
				else
				{
					rsigOn = true;
					rstrip.setPixelColor(i, 255, 165, 0);
					rstrip.show();
				}
			}
			else
			{
				//debug("RSIG OFF");
				rsigOn = false;
				rstrip.setPixelColor(i, 0, 0, 0);
				rstrip.show();
			}
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
void TaskMoveWiper(void* pvParameters)
{
	Servo wiper;
	wiper.attach(WIPER_PIN);
	int wiperPos = 0;
	while (1)
	{
		if (peripheralStates[Wiper] == STATE_EN)
		{
			if (wiperPos == 0)
				wiperPos = 180;
			else
				wiperPos = 0;
		}
		else if (peripheralStates[Wiper] == STATE_DS)
		{
			wiperPos = 0;
		}
		wiper.write(wiperPos);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
void doReceiveAction(Packet* q)
{
	if (q->ID == BT)
	{
		for (int i = 0; i < NUMSTATES; i++)
		{
			peripheralStates[i] = q->data[0][i];
		}
		// kick up taskToggle to toggle accessories such as Headlights Horn etc
		xTaskAbortDelay(taskToggle);
		// kick up time-controlled tasks so they respond immediately.
		// Example: "turn off blinking signal lights" should apply immediately instead of happening at the end of a blink cycle
		xTaskAbortDelay(taskBlink);
	}
}