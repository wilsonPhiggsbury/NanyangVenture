#pragma once

// Wiring naming convention: DEVICE_PINTYPE_PINNAME

// internal use, no exterior connections should be made to these pins
#define SD_SPI_CS_PIN 4			// used by shield for SD card
#define CAN_CS_PIN 48			// used by CAN shield
#define CAN_INT_PIN 19
//#define TFT1_SPI_RS 23
//#define TFT1_SPI_RST 25
//#define TFT1_SPI_CS 27



// rear peripherals
#define SPEEDOMETER_INTERRUPT_PIN 18
#define RUNNINGLIGHT_PIN 9
#define BRAKELIGHT_PIN 10
#define LSIG_PIN 11
#define RSIG_PIN 12

// current sensor inputs
#define CAP_IN_VPIN A0
#define CAP_IN_APIN A1
#define CAP_OUT_VPIN A2
#define CAP_OUT_APIN A3
#define MOTOR_VPIN A4
#define MOTOR_APIN A5

// fuel cell inputs
#define FC_MASTER_PORT Serial3
#define FC_SLAVE_PORT Serial2