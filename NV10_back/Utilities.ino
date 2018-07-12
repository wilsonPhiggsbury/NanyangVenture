

void printEEPROM()
{
	for (int address = 0; address < EEPROM.length(); address += sizeof(unsigned int))
	{
		unsigned int tis;
		EEPROM.get(address, tis);
		if (tis == 65535)continue;
		Serial.print(address);
		Serial.print(" ");
		Serial.println(tis);
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
void populateEEPROM()
{
	unsigned int allData[NUM_MOTORS][max(V_ENTRIES,A_ENTRIES)];
	populateAttribute('V', allData);
	populateAttribute('A', allData);
}
void populateAttribute(char which, unsigned int allData[NUM_MOTORS][max(V_ENTRIES,A_ENTRIES)])
{
	unsigned int entries = MotorLogger::getEntries(which);
	// flood per line with 50ms delay per line
	for (int j = 0; j < entries; j++)
	{
		// wait 3s for data
		uint8_t timeout = 3000;
		while (!Serial.available() && timeout > 0)
		{
			delay(1);
			timeout--;
		}
		if (timeout == 0)
			break;
		// read this incoming line 
		// (it is the responsibility for PC side python program to gurante incoming line ends with \n)
		char thisLine[16];
		Serial.readStringUntil('\n').toCharArray(thisLine, 16);
		// first integer used to verify validity, remaining goes to EEPROM assignments
		int thisInt;
		char* ptr;
		ptr = strtok(thisLine, "\t");
		if (thisInt%entries != 0)
		{
			debug(F("EEPROM upload fail!"));
			return;
		}
		for (int i = 0; i < NUM_MOTORS; i++)
		{
			strtok(NULL, "\t");
			if (ptr == NULL)
			{
				debug(F("strtok is not working as expected."));
				break;
			}
			thisInt = atoi(ptr);
			allData[i][j] = thisInt;
		}
	}
	for (int i = 0; i < entries; i++)
	{
		motors[i].updateTable(which, allData[i]);
	}
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
	// path charArray is completed! e.g. /LOG_0002
	// construct the path
	strcat(path, index);
	SD.mkdir(path);
	// initialize the interior folder structure
	strcat(path, "/");
	strcpy(path + FILENAME_HEADER_LENGTH + 1, FUELCELL_FILENAME);
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