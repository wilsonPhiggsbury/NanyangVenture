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
Adafruit_NeoPixel lstrip = Adafruit_NeoPixel(PIXELS, LSIG_OUTPUT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rstrip = Adafruit_NeoPixel(PIXELS, RSIG_OUTPUT, NEO_GRB + NEO_KHZ800);
const uint32_t SIG_COLOR = Adafruit_NeoPixel::Color(255, 165, 0);
const uint32_t NO_COLOR = Adafruit_NeoPixel::Color(0, 0, 0);

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
	debug("NV10 front");
	serializer.init(CAN_SPI_CS);
	serializer.onlyListenFor(BT);

	pinMode(PEDALBRAKE_INTERRUPT, INPUT);
	attachInterrupt(digitalPinToInterrupt(PEDALBRAKE_INTERRUPT), BRAKE_ISR, CHANGE);
	
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
void TaskToggle(void* pvParameters)
{
	Packet brakeOrders;
	brakeOrders.ID = BK;
	pinMode(HORN_OUTPUT, OUTPUT);
	pinMode(HEADLIGHTS_OUTPUT, OUTPUT);
	while (1)
	{
		// ** hotfix for brake status **
		bool brakeOnNew = !digitalRead(PEDALBRAKE_INTERRUPT);
		debug_("BRAKE IS "); debug(digitalRead(PEDALBRAKE_INTERRUPT));
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
			digitalWrite(HORN_OUTPUT, LOW);
			vTaskDelay(pdMS_TO_TICKS(2000));
			digitalWrite(HORN_OUTPUT, HIGH);
			peripheralStates[Horn] = STATE_DS;
			debug("beep off.");
		}
		else if (peripheralStates[Horn] == STATE_DS)
		{
			digitalWrite(HORN_OUTPUT, HIGH);
		}

		if (peripheralStates[Headlights] == STATE_EN)
		{
			digitalWrite(HEADLIGHTS_OUTPUT, HIGH);
		}
		else if (peripheralStates[Headlights] == STATE_DS)
		{
			digitalWrite(HEADLIGHTS_OUTPUT, LOW);
		}
		vTaskDelay(pdMS_TO_TICKS(200));
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
		}
		else
		{
			if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Lsig] == STATE_EN)
			{
				sigOn = true;
				setRGB(lstrip, PIXELS, SIG_COLOR);
			}
			if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Rsig] == STATE_EN)
			{
				sigOn = true;
				setRGB(rstrip, PIXELS, SIG_COLOR);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
void TaskMoveWiper(void* pvParameters)
{
	Servo wiper;
	bool wiperActivated = false;
	int wiperPos = 0;
	while (1)
	{
		if (!wiper.attached() && peripheralStates[Wiper] == STATE_EN)
		{
			wiper.attach(WIPER_PWM_SERVO);
		}
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
	//static unsigned long brakePinDebounceLastTime = 0;
	//if (millis() - brakePinDebounceLastTime < 100)
	//	return;
	//brakePinDebounceLastTime = millis();

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
					if (peripheralStates[Wiper] == STATE_EN)debug(F("WIPER ON"));
					else debug(F("WIPER OFF"));
					peripheralStates[i] = q->data[0][i];
					break;
				case Hazard:
					if (peripheralStates[Hazard] == STATE_EN)debug(F("Hzd ON"));
					else debug(F("Hzd OFF"));
					peripheralStates[i] = q->data[0][i];
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
void setRGB(Adafruit_NeoPixel& strip, uint8_t numLights, uint32_t color)
{
	for (int i = 0; i < numLights; i++)
	{
		strip.setPixelColor(i, color);
	}
	strip.show();
}