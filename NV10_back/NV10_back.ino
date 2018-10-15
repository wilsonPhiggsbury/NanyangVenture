/*
 Name:		NV10_back.ino
 Created:	6/19/2018 1:42:24 PM
 Author:	MX
*/

#include <Arduino_FreeRTOS.h>
#include <queue.h>
//#include <FreeRTOS_AVR.h>
#include <mcp_can.h>
#include "Behaviour.h"
#include <SPI.h>
#include <SD.h>

//#include "JoulemeterDisplay.h"	
#include "CurrentSensorLogger.h"
#include "FuelCellLogger.h"
#include "Speedometer.h"

#include "Wiring.h"

// file names
const char FUELCELL_FILENAME[] = "FC.txt";
const char CURRENTSENSOR_FILENAME[] = "CS.txt";
const uint8_t FILENAME_HEADER_LENGTH = 1 + 8 + 1;
// sample filename: /LOG_0002/12345678.txt   1+8+1+8+4+1, 
//											  ^^^ | ^^^
//										   HEADER | FILENAME

// define queues
QueueHandle_t queueForLogSend = xQueueCreate(1, sizeof(QueueItem));
//QueueHandle_t queueForDisplay = xQueueCreate(1, sizeof(QueueItem));
QueueHandle_t queueForSendCAN = xQueueCreate(1, sizeof(QueueItem));
// define instances of main modules
HESFuelCell hydroCells[NUM_FUELCELLS] = {
	HESFuelCell(0, &FC_MASTER_PORT),
	HESFuelCell(1, &FC_SLAVE_PORT)
};
AttopilotCurrentSensor motors[NUM_CURRENTSENSORS] = {
	AttopilotCurrentSensor(0,CAP_IN_VPIN,CAP_IN_APIN),
	AttopilotCurrentSensor(1,CAP_OUT_VPIN,CAP_OUT_APIN),
	AttopilotCurrentSensor(2,MOTOR_VPIN,MOTOR_APIN)
};
// ATTR: wheel diameter
Speedometer speedo = Speedometer(0, 545);
MCP_CAN CANObj = MCP_CAN(CAN_CS_PIN);
// define globals
bool SD_avail;
bool CAN_avail;
char path[FILENAME_HEADER_LENGTH + 8 + 4 + 1]; // +8 for filename, +4 for '.txt', +1 for '\0'

// define tasks, types are: input, control, output
void ReadFuelCell(void *pvParameters);		// Input task:		Refreshes class variables for fuel cell Volts, Amps, Watts and Energy
void ReadMotorPower(void *pvParameters);	// Input task:		Refreshes class variables for motor Volts and Amps
void QueueOutputData(void *pvParameters);	// Control task:	Controls frequency to queue payload from above tasks to output tasks
void LogSendData(void *pvParameters);		// Output task:		Data logged in SD card and sent through XBee. Logged and sent payload should be consistent, hence they are grouped together
void DisplayData(void *pvParameters);		// Output task:		Display on LCD screen
void SendCANFrame(void* pvParameters);

//// _______________OPTIONAL_____________
//void TaskReceiveCommands(void *pvParameters);	// Input task:		Enable real-time control of Arduino (if any)
//void TaskRefreshPeripherals(void *pvParameters);// Control task:	Updates all CAN-BUS peripherals


// the setup function runs once when you press reset or power the board
void setup() {
	// initialize serial communication at 9600 bits per second:
	Serial.begin(9600);
	delay(100);

	// create all files in a new directory
	SD_avail = initSD(path);
	if (SD_avail)
		HESFuelCell::setPath(path);
	// initialize speedometer
	attachInterrupt(digitalPinToInterrupt(SPEEDOMETER_INTERRUPT_PIN), storeWheelInterval_ISR, FALLING);
	// initialize CAN bus
	CAN_avail = CANObj.begin(NV_CANSPEED) == CAN_OK;
	if (!CAN_avail)
	{
		Serial.println(F("NV10_back CAN init fail!"));
	}
	else
	{
		Serial.println(F("NV10_back CAN init success!"));
	}
	// Now set up all Tasks to run independently. Task functions are found in Tasks.ino
	xTaskCreate(
		ReadFuelCell
		, (const portCHAR *)"Fuel"
		, 725
		, hydroCells
		, 3
		, NULL);
	xTaskCreate(
		ReadMotorPower
		, (const portCHAR *)"CSensor"
		, 175
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
	if (CAN_avail)
	{
		xTaskCreate(
			SendCANFrame
			, (const portCHAR *) "CAN la!" // where got cannot?
			, 500  // Stack size
			, NULL
			, 1  // Priority
			, NULL);
	}
	vTaskStartScheduler();

}

void loop()
{
	// Empty. Things are done in Tasks.
}
