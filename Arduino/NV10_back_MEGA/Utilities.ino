#include <EEPROM.h>
/// <summary>
/// Initializes SD card if present. Also creates a new folder to store this session's data into.
/// </summary>
/// <param name="path">Generated folder path is stored into here.</param>
/// <returns></returns>
const uint8_t FILENAME_DIGITS = 4;
bool initSD(char* path)
{
	EEPROM.begin();
	// begin SD card connection
	if (!tryReadSd())
	{
		return false;
	}

	strcpy(path, "/LOG_"); // Arduino does not like lengthy path names. Please keep to under 4 alphabets.

	// Comb through existing files in SD card to obtain the latest index. Use it to name our new folder.
	File sub, root = SD.open("/");
	int existingIndex = getNewFolderIndex(sub, root); // start from index 0

	// wipe card (30 is tested to still NOT crash arduino. No gurantee beyond)
	if (existingIndex >= 30)
	{
		promptWipeSd(sub);
	}

	root.close();
	sub.close();

	createDirectory(path, existingIndex);
	createFCLogFiles(path);
	createCSLogFiles(path);
	// *path should always contain /LOG_****/
	// We will still use *path variable whenever we write to a file. 
	// Clean up trailing file names after use.
	strcpy(path + FILENAME_HEADER_LENGTH, "");
	HESFuelCell::setPath(path);
	return true;
}
void storeWheelInterval_ISR()
{
	speedo.storeWheelInterval();
}
bool tryReadSd()
{
	if (!SD.begin(SD_SPI_CS_PIN))
	{
		return false;
	}
	// skip looking at SD card if it is too full.
	if (EEPROM.read(1) == 1)
	{
		EEPROM.write(1, 0);
		Serial.println(F("NOT logging into SD card. SD card full."));
		return false;
	}
	return true;
}

int getNewFolderIndex(File &sub, File &root)
{
	int existingIndex = 0; // start from index 0
	while (sub = root.openNextFile())
	{
		if (sub.isDirectory())
		{
			int thisIndex = atoi(sub.name() + strlen(path) - 1); // -1 for excluding the '/' at the beginning, as sub.name() does not contain '/'
			existingIndex = max(thisIndex, existingIndex);
		}
	}
	return existingIndex;
}

void promptWipeSd(File &sub)
{
	File sub2;
	char index[FILENAME_DIGITS + 1];
		Serial.println(F("SD card is nearing threshold to crash Arduino. Wipe the card? (y/n)"));
		while (!Serial.available());
		delay(100);
		char response = Serial.read();
		while (Serial.available())
			Serial.read();
		if (response == 'y')
		{
			// wipe card
			Serial.println(F("Wiping card..."));
			for (int i = 1; i < 50; i++)
			{
				sprintf(index, "%04d", i);
				strcpy(path + 5, index);
				strcat(path, "/");
				Serial.println(path);

				sub = SD.open(path);
				while (sub2 = sub.openNextFile(FILE_WRITE))
				{
					Serial.print("\t");
					Serial.println(sub2.name());
					strcat(path, sub2.name());
					SD.remove(path);
					strcpy(path + FILENAME_HEADER_LENGTH, "");
					sub2.close();
				}
				sub.close();
				SD.rmdir(path);

				delay(50);
			}
			Serial.println(F("Card wiped. Resetting in..."));
			delay(1000);
			for (int i = 5; i >= 0; i--)
			{
				Serial.print(i);
				Serial.print("...");
				delay(1000);
			}
		}
		else
		{
			// tell arduino to ignore sd card on next reset.
			EEPROM.write(1, 1);
		}
		// reset the arduino
		asm volatile ("  jmp 0");
}
void createFCLogFiles(char* path)
{
	File sub;
	strcpy(path + FILENAME_HEADER_LENGTH, FUELCELL_FILENAME);
	sub = SD.open(path, FILE_WRITE);
	sub.println(F("\tMillis\t  V_m  A_m   W_m   Wh_mTmp_m Pres_m Vcap_m State_m\t  V_s  A_s   W_s   Wh_sTmp_s Pres_s Vcap_s State_s"));
	sub.close();
}
void createCSLogFiles(char* path)
{
	File sub;
	strcpy(path + FILENAME_HEADER_LENGTH, CURRENTSENSOR_FILENAME);
	sub = SD.open(path, FILE_WRITE);
	sub.println(F("\tMillis\t V_cI A_cI\t V_cO A_cO\t V_MT A_MT"));
	sub.close();
}
void createDirectory(char* path, int indexInt)
{
	char index[FILENAME_DIGITS + 1];
	// pad '0' on the front if number contains less than 4 digits
	sprintf(index, "%04d", indexInt + 1);
	// A new index number is assigned to *path! e.g. /LOG_0002
	strcpy(path + 5, index);
	Serial.print("Logging into folder ");	Serial.println(path);
	SD.mkdir(path);
	// initialize the interior folder structure
	strcat(path, "/");	
}