// 
// 
// 

#include "CANSerializer.h"

void CANSerializer::init(uint8_t cs)
{
	CAN = new MCP_CAN(cs);
	CAN->begin(MCP_STD, CAN_1000KBPS, MCP_16MHZ);
}

bool CANSerializer::sendCanFrame(CANFrame *f)
{
	CAN->sendMsgBuf(f->id, f->length, f->payload);
	return false;
}

bool CANSerializer::receiveCanFrame(CANFrame *f)
{
	CAN->readMsgBuf(&f->id, &f->length, f->payload);
	return false;
}
