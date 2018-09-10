/*
 Name:		DashboardTest.ino
 Created:	8/31/2018 2:34:05 AM
 Author:	MX
*/
#include <ILI9488.h>>
// the setup function runs once when you press reset or power the board
ILI9488 screen = ILI9488(4, 5, 3);
uint16_t vPos = 0;
uint16_t hPos = 0;
uint16_t margin = 12;
uint16_t textSize = 36;
uint32_t prevMicros;
void setup() {
	
}

// the loop function runs over and over again until power down or reset
void loop() {

}

unsigned long testText(ILI9488& tft) {
	tft.fillScreen(ILI9488_BLACK);
	unsigned long start = micros();
	tft.setCursor(0, 0);
	tft.setTextColor(ILI9488_WHITE);  tft.setTextSize(1);
	tft.println("Hello World!");
	tft.setTextColor(ILI9488_YELLOW); tft.setTextSize(2);
	tft.println(1234.56);
	tft.setTextColor(ILI9488_RED);    tft.setTextSize(3);
	tft.println(0xDEADBEEF, HEX);
	tft.println();
	tft.setTextColor(ILI9488_GREEN);
	tft.setTextSize(5);
	tft.println("Groop");
	tft.setTextSize(2);
	tft.println("I implore thee,");
	tft.setTextSize(1);
	tft.println("my foonting turlingdromes.");
	tft.println("And hooptiously drangle me");
	tft.println("with crinkly bindlewurdles,");
	tft.println("Or I will rend thee");
	tft.println("in the gobberwarts");
	tft.println("with my blurglecruncheon,");
	tft.println("see if I don't!");
	return micros() - start;
}