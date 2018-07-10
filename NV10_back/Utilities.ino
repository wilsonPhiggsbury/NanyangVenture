

void printEEPROM()
{
	for (int address = 0; address < EEPROM.length(); address += sizeof(unsigned int))
	{
		unsigned int tis;
		EEPROM.get(address, tis);
		if (tis == 65535)continue;
		//Serial.print(address);
		//Serial.print(" ");
		//Serial.println(tis);
	}
}
void wipeEEPROM()
{
	debug(F("Wiping EEPROM..."));
	for (int address = 0; address < EEPROM.length(); address += 1)
	{
		uint16_t tis;
		EEPROM.write(address, 255);
	}
	debug(F("EEPROM wiped."));
}

bool initSD(char* path)
{
	if (!SD.begin(SD_SPI_CS))
	{
		debug(F("SD card not present!"));
		return false;
	}
	strcat(path, "/");
	strcat(path, FILENAME_HEADER);
	strcat(path, "_");

	uint8_t fileNameLength = strlen(FILENAME_HEADER);
	char index[FILENAME_INDEX+1] = "";

	File f = SD.open("/");
	File sub;
	int existingIndex = atoi(index);
	while (sub = f.openNextFile())
	{
		if (sub.isDirectory())
		{
			debug("Examining: ");
			debug(sub.name());
			int thisIndex = atoi(sub.name() + fileNameLength + 1);
			existingIndex = max(thisIndex, existingIndex);
		}
	}
	
	// pad '0' on the front if number very small
	uint8_t paddingZeroCounter = FILENAME_INDEX;
	int existingIndex_tmp = existingIndex + 1;
	while (existingIndex_tmp > 0)
	{
		existingIndex_tmp /= 10;
		paddingZeroCounter--;
	}

	for (uint8_t i = 0; i < paddingZeroCounter; i++)
		index[i] = '0';
	if(paddingZeroCounter != FILENAME_INDEX)
		itoa(existingIndex+1, index+paddingZeroCounter, DEC);

	f.close();
	sub.close();
	// construct the path
	strcat(path, index);
	SD.mkdir(path);
	// initialize the interior folder structure
	strcpy(path + FILENAME_HEADER_LENGTH, FUELCELL_FILENAME);
	sub = SD.open(path, FILE_WRITE);
	sub.close();
	strcpy(path + FILENAME_HEADER_LENGTH, MOTOR_FILENAME);
	sub = SD.open(MOTOR_FILENAME, FILE_WRITE);
	sub.close();
	strcpy(path + FILENAME_HEADER_LENGTH, FUELCELL_RAW_FILENAME);
	sub = SD.open(FUELCELL_RAW_FILENAME, FILE_WRITE);
	sub.close();
	return true;
}