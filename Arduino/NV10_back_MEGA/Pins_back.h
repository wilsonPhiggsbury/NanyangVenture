#pragma once

// Wiring naming convention: DEVICE_PINTYPE_PINNAME

#if defined(__AVR_ATmega1284P__)

// internal use, no exterior connections should be made to these pins
#define SD_SPI_CS 4			// used by shield for SD card
#define CAN_SPI_CS 48			// used by CAN shield
#define CAN_INTERRUPT 19
//#define TFT1_SPI_RS 23
//#define TFT1_SPI_RST 25
//#define TFT1_SPI_CS 27



// rear peripherals
#define SPEEDOMETER_INTERRUPT 18
#define RUNNINGLIGHT_OUTPUT 9
#define BRAKELIGHT_OUTPUT 10
#define LSIG_OUTPUT 11
#define RSIG_OUTPUT 12

// current sensor inputs
#define CAPIN_INPUT_VOLT A0
#define CAPIN_INPUT_AMP A1
#define CAPOUT_INPUT_VOLT A2
#define CAPOUT_INPUT_AMP A3
#define MOTOR_INPUT_VOLT A4
#define MOTOR_INPUT_AMP A5

#define FCLEFT_SERIAL Serial
#define FCRIGHT_SERIAL Serial1
// fuel cell inputs
#elif defined(__AVR_ATmega2560__)

// internal use, no exterior connections should be made to these pins
#define SD_SPI_CS 4			// used by shield for SD card
#define CAN_SPI_CS 48			// used by CAN shield
#define CAN_INTERRUPT 19
//#define TFT1_SPI_RS 23
//#define TFT1_SPI_RST 25
//#define TFT1_SPI_CS 27



// rear peripherals
#define SPEEDOMETER_INTERRUPT 18
#define RUNNINGLIGHT_OUTPUT 9
#define BRAKELIGHT_OUTPUT 10
#define LSIG_OUTPUT 11
#define RSIG_OUTPUT 12

// current sensor inputs
#define CAPIN_INPUT_VOLT A0
#define CAPIN_INPUT_AMP A1
#define CAPOUT_INPUT_VOLT A2
#define CAPOUT_INPUT_AMP A3
#define MOTOR_INPUT_VOLT A4
#define MOTOR_INPUT_AMP A5

#define FCLEFT_SERIAL Serial3
#define FCRIGHT_SERIAL Serial2
#else
#error "Unsupported board"
#endif