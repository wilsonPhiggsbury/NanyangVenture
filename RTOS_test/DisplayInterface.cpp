// 
// 
// 

#include "DisplayInterface.h"
#include "MotorLogger.h"

// Color definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF

DisplayLCD::DisplayLCD(LiquidCrystal_I2C& screenPtr):screen(screenPtr)
{
	screen.begin(20, 4);
	screen.backlight();
}
void DisplayLCD::printData(QueueItem& received)
{
	char toPrint[16] = ""; // instantiate all to '\0'
	// navigate pointer to start of real readings (to skip the millis)
	char *data = received.data;
	while (*(++data) != '\t');

	switch (received.ID)
	{
	case FuelCell:
		uint8_t paramsToCut, counter;

		if (data[1] == '-')
		{
			screen.setCursor(0, 1);
			screen.print(F("No data from Master!"));
		}
		else
		{
			// cut out the tabs, replace by " "
			screen.setCursor(0, 1);
			screen.print(F("                    "));
			screen.setCursor(0, 0);
			screen.print("FM:");
			data = parseFuelCellData(toPrint, data);
			screen.print(toPrint);
		}

		// move data pointer to FS section, and clear toPrint
		while (*(++data) != '\t');
		toPrint[0] = '\0';

		// do it again
		if (data[1] == '-')
		{
			screen.setCursor(0, 3);
			screen.print(F("No data from Slave!"));
		}
		else
		{
			screen.setCursor(0, 1);
			screen.print(F("                    "));
			screen.setCursor(0, 2);
			screen.print("FS:");
			data = parseFuelCellData(toPrint, data);
			screen.print(toPrint);
		}

		break;
	case Motor:
		break;
	}
}
char* parseFuelCellData(char* dest, char* source)
{
	uint8_t paramsToCut = 3;
	uint8_t counter = 0;
	while (paramsToCut > 0)
	{
		counter++;
		if (*(source + counter) == '\t')
		{
			paramsToCut--;
			strcat(dest, " ");
			strncat(dest, source + 1, counter - 1); // 14 = volt(5) + amp(5) + watt(5) - space(1)
			source += counter;
			counter = 0;
		}
	}
	return source;
}
DisplayTFT::DisplayTFT(TFT_ILI9163C& screenPtr):screen(screenPtr)
{
	screen.begin();
	screen.setRotation(3);
	screen.fillScreen(BLACK);
	/*
	screen.setTextWrap(true);
	screen.setTextColor(WHITE, BLACK);
	screen.setTextSize(1);
	screen.setCursor(0, 0);*/
}
void DisplayTFT::printData(QueueItem& received)
{
	char toPrint[20] = ""; // instantiate all to '\0'
	// skip the millis
	char *data = received.data;
	while (*data != '\t')
		data++;
	// change data string into float
	switch (received.ID)
	{
	case FuelCell:
		
		break;
	case Motor:
		int16_t cursorX = 0;
		for (int i = 0; i < 4; i++)
		{
			while (*data != '\t')data++;
			float thisInfo = atof(++data);
			uint8_t mapped_y;
			// input is either volts or amps, alternating and separated by tabs '\t'
			if(i%2 == 0)mapped_y = map(thisInfo, V_0, V_N, 127, 0);
			else mapped_y = map(thisInfo, A_0, A_N, 127, 0);
			// do color: lower = red, higher = blue
			uint8_t r, g, b, tmp_mapped_y;
			tmp_mapped_y = constrain(mapped_y, 32, 95);
			r = map(tmp_mapped_y, 32, 95, 0, 255);
			g = map(tmp_mapped_y, 32, 95, 127, 0);
			b = map(tmp_mapped_y, 32, 95, 255, 0);
			uint16_t color = screen.Color565(r,g,b);
			//uint8_t horizontalPos = (i + 1) * 8 + (i * 11);
			cursorX += 8;
			if (i < 2)
			{
				screen.fillRect(cursorX, 0, 128 / 16 - 1, mapped_y, BLACK);
				screen.fillRect(cursorX, 0 + mapped_y, 128 / 16 - 1, 127 - mapped_y, color);
			}
			else
			{
				screen.fillRect(cursorX + 128-(4 * (8 + 11)), 0, 128 / 16 - 1, mapped_y, BLACK);
				screen.fillRect(cursorX + 128-(4 * (8 + 11)), 0 + mapped_y, 128 / 16 - 1, 127 - mapped_y, color);
			}
			cursorX += 11;
		}
		
		break;
	}

	/*dataToWidth = map(datax[i], 0, 1024, (barHeight), 0);
	uint8_t b = map(datax[i], 0, 1024, 255, 0);
	uint8_t g = map(datax[i], 0, 1024, 128, 0);
	uint8_t r = map(datax[i], 0, 1024, 0, 255);
	div = (barHeight) / 10;
	color = tft.Color565(r, g, b);*/
}