/*
 Name:		ScratchPad.ino
 Created:	6/24/2018 11:37:49 PM
 Author:	MX
*/
#include <TFT_ILI9163C.h>
#include <Arduino_FreeRTOS.h>
#include <Adafruit_GFX.h>

#include <SPI.h>

// Color definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF

#define  NBINS                12
const uint8_t bar_Width = 3;

uint32_t     avrg_TmrF = 0;
uint16_t     t_b[NBINS];

uint16_t datax_;
#define __CS 10
#define __DC 9

void TaskQueueOutputData(void *pvParameters __attribute__((unused)));

TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC);
void setup() {
	Serial.begin(38400);
	//while(!Serial);
	tft.begin();
	tft.setRotation(2);
	tft.fillScreen(BLACK);
	tft.setTextWrap(true);
	tft.setTextColor(WHITE, BLACK);
	tft.setTextSize(1);
	tft.setCursor(0, 0);
	delay(1000);

	datax_ = 0;
	xTaskCreate(
		TaskQueueOutputData
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 400  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL
		, 1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);

}

// the loop function runs over and over again until power down or reset
void loop() {
	
}

void TaskQueueOutputData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	while (1) // A Task shall never return or exit.
	{
		datax_ += 16;
		if (datax_ >= 1024)
			datax_ = 0;
		//Print_Data();
		uint16_t mapped_y = map(datax_, 0, 1023, 127, 0);
		tft.fillRect(0, 0, 128 / 16 - 1, mapped_y, BLACK);
		tft.fillRect(0, 0+mapped_y, 128 / 16 - 1, 127 - mapped_y, WHITE);
		vTaskDelay(pdMS_TO_TICKS(20));  // one tick delay (15ms) in between reads for stability
	}
}
void verticalBarGraphs(uint16_t datax[], uint8_t barWidth, uint8_t barHeight, uint8_t vOrigin) {//3,12,64,10
	uint8_t startX;
	uint16_t color;
	uint8_t dataToWidth;
	uint8_t div;
	for (uint8_t i = 1; i <= NBINS - 1; i++) {
		startX = (i * 11);
		//tft.drawRect((startX-1),vOrigin,barWidth,barHeight,WHITE);//container
		tft.fillRect(startX, (vOrigin + 1), (bar_Width), dataToWidth, BLACK);//mask ok
		tft.fillRect(startX, (dataToWidth + vOrigin) + 1, (bar_Width), ((barHeight) - dataToWidth), color);//fillRect(X,Y,width,height,color)
	}
}