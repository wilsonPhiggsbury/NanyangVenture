#pragma once
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include "Frames.h"
#include "QueueItem.h"
#ifdef __AVR__

#elif defined _SAM3XA_
#endif
class CAN_Serializer
{
public:
	/* initializing functions */
	bool init(byte pin);
	void onlyListenFor(PacketID id);
	/* these 2 functions are called when you need to send / receive a queueItem */
	// Prepares the queueItem for sending. Only acutally sent when you call sendCAN_OneFrame()
	bool sendCAN(Packet* q);
	// Populates queueItem only if data is available.
	bool recvCAN(Packet * q);
	// Prepares the queueItem for sending into serial port. Is a wrapper function.
	static bool sendSerial(HardwareSerial & serial, Packet * q);
	// Populates queueItem only if complete Serial string is read
	static bool recvSerial(HardwareSerial & serial, Packet * q);
	/* these 2 functions should be called regularly to perform actual sending / receiving job */
	// Needs multiple calls to empty send a single queueItem. Should be called with small delay to prevent jamming.
	void sendCAN_OneFrame();
	// Designed to be called both inside/outside an ISR.
	void recvCAN_OneFrame();

	// debug printing
	static void printFrames(Frames & frames);
	static void printPacket(Packet & q);
	
private:
	uint16_t pin;
	MCP_CAN* CAN;
	Frames framesI;
	Frames framesO;
	// tracking variables for send/recv frames
	uint8_t pendingSend = 0;
	bool readyRecv;
	// scratch variables for receiving frame content
	unsigned long id;
	byte len;
	byte inBuffer[8];
};