// put this at the top of the script

//QueueHandle_t queueForCAN = xQueueCreate(1, sizeof(Packet));
//CAN_Serializer serializer;
//void TaskCAN(void* pvParameters);			// In/Out task:		Manages 2-way CAN bus comms

// put this inside setup()
/*
CAN_avail = serializer.init(CAN_CS_PIN);
xTaskCreate(
	TaskCAN
	, (const portCHAR *) "CAN la!" // where got cannot?
	, 1200  // Stack size
	, NULL
	, 3  // Priority
	, NULL);
*/

// implement this function as handler for receiving Packets from CAN
//void doReceiveAction(Packet* q)
//{
//	// do your stuff here
//}

// kept in .ino file for now, due to function signature consistency. No idea yet how to port it out into a separate project/library

void CAN_ISR()
{
	// pulse one frame in
	//serializer.receiveCanPacket();
}