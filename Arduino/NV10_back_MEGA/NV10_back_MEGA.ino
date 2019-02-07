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
const char* fileNames[] = { "FC", "CS", "SM", "FCraw" };
const char* fileExt = ".txt";
enum FileNames{FC, CS, SM, FCraw};
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
// heartbeat address: 0x700 | device ID
/*
CANSerializer 
to intialize its own device ID
to issue & listen to heartbeat frames
to buffer receive window for messages that need to be wrapped in multiple packets
to expose interface for querying if a device on an ID is alive

11 bits standard id:
vvvv 			functionality & purpose (FCMain, FCBackup, CS, SM, ST...)
     v			request
	   vvvv		originating device
            vv	frame number (usually unused)
1110 0 00XX 00 - heartbeat
1101 0 0000 00 - CS stats
1100 0 0000 00 - FC Main
1011 0 0000 00 - FC Backup
1010 0 0000 00 - CS
1001 0 0000 00 - SM
0111 0 0000 00 - Status
0110 0 00XX 00 - command

GLOBAL
to define data point function IDs #define CAN_ID_FCMAIN B1101

*/


//DataPoint* dpReceiveList[] = { &dpStatus }; // make a collection of all DPs, scan them whenever we receive message
// define queues
QueueHandle_t queueForLog = xQueueCreate(1, sizeof(char)*110);
QueueHandle_t queueForSend = xQueueCreate(1, sizeof(char)*110);
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
void outputToSD(void *pvParameters);		// Output task:		Data logged in SD card.
void outputToSerial(void *pvParameters);		// Output task:		Data sent through XBee.
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
	xTaskCreate(
		logCurrentSensor
		, (const portCHAR *)"CS"
		, 200
		, NULL
		, 3
		, NULL);
	xTaskCreate(
		logFuelCell
		, (const portCHAR *)"FC"
		, 500
		, NULL
		, 2
		, NULL);
	xTaskCreate(
		blinkRGB
		, (const portCHAR *)"SIG"
		, 150 // -25
		, NULL
		, 2
		, &taskBlink);
	xTaskCreate(
		outputToSD
		, (const portCHAR *) "log"
		, 800  // Stack size
		, NULL
		, 2  // Priority
		, NULL);
	xTaskCreate(
		outputToSerial
		, (const portCHAR *) "send"
		, 200  // Stack size
		, NULL
		, 1  // Priority
		, NULL);
	xTaskCreate(
		ioForCAN
		, (const portCHAR *) "CAN la!" // where got cannot?
		, 1200  // Stack size
		, NULL
		, 1  // Priority
		, NULL);
	//xTaskCreate(taskTest, "test", 200, NULL, 2, NULL);
	//vTaskStartScheduler();

}
void taskTest(void*)
{
	char local[20];
	CANFrame f;
	while (1)
	{
		vTaskDelay(50);
	}
}
void loop()
{
	// Empty. Things are done in Tasks.
}
/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void logFuelCell(void *pvParameters)  // This is a Task.
{
	//for (int i = 0; i < 50; i += 1)
	//{
	//	Serial.print(i);
	//	Serial.print("ms->");
	//	Serial.print(pdMS_TO_TICKS(i));
	//	Serial.println("ticks");
	//}
	Serial3.begin(19200);
	Serial3.setTimeout(50);

	TickType_t prevTick = xTaskGetTickCount(); // only needed when vTaskDelayUntil is called instead of vTaskDelay
	CANFrame frame;
	char bufRaw_[110]; // not meant to be directly accessed in task loop below
	strcpy(bufRaw_, fileNames[FCraw]); // fill it in with raw file name.
	strcat(bufRaw_, "\t");
	char *const bufRaw = bufRaw_ + strlen(fileNames[FCraw]) + 1;
	char buf[50];
	TickType_t delay = pdMS_TO_TICKS(500);
	while (1)
	{
		uint8_t bytesRead = Serial3.readBytesUntil('\n', bufRaw, 100);
		bufRaw[bytesRead] = '\0'; // this slot would be '\r' from the Fuel Cell, replace it with null terminator instead
		if (bytesRead)xQueueSend(queueForLog, &bufRaw_, 100); // log this raw data
		//debug("Bytes read: "); debug(bytesRead);
		//debug("Buf raw prior log: "); debugPrint(bufRaw_, 100);
		dpFCMain.insertData(bufRaw);
		//debug_(dpFCMain.getWatts()); debug_(" ");
		//debug_(dpFCMain.getTemperature()); debug_(" ");
		//debug_(dpFCMain.getPressure()); debug_(" ");
		//debugPrint(dpFCMain.getStatus(), 3);
		bufRaw[0] = '\0'; // clear this buffer to avoid inserting same data twice later (e.g. when serial port has no new data)
		dpFCMain.packString(buf);
		//debug("Buf raw prior send: "); debugPrint(bufRaw_, 100); debug();
		//debug("Buf: "); debugPrint(buf, 50);
		if (bytesRead)
		{
			xQueueSend(queueForLog, &buf, 100); // log and send this refined data
			xQueueSend(queueForSend, &buf, 100);
		}

		dpFCMain.packCAN(&frame);
		//xQueueSend(queueForCAN, &fMain, 100);
		vTaskDelayUntil(&prevTick, delay); // more accurate compared to vTaskDelay, since the delay is shrunk according to execution time of this piece of code
	}
}
void logCurrentSensor(void* pvParameters)
{
	Adafruit_ADS1115 adc;
	adc.setGain(GAIN_SIXTEEN);
	adc.begin();

	CANFrame frame;
	char buf[50];

	TickType_t prevTick = xTaskGetTickCount(); // only needed when vTaskDelayUntil is called instead of vTaskDelay
	TickType_t delay = pdMS_TO_TICKS(100);
	uint8_t sendDelay = 0;
	while (1)
	{
		int volt = analogRead(CAP_OUT_VPIN);
		int amps = abs(adc.readADC_Differential_0_1());
		dpCS.insertData(volt, analogRead(CAP_IN_APIN), analogRead(CAP_OUT_APIN), amps);
		dpCSStats.insertData(volt, amps);
		//debug_("volt: "); debug(dpCS.getVolt());
		//debug_("capIn: "); debug(dpCS.getAmpCapIn());
		//debug_("capOut: "); debug(dpCS.getAmpCapOut());
		//debug_("capMotor: "); debug(dpCS.getAmpMotor());
		sendDelay += 4; // overflows in 256/4 = 64 ticks
		if (sendDelay == 0) // procs once per 64 ticks (6400ms)
		{
			dpCSStats.packCAN(&frame);
			//if(CAN_avail)xQueueSend(queueForCAN, &frame, 100);
		}
		if (sendDelay % 16 == 0) // procs once per 4 ticks (400ms)
		{
			dpCS.packCAN(&frame);
			dpCS.packString(buf);
			//if(CAN_avail)xQueueSend(queueForCAN, &frame, 100);
			xQueueSend(queueForLog, &buf, 100);
			xQueueSend(queueForSend, &buf, 100);
		}
		vTaskDelayUntil(&prevTick, delay);
	}
}
/// <summary>
/// Output task to log and send data
/// </summary>
/// <param name="pvParameters"></param>
void outputToSD(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	// lookup table for various file names.
	char buf[110];
	char path[12];
	TickType_t delay = pdMS_TO_TICKS(100); // delay 300 ms, shorter than reading/queueing tasks since this task has lower priority

	while (1)
	{
		BaseType_t success = xQueueReceive(queueForLog, &buf, 0);
		if (success == pdPASS)
		{
			if (SD_avail)
			{
				// -------------- Store into SD -------------
				// Set path char array to the document we want to save to, determined by a const array
				debug_("opening filename "); debug(buf);
				char* ptr = strtok(buf, "\t");
				strcpy(path, ptr);
				strcat(path, fileExt);
				// DO NOT SWITCH OUT THIS TASK IN THE MIDST OF WRITING A FILE ON SD CARD
				vTaskSuspendAll();
				File writtenFile = card.open(path, FILE_WRITE);
				ptr = strtok(NULL, "\0");
				writtenFile.println(ptr);
				writtenFile.close();
				xTaskResumeAll();
			}
		}
		vTaskDelay(delay);
	}
}
void outputToSerial(void* pvParameters)
{
	char buf[110];
	TickType_t delay = pdMS_TO_TICKS(100); // delay 300 ms, shorter than reading/queueing tasks since this task has lower priority
	while (1)
	{
		if (xQueueReceive(queueForSend, &buf, 0))
		{
			Serial.println(buf);
		}
		vTaskDelay(delay);
	}

}
void ioForCAN(void *pvParameters)
{
	CANFrame pendingSend, pendingReceive;
	while (1)
	{
		// dispatch frames
		if (xQueueReceive(queueForCAN, &pendingSend, 0))
		{
			serializer.sendCanFrame(&pendingSend);
		}
		// receive frames (poll-based, infrequent frames expected)
		if (serializer.receiveCanFrame(&pendingReceive))
		{
			for (int i = 0; i < 1; i++)
			{
				if (dpStatus.checkMatchCAN(&pendingReceive))
				{
					manageLights();

				}
			}
		}
		vTaskDelay(pdMS_TO_TICKS(30));
	}
}
void blinkRGB(void* pvParameters)
{
	bool lsigOn = false, rsigOn = false;
	bool sigOn = false;
	Adafruit_NeoPixel* lsig = NULL;
	Adafruit_NeoPixel* rsig = NULL;
	// initialize light strips
	lightstrip.begin();
	brakestrip.begin();
	lightstrip.setBrightness(50);
	brakestrip.setBrightness(200);
	writeRGB(lightstrip, 7, RUNNING_LIGHT_COLOR);
	writeRGB(brakestrip, 7, BRAKE_LIGHT_COLOR);

	lstrip.begin();
	rstrip.begin();
	lstrip.setBrightness(50);
	rstrip.setBrightness(50);
	writeRGB(lstrip, 7, NO_LIGHT);
	writeRGB(rstrip, 7, NO_LIGHT);
	while (1)
	{
		if (sigOn)
		{
			writeRGB(lstrip, 7, NO_LIGHT);
			writeRGB(lstrip, 7, NO_LIGHT);
			sigOn = false;
		}
		else
		{
			if (dpStatus.getHazard() == STATE_EN || dpStatus.getLsig() == STATE_EN)
			{
				writeRGB(lstrip, 7, SIGNAL_LIGHT_COLOR);
				sigOn = true;
			}
			if (dpStatus.getHazard() == STATE_EN || dpStatus.getRsig() == STATE_EN)
			{
				writeRGB(rstrip, 7, SIGNAL_LIGHT_COLOR);
				sigOn = true;
			}
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
void manageLights()
{
	static bool prevLsig = false, prevRsig = false, prevHazard = false;
	// toggle brake lights on-the-go once received CAN message
	if (dpStatus.getBrake() == STATE_EN)
	{
		writeRGB(brakestrip, 7, BRAKE_LIGHT_COLOR);
		writeRGB(lightstrip, 7, BRAKE_LIGHT_COLOR);
	}
	else
	{
		writeRGB(brakestrip, 7, NO_LIGHT);
		writeRGB(lightstrip, 7, RUNNING_LIGHT_COLOR);
	}

	bool thisLsig = dpStatus.getLsig() == STATE_EN;
	bool thisRsig = dpStatus.getRsig() == STATE_EN;
	bool thisHazard = dpStatus.getHazard() == STATE_EN;
	// compare prev and this state. if any of them changed, kick up blinking task
	if(thisLsig != prevLsig || thisRsig != prevRsig || thisHazard != prevHazard)
		xTaskAbortDelay(taskBlink); // state changed, kick up the task
	prevLsig = thisLsig;
	prevRsig = thisRsig;
	prevHazard = thisHazard;
}