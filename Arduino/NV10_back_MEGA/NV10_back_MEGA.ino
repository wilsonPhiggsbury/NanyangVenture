/*
 Name:		NV10_back.ino
 Created:	6/19/2018 1:42:24 PM
 Author:	MX
*/

#include <Arduino_FreeRTOS.h>
#include <queue.h>
//#include <FreeRTOS_AVR.h>
#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_ADS1015.h>
#include <Adafruit_NeoPixel.h>
#include "Behaviour.h"

//#include "JoulemeterDisplay.h"	

#include <NV10FuelCell.h>
#include <NV10CurrentSensor.h>
#include <NV10CurrentSensorStats.h>
#include <NV10AccesoriesStatus.h>

#include "Pins_back.h"

// file names
SdFat card;
CANSerializer serializer;
// wheel diameter is 545 mm, feed into speedo
//Speedometer speedo = Speedometer(0, 545/2);
Adafruit_NeoPixel lstrip = Adafruit_NeoPixel(7, LSIG_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rstrip = Adafruit_NeoPixel(7, RSIG_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lightstrip = Adafruit_NeoPixel(7, RUNNINGLIGHT_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel brakestrip = Adafruit_NeoPixel(7, BRAKELIGHT_PIN, NEO_GRB + NEO_KHZ800);
const uint32_t RUNNING_LIGHT_COLOR = Adafruit_NeoPixel::Color(255, 255, 255);
const uint32_t BRAKE_LIGHT_COLOR = Adafruit_NeoPixel::Color(255, 0, 0);
const uint32_t SIGNAL_LIGHT_COLOR = Adafruit_NeoPixel::Color(255, 165, 0);
const uint32_t NO_LIGHT = Adafruit_NeoPixel::Color(0, 0, 0);
// sample filename: /LOG_0002/12345678.txt   1+8+1+8+4+1, 
//											  ^^^ | ^^^
//										   HEADER | FILENAME

// initialize data points. Their names are prepended with 'dp'
NV10FuelCell dpFCMain(0x01), dpFCBackup(0x02);
NV10CurrentSensor dpCS(0x03);
NV10CurrentSensorStats dpCSStats(0x04);

NV10AccesoriesStatus dpStatus(0x05);
DataPoint* dpReceiveList[] = { &dpStatus }; // make a collection of all DPs, scan them whenever we receive message
struct DataForLogSend
{
	bool logThis, sendThis;
	char data[100];
	DataForLogSend(bool log, bool send) :logThis(log), sendThis(send) {};
};
// define queues
QueueHandle_t queueForLogSend = xQueueCreate(1, sizeof(struct DataForLogSend));
QueueHandle_t queueForCAN = xQueueCreate(1, sizeof(CANFrame));
//QueueHandle_t queueForDisplay = xQueueCreate(1, sizeof(Packet));
TaskHandle_t taskBlink;
// define instances of main modules

//CAN_Serializer serializer;
// define globals
bool SD_avail = false, CAN_avail = false;

// define tasks, types are: input, control, output
void logFuelCell(void *pvParameters);		// Input task:		Refreshes class variables for fuel cell Volts, Amps, Watts and Energy
void logCurrentSensor(void *pvParameters);	// Input task:		Refreshes class variables for motor Volts and Amps
void outputToSdSerial(void *pvParameters);		// Output task:		Data logged in SD card and sent through XBee. Logged and sent payload should be consistent, hence they are grouped together
void ioForCAN(void* pvParameters);			// In/Out task:		Manages 2-way CAN bus comms
void blinkRGB(void *pvParameters);			// 
void taskTest(void*);

//// _______________OPTIONAL_____________
//void TaskReceiveCommands(void *pvParameters);	// Input task:		Enable real-time control of Arduino (if any)
//void TaskRefreshPeripherals(void *pvParameters);// Control task:	Updates all CAN-BUS peripherals


// the setup function runs once when you press reset or power the board
/// <summary>
/// Collects data from 3 different sources: Fuel Cell, Current Sensor, SpeedoMeter.
/// Outputs data to 2 different places: SD card & XBee Serial
/// Manages running lights, brake lights, signal lights.
/// </summary>
void setup() {
	Serial.begin(9600);
	delay(100);
	CAN_avail = serializer.init(CAN_CS_PIN);
	// create all files in a new directory
	SD_avail = initSD(card);
	Serial.print("SD avail: ");
	Serial.println(SD_avail);
	Serial.print("CAN avail: ");
	Serial.println(CAN_avail);

	// initialize speedometer
	//pinMode(SPEEDOMETER_INTERRUPT_PIN, INPUT_PULLUP);
	//attachInterrupt(digitalPinToInterrupt(SPEEDOMETER_INTERRUPT_PIN), storeWheelInterval_ISR, FALLING);

	// Now set up all Tasks to run independently. Task functions are found in Tasks.ino
	//xTaskCreate(
	//	logCurrentSensor
	//	, (const portCHAR *)"CS"
	//	, 200
	//	, NULL
	//	, 3
	//	, NULL);
	//xTaskCreate(
	//	logFuelCell
	//	, (const portCHAR *)"FC"
	//	, 500
	//	, NULL
	//	, 2
	//	, NULL);
	xTaskCreate(
		blinkRGB
		, (const portCHAR *)"SIG"
		, 150 // -25
		, NULL
		, 2
		, &taskBlink);
	//xTaskCreate(
	//	outputToSdSerial
	//	, (const portCHAR *) "LogSend"
	//	, 800  // Stack size
	//	, NULL
	//	, 1  // Priority
	//	, NULL);
	xTaskCreate(
		ioForCAN
		, (const portCHAR *) "CAN la!" // where got cannot?
		, 1200  // Stack size
		, NULL
		, 1  // Priority
		, NULL);
	xTaskCreate(taskTest, "test", 200, NULL, 2, NULL);
	//vTaskStartScheduler();

}
void taskTest(void*)
{
	char local[20];
	CANFrame f;
	while (1)
	{
		Serial.print("lsig: "); Serial.println(dpStatus.getLsig());
		Serial.print("rsig: "); Serial.println(dpStatus.getRsig());
		Serial.print("brake: "); Serial.println(dpStatus.getBrake());
		Serial.println("-------");
		vTaskDelay(50);
	}
}
void loop()
{
	// Empty. Things are done in Tasks.
}
