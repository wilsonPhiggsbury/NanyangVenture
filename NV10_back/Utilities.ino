
void wipeEEPROM()
{
	for (int address = 0; address < EEPROM.length(); address += 1)
	{
		uint16_t tis;
		EEPROM.write(address, 255);
	}
}
bool initSD(char* path)
{
	const uint8_t FILENAME_INDEX = 4;
	if (!SD.begin(SD_SPI_CS))
	{
		return false;
	}
	strcpy(path, "/");
	strcat(path, "LOG"); // Arduino does not like lengthy path names. Please keep to under 4 alphabets.
	strcat(path, "_");

	// Comb through existing files in SD card to obtain the latest index. Use it to name our new folder.
	File f = SD.open("/");
	File sub;
	char index[FILENAME_INDEX + 1] = "";
	int existingIndex = atoi(index);
	while (sub = f.openNextFile())
	{
		if (sub.isDirectory())
		{
			int thisIndex = atoi(sub.name() + strlen(path) - 1); // -1 for excluding the '/' at the beginning, as sub.name() does not contain '/'
			existingIndex = max(thisIndex, existingIndex);
		}
	}
	
	// pad '0' on the front if number contains less than 4 digits
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

	// A new index number is assigned to *path! e.g. /LOG_0002
	strcat(path, index);
	SD.mkdir(path);
	// initialize the interior folder structure
	strcat(path, "/");

	strcpy(path + FILENAME_HEADER_LENGTH, FUELCELL_FILENAME);
	sub = SD.open(path, FILE_WRITE);
	sub.println(F("Millis\tV_m\tA_m\tW_m\tWh_m\tVcap_m\tState"));
	sub.close();

	strcpy(path + FILENAME_HEADER_LENGTH, MOTOR_FILENAME);
	sub = SD.open(path, FILE_WRITE);
	sub.println(F("Millis\tV_left\tV_right\tV_cap\tA_left\tA_right\tA_cap"));
	sub.close();

	strcpy(path + FILENAME_HEADER_LENGTH, MASTER_FUELCELL_RAW_FILENAME);
	sub = SD.open(path, FILE_WRITE);
	sub.close();
	strcpy(path + FILENAME_HEADER_LENGTH, SLAVE_FUELCELL_RAW_FILENAME);
	sub = SD.open(path, FILE_WRITE);
	sub.close();
	// *path should always contain /LOG_****/
	// We will still use *path variable whenever we write to a file. 
	// Clean up trailing file names after use.
	strcpy(path + FILENAME_HEADER_LENGTH, "");
	return true;
}