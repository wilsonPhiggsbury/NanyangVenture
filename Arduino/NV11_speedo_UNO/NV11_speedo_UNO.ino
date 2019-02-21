/*
 Name:		NV11_speedo_UNO.ino
 Created:	2/18/2019 3:14:55 PM
 Author:	MX
*/

#include <ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/UInt32.h>
#include "Speedometer.h"
#include "Pins_speedo.h"
Speedometer speedoBL = Speedometer(SPEEDO_BL_A, SPEEDO_BL_B, 545, 500, true);
Speedometer speedoBR = Speedometer(SPEEDO_BR_A, SPEEDO_BR_B, 545, 500, false);
ros::NodeHandle nh;//_<ArduinoHardware, 10, 10, 100, 105, ros::DefaultReadOutBuffer_>
std_msgs::Float32 blSpeed, brSpeed;
std_msgs::UInt32 blDist, brDist;
ros::Publisher blSpeedPublisher("blSpeed",&blSpeed), blDistPublisher("blDist",&blDist),brSpeedPublisher("brSpeed",&brSpeed), brDistPublisher("brDist",&brDist);
uint32_t lastTime = 0;

void setup() {
	//Serial.begin(9600);
	attachInterrupt(digitalPinToInterrupt(SPEEDO_BL_A), tick, FALLING);
	attachInterrupt(digitalPinToInterrupt(SPEEDO_BR_A), tick, FALLING);
	//handle.getHardware()->setBaud(9600);
	nh.initNode();
	nh.advertise(blSpeedPublisher);
	nh.advertise(blDistPublisher);
	nh.advertise(brSpeedPublisher);
	nh.advertise(brDistPublisher);
}

// the loop function runs over and over again until power down or reset
void loop() {
	delay(100);
	blSpeed.data = speedoBL.calculateSpeedKmh();
	blDist.data = speedoBL.getTotalDistTravelled();
	brSpeed.data = speedoBR.calculateSpeedKmh();
	brDist.data = speedoBR.getTotalDistTravelled();
  
	blSpeedPublisher.publish(&blSpeed);
	blDistPublisher.publish(&blDist);
	brSpeedPublisher.publish(&brSpeed);
	brDistPublisher.publish(&brDist);
  
	nh.spinOnce();
}

void tick()
{
	speedoBL.trip();
	speedoBR.trip();
}
