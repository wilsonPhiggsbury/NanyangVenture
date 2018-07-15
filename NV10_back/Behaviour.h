#pragma once
#define DEBUG 0

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#if DEBUG == 1
#define debug(...) Serial.println(__VA_ARGS__)
#define debug_(...) Serial.print(__VA_ARGS__)
#else
#define debug(...)
#define debug_(...)
#endif
#include <SPI.h>
#include <Wire.h>

#define NUM_MOTORS 3
#define NUM_FUELCELLS 2

// Declare data struct constants
#define FUELCELL_DATALEN 70 // (9+29+29) (timestamp + hydrocell1 + hydrocell2)
#define MOTOR_DATALEN 76 // (9 + 3*(5+5+5+7) + 1) (timestamp + motors*(volt + amp + ampPeak + Wh))
// define structs
typedef enum
{
	FuelCell,
	Motor
}DataSource;

typedef struct
{
	DataSource ID;
	char data[max(FUELCELL_DATALEN, MOTOR_DATALEN)];
}QueueItem;



// if any of below interval fufils:
//		(interval / QUEUE_DATA_INTERVAL) % 100 = 0 
// then it is valid. Otherwise execution may be delayed sometimes.

#define FUELCELL_LOGSEND_INTERVAL 1500 // must be higher than 1000ms, meaningless if send faster than fuel cell data coming in
#define MOTOR_LOGSEND_INTERVAL 300 // enables furious plotting on PC side, try not to overtax the XBee though
#define BACK_LCD_REFRESH_INTERVAL 10*MOTOR_LOGSEND_INTERVAL // can be slower, won't look it it often
#define HUD_REFRESH_INTERVAL 600 // snappy feedback on board!

// Declare task intervals
#define READ_FC_INTERVAL 750 // do not anyhow change, fuel cell data comes once per 1000ms, and faster read as sometimes need 2 reads to process
#define READ_MT_INTERVAL 300
#define QUEUE_DATA_INTERVAL 150 // keep small, fine tune in definitions below
#define LOGSEND_INTERVAL min(MOTOR_LOGSEND_INTERVAL, FUELCELL_LOGSEND_INTERVAL)
#define DISPLAY_INTERVAL QUEUE_DATA_INTERVAL