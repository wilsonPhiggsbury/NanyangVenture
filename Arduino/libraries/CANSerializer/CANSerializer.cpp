/*
 Name:		CANSerializer.cpp
 Created:	2/15/2019 3:38:38 PM
 Author:	MX
 Editor:	http://www.visualmicro.com
*/

#include "CANSerializer.h"

bool CANSerializer::init(uint8_t cs)
{
	CAN = new MCP_CAN(cs);
	bool initSuccess = CAN->begin(MCP_STDEXT, CAN_1000KBPS, MCP_16MHZ) == CAN_OK;
	if (initSuccess)
	{
		CAN->setMode(MCP_NORMAL);
		return true;
	}
	return false;
}

bool CANSerializer::sendCanFrame(CANFrame *f)
{
	return CAN->sendMsgBuf(f->id, f->length, f->payload) == CAN_OK;
}

bool CANSerializer::receiveCanFrame(CANFrame *f)
{
	if (CAN->checkReceive() == CAN_MSGAVAIL && CAN->checkError() == CAN_OK)
	{
		CAN->readMsgBuf(&f->id, &f->length, f->payload);
		return true;
	}
	return false;
}
