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

void TaskCAN(void *pvParameters){
	Packet in, out;
	bool sent, received;
	int receiveTimeout = 200;
	unsigned long lastTime = 0;
	attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), CAN_ISR, FALLING);
	debug(F("CAN started."));
	while (1)
	{
		// anything to send?
		BaseType_t success = xQueueReceive(queueForCAN, &out, 0);
		if (success)
		{
			sent = serializer.sendCanPacket(&out);
			if(!sent)
			{
				Serial.println(F("!!"));
			}
		}
		// anything to receive?
		received = serializer.receiveCanPacket(&in);
		if (received)
		{
			debug_(F("----------Ticks left: ")); debug_(receiveTimeout); debug_(F("----------"));
			receiveTimeout = 200;
			serializer.receiveTimeout();
			doReceiveAction(&in);
		}
		else
		{
			if (--receiveTimeout <= 0)
			{
				receiveTimeout = 50;
				debug_(F("----------CAN receive timeout @ ")); debug_(millis());debug_(F(" ms----------"));
				serializer.receiveTimeout();
			}
		}
		// pulse one frame out
		serializer.sendCAN_OneFrame();
		//debug(millis() - lastTime);
		lastTime = millis();
		vTaskDelay(2); // ~35ms delay between calls
	}
}
void CAN_ISR()
{
	// pulse one frame in
	serializer.recvCAN_OneFrame();
}