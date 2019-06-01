/*
 Name:		NV10_back.ino
 Created:	6/19/2018 1:42:24 PM
 Author:	MX
*/
#define DEBUG 1
#if DEBUG
#define debug_(str) Serial.print(str)
#define debug(str)  Serial.println(str)
#else
#define debug_(...)
#define debug(...)
#endif

#include <MemoryFree.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <SdFat.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_ADS1015.h>
//#include <FreeRTOS_AVR.h>
#include <CANSerializer.h>
#include <NV10FuelCell.h>
#include <NV10CurrentSensor.h>
#include <NV10CurrentSensorStats.h>
#include <NV10AccesoriesStatus.h>
#include <NV11DataSpeedo.h>

//#include "JoulemeterDisplay.h"
#include "Speedometer.h"

#include "Pins_back.h"

NV10FuelCell dataFC = NV10FuelCell(0x13);
NV10CurrentSensor dataCS = NV10CurrentSensor(0x11);
NV10CurrentSensorStats dataCSStats = NV10CurrentSensorStats(0x12);
NV11DataSpeedo dataSpeedo = NV11DataSpeedo(0x0A);
NV10AccesoriesStatus dataAcc = NV10AccesoriesStatus(0x10);

// struct for log send exists to wrap strings properly with path names
typedef struct  {
	bool logThis;
	bool sendThis;
	char path[10];
	char data[100];
}StructForLogSend;
QueueHandle_t queueForLogSend = xQueueCreate(1, sizeof(StructForLogSend));
QueueHandle_t queueForCAN = xQueueCreate(1, sizeof(CANFrame));
//QueueHandle_t queueForDisplay = xQueueCreate(1, sizeof(Packet));
TaskHandle_t taskBlink;

