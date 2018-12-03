/*
 Name:		DUE_button_test.ino
 Created:	12/3/2018 4:14:21 PM
 Author:	MX
*/
#include "Pins_dashboard.h"
#include <CAN_Serializer.h>
#include <FreeRTOS_ARM.h>

const unsigned int buttonPins[] = { BTN_HAZARD,BTN_HEADLIGHT,BTN_HORN,BTN_LSIG,BTN_RSIG,BTN_WIPER };
int triggered[NUM_BUTTONS];
void TaskTest(void*);
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	setDebounce(buttonPins, NUM_BUTTONS, 900); // PROBLEMATIC
	pinMode(BTN_HEADLIGHT, INPUT_PULLUP);
	attachInterrupt(BTN_HEADLIGHT, [] {
		int* a = &triggered[getIndexFromButton(BTN_HEADLIGHT)];
		if(*a != 1) *a = 1;
		else *a = 0;
	}, FALLING);

}

// the loop function runs over and over again until power down or reset
void loop() {
	if (Serial.available())
	{
		uint16_t newDebounce = Serial.parseInt();
		setDebounce(buttonPins, NUM_BUTTONS, newDebounce);
		Serial.print("New debounce value: ");
		Serial.println(newDebounce, HEX);
	}
	for (uint8_t index = 0; index < NUM_BUTTONS; index++)
	{
		if (triggered[index] != 0)
		{
			Serial.println(triggered[index]);
		}
	}
	//delay(1000);
}
void TaskTest(void* a)
{
	while (1)
	{
		
	}
}
uint8_t getIndexFromButton(uint32_t pin)
{
	for (uint8_t index = 0; index < NUM_BUTTONS; index++)
		if (buttonPins[index] == pin)
			return index;
}

void setDebounce(const unsigned int pins[], uint8_t numPins, uint16_t waitTimeMultiplier)
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
	PIOA->PIO_SCDR = waitTimeMultiplier;
	PIOB->PIO_SCDR = waitTimeMultiplier;
	PIOC->PIO_SCDR = waitTimeMultiplier;
	PIOD->PIO_SCDR = waitTimeMultiplier;
}