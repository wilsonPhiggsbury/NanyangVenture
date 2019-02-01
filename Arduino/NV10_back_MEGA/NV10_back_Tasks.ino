/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void logFuelCell(void *pvParameters)  // This is a Task.
{
	TickType_t prevTick = xTaskGetTickCount(); // only needed when vTaskDelayUntil is called instead of vTaskDelay
	NV10FuelCell fc, fc2;
	char buf[] = ">>62.5V 00.0A 0112W 00000Wh 019.6C 026.0C 032.8C 025.0C 0.79B 52.2V 029.4C IN 00.0C 00 0000 ";
	char buf2[] = ">>62.5V 00.0A 1144W 00000Wh 019.6C 056.0C 032.8C 025.0C 1.00B 52.2V 029.4C SD 00.0C 00 0000 ";
	fc.insertData(buf);
	fc2.insertData(buf2);
	buf[0] = buf2[0] = 0; // clear string buffer
	char s[40], s2[40];
	TickType_t delay = pdMS_TO_TICKS(READ_FC_INTERVAL);
	while (1)
	{
		// pack string
		fc.packString(s);
		fc2.packString(s2);

		fc2.unpackString(s);
		fc.unpackString(s2);
		Serial.println("FC1");
		Serial.print(" Watts: "); Serial.print(fc.getWatts());
		Serial.print(" Pressure: "); Serial.print(fc.getPressure());
		Serial.print(" Temperature: "); Serial.print(fc.getTemperature());
		Serial.print(" Status: "); Serial.print(fc.getStatus());
		Serial.println();

		Serial.println("FC2");
		Serial.print(" Watts: "); Serial.print(fc2.getWatts());
		Serial.print(" Pressure: "); Serial.print(fc2.getPressure());
		Serial.print(" Temperature: "); Serial.print(fc2.getTemperature());
		Serial.print(" Status: "); Serial.print(fc2.getStatus());
		Serial.println();
		// pack can
		vTaskDelayUntil(&prevTick, delay); // more accurate compared to vTaskDelay, since the delay is shrunk according to execution time of this piece of code
	}
}
void logCurrentSensor(void* pvParameters)
{

	TickType_t prevTick = xTaskGetTickCount(); // only needed when vTaskDelayUntil is called instead of vTaskDelay
	TickType_t delay = pdMS_TO_TICKS(READ_MT_INTERVAL);
	while (1)
	{

		vTaskDelayUntil(&prevTick, delay);
	}
}
void outputToSd(void *pvParameters)
{

	while (1) // A Task shall never return or exit.
	{
		
		vTaskDelay(1);
	}
}

/// <summary>
/// Output task to log and send data
/// </summary>
/// <param name="pvParameters"></param>
//void outputToSerial(void *pvParameters __attribute__((unused)))  // This is a Task.
//{
//	Packet received;
//	char data[MAX_STRING_LEN];
//	TickType_t delay = pdMS_TO_TICKS(LOGSEND_INTERVAL); // delay 300 ms, shorter than reading/queueing tasks since this task has lower priority
//
//	while (1)
//	{
//		BaseType_t success = xQueueReceive(queueForLogSend, &received, 100);
//		if (success == pdPASS)
//		{
//			received.toString(data);
//
//			// -------------- Store into SD -------------
//			if (SD_avail)
//			{
//				// Set path char array to the document we want to save to, determined by a const array
//				strcpy(path, STRING_HEADER[received.ID]);
//				strcat(path, ".txt");
//				// DO NOT SWITCH OUT THIS TASK IN THE MIDST OF WRITING A FILE ON SD CARD
//				vTaskSuspendAll();
//				File writtenFile = card.open(path, FILE_WRITE);
//				writtenFile.println(data);
//				writtenFile.close();
//				xTaskResumeAll();
//			}
//			// finally print out the payload to be transmitted by XBee
//			Serial.println(data);
//		}
//
//		vTaskDelay(delay);
//	}
//}
//
//void blinkRGB(void* pvParameters)
//{
//	bool lsigOn = false, rsigOn = false;
//	// initialize light strips
//	lightstrip.begin();
//	brakestrip.begin();
//	lightstrip.setBrightness(50);
//	brakestrip.setBrightness(200);
//	for (int i = 0; i < 7; i++)
//	{
//		lightstrip.setPixelColor(i, 255, 255, 255);
//		brakestrip.setPixelColor(i, 0, 0, 0);
//	}
//	lightstrip.show();
//	brakestrip.show();
//
//	lstrip.begin();
//	rstrip.begin();
//	lstrip.setBrightness(50);
//	rstrip.setBrightness(50);
//	while (1)
//	{
//		if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Lsig] == STATE_EN)
//		{
//			if (lsigOn)
//			{
//				lsigOn = false;
//				for (int i = 0; i < 7; i++)
//				{
//					lstrip.setPixelColor(i, 0, 0, 0);
//					lstrip.show(); 
//				}
//			}
//			else
//			{
//				lsigOn = true;
//				for (int i = 0; i < 7; i++)
//				{
//					lstrip.setPixelColor(i, 255, 165, 0);
//					lstrip.show();
//				}
//			}
//		}
//		else
//		{
//			lsigOn = false;
//			for (int i = 0; i < 7; i++)
//			{
//				lstrip.setPixelColor(i, 0, 0, 0);
//				lstrip.show();
//			}
//		}
//
//		if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Rsig] == STATE_EN)
//		{
//			if (rsigOn)
//			{
//				rsigOn = false;
//				for (int i = 0; i < 7; i++)
//				{
//					rstrip.setPixelColor(i, 0, 0, 0);
//					rstrip.show();
//				}
//			}
//			else
//			{
//				rsigOn = true;
//				for (int i = 0; i < 7; i++)
//				{
//					rstrip.setPixelColor(i, 255, 165, 0);
//					rstrip.show();
//				}
//			}
//		}
//		else
//		{
//			rsigOn = false;
//			for (int i = 0; i < 7; i++)
//			{
//				rstrip.setPixelColor(i, 0, 0, 0);
//				rstrip.show();
//			}
//		}
//		vTaskDelay(pdMS_TO_TICKS(500));
//	}
//}
//void doReceiveAction(Packet* q)
//{
//	// ************************* Code for signal lights ****************************
//	// kick up the blinking task and tell it to start or stop rightaway
//	if (q->ID == BT)
//	{
//		for (int i = 0; i < NUMSTATES; i++)
//			peripheralStates[i] = q->data[0][i];
//		xTaskAbortDelay(taskBlink);
//	}
//	else if (q->ID == BK)
//	{
//		if (q->data[0][0] == STATE_EN)
//		{
//			for (int i = 0; i < 7; i++)
//			{
//				brakestrip.setPixelColor(i, 255, 0, 0);
//				lightstrip.setPixelColor(i, 255, 0, 0);
//			}
//		}
//		else if (q->data[0][0] == STATE_DS)
//		{
//			for (int i = 0; i < 7; i++)
//			{
//				brakestrip.setPixelColor(i, 0, 0, 0);
//				lightstrip.setPixelColor(i, 255, 255, 255);
//			}
//		}
//		brakestrip.show();
//		lightstrip.show();
//	}
//}