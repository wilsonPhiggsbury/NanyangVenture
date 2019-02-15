// 
// 
// 

#include "DataPoint.h"
const char* STRING_HEADER[] = { "FC","CS","Cs","SM","H2","ST","CM","HB" };
//DataPoint::DataPoint(eEncodingPreset (&presets)[8])
//{
//	uint8_t overFlow = 0;
//	for (int i = 0; i < sizeof(presets); i++)
//	{
//		this->presets[i] = presets[i];
//		switch (presets[i])
//		{
//		case preset_uint8:
//		case preset_uint8x10:
//			overFlow += 1;
//			break;
//		case preset_uint16:
//		case preset_uint16x10:
//			overFlow += 2;
//			break;
//		case preset_float:
//		case preset_uint32:
//			overFlow += 4;
//			break;
//		default:
//			break;
//		}
//	}
//	
//	if (overFlow > 8)
//		errorFlag = 1;
//} 
//bool DataPoint::checkError()
//{
//	return errorFlag;
//}
// TODO: see DataPoint.h file

void DataPoint::setCanId(uint8_t id)
{
	CANId = id;
}
uint8_t DataPoint::getCanId()
{
	return CANId;
}
DataPoint::DataPoint(uint8_t CANId, const uint8_t CANLength):CANId(CANId), CANLength(CANLength)
{
}

const char * DataPoint::getStringHeader()
{
	return strHeader;
}

void DataPoint::packCANDefault(CANFrame *f)
{
	f->id = CANId;
	f->length = this->CANLength;
}
bool DataPoint::checkMatchCAN(const CANFrame *f)
{
	if (f->id == this->CANId && f->length == this->CANLength)
	{
		this->unpackCAN(f);
		return true;
	}
	return false;
}

char* DataPoint::packStringDefault(char * str)
{
	int charsPrinted = sprintf(str, "%s\t%x\t", getStringHeader(), timeStamp);
	str += charsPrinted;// incr counter of the calling function
	return str;
}
bool DataPoint::checkMatchString(char * str)
{
	if (!strncmp(str, getStringHeader(), 2))
	{
		this->unpackString(str);
		return true;
	}
	return false;
}

void debugPrint(char * toPrint, int len)
{
#if DEBUG
	for (int i = 0; i < len; i++)
	{
		char c = *(toPrint + i);
		if (isPrintable(c))
			Serial.print(c);
		else
		{
			Serial.print("<");
			Serial.print((uint8_t)c);
			Serial.print(">");
		}
	}
	Serial.println();
#endif
}

void print(char * toPrint)
{
	Serial.println(toPrint);
}
