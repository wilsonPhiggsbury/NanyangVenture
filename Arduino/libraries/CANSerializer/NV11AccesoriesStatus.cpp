// 
// 
// 
#include "NV11AccesoriesStatus.h"
// parameter(CANbytes, stringChars)
// param1(2,4), param2(1,2)
NV11AccesoriesStatus::NV11AccesoriesStatus(uint8_t CANId) :DataPoint("ST", CANId, 6)
{
	debug(F("DataPoint ST:\t0x10\t6"));
}

void NV11AccesoriesStatus::setLsig(uint8_t status)
{
	lsig = status;
}

void NV11AccesoriesStatus::setRsig(uint8_t status)
{
	rsig = status;
}

void NV11AccesoriesStatus::setWiper(uint8_t status)
{
	wiper = status;
}

void NV11AccesoriesStatus::setHazard(uint8_t status)
{
	hazard = status;
}

void NV11AccesoriesStatus::setHeadlights(uint8_t status)
{
	headlights = status;
}

void NV11AccesoriesStatus::setBrake(uint8_t status)
{
	brake = status;
}

void NV11AccesoriesStatus::setFourWS(uint8_t status)
{
	brake = status;
}

uint8_t NV11AccesoriesStatus::getLsig()
{
	return lsig;
}

uint8_t NV11AccesoriesStatus::getRsig()
{
	return rsig;
}

uint8_t NV11AccesoriesStatus::getWiper()
{
	return wiper;
}

uint8_t NV11AccesoriesStatus::getHazard()
{
	return hazard;
}

uint8_t NV11AccesoriesStatus::getHeadlights()
{
	return headlights;
}

uint8_t NV11AccesoriesStatus::getBrake()
{
	return brake;
}

uint8_t NV11AccesoriesStatus::getFourWS()
{
	return fourWS;
}

void NV11AccesoriesStatus::insertData(uint8_t lsig, uint8_t rsig, uint8_t wiper, uint8_t hazard, uint8_t headlights, uint8_t brake)
{
	timeStamp = millis();
	this->lsig = lsig;
	this->rsig = rsig;
	this->wiper = wiper;
	this->hazard = hazard;
	this->headlights = headlights;
	this->brake = brake;
}

void NV11AccesoriesStatus::packString(char *str)
{
	char* shiftedStr = DataPoint::packStringDefault(str);
	// param1 = 4, param2 = 2
	sprintf(shiftedStr, "%d\t%d\t%d\t%d\t%d\t%d", lsig, rsig, hazard, headlights, brake, wiper);
}

void NV11AccesoriesStatus::unpackString(char * str)
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
