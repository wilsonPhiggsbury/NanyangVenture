#pragma once
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include "Packet.h"
#ifdef __AVR__

#elif defined _SAM3XA_
#endif
class CAN_Serializer
{
public:
	/* initializing functions */
	bool init(byte pin);
	void onlyListenFor(PacketID id);
	/* these 2 functions are called when you need to send / receive a Packet */
	// Prepares the Packet for sending. Only acutally sent when you call sendCAN_OneFrame()
	bool sendCanPacket(Packet* q);
	// Populates Packet only if data is available.
	bool receiveCanPacket(Packet * q);
	// Prepares the Packet for sending into serial port. Is a wrapper function.
	static bool sendSerial(HardwareSerial & serial, Packet * q);
	// Populates Packet only if complete Serial string is read
	static bool receiveSerial(HardwareSerial & serial, Packet * q);
	
	// called upon receive timeout. patch for randomly spoiling CAN bus.
	void receiveTimeout();

	// debug printing
	static void printPacket(Packet & q);
	
private:
	uint16_t pin;
	MCP_CAN* CAN;
	NV_CanFrame framesIn;
	NV_CanFrame framesOut;
	// tracking variables for send/recv frames
	uint8_t pendingSend = 0;
	bool readyToRecieve;
	// scratch variables for receiving frame content
	unsigned long id;
	byte len;
	byte inBuffer[8];
};