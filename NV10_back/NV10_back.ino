/*
 Name:		NV10_back.ino
 Created:	6/19/2018 1:42:24 PM
 Author:	MX
*/

#include <mcp_can.h>
#include "CAN_ID_protocol.h"
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#include "JoulemeterDisplay.h"	
#include "CurrentSensorLogger.h"
#include "FuelCellLogger.h"
#include "Speedometer.h"

#include "Wiring.h"

// define queues
QueueHandle_t queueForLogSend = xQueueCreate(1, sizeof(QueueItem));
QueueHandle_t queueForDisplay = xQueueCreate(1, sizeof(QueueItem));
QueueHandle_t queueForSendCAN = xQueueCreate(1, sizeof(QueueItem));
HESFuelCell hydroCells[NUM_FUELCELLS] = {
	HESFuelCell(0, &FC_MASTER_PORT),
	HESFuelCell(1, &FC_SLAVE_PORT)
};
AttopilotCurrentSensor motors[NUM_CURRENTSENSORS] = {
	AttopilotCurrentSensor(0,L_WHEEL_VPIN,L_WHEEL_APIN),
	AttopilotCurrentSensor(1,R_WHEEL_VPIN,R_WHEEL_APIN),
	AttopilotCurrentSensor(2,SUPERCAP_VPIN,SUPERCAP_APIN)
};
Speedometer speedo = Speedometer(1000);
MCP_CAN CANObj = MCP_CAN(CAN_CS_PIN);
// define globals
bool SD_avail;
char path[FILENAME_HEADER_LENGTH + 8 + 4 + 1]; // +8 for filename, +4 for '.txt', +1 for '\0'

// define tasks, types are: input, control, output
void ReadFuelCell(void *pvParameters);		// Input task:		Refreshes class variables for fuel cell Volts, Amps, Watts and Energy
void ReadMotorPower(void *pvParameters);	// Input task:		Refreshes class variables for motor Volts and Amps
void QueueOutputData(void *pvParameters);	// Control task:	Controls frequency to queue data from above tasks to output tasks
void LogSendData(void *pvParameters);		// Output task:		Data logged in SD card and sent through XBee. Logged and sent data should be consistent, hence they are grouped together
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
	debug(SERIAL_RX_BUFFER_SIZE);
	// create all files in a new directory
	SD_avail = initSD(path);
	// initialize speedometer
	attachInterrupt(digitalPinToInterrupt(SPEEDOMETER_INTERRUPT_PIN), storeWheelInterval_ISR, FALLING);
	
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
		, 2
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
		, 675  // Stack size
		, NULL
		, 1  // Priority
		, NULL);
	xTaskCreate(
		DisplayData
		, (const portCHAR *) "Display"
		, 350  // Stack size
		, NULL
		, 1  // Priority
		, NULL);
	xTaskCreate(
		SendCANFrame
		, (const portCHAR *) "Nothing"
		, 100  // Stack size
		, NULL
		, 0  // Priority
		, NULL);
	// Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.

}

void loop()
{
	// Empty. Things are done in Tasks.
}
