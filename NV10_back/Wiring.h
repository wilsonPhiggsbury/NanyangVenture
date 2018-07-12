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
// Wiring naming convention: DEVICE_PINTYPE_PINNAME
#define SD_SPI_CS 4			// used by shield, no actual wiring
#define TFT1_SPI_RS 9
#define TFT1_SPI_CS 10
#define TFT1_SPI_RST 11
#define LCD1_I2C_ADDR 0x27

// Declare queue string structures:
/*
				ms	\t	V	\t	A	\t	W	\t	Wh	\t	V	\t	A	\t	W	\t	Wh
FC datachunks:	<=8	1	4	1	4	1	4	1	5	1	4	1	4	1	4	1	5

				ms	\t	V	\t	A	\t	V	\t	A	\t	V	\t	A	\t	V	\t	A
MT datachunks:	<=8	1	<4	1	<4	1	<4	1	<4	1	<4	1	<4	1	<4	1	<4

*/
#define FUELCELL_DATALEN 70 // (9+29+29) (timestamp + hydrocell1 + hydrocell2)
#define MOTOR_DATALEN 49 // (9 + 4*(5+5)) (timestamp + motors*(volt + amp))
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


