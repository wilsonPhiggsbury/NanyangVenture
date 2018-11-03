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
typedef enum {
	Headlights,
	Horn,
	Lsig,
	Rsig,
	Wiper,
	Hazard
}State;
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
	attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), CAN_ISR, FALLING);
	
	queueToggleAccessories = xQueueCreate(1, sizeof(QueueItem));
	xTaskCreate(
		TaskToggleAccessories
		, (const portCHAR *)"HEAD"
		, 300
		, NULL
		, 1
		, NULL);
	xTaskCreate(
		TaskReceiveCAN
		, (const portCHAR *)"CAN la"
		, 400
		, NULL
		, 1
		, NULL);
	xTaskCreate(
		TaskFlashSignals
		, (const portCHAR *)"SIG"
		, 200
		, NULL
		, 2
		, &taskFlashSignals);
	xTaskCreate(
		TaskMoveWiper
		, (const portCHAR *)"SIG"
		, 100
		, NULL
		, 2
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
				digitalWrite(HORN_PIN, HIGH);
			}
			else if (states[Horn] == STATE_DS)
			{
				digitalWrite(HORN_PIN, LOW);
			}

			if (states[Headlights] == STATE_EN)
			{
				digitalWrite(HEADLIGHTS_PIN, HIGH);
			}
			else if (states[Headlights] == STATE_DS)
			{
				digitalWrite(HEADLIGHTS_PIN, LOW);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(250));
	}
}
void TaskFlashSignals(void* pvParameters)
{
	bool lsigOn, rsigOn;
	//pinMode(LSIG_PIN, OUTPUT);
	//pinMode(RSIG_PIN, OUTPUT);
	lstrip.begin();
	rstrip.begin();
	lstrip.setBrightness(64);
	rstrip.setBrightness(64);

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
				}
				else
				{
					lsigOn = true;
					lstrip.setPixelColor(i, 255, 165, 0);
					lstrip.show();
				}
			}
			else
			{
				lsigOn = false;
				lstrip.setPixelColor(i, 0, 0, 0);
				lstrip.show();
			}

			if (states[Hazard] == STATE_EN || states[Rsig] == STATE_EN)
			{
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
			// ------------------------------ covert CAN frames into String for Serial -----------------------------------
			CANObj.readMsgBufID(&id, &len, inBuffer);
			bool isLastFrame = frames.addItem(id, len, inBuffer);
			if (isLastFrame)
			{
				bool convertSuccess = frames.toQueueItem(&incoming);
				// only receive button type frames
				if (convertSuccess && incoming.ID == BT)
				{
					for (int i = 0; i < NUMSTATES; i++)
					{
						states[i] = incoming.data[0][i];
						// kick up task to toggle accessories such as Headlights Horn etc
						xQueueSend(queueToggleAccessories, &incoming, 100);
						// unblock time-controlled tasks such as blinking signal lights
					}
					//// print data
					//char payload[MAX_STRING_LEN];
					//incoming.toString(payload);
					//Serial.println(payload);
				}
				frames.clear();
			}

			CAN_incoming = false;
		}
		else if (CAN_incoming == -1)
		{
			Serial.println("~");
			vTaskDelay(50);
		}
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