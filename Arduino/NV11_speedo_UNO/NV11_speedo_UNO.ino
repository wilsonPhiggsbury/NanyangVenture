/*
 Name:		NV11_speedo_UNO.ino
 Created:	2/18/2019 3:14:55 PM
 Author:	MX
*/
//#include <CANSerializer.h>
//#include <NV11DataSpeedo.h>

#include <ros.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/UInt32MultiArray.h>

#include "Speedometer.h"
#include "Pins_speedo.h"


//CANSerializer serializer;
//NV11DataSpeedo dataSpeedo = NV11DataSpeedo(0x0A);

Speedometer speedoBL = Speedometer(SPEEDO_BL_A, SPEEDO_BL_B, 545, 500, true);
Speedometer speedoBR = Speedometer(SPEEDO_BR_A, SPEEDO_BR_B, 545, 500, false);

ros::NodeHandle nh;//_<ArduinoHardware, 10, 10, 100, 105, ros::DefaultReadOutBuffer_>
std_msgs::Float32MultiArray speedKmh;
std_msgs::UInt32MultiArray distMm;
ros::Publisher speedPublisher("speedKmh", &speedKmh), distPublisher("distMm", &distMm);
//ros::Subscriber 

void setup() {
	//serializer.init(CAN_CS);
	//Serial.begin(9600);
	attachInterrupt(digitalPinToInterrupt(SPEEDO_BL_A), tickL, FALLING);
	attachInterrupt(digitalPinToInterrupt(SPEEDO_BR_A), tickR, FALLING);
	//handle.getHardware()->setBaud(9600);

	nh.initNode();
	nh.advertise(speedPublisher);
	nh.advertise(distPublisher);
	// in AVR port, only a length and a pointer is supplied. Pointer should point towards a pre-allocated array
	speedKmh.data_length = 2;
	distMm.data_length = 2;
	//nh.subscribe(&dummyScr);
}

// the loop function runs over and over again until power down or reset
void loop() {
	static uint8_t counter = 0;
	static float speedsPrimitive[2];
	static uint32_t distPrimitive[2];
	counter++;

	//Serial.print(speedoBL.getSpeedKmh()); Serial.print("\t");
	//Serial.print(speedoBL.getTotalDistTravelled()); Serial.print("\t");
	//Serial.print(speedoBR.getSpeedKmh()); Serial.print("\t");
	//Serial.print(speedoBR.getTotalDistTravelled()); Serial.println();

	speedsPrimitive[0] = speedoBL.getSpeedKmh();
	speedsPrimitive[1] = speedoBR.getSpeedKmh();
	speedKmh.data = speedsPrimitive;
	speedPublisher.publish(&speedKmh);
	distPrimitive[0] = speedoBL.getTotalDistTravelled();
	distPrimitive[1] = speedoBR.getTotalDistTravelled();
	distMm.data = distPrimitive;
	
	speedPublisher.publish(&speedKmh);
	distPublisher.publish(&distMm);
	nh.spinOnce();

//	if (counter % 8 == 0)
//	{
//		CANFrame f;
//		dataSpeedo.insertData((speedsPrimitive[0]+speedsPrimitive[1])/2);
//		dataSpeedo.packCAN(&f);
//		serializer.sendCanFrame(&f);
//	}
	delay(100);
}

void tickL()
{
	speedoBL.trip();
}
void tickR()
{
	speedoBR.trip();
}