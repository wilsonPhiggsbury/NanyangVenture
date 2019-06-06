/*
 Name:		MultitaskLights.ino
 Created:	11/1/2018 1:31:02 AM
 Author:	MX
*/

#include <CANSerializer.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "Pins_front.h"

#include <NV10AccesoriesStatus.h>

NV10AccesoriesStatus dataAcc;
/*
hl
sig (blink)
horn
brake in & CAN send
CAN recv
*/
#define PIXELS 6
Adafruit_NeoPixel lstrip = Adafruit_NeoPixel(PIXELS, LSIG_OUTPUT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rstrip = Adafruit_NeoPixel(PIXELS, RSIG_OUTPUT, NEO_GRB + NEO_KHZ800);
const uint32_t SIG_COLOR = Adafruit_NeoPixel::Color(255, 165, 0);
const uint32_t NO_COLOR = Adafruit_NeoPixel::Color(0, 0, 0);

// define globals
CANSerializer serializer;

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
QueueHandle_t queueForCAN = xQueueCreate(1, sizeof(CANFrame));
TaskHandle_t taskBlink, taskMoveWiper, taskToggle;

void setup() {
	Serial.begin(9600);

	pinMode(PEDALBRAKE_INTERRUPT, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PEDALBRAKE_INTERRUPT), BRAKE_ISR, FALLING);

	if (!serializer.init(CAN_SPI_CS))
		debug("CAN FAIL!");
	xTaskCreate(
		TaskToggle
		, (const portCHAR *)"HEAD"
		, 300
		, NULL
		, 2
		, &taskToggle);
	xTaskCreate(
		TaskCAN
		, (const portCHAR *)"CAN la"
		, 400
		, NULL
		, 2
		, NULL);
	xTaskCreate(
		TaskBlink
		, (const portCHAR *)"SIG"
		, 150
		, NULL
		, 3
		, &taskBlink);
	xTaskCreate(
		TaskMoveWiper
		, (const portCHAR *)"WIPE"
		, 150
		, NULL
		, 3
		, &taskMoveWiper);
	
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
void TaskToggle(void* pvParameters)
{
	CANFrame f;
	pinMode(HORN_OUTPUT, OUTPUT);
	pinMode(HEADLIGHTS_OUTPUT, OUTPUT);
	bool brakeOff = true;
	while (1)
	{
		// poll to disable brake if brake is on
		if (dataAcc.getBrake() == STATE_EN)
		{
			if (digitalRead(PEDALBRAKE_INTERRUPT))
			{
				dataAcc.setBrake(STATE_DS);
				dataAcc.packCAN(&f);
				xQueueSend(queueForCAN, &f, 100);
			}
		}

		//if (dataAcc.getHorn() == STATE_EN)
		//{
		//	debug("BEEEP!");
		//	digitalWrite(HORN_OUTPUT, LOW);
		//	vTaskDelay(pdMS_TO_TICKS(500));
		//	digitalWrite(HORN_OUTPUT, HIGH);
		//	peripheralStates[Horn] = STATE_DS;
		//	debug("beep off.");
		//}
		//else
		//{
		//	digitalWrite(HORN_OUTPUT, HIGH);
		//}

		if (dataAcc.getHeadlights() == STATE_EN)
		{
			digitalWrite(HEADLIGHTS_OUTPUT, HIGH);
		}
		else if (dataAcc.getHeadlights() == STATE_DS)
		{
			digitalWrite(HEADLIGHTS_OUTPUT, LOW);
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
void TaskBlink(void* pvParameters)
{
	bool sigOn = false;

	lstrip.begin();
	rstrip.begin();
	lstrip.setBrightness(255);
	rstrip.setBrightness(255);
	while (1)
	{
		if (sigOn)
		{
			sigOn = false;
			setRGB(lstrip, PIXELS, NO_COLOR);
			setRGB(rstrip, PIXELS, NO_COLOR);
			debug("SIG OFF");
		}
		else
		{
			if (dataAcc.getHazard() == STATE_EN || dataAcc.getLsig() == STATE_EN)
			{
				sigOn = true;
				setRGB(lstrip, PIXELS, SIG_COLOR);
				debug("LSIG ON");
			}
			if (dataAcc.getHazard() == STATE_EN || dataAcc.getRsig() == STATE_EN)
			{
				sigOn = true;
				setRGB(rstrip, PIXELS, SIG_COLOR);
				debug("RSIG ON");
			}
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
void TaskMoveWiper(void* pvParameters)
{
	Servo wiper;
	wiper.attach(WIPER_PWM_SERVO, 900, 2000);
	int wiperPos = 0;
	while (1)
	{
		if (dataAcc.getWiper() == STATE_EN)
		{
			if (wiperPos == 0)
				wiperPos = 180;
			else
				wiperPos = 0;
			debug("WIPE ON");
		}
		else if (dataAcc.getWiper() == STATE_DS)
		{
			wiperPos = 0;
			debug("WIPE OFF");
		}
		wiper.write(wiperPos);
		vTaskDelay(pdMS_TO_TICKS(800));
	}
}
void TaskCAN(void *pvParameters) {

	CANFrame f;
	while (1)
	{
		// anything to send
		BaseType_t recvQueue = xQueueReceive(queueForCAN, &f, 100);
		if (recvQueue == pdTRUE)
		{
			serializer.sendCanFrame(&f);
#if DEBUG
			char str[100];
			dataAcc.packString(str);
			debug(str);
#endif
		}
		// anything to recv
		if (serializer.receiveCanFrame(&f))
		{
			if (dataAcc.checkMatchCAN(&f))
			{
				dataAcc.unpackCAN(&f);
				xTaskAbortDelay(taskBlink);
				xTaskAbortDelay(taskMoveWiper);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}
void setRGB(Adafruit_NeoPixel& strip, uint8_t numLights, uint32_t color)
{
	for (int i = 0; i < numLights; i++)
		strip.setPixelColor(i, color);
	strip.show();
}
void BRAKE_ISR()
{
	CANFrame f;
	dataAcc.setBrake(STATE_EN);
	dataAcc.packCAN(&f);
	xQueueSendFromISR(queueForCAN, &f, pdFALSE);
}