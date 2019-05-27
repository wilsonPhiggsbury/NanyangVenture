/*
 Name:		NV10_back.ino
 Created:	6/19/2018 1:42:24 PM
 Author:	MX
*/

#include <MemoryFree.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <SdFat.h>
#include <SPI.h>
//#include <FreeRTOS_AVR.h>
#include <Adafruit_NeoPixel.h>
#include <CAN_Serializer.h>
#include "Behaviour.h"

//#include "JoulemeterDisplay.h"	
#include "CurrentSensorLogger.h"
#include "FuelCellLogger.h"
#include "Speedometer.h"

#include "Pins_back.h"

// file names
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
	HESFuelCell(0, &FCLEFT_SERIAL)
};
AttopilotCurrentSensor motors[NUM_CURRENTSENSORS] = {
	AttopilotCurrentSensor(0,CAPIN_INPUT_VOLT,CAPIN_INPUT_AMP),
	AttopilotCurrentSensor(1,CAPOUT_INPUT_VOLT,CAPOUT_INPUT_AMP),
	AttopilotCurrentSensor(2,MOTOR_INPUT_VOLT,MOTOR_INPUT_AMP)
};
// wheel diameter is 545 mm, feed into speedo
Speedometer speedo = Speedometer(0, 545/2);
Adafruit_NeoPixel lstrip = Adafruit_NeoPixel(7, LSIG_OUTPUT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rstrip = Adafruit_NeoPixel(7, RSIG_OUTPUT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lightstrip = Adafruit_NeoPixel(7, RUNNINGLIGHT_OUTPUT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel brakestrip = Adafruit_NeoPixel(7, BRAKELIGHT_OUTPUT, NEO_GRB + NEO_KHZ800);
const uint8_t PIXELS = 7;
const uint32_t LIGHT_COLOR = Adafruit_NeoPixel::Color(32, 0, 0);
const uint32_t BRAKE_COLOR = Adafruit_NeoPixel::Color(255, 0, 0);
const uint32_t SIG_COLOR = Adafruit_NeoPixel::Color(255, 165, 0);
const uint32_t NO_COLOR = Adafruit_NeoPixel::Color(0, 0, 0);
CAN_Serializer serializer;
// define globals
bool SD_avail = false, CAN_avail = false;
char path[FILENAME_HEADER_LENGTH + 8 + 4 + 1]; // +8 for filename, +4 for '.txt', +1 for '\0'
SdFat card;
// define tasks, types are: input, control, output
void TaskLogFuelCell(void *pvParameters);		// Input task:		Refreshes class variables for fuel cell Volts, Amps, Watts and Energy
void TaskLogCurrentSensor(void *pvParameters);	// Input task:		Refreshes class variables for motor Volts and Amps
void QueueOutputData(void *pvParameters);	// Control task:	Controls frequency to queue payload from above tasks to output tasks
void LogSendData(void *pvParameters);		// Output task:		Data logged in SD card and sent through XBee. Logged and sent payload should be consistent, hence they are grouped together
void TaskCAN(void* pvParameters);			// In/Out task:		Manages 2-way CAN bus comms
void TaskBlink(void *pvParameters);			// 

volatile bool brakeOn = false;
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

	CAN_avail = serializer.init(CAN_SPI_CS);
	// create all files in a new directory
	SD_avail = initSD(card);
	Serial.print("SD avail: ");
	Serial.println(SD_avail);
	Serial.print("CAN avail: ");
	Serial.println(CAN_avail);

	// initialize speedometer
	pinMode(SPEEDOMETER_INTERRUPT, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(SPEEDOMETER_INTERRUPT), storeWheelInterval_ISR, FALLING);

	// Now set up all Tasks to run independently. Task functions are found in Tasks.ino
	xTaskCreate(
		TaskLogFuelCell
		, (const portCHAR *)"Fuel"
		, 750
		, hydroCells
		, 3
		, NULL);
	//xTaskCreate(
	//	TaskLogCurrentSensor
	//	, (const portCHAR *)"CSensor"
	//	, 200
	//	, motors
	//	, 3
	//	, NULL);
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
/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void TaskLogFuelCell(void *pvParameters)  // This is a Task.
{
	// Obtain fuel cell object references from parameter passed in
	HESFuelCell* fuelCell = (HESFuelCell*)pvParameters;

	TickType_t prevTick = xTaskGetTickCount(); // only needed when vTaskDelayUntil is called instead of vTaskDelay
	TickType_t delay = pdMS_TO_TICKS(READ_FC_INTERVAL);
	while (1)
	{
		for (int i = 0; i < NUM_FUELCELLS; i++)
		{
			(fuelCell + i)->logData();
		}
		vTaskDelayUntil(&prevTick, delay); // more accurate compared to vTaskDelay, since the delay is shrunk according to execution time of this piece of code
	}
}
void TaskLogCurrentSensor(void* pvParameters)
{
	// Obtain current sensor object references from parameter passed in
	AttopilotCurrentSensor* sensor = (AttopilotCurrentSensor*)pvParameters;

	TickType_t prevTick = xTaskGetTickCount(); // only needed when vTaskDelayUntil is called instead of vTaskDelay
	TickType_t delay = pdMS_TO_TICKS(READ_MT_INTERVAL);
	while (1)
	{
		for (int i = 0; i < NUM_CURRENTSENSORS; i++)
		{
			(sensor + i)->logData();
		}
		vTaskDelayUntil(&prevTick, delay);
	}
}
void QueueOutputData(void *pvParameters)
{
	const uint16_t fuelcell_logsend = FUELCELL_LOGSEND_INTERVAL / QUEUE_DATA_INTERVAL;
	//const uint16_t motor_logsend = MOTOR_LOGSEND_INTERVAL / QUEUE_DATA_INTERVAL;
	const uint16_t speedo_refresh_interval = SPEEDOMETER_REFRESH_INTERVAL / QUEUE_DATA_INTERVAL;

	Packet outgoing;
	uint8_t syncCounter = 0;
	auto delay = pdMS_TO_TICKS(QUEUE_DATA_INTERVAL);

	while (1) // A Task shall never return or exit.
	{
		if (++syncCounter >= 128)
			syncCounter = 0;
		// Send one outgoing fuel cell (FC) frame every x ms
		if (syncCounter % fuelcell_logsend == 0)
		{
			// Arrange for outgoing fuel cell payload
			outgoing.ID = FC;
			// todo: move dumptimestamp func into packet class
			HESFuelCell::dumpTimestampInto(&outgoing.timeStamp);
			for (auto i = 0; i < NUM_FUELCELLS; i++)
			{
				if (hydroCells[i].hasUpdated())
				{
					hydroCells[i].dumpDataInto(outgoing.data);
				}
				else
				{
					for (int j = 0; j < NUM_DATASETS; j++)
						for (int k = 0; k < NUM_DATASUBSETS; k++)
							outgoing.data[j][k] = 0;
				}
			}
			/* ------------------DATA FORMAT------------------
			FM								FS
			millis		V	A	W	Wh	T	P	V_c	St	V	A	W	Wh	T	P	V_c	St
			--------------------------------------------------*/
			xQueueSend(queueForLogSend, &outgoing, 100);
			if (CAN_avail)
				xQueueSend(queueForCAN, &outgoing, 100);
		}


		/* ------------------DATA FORMAT------------------
		Lwheel(L) -> Rwheel(R) -> Capacitor(c)

		millis		V_L		V_R		V_c		A_L		A_R		A_c		Ap_L*	Ap_R*	Ap_c*	Wh_L*	Wh_R*	Wh_c*
		*: only for display, not for logsend
		--------------------------------------------------*/
		// send one outgoing current sensor (CS) every x ms
		//if (syncCounter % motor_logsend == 0)
		//{
		//	// Arrange for outgoing current sensor payload
		//	outgoing.ID = CS;
		//	AttopilotCurrentSensor::dumpTimestampInto(&outgoing.timeStamp);
		//	for (int i = 0; i < NUM_CURRENTSENSORS; i++)
		//	{
		//		motors[i].dumpDataInto(outgoing.data);//len 5
		//	}
		//	xQueueSend(queueForLogSend, &outgoing, 100);
		//	if (CAN_avail)
		//		xQueueSend(queueForCAN, &outgoing, 100);
		//	//if (syncCounter % (back_lcd_refresh) == 0)
		//	//{
		//	//	for (int i = 0; i < NUM_CURRENTSENSORS; i++)
		//	//	{
		//	//		strcat(outgoing.payload, "\t");
		//	//		motors[i].dumpAmpPeakInto(outgoing.payload);//len 5
		//	//	}
		//	//	for (int i = 0; i < NUM_CURRENTSENSORS; i++)
		//	//	{
		//	//		strcat(outgoing.payload, "\t");
		//	//		motors[i].dumpTotalEnergyInto(outgoing.payload);//len 7
		//	//	}
		//	//}
		//}
		// send one outgoing speedometer (SM) every x ms
		if (syncCounter % speedo_refresh_interval == 0)
		{
			outgoing.ID = SM;
			Speedometer::dumpTimestampInto(&outgoing.timeStamp);
			speedo.dumpDataInto(outgoing.data);
			xQueueSend(queueForLogSend, &outgoing, 100);
			if (CAN_avail)
				xQueueSend(queueForCAN, &outgoing, 100);
		}
		vTaskDelay(delay);
	}
}

/// <summary>
/// Output task to log and send data
/// </summary>
/// <param name="pvParameters"></param>
void LogSendData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	Packet received;
	char data[MAX_STRING_LEN];
	TickType_t delay = pdMS_TO_TICKS(LOGSEND_INTERVAL); // delay 300 ms, shorter than reading/queueing tasks since this task has lower priority

	while (1)
	{
		BaseType_t success = xQueueReceive(queueForLogSend, &received, 100);
		if (success == pdPASS)
		{
			received.toString(data);

			// -------------- Store into SD -------------
			// log
			if (SD_avail)
			{
				// Set path char array to the document we want to save to, determined by a const array
				strcpy(path, frameType_shortNames[received.ID]);
				strcat(path, ".txt");
				// DO NOT SWITCH OUT THIS TASK IN THE MIDST OF WRITING A FILE ON SD CARD
				vTaskSuspendAll();
				File writtenFile = card.open(path, FILE_WRITE);
				writtenFile.println(data);
				writtenFile.close();
				xTaskResumeAll();
				// *path should only remain as /LOG_****/, clean up after use
				strcpy(path, "");
			}
			// finally print out the payload to be transmitted by XBee
			Serial.println(data);
		}

		vTaskDelay(delay);
	}
}

void TaskBlink(void* pvParameters)
{
	bool sigOn = false;
	// initialize light strips
	lightstrip.begin();
	brakestrip.begin();
	lightstrip.setBrightness(200);
	brakestrip.setBrightness(200);
	setRGB(lightstrip, PIXELS, LIGHT_COLOR);
	setRGB(brakestrip, PIXELS, BRAKE_COLOR);

	lstrip.begin();
	rstrip.begin();
	lstrip.setBrightness(50);
	rstrip.setBrightness(50);
	while (1)
	{
		//if (peripheralStates[Headlights] == STATE_EN)debug(F("Headlights ON"));
		//else debug(F("Headlights OFF"));
		//if (peripheralStates[Rsig] == STATE_EN)debug(F("Rsig ON"));
		//else debug(F("Rsig OFF"));
		//debug(F("--------"));
		if (sigOn)
		{
			sigOn = false;
			setRGB(lstrip, PIXELS, NO_COLOR);
			setRGB(rstrip, PIXELS, NO_COLOR);
		}
		else
		{
			if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Lsig] == STATE_EN)
			{
				sigOn = true;
				setRGB(lstrip, PIXELS, SIG_COLOR);
			}
			if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Rsig] == STATE_EN)
			{
				sigOn = true;
				setRGB(rstrip, PIXELS, SIG_COLOR);
			}
		}
		if (brakeOn == STATE_EN)
		{
			setRGB(brakestrip, PIXELS, BRAKE_COLOR);
			setRGB(lightstrip, PIXELS, BRAKE_COLOR);
		}
		else if (brakeOn == STATE_DS)
		{
			setRGB(brakestrip, PIXELS, NO_COLOR);
			setRGB(lightstrip, PIXELS, LIGHT_COLOR);
		}

		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
void doReceiveAction(Packet* q)
{
	// ************************* Code for signal lights ****************************
	// kick up the blinking task and tell it to start or stop rightaway
	if (q->ID == BT)
	{
		for (int i = 0; i < NUMSTATES; i++)
			peripheralStates[i] = q->data[0][i];
		xTaskAbortDelay(taskBlink);
	}
	else if (q->ID == BK)
	{
		brakeOn = q->data[0][0] == STATE_EN;
		if (brakeOn == STATE_EN)
		{
			debug("Brake ON");
			setRGB(brakestrip, PIXELS, BRAKE_COLOR);
			setRGB(lightstrip, PIXELS, BRAKE_COLOR);
		}
		else if (brakeOn == STATE_DS)
		{
			debug("Brake OFF");
			setRGB(brakestrip, PIXELS, NO_COLOR);
			setRGB(lightstrip, PIXELS, LIGHT_COLOR);
		}
	}
}