#include "CAN_Serializer.h"

/*
WARNING
This library has been depreceated. It is only used in old NV10 code.
*/
bool CAN_Serializer::init(byte pin)
{
	CAN = new MCP_CAN(pin);
	if (CAN->begin(MCP_STDEXT, NV_CANSPEED, MCP_16MHZ) == CAN_OK)
	{
		CAN->setMode(MCP_NORMAL);
		return true;
	}
	else
	{
		return false;
	}
}
/// <summary>
/// Blocks out other unwanted PacketIDs except for the specified one.
/// </summary>
/// <param name="ID"></param>
void CAN_Serializer::onlyListenFor(PacketID ID)
{
	unsigned long id = ID;
	unsigned long maskVal = (unsigned long)B1111 << 18;
	unsigned long filterVal = (id << 18);
	// maskVal = 0x003C0000
	CAN->init_Mask(0, 0, maskVal);                // Init id mask: XXOO OOXX (X = ignore these bits, O = control these bits)
	CAN->init_Filt(0, 0, filterVal);                // Init filter:  XX01 00XX (Only accept button frames)
	CAN->init_Filt(1, 0, filterVal);                // Init filter:  Duplicate definition same as above

	CAN->init_Mask(1, 0, maskVal | B11 << 16);                // Init id mask: XXOO OOOO
	CAN->init_Filt(2, 0, filterVal | 0 << 16);                // Init filter:  XX01 0000 <--- these are actually redundancies
	CAN->init_Filt(3, 0, filterVal | 1 << 16);                // Init filter:  XX01 0001 <--- must fill in for compulsory's sake
	CAN->init_Filt(4, 0, filterVal | 2 << 16);                // Init filter:  XX01 0010 <--- they are explicit commands to accept termination bits
	CAN->init_Filt(5, 0, filterVal | 3 << 16);                // Init filter:  XX01 0011 <--- could've done without, but MCP_CAN needs these to work properly
}
bool CAN_Serializer::sendCanPacket(Packet* packet)
{
	packet->toFrames(&framesOut);
	byte status = CAN->sendMsgBuf(framesOut.id, 0, framesOut.len, framesOut.payload);
	return status == CAN_OK;
}
bool CAN_Serializer::receiveCanPacket(Packet* packet)
{
	if (CAN->checkReceive() == CAN_MSGAVAIL && CAN->checkError() == CAN_OK)
	{
		CAN->readMsgBuf(&framesIn.id, &framesIn.len, framesIn.payload);
		framesIn.toPacket(packet);
		return true;
	}
	return false;
}
bool CAN_Serializer::sendSerial(HardwareSerial& serial, Packet* packet)
{
	char payload[MAX_STRING_LEN];
	packet->toString(payload);
	serial.println(payload);
	return true;
}
bool CAN_Serializer::receiveSerial(HardwareSerial& serial, Packet* packet)
{
	char payload[MAX_STRING_LEN];
	if (serial.available())
	{
		int bytesRead = serial.readBytesUntil('\n', payload, MAX_STRING_LEN);
		if (bytesRead > 0)
			payload[bytesRead - 1] = '\0';
		bool convertSuccess = Packet::toPacket(payload, packet);
		return convertSuccess;
	}
	else
	{
		return false;
	}
}
void CAN_Serializer::receiveTimeout()
{
	debug_("Error code: "); debug(CAN->checkError());
}
void CAN_Serializer::printPacket(Packet& q)
{
	char qstr[MAX_STRING_LEN];
	q.toString(qstr);
	Serial.println(qstr);
}