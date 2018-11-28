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
	CAN_Serializer(byte CS_pin);
	void init();
	void onlyListenFor(DataSource id);
	/* these 2 functions are called when you need to send / receive a queueItem */
	// Prepares the queueItem for sending. Only acutally sent when you call sendOneFrame()
	bool send(QueueItem* q);
	// Populates queueItem only if data is available.
	bool recv(QueueItem * q);
	/* these 2 functions should be called regularly to perform actual sending / receiving job */
	// Needs multiple calls to empty send a single queueItem. Should be called with small delay to prevent jamming.
	void sendOneFrame();
	// Designed to be called both inside/outside an ISR.
	void recvOneFrame();


private:
	uint16_t pin;
	MCP_CAN* CAN;
	NV_CanFrames framesI;
	NV_CanFrames framesO;
	// tracking variables for send/recv frames
	uint8_t pendingSend;
	bool readyRecv;
	// scratch variables for receiving frame content
	unsigned long id;
	byte len;
	byte inBuffer[8];
	void printFrames(NV_CanFrames & frames);
	void printQueue(QueueItem & q);
};