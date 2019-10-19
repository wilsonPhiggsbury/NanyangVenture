// NV10CurrentSensorStats.h

#ifndef _DATAPOINTTEMPLATE_h
#define _DATAPOINTTEMPLATE_h

#include <DataPoint.h>

/* fields to change are marked with "##"

	demo field:
		XX	XX	XX	XX	XX	XX	XX	XX
		p1	p2	p3	p3	p4	p4	p4	p4
	1B	0	1	2	3	4	5	6	7
	2B	0	-	1	-	2	-	3	-
	4B	0	-	-	-	1	-	-	-
		^								param 1 starts here, at 1B[0]
			^							param 2 starts here, at 1B[1]
				^						param 3 starts here, at 2B[1]
						^				param 4 starts here, at 4B[1]
*/
class DataPointTemplate:public DataPoint
{
 protected:
	 uint8_t& param1 = data.Byte[0]; //## is example param that has expected range [0, 255]
	 uint8_t& param2 = data.Byte[1]; //## is example param that has expected range [0, 1] with 1 decimal precision
	 uint16_t& param3 = data.UInt[1];//## is example param that has expected range [-100, 100] with 1 decimal precision
	 uint32_t& param4 = data.Long[1];//## is example param represented by float (yields maximum precision system can provide)

 public:
	DataPointTemplate();

	void insertData(uint8_t param1, float param2, float param3, float param4);
	uint16_t getParam1();
	float getParam2();
	float getParam3();
	float getParam4();
	void packString(char*);
	void unpackString(char * str);
};

#endif

