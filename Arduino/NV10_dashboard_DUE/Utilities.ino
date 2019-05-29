void dummyData(Packet* q, PacketID id) {
	q->ID = id;
	int i, j;
	i = FRAME_INFO_SETS[id];
	j = FRAME_INFO_SUBSETS[id];
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
//centerLCD.drawRGBBitmap(77, 0, image, 150, 150);
//centerLCD.setAddrWindow(120, 80, 240, 160);
//centerLCD.pushColors(image, 9600, true);
//centerLCD.fillRect(240, 80, 120, 80, ILI9488_RED);
//void TaskTest(void* pvParameters)
//{
//	pinMode(LCD_OUTPUT_BACKLIGHT, OUTPUT);
//	digitalWrite(LCD_OUTPUT_BACKLIGHT, HIGH);
//	ILI9488 scr = ILI9488(LCDLEFT_SPI_CS, LCD_OUTPUT_DC, LCD_RIGHT_RST);
//	scr.begin();
//	scr.setRotation(1);
//	scr.fillScreen(ILI9488_PURPLE);
//	DisplayBar t = DisplayBar(&scr, 200, 100, 200, 100, DisplayBar::BOTTOM_TO_TOP);
//	t.setColors(ILI9488_WHITE, ILI9488_BLUE);
//	t.setRange(50, 100);
//	int i = 0;
//	while (1)
//	{
//		i += 10;
//		if (i > 100)
//			i = 0;
//		t.updateFloat(i);
//		vTaskDelay(40);
//	};
//}
