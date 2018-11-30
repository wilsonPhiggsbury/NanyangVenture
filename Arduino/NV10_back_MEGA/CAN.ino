// put this at the top of the script

//QueueHandle_t queueForCAN = xQueueCreate(1, sizeof(QueueItem));
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

// implement this function as handler for receiving QueueItems from CAN
//void doReceiveAction(QueueItem* q)
//{
//	// do your stuff here
//}

void TaskCAN(void *pvParameters){
	QueueItem in, out;
	bool sent, received;
	attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), CAN_ISR, FALLING);
	debug(F("CAN started."));
	while (1)
	{
		// anything to send?
		BaseType_t success = xQueueReceive(queueForCAN, &out, 0);
		if (success)
		{
			sent = serializer.sendCAN(&out);
			if(!sent)
			{
				Serial.println(F("!!"));
			}
		}
		// anything to receive?
		received = serializer.recvCAN(&in);
		if (received)
		{
			doReceiveAction(&in);
		}
		// pulse one frame out
		serializer.sendCAN_OneFrame();
		vTaskDelay(pdMS_TO_TICKS(20));
	}
}
void CAN_ISR()
{
	// pulse one frame in
	serializer.recvCAN_OneFrame();
}