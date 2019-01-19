/*
 Name:		NV10_dashboard.ino
 Created:	8/15/2018 1:35:26 PM
 Author:	MX
*/
// 3 Female to Male for SCK, MISO, MOSI	SHARED
// 3 Male to Male for 3.3V, GND, LED	SHARED
// 3x3 Male to Male for CS, DC, RST

#include <FreeRTOS_ARM.h>
#include <ILI9488.h>
#include <CAN_Serializer.h>
#include "Pins_Dashboard.h"
// ----------------------

#include "DashboardScreenManager.h"
#include "DisplayBar.h"
#include "DisplayText.h"
//#include "DisplayDrawing.h"
//#include "Bitmaps.h"

const unsigned int buttonPins[] = { BTN_HAZARD,BTN_HEADLIGHT,BTN_HORN,BTN_LSIG,BTN_RSIG,BTN_WIPER };
void TaskRefreshScreen(void* pvParameters);
void TaskReadSerial(void* pvParameters);
void TaskCaptureButtons(void* pvParameters);
void TaskTest(void* pvParameters);
QueueHandle_t queueForDisplay;
void setup() {
	Serial.begin(9600);
	Serial1.begin(9600);
	//Serial1.setTimeout(100);
	delay(1000);
	queueForDisplay = xQueueCreate(1, sizeof(Packet));
	// setup buttons
	for (int i = 0; i < NUM_BUTTONS; i++)
	{
		unsigned int thisPin = buttonPins[i];
		pinMode(thisPin, INPUT_PULLUP);
	}
	// I tried putting attachinterrupt in the for loop above but failed. Lambda functions complain.
	// So here, have some wall text.
	//attachInterrupt(digitalPinToInterrupt(BTN_HAZARD), [] {
	//	if (peripheralStates[Hazard] == STATE_EN)
	//		peripheralStates[Hazard] = STATE_DS;
	//	else
	//		peripheralStates[Hazard] = STATE_EN;
	//}, FALLING);
	//attachInterrupt(digitalPinToInterrupt(BTN_WIPER), [] {
	//	if (peripheralStates[Wiper] == STATE_EN)
	//		peripheralStates[Wiper] = STATE_DS;
	//	else
	//		peripheralStates[Wiper] = STATE_EN;
	//}, FALLING);
	attachInterrupt(digitalPinToInterrupt(BTN_HORN), [] {
		if (peripheralStates[Horn] == STATE_EN)
			peripheralStates[Horn] = STATE_DS;
		else
			peripheralStates[Horn] = STATE_EN;
	}, FALLING);
	attachInterrupt(digitalPinToInterrupt(BTN_HEADLIGHT), [] {
		if (peripheralStates[Headlights] == STATE_EN)
			peripheralStates[Headlights] = STATE_DS;
		else
			peripheralStates[Headlights] = STATE_EN;
	}, FALLING);
	attachInterrupt(digitalPinToInterrupt(BTN_LSIG), [] {
		if (peripheralStates[Lsig] == STATE_EN)
			peripheralStates[Lsig] = STATE_DS;
		else
			peripheralStates[Lsig] = STATE_EN;
	}, FALLING);
	attachInterrupt(digitalPinToInterrupt(BTN_RSIG), [] {
		if (peripheralStates[Rsig] == STATE_EN)
			peripheralStates[Rsig] = STATE_DS;
		else
			peripheralStates[Rsig] = STATE_EN;
	}, FALLING);
	//attachInterrupt(digitalPinToInterrupt(BTN_RADIO), [] {
	//	peripheralStates[Radio] = STATE_EN;
	//}, FALLING);
	// ^^^^^ button logic (toggle), free to substitute with other logics if appropriate ^^^^^

	// 480 x 320 pixels
	/*Dashboard Info
		Fuel cell info x2
			Voltage
			Current
			Temperature(stack)
			Pressure
			Status(SS, IN, OP, SD, etc.)
		Capacitor & Motors x3
			Voltage
			CS
			Current

		Car Speed
		Acceleration
		Handbrake light
		Cylinder Pressure
	*/
	xTaskCreate(
		TaskRefreshScreen
		, (const portCHAR *)"Refresh"  // A name just for humans
		, 2000 // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL // Any pointer to pass in
		, 3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);

	xTaskCreate(
		TaskReadSerial
		, (const portCHAR *)"ReadS1"  // A name just for humans
		, 1000  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL // Any pointer to pass in
		, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);
	xTaskCreate(
		TaskCaptureButtons
		, (const portCHAR *)"ReadBT"  // A name just for humans
		, 600  
		, NULL 
		, 2  
		, NULL);
	debug("ALL tasks initialized.");
	vTaskStartScheduler();
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
void TaskRefreshScreen(void* pvParameters)
{
	const uint8_t CAN_resetThreshold = 15;
	uint8_t CAN_resetCounter = 0;

	pinMode(LCD_BACKLIGHT, OUTPUT);
	digitalWrite(LCD_BACKLIGHT, HIGH);
	pinMode(CAN_RST_PIN, OUTPUT);
	digitalWrite(CAN_RST_PIN, HIGH);

	Packet received;
	char content[FLOAT_TO_STRING_LEN + 1];
	DashboardScreenManager screens = DashboardScreenManager(&received); // Singleton Facade pattern probably?

	TickType_t delay = pdMS_TO_TICKS(200);
	while (1)
	{
		BaseType_t success = xQueueReceive(queueForDisplay, &received, 0);
		if (success)
		{
			CAN_resetCounter = 0;
			//// dummy data
			//dummyData(&received, FC);
			//screens.refreshDataWidgets();
			//dummyData(&received, CS);
			//screens.refreshDataWidgets();
			//dummyData(&received, SM);
			//screens.refreshDataWidgets();
			screens.refreshDataWidgets();
			
		}
		else
		{
			if (CAN_resetCounter++ >= CAN_resetThreshold)
			{
				screens.refreshDataWidgets(NULL);
				digitalWrite(CAN_RST_PIN, LOW);
				vTaskDelay(delay);
				digitalWrite(CAN_RST_PIN, HIGH);
				CAN_resetCounter = 0; debug("Resetting 328P...");
			}
		}
		screens.refreshAnimatedWidgets();
		vTaskDelay(delay);
	}
}
void TaskReadSerial(void* pvParameters)
{
	Packet outgoing;
	TickType_t delay = pdMS_TO_TICKS(200);
	while (1)
	{
		if (CAN_Serializer::receiveSerial(Serial1, &outgoing))
		{
			xQueueSend(queueForDisplay, &outgoing, 100);
#if DEBUG
			char payload[MAX_STRING_LEN];
			outgoing.toString(payload);
			debug(payload);
#endif
		}
		vTaskDelay(delay);
	}
}
void TaskCaptureButtons(void* pvParameters)
{
	Packet buttonCommand;
	buttonCommand.ID = BT;
	peripheralStates[Horn] = peripheralStates[Wiper] = peripheralStates[Hazard] = STATE_DS;
	buttonCommand.data[0][Horn] = buttonCommand.data[0][Wiper] = buttonCommand.data[0][Hazard] = STATE_DS;
	TickType_t delay = pdMS_TO_TICKS(200);
	setDebounce(buttonPins, NUM_BUTTONS, 900); // for some reason, setDebounce() if called in setup() seems to have no effect
	while (1)
	{
		bool stateChanged = false;
		for (int i = 0; i < NUM_BUTTONS; i++)
		{
			// only update peripheral states if changed. 
			// Flag the existence of change with stateChanged boolean to trigger immediate broadcast later.
			if (buttonCommand.data[0][i] != peripheralStates[i])
			{
				stateChanged = true;
				buttonCommand.data[0][i] = peripheralStates[i];
			}
		}
		buttonCommand.timeStamp = millis();
		// broadcast peripheral states when requested (pull)
		// broadcast immediately if there are updates (push)
		bool peripheralStateRequest = false;
		if (peripheralStateRequest || stateChanged)
		{
			CAN_Serializer::sendSerial(Serial1, &buttonCommand);
#if DEBUG
			if (stateChanged)
			{
				debug(F("____________"));
				CAN_Serializer::sendSerial(Serial, &buttonCommand);
				debug(F("____________"));
			}
			//for (int i = 0; i < NUM_BUTTONS; i++)
			//{
			//	debug_(buttonCommand.data[0][i]);
			//	debug_("\t");
			//}
			//debug();
#endif
			// disable horn immediately here, front has code to auto-determine how long to beep the horn
			buttonCommand.data[0][Horn] = peripheralStates[Horn] = STATE_DS;
			// tell the display about my button state too
			xQueueSend(queueForDisplay, &buttonCommand, 100);
		}
		vTaskDelay(delay);
	}
}