#pragma once
#include <SPI.h>
// Declare data struct constants
#define FUELCELL_DATALEN 88 // (9+39+39+1) (timestamp + hydrocell1 + hydrocell2)
#define MOTOR_DATALEN 76 // (9 + 3*(5+5+5+7) + 1) (timestamp + motors*(volt + amp + ampPeak + Wh))
// define structs
typedef enum
{
	FC,
	CS,
	SM
}DataSource;

typedef struct
{
	DataSource ID;
	char data[max(FUELCELL_DATALEN, MOTOR_DATALEN)] = "";
}QueueItem;


#define FUELCELL_LOGSEND_INTERVAL 1000 // must be higher than 1000ms, meaningless if send faster than fuel cell data coming in
#define MOTOR_LOGSEND_INTERVAL 300 // enables furious plotting on PC side, try not to overtax the XBee though
#define BACK_LCD_REFRESH_INTERVAL 2*MOTOR_LOGSEND_INTERVAL // can be slower, won't look at it often
#define SPEEDOMETER_REFRESH_INTERVAL 500
#define CAN_FRAME_INTERVAL 500

// Declare task intervals
#define READ_FC_INTERVAL 345 // do not anyhow change, fuel cell data comes once per 1000ms, and faster read as sometimes need 2 reads to process
#define READ_MT_INTERVAL 300
#define QUEUE_DATA_INTERVAL 150 // keep small, fine tune in definitions above
#define LOGSEND_INTERVAL min(MOTOR_LOGSEND_INTERVAL, FUELCELL_LOGSEND_INTERVAL)
#define DISPLAY_INTERVAL QUEUE_DATA_INTERVAL