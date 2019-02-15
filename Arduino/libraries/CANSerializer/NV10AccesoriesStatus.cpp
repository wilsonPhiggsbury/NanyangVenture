// 
// 
// 
#include "NV10AccesoriesStatus.h"
// parameter(CANbytes, stringChars)
// param1(2,4), param2(1,2)
NV10AccesoriesStatus::NV10AccesoriesStatus(uint8_t CANId) :DataPoint(CANId, 1)
{
	strcpy(strHeader, "ST");
}

void NV10AccesoriesStatus::setLsig(int8_t status)
{
	lsig = status;
}

void NV10AccesoriesStatus::setRsig(int8_t status)
{
	rsig = status;
}

void NV10AccesoriesStatus::setWiper(int8_t status)
{
	wiper = status;
}

void NV10AccesoriesStatus::setHazard(int8_t status)
{
	hazard = status;
}

void NV10AccesoriesStatus::setHeadlights(int8_t status)
{
	headlights = status;
}

void NV10AccesoriesStatus::setBrake(int8_t status)
{
	brake = status;
}

int8_t NV10AccesoriesStatus::getLsig()
{
	return lsig;
}

int8_t NV10AccesoriesStatus::getRsig()
{
	return rsig;
}

int8_t NV10AccesoriesStatus::getWiper()
{
	return wiper;
}

int8_t NV10AccesoriesStatus::getHazard()
{
	return hazard;
}

int8_t NV10AccesoriesStatus::getHeadlights()
{
	return headlights;
}

int8_t NV10AccesoriesStatus::getBrake()
{
	return brake;
}

void NV10AccesoriesStatus::insertData(int8_t lsig, int8_t rsig, int8_t wiper, int8_t hazard, int8_t headlights, int8_t brake)
{
	this->lsig = lsig;
	this->rsig = rsig;
	this->wiper = wiper;
	this->hazard = hazard;
	this->headlights = headlights;
	this->brake = brake;
}

void NV10AccesoriesStatus::packCAN(CANFrame *f)
{
	byte compressedStats = (lsig&B1)<<7 | (rsig&B1)<<6 | (hazard&B1)<<5 | (headlights&B1)<<4 | (brake&B1)<<3 | wiper&B111;
	DataPoint::packCANDefault(f);
	memcpy(f->payload, &compressedStats, sizeof(byte));
}

void NV10AccesoriesStatus::unpackCAN(const CANFrame *f)
{
	byte compressedStats;
	memcpy(&compressedStats, f->payload, sizeof(byte));
	lsig = compressedStats >> 7 & B1;
	rsig = compressedStats >> 6 & B1;
	hazard = compressedStats >> 5 & B1;
	headlights = compressedStats >> 4 & B1;
	brake = compressedStats >> 3 & B1;
	wiper = compressedStats & B111;
}

void NV10AccesoriesStatus::packString(char *str)
{
	char* shiftedStr = DataPoint::packStringDefault(str);
	// param1 = 4, param2 = 2
	sprintf(shiftedStr, "%d\t%d\t%d\t%d\t%d\t%d", lsig, rsig, hazard, headlights, brake, wiper);
}

void NV10AccesoriesStatus::unpackString(char * str)
{
	char* ptr = strtok(str, "\t");
	ptr = strtok(NULL, "\t");
	timeStamp = strtoul(ptr, NULL, 16);

	ptr = strtok(NULL, "\t");
	lsig = atoi(ptr);

	ptr = strtok(NULL, "\t");
	rsig = atoi(ptr);

	ptr = strtok(NULL, "\t");
	hazard = atoi(ptr);

	ptr = strtok(NULL, "\t");
	headlights = atoi(ptr);

	ptr = strtok(NULL, "\t");
	brake = atoi(ptr);

	ptr = strtok(NULL, "\t");
	wiper = atoi(ptr);
}
