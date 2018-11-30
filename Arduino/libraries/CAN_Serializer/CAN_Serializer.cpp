#include "CAN_Serializer.h"

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
void CAN_Serializer::onlyListenFor(DataSource ID)
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
bool CAN_Serializer::sendCAN(QueueItem* q)
{
	// only accept the queueItem when available
	if (!pendingSend)
	{
		framesO.clear();
		q->toFrames(&framesO);
		pendingSend = framesO.getNumFrames();
		return true;
	}
	else
	{
		return false;
	}
}
bool CAN_Serializer::recvCAN(QueueItem* q)
{
	if (readyRecv)
	{
		bool status = framesI.toQueueItem(q);
		framesI.clear();
		readyRecv = false;
		return status;
	}
	else
	{
		return false;
	}
}
bool CAN_Serializer::sendSerial(HardwareSerial& serial, QueueItem* q)
{
	char payload[MAX_STRING_LEN];
	q->toString(payload);
	serial.println(payload);
	return true;
}
bool CAN_Serializer::recvSerial(HardwareSerial& serial, QueueItem* q)
{
	char payload[MAX_STRING_LEN];
	if (serial.available())
	{
		int bytesRead = serial.readBytesUntil('\n', payload, MAX_STRING_LEN);
		if (bytesRead > 0)
			payload[bytesRead - 1] = '\0';
		bool convertSuccess = QueueItem::toQueueItem(payload, q);
		return convertSuccess;
	}
	else
	{
		return false;
	}
}
void CAN_Serializer::sendCAN_OneFrame()
{
	// ---------------------  pulse out one frame -----------------------
	// ------------ does nothing if there is nothing to send ------------
	if(pendingSend)
	{
		// total - current = index of frameToSend
		NV_CanFrame& frameToSend = framesO.frames[framesO.getNumFrames()-pendingSend];
		byte status = CAN->sendMsgBuf(frameToSend.id, 0, frameToSend.length, frameToSend.payload);

		if (status != CAN_OK)
		{
			// abort sending this full frame
			pendingSend = 0;
		}
		else
		{
			pendingSend--;
		}
	}
}
void CAN_Serializer::recvCAN_OneFrame()
{
	// ---------------------  pulse in one frame -----------------------
	// ---------- updates readyRecv when a queueItem is ready ----------
	if (CAN->checkReceive() == CAN_MSGAVAIL && CAN->checkError() == CAN_OK)
	{
		CAN->readMsgBuf(&id, &len, inBuffer);
		bool isLastFrame = framesI.addItem(id, len, inBuffer);
		readyRecv = isLastFrame;
	}

}
void CAN_Serializer::printFrames(NV_CanFrames& frames)
{
	for (uint8_t i = 0; i < frames.getNumFrames(); i++)
	{
		NV_CanFrame f = frames.frames[i];
		Serial.print("ID: ");
		Serial.println(f.id);
		for (uint8_t j = 0; j < f.length; j++)
		{
			Serial.print(f.payload[j], HEX);
			Serial.print(" ");
		}
		Serial.println();
	}
	Serial.println("---------");
}
void CAN_Serializer::printQueue(QueueItem& q)
{
	char qstr[MAX_STRING_LEN];
	q.toString(qstr);
	Serial.println(qstr);
}