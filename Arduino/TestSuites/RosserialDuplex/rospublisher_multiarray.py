import rospy
from std_msgs.msg import UInt32MultiArray

def talker():
    pub = rospy.Publisher('daRx', UInt32MultiArray, queue_size=10)
    rospy.init_node('talker', anonymous=True)
    rate = rospy.Rate(10) # 10hz
    while not rospy.is_shutdown():
        toSend = {layout= {dim= [], data_offset= []},\
        data = [1,10,100,1000]\
        }
        rospy.loginfo(toSend.data)
        pub.publish(toSend)
        rate.sleep()

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException:
        pass
