// DataPoint.h

#ifndef _DATAPOINT_h
#define _DATAPOINT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#define DEBUG 0
#if DEBUG
#define debug_(str) Serial.print(str)
#define debug(str)  Serial.println(str)
#else
#define debug_(...)
#define debug(...)
#endif

#include <CANSerializer.h>

extern const char* STRING_HEADER[];// Fuel Cell, Current Sensor, current sensor stats, Speedometer, Hydrogen Tank Bar, Status of lights, Commands, Heartbeat
void debugPrint(char* toPrint, int len);
void print(char * toPrint);
class DataPoint
{
public:
	/// <summary>
	/// Provide a standard CAN ID for each DataPoint
	/// </summary>
	/// <param name="id">value between 0 - 0x7FF</param>
	void setCanId(uint8_t id);
	uint8_t getCanId();
	//virtual void insertData() = 0; cancelled due to different function signatures in different implementations
	bool checkMatchString(char * str);
	bool checkMatchCAN(const CANFrame*);
protected:
	//typedef enum encodingPreset
	//{
	//	nil,
	//	preset_uint8,		// [0, 255]
	//	preset_uint8x10,	// [0, 25.5]
	//	preset_uint16,		// [0, 65535]
	//	preset_uint16x10,	// [0, 6553.5]
	//	preset_float,		// floats are awesome!
	//	preset_uint32,		// [0, 4,294,967,295]
	//}eEncodingPreset;
	//
	//DataPoint(eEncodingPreset (&presets)[8]);
	DataPoint(uint8_t CANId, const uint8_t CANLength);
	char strHeader[3];
	// TODO: utilize encodingPresets to specify CAN encoding. Proposal: child classes use aliases to parent class' variables
	unsigned long timeStamp;
	const uint8_t CANLength;
	uint8_t CANId;
	const char* getStringHeader();
	void packCANDefault(CANFrame*);
	char* packStringDefault(char * str);
	virtual void packCAN(CANFrame*) = 0;
	virtual void packString(char * str) = 0;
	virtual void unpackCAN(const CANFrame*) = 0;
	virtual void unpackString(char * str) = 0;

	bool checkError();
private:
	//eEncodingPreset presets[8];
	//byte errorFlag = 0;
	// TODO: error flag appropriate?
};

#endif

