/// <summary>
/// Initializes SD card if present. Also creates a new folder to store this session's data into.
/// </summary>
/// <param name="card">Supply a declared SdFat object here.</param>
/// <returns></returns>
const uint8_t FILENAME_DIGITS = 4;
bool initSD(SdFat& card)
{
	if (!card.begin(SD_SPI_CS_PIN))
		return false;
	File f;
	// obtain number of existing entries in SD card
	uint16_t folderEntries = 0;
	char folderPath[18] = "/NV10_";	// max 8 chars. len("NV10_") = 5, pad 3 digits to entry
	do {
		sprintf(folderPath + 6, "%03d", ++folderEntries);	// pad '0' on the front if number contains less than 3 digits
		f = card.open(folderPath);
	} while (f != NULL);
	
	debug_("Folder entries: "); debug(folderEntries);
	// Wipe when too full, or on user request ('~' char)
	if (folderEntries > 30)
	{
		Serial.println(F("SD card is nearing threshold to crash Arduino."));
		initiateWipe(card);
	}
	for (int wipeWindow = 10; wipeWindow >= 0; wipeWindow--)
	{
		if (Serial.read() == '~')
		{
			Serial.println(F("Requesting to wipe SD card."));
			initiateWipe(card);
		}
		flushRX();
		delay(100);
	}

	// Make new directory and operate within it.
	card.mkdir(folderPath);
	card.chdir(folderPath);

	// initialize FC column text
	f = card.open("FC.txt", FILE_WRITE);
	f.println(F("Millis	Watt	P	Tmp	Status"));
	f.close();
	// initialize CS column text
	f = card.open("CS.txt", FILE_WRITE);
	f.println(F("Millis	 Volt	CapIn	CapOut	Motor"));
	f.close();
	// initialize SM column text
	f = card.open("SM.txt", FILE_WRITE);
	f.println(F("Millis\tkm/h"));
	f.close();
	
	return true;
}
void storeWheelInterval_ISR()
{
	//speedo.storeWheelInterval();
}

void initiateWipe(SdFat& card)
{
	File sub2;
	char index[FILENAME_DIGITS + 1];
	Serial.println(F("Wipe the card? (y/n)"));

	char response = '\0';
	while (response != 'y' && response != 'n')
	{
		delay(100);
		response = Serial.read();
	}
	if (response == 'y')
	{
		// wipe card
		Serial.print(F("Wiping card"));
		card.wipe(&Serial);
		Serial.println(F("done"));
		if (!card.begin(SD_SPI_CS_PIN))
		{
			Serial.println(F("Card corrupted. Please format manually.\nArduino will reset in 5 seconds..."));
			delay(5000);
			softReset();
		}
	}
	flushRX();
}
void flushRX()
{
	// clean up any trailing characters in serial RX buffer
	while (Serial.available())
		Serial.read();
}
void softReset()
{
	asm volatile ("  jmp 0");
}

void writeRGB(Adafruit_NeoPixel& light, uint8_t numLights, uint32_t color)
{
	for (int i = 0; i < numLights; i++)
	{
		light.setPixelColor(i, color);
	}
	light.show();
}