#include "Constants.h"
class Frames;
// struct to pass payload between different tasks in same microcontroller
class Packet {
	friend class Frames;
	friend class CAN_Serializer;
public:
	PacketID ID;
	unsigned long timeStamp;
	float data[NUM_DATASETS][NUM_DATASUBSETS];
	void toString(char* putHere);
	static bool toPacket(char * str, Packet * Packet); //		<--- *NOT YET verified if it works on AVR chips*
private:
	void toFrames(Frames* putHere);
};