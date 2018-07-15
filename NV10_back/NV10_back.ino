/*
 Name:		RTOS_test.ino
 Created:	6/19/2018 1:42:24 PM
 Author:	MX
*/
/*IMPORTANT:
	Please change SERIAL_RX_BUFFER_SIZE inside of HardwareSerial.h to 128, or fuel cell data will be incomplete.
	Attempting to parse an incomplete data will result in rubbish.
*/

#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
#include <queue.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#include "JoulemeterDisplay.h"
#include "CurrentSensorLogger.h"
#include "FuelCellLogger.h"



// file names
const char FUELCELL_FILENAME[] = "FC_neat.txt";
const char FUELCELL_RAW_FILENAME[] = "FC_raw.txt";
const char MOTOR_FILENAME[] = "MT_CAP.txt";
const uint8_t FILENAME_HEADER_LENGTH = 1+3+1+4+1; /* 
												  /LOG_0002/ -> 1+3+1+4+1
												  */

// define queues
QueueHandle_t queueForLogSend = xQueueCreate(1, sizeof(QueueItem));
QueueHandle_t queueForDisplay = xQueueCreate(1, sizeof(QueueItem));
// declare globals (please keep to minimum)
bool SD_avail;
char path[FILENAME_HEADER_LENGTH+ 8 + 4 + 1] = ""; // +8 for filename, +4 for '.txt', +1 for '\0'
// sample filename: /LOG_0002/12345678.txt   1+8+1+8+4+1
HESFuelCell hydroCells[NUM_FUELCELLS] = { HESFuelCell(&Serial3),HESFuelCell(&Serial2) };
AttopilotCurrentSensor motors[NUM_MOTORS] = { AttopilotCurrentSensor(0,A0,A1),AttopilotCurrentSensor(1,A2,A3),AttopilotCurrentSensor(2,A4,A5) };

// define tasks, types are: input, control, output
void TaskReadFuelCell(void *pvParameters);		// Input task:		Refreshes class variables for fuel cell Volts, Amps, Watts and Energy
void TaskReadMotorPower(void *pvParameters);	// Input task:		Refreshes class variables for motor Volts and Amps
void TaskQueueOutputData(void *pvParameters);	// Control task:	Controls frequency to queue data from above tasks to output tasks
void TaskLogSendData(void *pvParameters);		// Output task:		Data logged in SD card and sent through XBee. Logged and sent data should be consistent, hence they are grouped together
void TaskDisplayData(void *pvParameters);		// Output task:		Display on LCD screen

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
	// define objects for more complicated procedures
	
	// Now set up all Tasks to run independently.
	xTaskCreate(
		TaskReadFuelCell
		, (const portCHAR *)"Fuel"
		, 150
		, hydroCells
		, 2
		, NULL);
	xTaskCreate(
		TaskReadMotorPower
		, (const portCHAR *)"Motor"
		, 150
		, motors
		, 3
		, NULL);
	xTaskCreate(
		TaskQueueOutputData
		, (const portCHAR *)"Enqueue"  // A name just for humans
		, 250  // This stack size can be checked & adjusted by reading the Stack Highwater
		, NULL // Any pointer to pass in
		, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
		, NULL);
	xTaskCreate(
		TaskLogSendData
		, (const portCHAR *) "LogSend"
		, 650  // Stack size
		, NULL
		, 1  // Priority
		, NULL);
	xTaskCreate(
		TaskDisplayData
		, (const portCHAR *) "Display"
		, 400  // Stack size
		, NULL
		, 1  // Priority
		, NULL);Serial.print("Buffer size: ");Serial.println(SERIAL_RX_BUFFER_SIZE);
	// Now the Task scheduler, which takes over control of scheduling individual Tasks, is automatically started.
}

