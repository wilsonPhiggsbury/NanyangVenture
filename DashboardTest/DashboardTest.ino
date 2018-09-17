/*
 Name:		DashboardTest.ino
 Created:	8/31/2018 2:34:05 AM
 Author:	MX
*/
//#include <FreeRTOS_AVR.h>
//#include <basic_io_avr.h>
//#include <timers.h>
//#include <task.h>
//#include <StackMacros.h>
//#include <semphr.h>
//#include <queue.h>
//#include <projdefs.h>
//#include <portmacro.h>
//#include <portable.h>
//#include <mpu_wrappers.h>
//#include <list.h>
//#include <FreeRTOSConfig.h>
//#include <FreeRTOS.h>
//#include <event_groups.h>
//#include <deprecated_definitions.h>
//#include <croutine.h>
#include <ILI9488.h>
#include <SPI.h>
// the setup function runs once when you press reset or power the board
ILI9488 screen = ILI9488(10, 7, 8);
uint8_t screenLED = 7;
uint16_t vPos = 0;
uint16_t hPos = 0;
uint16_t margin = 12;
uint16_t textSize = 36;
uint32_t prevMicros;
void setup() {
	pinMode(screenLED, OUTPUT);
	digitalWrite(screenLED, HIGH);
	screen.begin();
	screen.setRotation(1);
	testText(screen);
}

// the loop function runs over and over again until power down or reset
void loop() {

}

unsigned long testText(ILI9488& tft) {
	tft.fillScreen(ILI9488_PURPLE);
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