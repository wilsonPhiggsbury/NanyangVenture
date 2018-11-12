/*
 Name:		MultitaskLights.ino
 Created:	11/1/2018 1:31:02 AM
 Author:	MX
*/
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "Pins_front.h"
#include "FrameFormats.h"

#define STATE_EN 1.0
#define STATE_DS 0.0
QueueHandle_t queueToggleAccessories;
Adafruit_NeoPixel lstrip = Adafruit_NeoPixel(7, LSIG_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rstrip = Adafruit_NeoPixel(7, RSIG_PIN, NEO_GRB + NEO_KHZ800);
MCP_CAN CANObj = MCP_CAN(CAN_CS_PIN);
volatile int CAN_incoming = 0;

#define NUMSTATES 6
float states[NUMSTATES] = { 0,0,0,0,0,0 };
/*
states:
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
void TaskToggleAccessories(void* pvParameters);
void TaskReceiveCAN(void* pvParameters);
void TaskFlashSignals(void* pvParameters);
void TaskMoveWiper(void* pvParameters);
TaskHandle_t taskFlashSignals, taskMoveWiper;
void setup() {
	Serial.begin(9600);
	// mask 9 bits, check if ID == BT
	unsigned long filter = BT << 2;
	if (CANObj.begin(NV_CANSPEED) != CAN_OK)
		Serial.println("CAN Init fail!");
	//CANObj.init_Mask(0, 0, 0x07FF << 16);
	//CANObj.init_Filt(0, 0, 0x0010 << 16);
	//CANObj.init_Filt(1, 0, 0x0011 << 16);
	//CANObj.init_Filt(2, 0, 0x0012 << 16);
	//CANObj.init_Filt(3, 0, 0x0013 << 16);
	attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), CAN_ISR, FALLING);
	
	queueToggleAccessories = xQueueCreate(1, sizeof(QueueItem));
	xTaskCreate(
		TaskToggleAccessories
		, (const portCHAR *)"HEAD"
		, 300 // -25
		, NULL
		, 2
		, NULL);
	xTaskCreate(
		TaskReceiveCAN
		, (const portCHAR *)"CAN la"
		, 500 // -25
		, NULL
		, 1
		, NULL);
	xTaskCreate(
		TaskFlashSignals
		, (const portCHAR *)"SIG"
		, 150 // -25
		, NULL
		, 3
		, &taskFlashSignals);
	xTaskCreate(
		TaskMoveWiper
		, (const portCHAR *)"WIPE"
		, 150 // -25
		, NULL
		, 3
		, &taskMoveWiper);
	
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
void TaskToggleAccessories(void* pvParameters)
{
	QueueItem incoming;
	pinMode(HORN_PIN, OUTPUT);
	pinMode(HEADLIGHTS_PIN, OUTPUT);
	while (1)
	{
		BaseType_t success = xQueueReceive(queueToggleAccessories, &incoming, 0);
		if (success)
		{
			if (states[Horn] == STATE_EN)
			{
				Serial.println(F("HORN ON"));
				digitalWrite(HORN_PIN, HIGH);
			}
			else if (states[Horn] == STATE_DS)
			{
				Serial.println(F("HORN OFF"));
				digitalWrite(HORN_PIN, LOW);
			}

			if (states[Headlights] == STATE_EN)
			{
				Serial.println(F("LIGHTS ON"));
				digitalWrite(HEADLIGHTS_PIN, HIGH);
			}
			else if (states[Headlights] == STATE_DS)
			{
				Serial.println(F("LIGHTS OFF"));
				digitalWrite(HEADLIGHTS_PIN, LOW);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}
void TaskFlashSignals(void* pvParameters)
{
	bool lsigOn = false, rsigOn = false;
	//pinMode(LSIG_PIN, OUTPUT);
	//pinMode(RSIG_PIN, OUTPUT);
	lstrip.begin();
	rstrip.begin();
	lstrip.setBrightness(64);
	rstrip.setBrightness(64);
	//pinMode(LED_BUILTIN, OUTPUT);
	while (1)
	{
		for (int i = 0; i < 7; i++)
		{
			if (states[Hazard] == STATE_EN || states[Lsig] == STATE_EN)
			{
				if (lsigOn)
				{
					lsigOn = false;
					lstrip.setPixelColor(i, 0, 0, 0);
					lstrip.show();
					//digitalWrite(LED_BUILTIN, LOW);
				}
				else
				{
					lsigOn = true;
					lstrip.setPixelColor(i, 255, 165, 0);
					lstrip.show();
					//digitalWrite(LED_BUILTIN, HIGH);
				}
			}
			else
			{
				lsigOn = false;
				lstrip.setPixelColor(i, 0, 0, 0);
				lstrip.show();
				//digitalWrite(LED_BUILTIN, LOW);
			}

			if (states[Hazard] == STATE_EN || states[Rsig] == STATE_EN)
			{
				//Serial.println("RSIG ON");
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
				//Serial.println("RSIG OFF");
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
		if (states[Wiper] == STATE_EN)
		{
			if (wiperPos == 0)
				wiperPos = 180;
			else
				wiperPos = 0;
		}
		else if (states[Wiper] == STATE_DS)
		{
			wiperPos = 0;
		}
		wiper.write(wiperPos);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
void TaskReceiveCAN(void* pvParameters)
{
	unsigned long id;
	byte len;
	byte inBuffer[8];

	QueueItem incoming;
	NV_CanFrames frames;
	while (1) // A Task shall never return or exit.
	{
		if (CAN_incoming == 1)
		{
			vTaskSuspendAll();
			// ------------------------------ covert CAN frames into String for Serial -----------------------------------
			CANObj.readMsgBufID(&id, &len, inBuffer);
			bool isLastFrame = frames.addItem(id, len, inBuffer);
			Serial.print(id);
			if (isLastFrame)
			{
				bool convertSuccess = frames.toQueueItem(&incoming);
				// only receive button type frames
				if (convertSuccess && incoming.ID == BT)
				{
					for (int i = 0; i < NUMSTATES; i++)
					{
						states[i] = incoming.data[0][i];
					}
					// kick up task to toggle accessories such as Headlights Horn etc
					xQueueSend(queueToggleAccessories, &incoming, 100);
					// unblock time-controlled tasks such as blinking signal lights
					xTaskAbortDelay(taskFlashSignals);
					//// print data
					//char payload[MAX_STRING_LEN];
					//incoming.toString(payload);
					//Serial.println(payload);
				}
				frames.clear(); Serial.println();
			}

			CAN_incoming = 0;
			xTaskResumeAll();
		}
		else if (CAN_incoming == -1)
		{
			Serial.println("~");
		}
		vTaskDelay(pdMS_TO_TICKS(5));
	}

}
void CAN_ISR()
{
	if (CANObj.checkError() == CAN_OK)
	{
		CAN_incoming = 1;
	}
	else
	{
		CAN_incoming = -1;
	}
}