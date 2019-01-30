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

#define PIXELS 6
Adafruit_NeoPixel lstrip = Adafruit_NeoPixel(PIXELS, LSIG_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rstrip = Adafruit_NeoPixel(PIXELS, RSIG_PIN, NEO_GRB + NEO_KHZ800);
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

static bool brakeOn = false;
void setup() {
	Serial.begin(9600);

	serializer.init(CAN_CS_PIN);
	serializer.onlyListenFor(BT);

	pinMode(BRAKE_PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(BRAKE_PIN), BRAKE_ISR, CHANGE);
	
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
	Packet brakeOrders;
	brakeOrders.ID = BK;
	pinMode(HORN_PIN, OUTPUT);
	pinMode(HEADLIGHTS_PIN, OUTPUT);
	while (1)
	{
		// ** hotfix for brake status **
		bool brakeOnNew = !digitalRead(BRAKE_PIN);
		if (brakeOnNew != brakeOn)
		{
			brakeOn = brakeOnNew;
			brakeOrders.timeStamp = millis();
			brakeOrders.data[0][0] = brakeOn ? STATE_EN : STATE_DS;
			xQueueSend(queueForCAN, &brakeOrders, 100);
		}
		// ** end hotfix **
		if (peripheralStates[Horn] == STATE_EN)
		{
			debug("BEEEP!");
			digitalWrite(HORN_PIN, HIGH);
			vTaskDelay(pdMS_TO_TICKS(1000));
			digitalWrite(HORN_PIN, LOW);
			peripheralStates[Horn] = STATE_DS;
			debug("beep off.");
		}
		else if (peripheralStates[Horn] == STATE_DS)
		{
			digitalWrite(HORN_PIN, LOW);
		}

		if (peripheralStates[Headlights] == STATE_EN)
		{
			digitalWrite(HEADLIGHTS_PIN, HIGH);
		}
		else if (peripheralStates[Headlights] == STATE_DS)
		{
			digitalWrite(HEADLIGHTS_PIN, LOW);
		}
		vTaskDelay(pdMS_TO_TICKS(200));
	}
}
void TaskBlink(void* pvParameters)
{
	bool lsigOn = false, rsigOn = false;

	lstrip.begin();
	rstrip.begin();
	lstrip.setBrightness(255);
	rstrip.setBrightness(255);
	while (1)
	{
		if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Lsig] == STATE_EN)
		{
			if (lsigOn)
			{
				lsigOn = false;
				digitalWrite(LED_BUILTIN, LOW);
				for (int i = 0; i < PIXELS; i++)
				{
					lstrip.setPixelColor(i, 0, 0, 0);
					lstrip.show();
					digitalWrite(LED_BUILTIN, LOW);
				}
			}
			else
			{
				lsigOn = true;
				digitalWrite(LED_BUILTIN, HIGH);
				for (int i = 0; i < PIXELS; i++)
				{
					lstrip.setPixelColor(i, 255, 165, 0);
					lstrip.show();
					digitalWrite(LED_BUILTIN, HIGH);
				}
			}
		}
		else
		{
			lsigOn = false;
			digitalWrite(LED_BUILTIN, LOW);
			for (int i = 0; i < PIXELS; i++)
			{
				lstrip.setPixelColor(i, 0, 0, 0);
				lstrip.show();
				digitalWrite(LED_BUILTIN, LOW);
			}
		}

		if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Rsig] == STATE_EN)
		{
			if (rsigOn)
			{
				rsigOn = false;
				for (int i = 0; i < PIXELS; i++)
				{
					rstrip.setPixelColor(i, 0, 0, 0);
					rstrip.show();
				}
			}
			else
			{
				rsigOn = true;
				for (int i = 0; i < PIXELS; i++)
				{
					rstrip.setPixelColor(i, 255, 165, 0);
					rstrip.show();
				}
			}
		}
		else
		{
			rsigOn = false;
			for (int i = 0; i < PIXELS; i++)
			{
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
void BRAKE_ISR()
{
	static unsigned long brakePinDebounceLastTime = 0;
	if (millis() - brakePinDebounceLastTime < 100)
		return;
	brakePinDebounceLastTime = millis();

	// toggle the status whenever pin change detected. the status is kept up to sync in taskToggle.
	brakeOn = !brakeOn;
	float brake = brakeOn ? STATE_EN : STATE_DS;
	Packet out;
	out.ID = BK;
	out.timeStamp = millis();// brakePinDebounceLastTime;
	out.data[0][0] = brake;
	// xQueueSendFromISR writes to an extra parameter, indicating if it has woken up a higher priority task
	// in our case the only task being woken up is taskCAN, which is lowest priority, hence the extra parameter is not used
	xQueueSendFromISR(queueForCAN, &out, NULL);
}
void doReceiveAction(Packet* q)
{
	if (q->ID == BT)
	{
		for (int i = 0; i < NUMSTATES; i++)
		{
			if (peripheralStates[i] != q->data[0][i])
			{
				switch (i)
				{
				case Horn:
					peripheralStates[i] = q->data[0][i];
					break;
				case Wiper:
					if (peripheralStates[Wiper] == STATE_EN)debug(F("Wiper should not be on!"));
					break;
				case Hazard:
					if (peripheralStates[Hazard] == STATE_EN)debug(F("Hazard should not be on!"));
					break;
				case Lsig:
					if (peripheralStates[Lsig] == STATE_EN)debug(F("Lsig ON"));
					else debug(F("Lsig OFF"));
					peripheralStates[i] = q->data[0][i];
					break;
				case Rsig:
					if (peripheralStates[Rsig] == STATE_EN)debug(F("Rsig ON"));
					else debug(F("Rsig OFF"));
					peripheralStates[i] = q->data[0][i];
					break;
				case Headlights:
					if (peripheralStates[Headlights] == STATE_EN)debug(F("Headlights ON"));
					else debug(F("Headlights OFF"));
					peripheralStates[i] = q->data[0][i];
					break;
				}
			}
		}
		// kick up taskToggle to toggle accessories such as Headlights Horn etc
		xTaskAbortDelay(taskToggle);
		// kick up time-controlled tasks so they respond immediately.
		// Example: "turn off blinking signal lights" should apply immediately instead of happening at the end of a blink cycle
		xTaskAbortDelay(taskBlink);
	}
}