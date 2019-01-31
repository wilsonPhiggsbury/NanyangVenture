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
	const uint16_t folderEntries = card.rootDirEntryCount();
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
		delay(200);
	}

	// Make new directory and operate within it.
	char folderPath[18] = "/NV10_";	// max 8 chars. len("NV10_") = 5, pad 3 digits to entry
	sprintf(folderPath, "%03d", folderEntries);	// pad '0' on the front if number contains less than 3 digits
	card.mkdir(folderPath);
	card.chdir(folderPath);

	File f;
	// initialize FC column text
	f = card.open("FC.txt", FILE_WRITE);
	f.println(F("\tMillis\t  V_m  A_m   W_m   Wh_mTmp_m Pres_m Vcap_m State_m\t  V_s  A_s   W_s   Wh_sTmp_s Pres_s Vcap_s State_s"));
	f.close();
	// initialize CS column text
	f = card.open("CS.txt", FILE_WRITE);
	f.println(F("\tMillis\t V_cI A_cI\t V_cO A_cO\t V_MT A_MT"));
	f.close();
	// initialize SM column text
	f = card.open("SM.txt", FILE_WRITE);
	f.println(F("\tMillis\t km/h"));
	f.close();
	
	return true;
}
void storeWheelInterval_ISR()
{
	speedo.storeWheelInterval();
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