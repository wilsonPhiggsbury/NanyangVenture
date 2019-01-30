/*
 Name:		NV10_back.ino
 Created:	6/19/2018 1:42:24 PM
 Author:	MX
*/

#include <MemoryFree.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
//#include <FreeRTOS_AVR.h>
#include <Adafruit_NeoPixel.h>
#include <CAN_Serializer.h>
#include <SPI.h>
#include <SD.h>
#include "Behaviour.h"

//#include "JoulemeterDisplay.h"	
#include "CurrentSensorLogger.h"
#include "FuelCellLogger.h"
#include "Speedometer.h"

#include "Pins_back.h"

// file names
const char FUELCELL_FILENAME[] = "FC.txt";
const char CURRENTSENSOR_FILENAME[] = "CS.txt";
const uint8_t FILENAME_HEADER_LENGTH = 1 + 8 + 1;
// sample filename: /LOG_0002/12345678.txt   1+8+1+8+4+1, 
//											  ^^^ | ^^^
//										   HEADER | FILENAME

// define queues
QueueHandle_t queueForLogSend = xQueueCreate(1, sizeof(Packet));
QueueHandle_t queueForCAN = xQueueCreate(1, sizeof(Packet));
//QueueHandle_t queueForDisplay = xQueueCreate(1, sizeof(Packet));
TaskHandle_t taskBlink;
// define instances of main modules
HESFuelCell hydroCells[NUM_FUELCELLS] = {
	HESFuelCell(0, &FC_MASTER_PORT)
};
AttopilotCurrentSensor motors[NUM_CURRENTSENSORS] = {
	AttopilotCurrentSensor(0,CAP_IN_VPIN,CAP_IN_APIN),
	AttopilotCurrentSensor(1,CAP_OUT_VPIN,CAP_OUT_APIN),
	AttopilotCurrentSensor(2,MOTOR_VPIN,MOTOR_APIN)
};
// wheel diameter is 545 mm, feed into speedo
Speedometer speedo = Speedometer(0, 545/2);
Adafruit_NeoPixel lstrip = Adafruit_NeoPixel(7, LSIG_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rstrip = Adafruit_NeoPixel(7, RSIG_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lightstrip = Adafruit_NeoPixel(7, RUNNINGLIGHT_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel brakestrip = Adafruit_NeoPixel(7, BRAKELIGHT_PIN, NEO_GRB + NEO_KHZ800);
CAN_Serializer serializer;
// define globals
bool SD_avail = false, CAN_avail = false;
char path[FILENAME_HEADER_LENGTH + 8 + 4 + 1]; // +8 for filename, +4 for '.txt', +1 for '\0'

// define tasks, types are: input, control, output
void TaskLogFuelCell(void *pvParameters);		// Input task:		Refreshes class variables for fuel cell Volts, Amps, Watts and Energy
void TaskLogCurrentSensor(void *pvParameters);	// Input task:		Refreshes class variables for motor Volts and Amps
void QueueOutputData(void *pvParameters);	// Control task:	Controls frequency to queue payload from above tasks to output tasks
void LogSendData(void *pvParameters);		// Output task:		Data logged in SD card and sent through XBee. Logged and sent payload should be consistent, hence they are grouped together
void TaskCAN(void* pvParameters);			// In/Out task:		Manages 2-way CAN bus comms
void TaskBlink(void *pvParameters);			// 

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
	SD_avail = initSD(path);
	Serial.print("SD avail: ");
	Serial.println(SD_avail);
	Serial.print("CAN avail: ");
	Serial.println(CAN_avail);

	// initialize speedometer
	pinMode(SPEEDOMETER_INTERRUPT_PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(SPEEDOMETER_INTERRUPT_PIN), storeWheelInterval_ISR, FALLING);

	// Now set up all Tasks to run independently. Task functions are found in Tasks.ino
	xTaskCreate(
		TaskLogFuelCell
		, (const portCHAR *)"Fuel"
		, 750
		, hydroCells
		, 3
		, NULL);
	xTaskCreate(
		TaskLogCurrentSensor
		, (const portCHAR *)"CSensor"
		, 200
		, motors
		, 3
		, NULL);
	xTaskCreate(
		QueueOutputData
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 275  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL // Any pointer to pass in
		, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);
	xTaskCreate(
		LogSendData
		, (const portCHAR *) "LogSend"
		, 800  // Stack size
		, NULL
		, 1  // Priority
		, NULL);
	xTaskCreate(
		TaskBlink
		, (const portCHAR *)"SIG"
		, 150 // -25
		, NULL
		, 3
		, &taskBlink);
	xTaskCreate(
		TaskCAN
		, (const portCHAR *) "CAN la!" // where got cannot?
		, 1200  // Stack size
		, NULL
		, 2  // Priority
		, NULL);
	//vTaskStartScheduler();
	debug_(F("Free Memory in Bytes: "));
	debug(freeMemory());
}

void loop()
{
	// Empty. Things are done in Tasks.
}
