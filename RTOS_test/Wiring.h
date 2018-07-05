#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#define LCD1_I2C_ADDR 0x27
#define TFT1_SPI_CS 10
#define TFT1_SPI_RS 9
#define TFT1_SPI_RST 11

// Declare queue string structures:
/*
				ms	\t	V	\t	A	\t	W	\t	Wh	\t	V	\t	A	\t	W	\t	Wh
FC datachunks:	[8	1	4	1	4	1	4	1	5	1	4	1	4	1	4	1	5

				ms	\t	V	\t	A	\t	V	\t	A	\t	V	\t	A	\t	V	\t	A
MT datachunks:	[8	1	4	1	4	1	4	1	4	1	4	1	4	1	4	1	4

*/

// Declare queue data structures: ID of enum type and data of String type
#define IDLEN 2
#define FUELCELL_DATALEN 60
#define MOTOR_DATALEN 60

#include <SPI.h>
#include <Wire.h>

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