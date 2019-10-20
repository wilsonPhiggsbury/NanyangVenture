/*
 Name:		RosserialDuplex.ino
 Created:	3/18/2019 11:32:03 PM
 Author:	MX
*/

#include <ros.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/UInt32MultiArray.h>

void messageCb(const std_msgs::UInt32MultiArray& recv);
float floats[4];
std_msgs::Float32MultiArray daFloat;
std_msgs::UInt32MultiArray daLong;
ros::NodeHandle nh;//_<ArduinoHardware, 10, 10, 100, 105, ros::DefaultReadOutBuffer_>
ros::Publisher daPublisher("daTx", &daFloat);
ros::Subscriber<std_msgs::UInt32MultiArray> daSubscriber("daRx", messageCb);
void setup() {
	pinMode(LED_BUILTIN, OUTPUT);

#ifdef __AVR_ATmega2560__
	Serial1.begin(9600);
#endif
	//nh.getHardware()->setBaud(9600);
	nh.initNode();
	nh.advertise(daPublisher);
	nh.subscribe(daSubscriber);
}

void loop() {
	static uint8_t counter = 0;
	if ((counter++) % 8 == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			floats[i] = random(0, 100) / 10.0;
		}
		daFloat.data_length = sizeof(floats) / sizeof(floats[0]);
		daFloat.data = floats;
		daPublisher.publish(&daFloat);
	}
	nh.spinOnce();
	delay(100);
}
void messageCb(const std_msgs::UInt32MultiArray& recv) 
{
#ifdef __AVR_ATmega2560__
	for (int i = 0; i < recv.data_length; i++)
	{
		Serial1.print(i);
		Serial1.print("\t");
		Serial1.print(recv.data[i]);
	}
#endif
}
/* to send multi dimensional array msgs:
--------------------------------------
rostopic pub /daRx std_msgs/UInt32MultiArray -r 1 """
> layout:
>  dim: []
>  data_offset: 0
> data: [1, 2, 4, 5]
> """

python script for publishing:
--------------------------------
import rospy
from std_msgs.msg import UInt32MultiArray

def talker():
    pub = rospy.Publisher('daRx', UInt32MultiArray, queue_size=10)
    rospy.init_node('talker', anonymous=True)
    rate = rospy.Rate(10) # 10hz
    while not rospy.is_shutdown():
        toSend = UInt32MultiArray(data=[0,10,20,1000])
        rospy.loginfo(toSend.data)
        pub.publish(toSend)
        rate.sleep()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException:
        pass
 */
