// put this at the top of the script

//QueueHandle_t queueForCAN = xQueueCreate(1, sizeof(QueueItem));
//CAN_Serializer serializer = CAN_Serializer(CAN_CS_PIN);
//void TaskCAN(void* pvParameters);			// In/Out task:		Manages 2-way CAN bus comms

// put this inside setup()
/*
CAN_avail = serializer.init();
xTaskCreate(
	TaskCAN
	, (const portCHAR *) "CAN la!" // where got cannot?
	, 1200  // Stack size
	, NULL
	, 3  // Priority
	, NULL);
*/
void TaskCAN(void *pvParameters){
	QueueItem in, out;
	bool sent, received;
	attachInterrupt(digitalPinToInterrupt(CAN_INT_PIN), CAN_ISR, FALLING);
	Serial.println(F("CAN started."));
	while (1)
	{
		// anything to send?
		BaseType_t success = xQueueReceive(queueForCAN, &out, 0);
		if (success)
		{
			sent = serializer.send(&out);
			if(!sent)
			{
				Serial.println(F("!!"));//sent = serializer.send(&out);
			}
		}
		// anything to receive?
		received = serializer.recv(&in);
		if (received)
		{
			doReceiveAction(&in);
		}
		// pulse one frame out
		serializer.sendOneFrame();
		vTaskDelay(pdMS_TO_TICKS(20));
	}
}
void CAN_ISR()
{
	// pulse one frame in
	serializer.recvOneFrame();
}