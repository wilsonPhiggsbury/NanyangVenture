#pragma once

#define LCD1_I2C_ADDR 0x27

// Wiring naming convention: DEVICE_PINTYPE_PINNAME

// internal use, no exterior connections should be made to these pins
#define SD_SPI_CS_PIN 4			// used by shield for SD card
#define CAN_CS_PIN 46			// used by CAN shield
#define CAN_INTERRUPT_PIN 19
//#define TFT1_SPI_RS 23
//#define TFT1_SPI_RST 25
//#define TFT1_SPI_CS 27



// rear peripherals
#define SPEEDOMETER_INTERRUPT_PIN 18
#define BRAKE_PIN 46
#define L_SIGNAL_PIN 47
#define R_SIGNAL_PIN 49

// current sensor inputs
#define L_WHEEL_VPIN A0
#define L_WHEEL_APIN A1
#define R_WHEEL_VPIN A2
#define R_WHEEL_APIN A3
#define SUPERCAP_VPIN A4
#define SUPERCAP_APIN A5

// fuel cell inputs
#define FC_MASTER_PORT Serial3
#define FC_SLAVE_PORT Serial2