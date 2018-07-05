// 
// 
// 

#include "DisplayInterface.h"

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
	char toPrint[20] = ""; // instantiate all to '\0'
	// navigate pointer to start of real readings (skip millis)
	char *data = received.data;
	while (*data != '\t')
		data++;

	switch (received.ID)
	{
	case FuelCell:
		screen.setCursor(0, 0);

		screen.print("FM: ");
		strncpy(toPrint, data + 1, 14); // 14 = volt(5) + amp(5) + watt(5) - space(1)
		screen.println(toPrint);

		screen.print("FS: ");
		strncpy(toPrint, data + 1 + 14 + 5 + 1 + 1, 14); // 21 = 14 + 5(Wh) + 1(space) + 1(offset to point to FS data)
		screen.println(toPrint);

		break;
	case Motor:
		break;
	}
}
DisplayTFT::DisplayTFT(TFT_ILI9163C& screenPtr):screen(screenPtr)
{
	screen.begin();
	screen.setRotation(2);
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
	char *data = received.data;
	while (*data != '\t')
		data++;
	// change data string into float
	switch (received.ID)
	{
	case FuelCell:
		
		break;
	case Motor:
		for (int i = 0; i < 4; i++)
		{
			while (*data != '\t')data++;
			float thisInfo = atof(++data);
			uint8_t mapped_y = map(thisInfo, 0, 20, 127, 0);
			screen.fillRect(i * 128/8 + 5, 0, 128 / 16 - 1, mapped_y, BLACK);
			screen.fillRect(i * 128/8 + 5, 0 + mapped_y, 128 / 16 - 1, 127 - mapped_y, WHITE);
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