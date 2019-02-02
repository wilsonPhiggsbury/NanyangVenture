/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void logFuelCell(void *pvParameters)  // This is a Task.
{
	for (int i = 0; i < 50; i += 1)
	{
		Serial.print(i);
		Serial.print("ms->");
		Serial.print(pdMS_TO_TICKS(i));
		Serial.println("ticks");
	}
	Serial3.begin(19200);
	Serial3.setTimeout(50);

	TickType_t prevTick = xTaskGetTickCount(); // only needed when vTaskDelayUntil is called instead of vTaskDelay
	CANFrame fMain, fSub;
	DataForLogSend bufRaw(false, true);
	DataForLogSend buf(true, true);
	TickType_t delay = pdMS_TO_TICKS(500);
	while (1)
	{
		uint8_t bytesRead = Serial3.readBytesUntil('\n', bufRaw.data, 100);
		bufRaw.data[bytesRead] = '\0'; // this slot would be '\r' from the Fuel Cell, replace it with null terminator instead
		if (bytesRead)xQueueSend(queueForLogSend, &bufRaw, 100); // log this raw data
		dpFCMain.insertData(bufRaw.data);
		bufRaw.data[0] = '\0'; // clear this buffer to avoid inserting same data twice later (e.g. when serial port has no new data)

		dpFCMain.packString(buf.data);
		if(bytesRead)xQueueSend(queueForLogSend, &buf, 100); // log and send this refined data
		
		dpFCMain.packCAN(&fMain);
		//xQueueSend(queueForCAN, &fMain, 100);
		vTaskDelayUntil(&prevTick, delay); // more accurate compared to vTaskDelay, since the delay is shrunk according to execution time of this piece of code
	}
}
void logCurrentSensor(void* pvParameters)
{
	Adafruit_ADS1115 adc;
	adc.setGain(GAIN_SIXTEEN);
	adc.begin();

	CANFrame f;
	DataForLogSend buf(false, true);
	buf.logThis = false;

	TickType_t prevTick = xTaskGetTickCount(); // only needed when vTaskDelayUntil is called instead of vTaskDelay
	TickType_t delay = pdMS_TO_TICKS(100);
	uint8_t sendDelay = 0;
	while (1)
	{
		int volt = analogRead(CAP_OUT_VPIN);
		int amps = abs(adc.readADC_Differential_0_1());
		dpCS.insertData(volt, analogRead(CAP_IN_APIN), analogRead(CAP_OUT_APIN), amps);
		dpCSStats.insertData(volt, amps);
		sendDelay += 4; // overflows in 256/4 = 64 ticks
		if (sendDelay == 0) // procs once per 64 ticks (6400ms)
		{
			dpCSStats.packCAN(&f);
			//if(CAN_avail)xQueueSend(queueForCAN, &f, 100);
		}
		if (sendDelay % 16 == 0) // procs once per 4 ticks (400ms)
		{
			dpCS.packCAN(&f);
			dpCS.packString(buf.data);
			//if(CAN_avail)xQueueSend(queueForCAN, &f, 100);
			xQueueSend(queueForLogSend, &buf, 100);
		}
		vTaskDelayUntil(&prevTick, delay);
	}
}
/// <summary>
/// Output task to log and send data
/// </summary>
/// <param name="pvParameters"></param>
void outputToSdSerial(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	struct DataForLogSend buf(false, false);
	char path[8 + 4 + 1]; // +8 for filename, +4 for '.txt', +1 for '\0'
	TickType_t delay = pdMS_TO_TICKS(100); // delay 300 ms, shorter than reading/queueing tasks since this task has lower priority

	while (1)
	{
		BaseType_t success = xQueueReceive(queueForLogSend, &buf, 0);
		if (success == pdPASS)
		{
			if (buf.sendThis)
			{
				// print everything before we butcher the string with strtok() below!
				Serial.println(buf.data);
			}
			if (SD_avail && buf.logThis)
			{
				char* ptr;
				ptr = strtok(buf.data, "\t");
				// -------------- Store into SD -------------
				// Set path char array to the document we want to save to, determined by a const array
				strcpy(path, ptr);
				strcat(path, ".txt");
				ptr = strtok(NULL, "\0");
				// DO NOT SWITCH OUT THIS TASK IN THE MIDST OF WRITING A FILE ON SD CARD
				vTaskSuspendAll();
				File writtenFile = card.open(path, FILE_WRITE);
				writtenFile.println(ptr);
				writtenFile.close();
				xTaskResumeAll();
			}
		}

		vTaskDelay(delay);
	}
}
void ioForCAN(void *pvParameters)
{
	CANFrame pendingSend, pendingReceive;
	while (1)
	{
		// dispatch frames
		if (xQueueReceive(queueForCAN, &pendingSend, 0))
		{
			serializer.sendCanFrame(&pendingSend);
		}
		// receive frames (poll-based, infrequent frames expected)
		if (serializer.receiveCanFrame(&pendingReceive))
		{
			for (int i = 0; i < 1; i++)
			{
				dpReceiveList[i]->checkMatchCAN(&pendingReceive);
			}
		}
	}
}
void blinkRGB(void* pvParameters)
{
	bool lsigOn = false, rsigOn = false;
	// initialize light strips
	lightstrip.begin();
	brakestrip.begin();
	lightstrip.setBrightness(50);
	brakestrip.setBrightness(200);
	writeRGB(lightstrip, 7, RUNNING_LIGHT_COLOR);
	writeRGB(brakestrip, 7, BRAKE_LIGHT_COLOR);

	lstrip.begin();
	rstrip.begin();
	lstrip.setBrightness(50);
	rstrip.setBrightness(50);
	writeRGB(lstrip , 7, NO_LIGHT);
	writeRGB(rstrip, 7, NO_LIGHT);
	while (1)
	{
		if (dpStatus.getHazard() == STATE_EN || dpStatus.getLsig() == STATE_EN)
		{
			if (lsigOn)
			{
				lsigOn = false;
				writeRGB(lstrip, 7, NO_LIGHT);
			}
			else
			{
				lsigOn = true;
				writeRGB(lstrip, 7, SIGNAL_LIGHT_COLOR);
			}
		}
		else
		{
			lsigOn = false;
			writeRGB(lstrip, 7, NO_LIGHT);
		}

		if (dpStatus.getHazard() == STATE_EN || dpStatus.getRsig() == STATE_EN)
		{
			if (rsigOn)
			{
				rsigOn = false;
				writeRGB(rstrip, 7, NO_LIGHT);
			}
			else
			{
				rsigOn = true;
				writeRGB(rstrip, 7, SIGNAL_LIGHT_COLOR);
			}
		}
		else
		{
			rsigOn = false;
			writeRGB(rstrip, 7, NO_LIGHT);
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
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