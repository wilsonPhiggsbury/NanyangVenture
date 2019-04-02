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
//void 
//	CANFrame f;
//	char toSerialPort[100];
//	char fromSerialPort[100];
//	if (serializer.receiveCanFrame(&f))
//	{
//		for (int i = 0; i < sizeof(canListenList) / sizeof(canListenList[0]); i++)
//		{
//			DataPoint& dataPoint = *canListenList[i];
//			if (dataPoint.checkMatchCAN(&f))
//			{
//				dataPoint.unpackCAN(&f);
//				dataPoint.packString(toSerialPort);
//				Serial.println(toSerialPort);
//				break;
//			}
//		}
//	}
//	if (Serial.available())
//	{
//		int bytesRead = Serial.readBytesUntil('\n', fromSerialPort, 100);
//		fromSerialPort[bytesRead - 1] = '\0'; // "\r\n" should be the terminator, replace '\r' with '\0'
//		for (int i = 0; i < sizeof(serialListenList) / sizeof(serialListenList[0]); i++)
//		{
//			DataPoint& dataPoint = *serialListenList[i];
//			if (dataPoint.checkMatchString(fromSerialPort))
//			{
//				dataPoint.unpackString(fromSerialPort);
//				dataPoint.packCAN(&f);
//				serializer.sendCanFrame(&f);
//				break;
//			}
//		}
//	}