// RGB light related variables
const uint8_t PIXELS = 7;
const uint32_t LIGHT_COLOR = Adafruit_NeoPixel::Color(255, 255, 255);
const uint32_t BRAKE_COLOR = Adafruit_NeoPixel::Color(255, 0, 0);
const uint32_t SIG_COLOR = Adafruit_NeoPixel::Color(255, 165, 0);
const uint32_t NO_COLOR = Adafruit_NeoPixel::Color(0, 0, 0);
Adafruit_NeoPixel lstrip = Adafruit_NeoPixel(7, LSIG_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rstrip = Adafruit_NeoPixel(7, RSIG_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lightstrip = Adafruit_NeoPixel(7, RUNNINGLIGHT_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel brakestrip = Adafruit_NeoPixel(7, BRAKE_PIN, NEO_GRB + NEO_KHZ800);
bool sigOn = false;

// define globals
bool SD_avail = false, CAN_avail = false;
SdFat card;

// wheel diameter is 545 mm, feed into speedo
Speedometer speedo = Speedometer(SPEEDOMETER_INTERRUPT_PIN, 545, 2);

// define tasks, types are: input, control, output
void TaskLogFuelCell(void *pvParameters);		// Input task:		Refreshes class variables for fuel cell Volts, Amps, Watts and Energy
void TaskLogCurrentSensor(void *pvParameters);	// Input task:		Refreshes class variables for motor Volts and Amps
void TaskSpeedo(void *pvParameters);	// Control task:	Controls frequency to queue payload from above tasks to output tasks
void TaskLogSendData(void *pvParameters);		// Output task:		Data logged in SD card and sent through XBee. Logged and sent payload should be consistent, hence they are grouped together
void TaskCAN(void* pvParameters);			// In/Out task:		Manages 2-way CAN bus comms
void TaskBlink(void *pvParameters);			// 

//// _______________OPTIONAL_____________
//void TaskReceiveCommands(void *pvParameters);	// Input task:		Enable real-time control of Arduino (if any)
//void TaskRefreshPeripherals(void *pvParameters);// Control task:	Updates all CAN-BUS peripherals

/// <summary>
/// Collects data from 3 different sources: Fuel Cell, Current Sensor, SpeedoMeter.
/// Outputs data to 2 different places: SD card & XBee Serial
/// Manages running lights, brake lights, signal lights.
/// </summary>
void setup() {
	Serial.begin(9600);
	while (!Serial);

	// create all files in a new directory
	SD_avail = initSD(card);
	Serial.print("SD avail: ");
	Serial.println(SD_avail);

	// Now set up all Tasks to run independently. Task functions are found in Tasks.ino
	xTaskCreate(
		TaskLogFuelCell
		, (const portCHAR *)"Fuel"
		, 450
		, NULL
		, 2
		, NULL);
	xTaskCreate(
		TaskLogCurrentSensor
		, (const portCHAR *)"CSensor"
		, 550
		, NULL
		, 2
		, NULL);
	xTaskCreate(
		TaskSpeedo
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 300  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL // Any pointer to pass in
		, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);
	xTaskCreate(
		TaskLogSendData
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
		, 300  // Stack size
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
void TaskLogFuelCell(void *pvParameters)
{
	CANFrame f;
	StructForLogSend s; // this is only for logging FC raw data
	s.logThis = true;
	s.sendThis = false;
	strcpy(s.path, "FCraw.txt");

	HardwareSerial& fcPort = Serial3;
	fcPort.begin(19200);
	fcPort.setTimeout(50);
	while (1)
	{
		if (fcPort.available())
		{
			uint8_t bytesRead = fcPort.readBytesUntil('\n', s.data, 100);
			// log the raw data
			xQueueSend(queueForLogSend, &s, 100);
			// CAN the processed fc data
			dataFC.insertData(s.data); // THIS will destroy s.data! Don't send s anymore after this line
			dataFC.packCAN(&f);
			xQueueSend(queueForCAN, &f, 100);
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
void TaskLogCurrentSensor(void *pvParameters)
{
	CANFrame f;
	StructForLogSend s;
	s.logThis = true;
	s.sendThis = true;
	strcpy(s.path, "CS.txt");
	/* 
	ADS1115 full scale range (FSR) table
	GAIN	FSR(mV)	LSB size(microV)
	0.5		6144	187.5
	1		4096	125
	2		2048	62.5
	4		1024	31.25  <-- we are using setGain(GAIN_FOUR) when measuring voltage, 
	8		512		15.625
	16		256		7.8125 <-- we are using setGain(GAIN_SIXTEEN) when measuring current, for highest resolution. 
	Keeping FSR small increases resolution, but it must be larger than shunt mV or the ADC may be damaged.
	75mV 200A shunt: 200A maps to - 75mV / 0.0078125mV per raw unit = 9600 raw unit
	75mV 200A shunt: 1A maps to - 9600 raw unit / 200 amp = 48 raw units per amp
	Hence, amp = raw * ADC scale
	where ADC scale = 0.0078125 * shuntA / shuntmV 
	*/
	const float SHUNT_100A75mV_SCALE = 0.0078125 * (100.0 / 75);
	const float SHUNT_200A75mV_SCALE = 0.0078125 * (200.0 / 75);
	const float VOLTAGEDIVIDER_SCALE = 1.024 / 60; // scale 60V to 1.024V maybe?
	/*
	ADS1115 i2c address
	Addr -> GND = 72 (default if not connected)
	Addr -> VDD = 73
	Addr -> SDA = 74 (may be problematic)
	Addr -> SCL = 75
	*/
	Adafruit_ADS1115 capCurrentADC = Adafruit_ADS1115(72);
	Adafruit_ADS1115 motorCurrentADC = Adafruit_ADS1115(73);
	Adafruit_ADS1115 capVoltageADC = Adafruit_ADS1115(75);
	capCurrentADC.setGain(GAIN_SIXTEEN);
	motorCurrentADC.setGain(GAIN_SIXTEEN);
	capVoltageADC.setGain(GAIN_FOUR); // scale 60V to 1.024V maybe?

	dataCSStats.syncTime(); // reset the internal millis count for better 1st reading
	uint8_t syncCounter = 0; // incr on every loop, for less frequent logsend messages
	while (1)
	{
		float capInCurrent = capCurrentADC.readADC_Differential_0_1() * SHUNT_100A75mV_SCALE;
		float capOutCurrent = capCurrentADC.readADC_Differential_2_3() * SHUNT_100A75mV_SCALE;
		float motorCurrent = motorCurrentADC.readADC_Differential_0_1() * SHUNT_100A75mV_SCALE;
		float capVoltage = capVoltageADC.readADC_Differential_0_1() * VOLTAGEDIVIDER_SCALE;
		dataCS.insertData(capVoltage, capInCurrent, capOutCurrent, motorCurrent);
		dataCSStats.insertData(capVoltage, motorCurrent);

		s.logThis = true;
		s.sendThis = true;
		strcpy(s.path, "CS.txt");
		dataCS.packString(s.data);
		xQueueSend(queueForLogSend, &s, 100);

		dataCS.packCAN(&f);
		xQueueSend(queueForCAN, &f, 100);

		if (syncCounter % 16 == 0)
		{
			s.logThis = true;
			s.sendThis = true;
			strcpy(s.path, "CSstats.txt");
			dataCSStats.packString(s.data);
			xQueueSend(queueForLogSend, &s, 100);
		}

		syncCounter++;
		vTaskDelay(pdMS_TO_TICKS(300));
	}
}
void TaskSpeedo(void *pvParameters)
{
	CANFrame f;
	StructForLogSend s;

	pinMode(SPEEDOMETER_INTERRUPT_PIN, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(SPEEDOMETER_INTERRUPT_PIN), [] {
		speedo.trip();
	}, FALLING);
	while (1)
	{
		float speedKmh = speedo.getSpeedKmh();
		dataSpeedo.insertData(speedKmh);

		s.logThis = true;
		s.sendThis = true;
		strcpy(s.path, "SM.txt");
		dataSpeedo.packString(s.data);
		xQueueSend(queueForLogSend, &s, 100);

		dataSpeedo.packCAN(&f);
		xQueueSend(queueForCAN, &f, 100);

		vTaskDelay(pdMS_TO_TICKS(500));
	}

}
void TaskBlink(void* pvParameters)
{
	debug(F("Blink task started."));
	// initialize light strips
	lightstrip.begin();
	brakestrip.begin();
	lightstrip.setBrightness(50);
	brakestrip.setBrightness(200);
	setRGB(lightstrip, PIXELS, LIGHT_COLOR);
	setRGB(brakestrip, PIXELS, BRAKE_COLOR);

	lstrip.begin();
	rstrip.begin();
	lstrip.setBrightness(50);
	rstrip.setBrightness(50);
	setRGB(lstrip, PIXELS, NO_COLOR);
	setRGB(rstrip, PIXELS, NO_COLOR);
	while (1)
	{
		if (sigOn)
		{
			sigOn = false;
			setRGB(lstrip, PIXELS, NO_COLOR);
			setRGB(rstrip, PIXELS, NO_COLOR);
		}
		else
		{
			if (dataAcc.getHazard() == STATE_EN || dataAcc.getLsig() == STATE_EN)
			{
				sigOn = true;
				setRGB(lstrip, PIXELS, SIG_COLOR);
			}
			if (dataAcc.getHazard() == STATE_EN || dataAcc.getRsig() == STATE_EN)
			{
				sigOn = true;
				setRGB(rstrip, PIXELS, SIG_COLOR);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
void TaskLogSendData(void *pvParameters)
{
	StructForLogSend s;

	BaseType_t stringToLog;
	while (1)
	{
		stringToLog = xQueueReceive(queueForLogSend, &s, 0);
		if (stringToLog == pdTRUE)
		{
			if (s.logThis && SD_avail)
			{
				// DO NOT SWITCH OUT THIS TASK IN THE MIDST OF WRITING A FILE ON SD CARD
				vTaskSuspendAll();
				File writtenFile = card.open(s.path, FILE_WRITE);
				writtenFile.println(s.data);
				writtenFile.close();
				xTaskResumeAll();
			}
			if (s.sendThis)
			{
				Serial.println(s.data);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
void TaskCAN(void *pvParameters)
{
	CANFrame f;

	CANSerializer serializer;
	CAN_avail = serializer.init(CAN_CS_PIN);
	debug_("CAN avail: ");
	debug(CAN_avail);

	BaseType_t canMsgToSend;
	while (1)
	{
		canMsgToSend = xQueueReceive(queueForCAN, &f, 0);
		if (canMsgToSend == pdTRUE)
		{
			serializer.sendCanFrame(&f);
		}

		if (serializer.receiveCanFrame(&f))
		{
			if (dataAcc.checkMatchCAN(&f))
			{
				dataAcc.unpackCAN(&f);
				// react to brake command
				if (dataAcc.getBrake() == STATE_EN)
				{
					setRGB(brakestrip, PIXELS, BRAKE_COLOR);
					setRGB(lightstrip, PIXELS, BRAKE_COLOR);
				}
				else
				{
					setRGB(brakestrip, PIXELS, NO_COLOR);
					setRGB(lightstrip, PIXELS, LIGHT_COLOR);
				}
				// react to signal lights command only when sigOn state does not follow dataAcc
				if (sigOn != dataAcc.getLsig() || dataAcc.getRsig() || dataAcc.getHazard())
					xTaskAbortDelay(taskBlink);
			}
			else
			{
				debug_(F("Unrecognized CAN id: "));
				debug(f.id);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}