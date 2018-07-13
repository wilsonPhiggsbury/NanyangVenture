
void populateAttribute(char which, uint16_t allData[NUM_MOTORS][max(V_ENTRIES, A_ENTRIES)])
{
	uint16_t entries = MotorLogger::getEntries(which);
	debug();debug(which);

	char thisLine[19] = "";
	// wait 1s for data, "--\n" will make it start listening for input. all other strings merely refresh the timeout
	uint8_t timeout;
	timeout = 5000;
	while (strcmp(thisLine, "--\n"))
	{
		while (!Serial.available() && timeout > 0)
		{
			delay(1);
			timeout--;
		}
		thisLine[0] = '\0';
		Serial.readStringUntil('\n').toCharArray(thisLine, 19);
		if (timeout == 0)
		{
			debug(F("Timeout at start!"));
			return;
		}
	}
	// flood per line with 50ms delay between
	for (int j = 0; j < entries; j++)
	{
		

		// read this incoming line 
		// (it is the responsibility for PC side python program to gurante incoming line ends with \n)
		//Serial.readStringUntil('\n').toCharArray(thisLine, 19);
		strcpy(thisLine, "0\t111\t122\t123\n");
		// first integer used to verify validity, remaining goes to EEPROM assignments
		int thisInt;
		char* ptr;
		ptr = strtok(thisLine, "\t");
		thisInt = atoi(ptr);
		if (thisInt%entries != 0)
		{
			debug(F("EEPROM upload fail!"));
			return;
		}
		for (int i = 0; i < NUM_MOTORS; i++)
		{
			ptr = strtok(NULL, "\t");
			thisInt = atoi(ptr);
			allData[i][j] = thisInt;
			debug_(i);debug_(" ");debug_(j);debug_(": ");debug(thisInt);
		}
	}
	// wait 0.5s for string "**\n" to indicate termination
	timeout = 500;
	while (strcmp(thisLine, "**\n"))
	{
		while (!Serial.available() && timeout > 0)
		{
			delay(1);
			timeout--;
		}
		thisLine[0] = '\0';
		Serial.readStringUntil('\n').toCharArray(thisLine, 19);
		if (timeout == 0)
		{
			debug(F("Timeout at termination!"));
			return;
		}
	}
	for (int i = 0; i < NUM_MOTORS; i++)
	{
		motors[i].updateTable(which, allData[i]);
	}
}

void printEEPROM()
{
	for (int address = 0; address < EEPROM.length(); address += sizeof(uint16_t))
	{
		uint16_t tis;
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
	uint16_t allData[NUM_MOTORS][max(V_ENTRIES,A_ENTRIES)];
	populateAttribute('V', allData);
	populateAttribute('A', allData);
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