void loop()
{
	// Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void TaskReadFuelCell(void *pvParameters)  // This is a Task.
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
void TaskReadMotorPower(void* pvParameters)
{
	// Obtain current sensor object references from parameter passed in
	AttopilotCurrentSensor* sensor = (AttopilotCurrentSensor*)pvParameters;

	TickType_t prevTick = xTaskGetTickCount(); // only needed when vTaskDelayUntil is called instead of vTaskDelay
	TickType_t delay = pdMS_TO_TICKS(READ_MT_INTERVAL);
	while (1)
	{
		for (int i = 0; i < NUM_MOTORS; i++)
		{
			(sensor + i)->logData();
		}
		vTaskDelayUntil(&prevTick, delay);
	}
	
}
void TaskQueueOutputData(void *pvParameters)
{
	const uint16_t fuelcell_logsend = FUELCELL_LOGSEND_INTERVAL / QUEUE_DATA_INTERVAL;
	const uint16_t motor_logsend = MOTOR_LOGSEND_INTERVAL / QUEUE_DATA_INTERVAL;
	const uint16_t hud_refresh = HUD_REFRESH_INTERVAL / QUEUE_DATA_INTERVAL;
	const uint16_t back_lcd_refresh = BACK_LCD_REFRESH_INTERVAL / QUEUE_DATA_INTERVAL;

	QueueItem outgoing;
	uint8_t syncCounter = 1;
	//HESFuelCell* masterCell = (HESFuelCell*)pvParameters;
	//HESFuelCell* slaveCell = ((HESFuelCell*)pvParameters) + 1;
	//AttopilotCurrentSensor* motor1 = ((Loggers*)pvParameters)->motors;
	//AttopilotCurrentSensor* motor2 = ((Loggers*)pvParameters)->motors + 1;
	//AttopilotCurrentSensor* motor3 = ((Loggers*)pvParameters)->motors + 2;
	TickType_t delay = pdMS_TO_TICKS(QUEUE_DATA_INTERVAL);
	BaseType_t success;

	while(1) // A Task shall never return or exit.
	{
		success = pdPASS;
		syncCounter++;
		if (syncCounter > 100)
			syncCounter = 1;
		// Arrange for outgoing fuel cell data
		outgoing.ID = FuelCell;
		outgoing.data[0] = '\0';
		/* ------------------DATA FORMAT------------------
						FM						FS
			millis		V	A	W	Wh	V_c	St	V	A	W	Wh	V_c	St
		--------------------------------------------------*/

		if (syncCounter % fuelcell_logsend == 0)
		{
			HESFuelCell::dumpTimestampInto(outgoing.data);
			for (int i = 0; i < NUM_FUELCELLS; i++)
			{
				strcat(outgoing.data, "\t");
				if (hydroCells[i].hasUpdated())
				{
					hydroCells[i].dumpDataInto(outgoing.data);
				}
				else
				{
					strcat(outgoing.data, "-");
				}
			}
			xQueueSend(queueForLogSend, &outgoing, 100);
			xQueueSend(queueForDisplay, &outgoing, 100);
		}
		

		// Arrange for outgoing motor data
		outgoing.ID = Motor;
		outgoing.data[0] = '\0';
		/* ------------------DATA FORMAT------------------
			Lwheel(L) -> Rwheel(R) -> Capacitor(c)

			millis		V_L		V_R		V_c		A_L		A_R		A_c		Ap_L*	Ap_R*	Ap_c*	Wh_L*	Wh_R*	Wh_c*
			*: only for display, not for logsend
		--------------------------------------------------*/
		if (syncCounter % motor_logsend == 0)
		{
			motors[0].dumpTimestampInto(outgoing.data);
			for (int i = 0; i < NUM_MOTORS; i++)
			{
				strcat(outgoing.data, "\t");
				motors[i].dumpVoltReadingInto(outgoing.data);//len 5
			}
			for (int i = 0; i < NUM_MOTORS; i++)
			{
				strcat(outgoing.data, "\t");
				motors[i].dumpAmpReadingInto(outgoing.data);//len 5
			}
			xQueueSend(queueForLogSend, &outgoing, 100);
			if (syncCounter % (back_lcd_refresh) == 0)
			{
				for (int i = 0; i < NUM_MOTORS; i++)
				{
					strcat(outgoing.data, "\t");
					motors[i].dumpAmpPeakInto(outgoing.data);//len 5
				}
				for (int i = 0; i < NUM_MOTORS; i++)
				{
					strcat(outgoing.data, "\t");
					motors[i].dumpTotalEnergyInto(outgoing.data);//len 7
				}
				xQueueSend(queueForDisplay, &outgoing, 100);
			}
		}
		vTaskDelay(delay);
	}
}

void TaskLogSendData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	QueueItem received;
	TickType_t delay = pdMS_TO_TICKS(LOGSEND_INTERVAL); // delay 300 ms, shorter than reading/queueing tasks since this task has lower priority
	
	while(1)
	{
		BaseType_t success = xQueueReceive(queueForLogSend, &received, 0);
		char shortFileName[3] = "";
		if (success == pdPASS)
		{
			switch (received.ID)
			{
			case FuelCell:
				strncpy(shortFileName, FUELCELL_FILENAME, 2);
				strcpy(path + FILENAME_HEADER_LENGTH, FUELCELL_FILENAME);
				break;
			case Motor:
				strncpy(shortFileName, MOTOR_FILENAME, 2);
				strcpy(path + FILENAME_HEADER_LENGTH, MOTOR_FILENAME);
				break;
			}
			Serial.print(shortFileName);
			Serial.print('\t');
			Serial.println(received.data);
			// -------------- Store into SD -------------
			if (SD_avail)
			{
				taskENTER_CRITICAL();
				File writtenFile = SD.open(path, FILE_WRITE);
				writtenFile.println(received.data);
				writtenFile.close();
				taskEXIT_CRITICAL();
			}
			// *path should only remain as /LOG_****/. Clean up after use
			strcpy(path + FILENAME_HEADER_LENGTH, "");
		}

		vTaskDelay(delay);  // poll more frequently since more data comes in at a time
	}
}
void TaskDisplayData(void *pvParameters)
{
	QueueItem received;
	LiquidCrystal_I2C lcdScreen = LiquidCrystal_I2C(LCD1_I2C_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
	DisplayLCD lcdManager = DisplayLCD(lcdScreen);

	TickType_t delay = pdMS_TO_TICKS(DISPLAY_INTERVAL);

	while (1)
	{
		BaseType_t success;
		while (xQueueReceive(queueForDisplay, &received, 0) == pdPASS)
		{
			lcdManager.printData(received);
		}		vTaskDelay(delay);
	}
}