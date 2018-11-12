/*
 Name:		NV10_dashboard.ino
 Created:	8/15/2018 1:35:26 PM
 Author:	MX
*/
// 3 Female to Male for SCK, MISO, MOSI	SHARED
// 3 Male to Male for 3.3V, GND, LED	SHARED
// 3x3 Male to Male for CS, DC, RST

#include <FreeRTOS_ARM.h>
#include <SPI.h>
#include <ILI9488.h>
#include "Wiring_Dashboard.h"
// dependent header files
#include "FrameFormats.h"
// ----------------------

#include "DashboardScreenManager.h"
#include "DisplayBar.h"
#include "DisplayText.h"
//#include "Bitmaps.h"

void TaskRefreshScreen(void* pvParameters);
void TaskReadSerial(void* pvParameters);
void TaskTest(void* pvParameters);
QueueHandle_t queueForDisplay;
void setup() {
	Serial.begin(9600);
	Serial1.begin(9600);
	Serial1.setTimeout(100);
	delay(100);
	queueForDisplay = xQueueCreate(1, sizeof(QueueItem));
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

	vTaskStartScheduler();
}

// the loop function runs over and over again until power down or reset
void loop() {
  
}
void TaskTest(void* pvParameters)
{
	pinMode(LCD_BACKLIGHT, OUTPUT);
	digitalWrite(LCD_BACKLIGHT, HIGH);
	ILI9488 scr = ILI9488(LCD_LEFT_CS, LCD_LEFT_DC, LCD_RIGHT_RST);
	scr.begin();
	scr.setRotation(1);
	scr.fillScreen(ILI9488_PURPLE);
	DisplayBar t = DisplayBar(&scr, 200, 100, 200, 100, DisplayBar::BOTTOM_TO_TOP);
	t.setColors(ILI9488_WHITE, ILI9488_BLUE);
	t.setRange(50, 100);
	int i = 0;
	while (1)
	{
		i += 10;
		if (i > 100)
			i = 0;
		t.updateFloat(i);
		vTaskDelay(40);
	};
}
void TaskRefreshScreen(void* pvParameters)
{
	const uint8_t CAN_resetThreshold = 10;
	uint8_t CAN_resetCounter = 0;
	

	pinMode(LCD_BACKLIGHT, OUTPUT);
	digitalWrite(LCD_BACKLIGHT, HIGH);

	QueueItem received;
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
			//screens.refreshScreens();
			//dummyData(&received, CS);
			//screens.refreshScreens();
			//dummyData(&received, SM);
			//screens.refreshScreens();
			screens.refreshScreens();
			char data[MAX_STRING_LEN];
			received.toString(data);
			Serial.println(data);
			
		}
		else
		{
			if (CAN_resetCounter++ >= CAN_resetThreshold)
			{
				screens.refreshScreens(NULL);
				digitalWrite(CAN_RST_PIN, LOW);
				vTaskDelay(delay);
				digitalWrite(CAN_RST_PIN, HIGH);
				CAN_resetCounter = 0;
			}
		}
		vTaskDelay(delay);
	}
}
void TaskReadSerial(void* pvParameters)
{
	unsigned int buttonPins[] = { BTN_HAZARD,BTN_HEADLIGHT,BTN_HORN,BTN_LSIG,BTN_RSIG,BTN_WIPER,BTN_RADIO };
	//setDebounce(buttonPins, 7, 0xff);
	QueueItem outgoing;
	char payload[MAX_STRING_LEN];
	TickType_t delay = pdMS_TO_TICKS(200);
	pinMode(CAN_RST_PIN, OUTPUT);
	digitalWrite(CAN_RST_PIN, HIGH);
	while (1)
	{

		if (Serial1.available())
		{
			int bytesRead = Serial1.readBytesUntil('\n', payload, MAX_STRING_LEN);
			if(bytesRead>0)
				payload[bytesRead-1] = '\0';
			// ------------------------------ covert back -----------------------------------
			bool convertSuccess = QueueItem::toQueueItem(payload, &outgoing);
			if(convertSuccess)
				xQueueSend(queueForDisplay, &outgoing, 100);
			//debugPrint(payload, bytesRead);
		}

		vTaskDelay(delay);
	}
}
//centerLCD.drawRGBBitmap(77, 0, image, 150, 150);
//centerLCD.setAddrWindow(120, 80, 240, 160);
//centerLCD.pushColors(image, 9600, true);
//centerLCD.fillRect(240, 80, 120, 80, ILI9488_RED);
/*
// voltage is an indication of capacitor charge, goes right
DisplayBar capVolt = DisplayBar(&centerLCD, 240+0, 160+30, 240, 80);
// current flowing from Fuel Cell into Capacitor, goes right
DisplayBar capCurrent_charge = DisplayBar(&centerLCD, 240+0, 160-30-80, 240, 80);
// current drawn from capacitor, goes left
DisplayBar capCurrent_discharge = DisplayBar(&centerLCD, 240-240, 160-30-80, 240, 80);
// current drawn from capacitor + FC, goes left
DisplayBar motorCurrent = DisplayBar(&centerLCD, 240-240, 160+30, 240, 80);

// dummy
DisplayText t = DisplayText(&centerLCD, 0, 0, 100, 40);

t.setMargin(12);
t.setColors(ILI9488_WHITE, ILI9488_BLACK);
capVolt.setMargin(6);
capCurrent_charge.setMargin(6);
capCurrent_discharge.setMargin(6);
motorCurrent.setMargin(6);

capVolt.setRange(45, 60);
capVolt.setOrientation(DisplayBar::LEFT_TO_RIGHT);
capVolt.setColors(ILI9488_CYAN, ILI9488_BLACK);
capCurrent_charge.setRange(0, 20);
capCurrent_charge.setOrientation(DisplayBar::LEFT_TO_RIGHT);
capCurrent_charge.setColors(ILI9488_DARKCYAN, ILI9488_BLACK);
capCurrent_discharge.setRange(0, 20);
capCurrent_discharge.setOrientation(DisplayBar::RIGHT_TO_LEFT);
capCurrent_discharge.setColors(ILI9488_MAROON, ILI9488_BLACK);
motorCurrent.setRange(0, 20);
motorCurrent.setOrientation(DisplayBar::RIGHT_TO_LEFT);
motorCurrent.setColors(ILI9488_RED, ILI9488_BLACK);

while (1)
{
char randStr[4];
itoa(random(45, 60), randStr, 10);
capVolt.update(randStr);
itoa(random(0, 20), randStr, 10);
capCurrent_charge.update(randStr);
itoa(random(0, 20), randStr, 10);
capCurrent_discharge.update(randStr);
itoa(random(0, 20), randStr, 10);
motorCurrent.update(randStr);
vTaskDelay(pdMS_TO_TICKS(300));

}
*/
void dummyData(QueueItem* q, DataSource id) {
	q->ID = id;
	int i, j;
	switch (id)
	{
	case FC:
		i = 2; j = 8;
		break;
	case CS:
		i = 3; j = 2;
		break;
	case SM:
		i = 1; j = 1;
		break;
	}
	for (int _i = 0; _i < i; _i++)
	{
		for (int _j = 0; _j < j; _j++)
		{
			q->data[_i][_j] = random(0, 10);
		}
	}
	switch (id)
	{
	case FC:
		q->data[0][7] = random(0, 1);
		q->data[0][3] = random(0, 100);
		break;
	case CS:
		q->data[2][1] = random(0, 40);
		q->data[2][0] = random(45, 60);
		break;
	case SM:
		
		break;
	}

}
void debugPrint(char* toPrint, int len)
{
	// DEBUG printing that prints out special bytes as uint
	int counter = 0;
	while (counter < len)
	{
		char tmp = toPrint[counter++];
		if (isPrintable(tmp))
			Serial.print(tmp);
		else
		{
			Serial.print("<");
			Serial.print((uint8_t)tmp);
			Serial.print(">");
		}
	}
	Serial.print("[");
	Serial.print(Serial.available());
	Serial.print("]");
	Serial.println("__");

}
void setDebounce(unsigned int pins[], uint8_t numPins, uint16_t waitTimeMultiplier)
{	
	/*
	http://ww1.microchip.com/downloads/en/devicedoc/atmel-11057-32-bit-cortex-m3-microcontroller-sam3x-sam3a_datasheet.pdf
	page 630 lists help on which PIO registers to fiddle to enable debouncing
	need to find out clock divider value, max value is 2^14 (PIO_SCDR)
	https://www.arduino.cc/en/Hacking/PinMappingSAM3X
	to help convert pins into PIOxyy (x = A, B, C, D) (y = a number in range [0, 31])
	*/
	uint32_t pinsBitMask_A = 0, pinsBitMask_B = 0, pinsBitMask_C = 0, pinsBitMask_D = 0;
	for (uint8_t i = 0; i < numPins; i++)
	{
		switch ((uint32_t)digitalPinToPort(pins[i]))
		{
		case (uint32_t)PIOA:
			pinsBitMask_A |= digitalPinToBitMask(pins[i]);
			break;
		case (uint32_t)PIOB:
			pinsBitMask_B |= digitalPinToBitMask(pins[i]);
			break;
		case (uint32_t)PIOC:
			pinsBitMask_C |= digitalPinToBitMask(pins[i]);
			break;
		case (uint32_t)PIOD:
			pinsBitMask_D |= digitalPinToBitMask(pins[i]);
			break;
		}
	}
	/*REG_PIOB_IFER |= 1 << 26; 
	PIOB->PIO_DIFSR |= 1 << 26;
	PIOB->PIO_SCDR |= 0xff;
	*/
	// Input Filter Enable Register:				Enables these bits to enable inupt filtering
	PIOA->PIO_IFER = pinsBitMask_A;
	PIOB->PIO_IFER = pinsBitMask_B;
	PIOC->PIO_IFER = pinsBitMask_C;
	PIOD->PIO_IFER = pinsBitMask_D;
	// Debouncing Input Filter Select Register:		We want Debounce filter, not Glitch filter! Debounce = 1, Glitch = 0
	PIOA->PIO_DIFSR = pinsBitMask_A;
	PIOB->PIO_DIFSR = pinsBitMask_B;
	PIOC->PIO_DIFSR = pinsBitMask_C;
	PIOD->PIO_DIFSR = pinsBitMask_D;
	// Slow Clock Divider Register:					Too big = unreponsive, Too small = can't feel the debounce
	PIOA->PIO_SCDR = 0xff;
	PIOB->PIO_SCDR = 0xff;
	PIOC->PIO_SCDR = 0xff;
	PIOD->PIO_SCDR = 0xff;
}