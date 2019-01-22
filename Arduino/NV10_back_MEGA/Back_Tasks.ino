/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void TaskLogFuelCell(void *pvParameters)  // This is a Task.
{
	// Obtain fuel cell object references from parameter passed in
	HESFuelCell* fuelCell = (HESFuelCell*)pvParameters;

	TickType_t prevTick = xTaskGetTickCount(); // only needed when vTaskDelayUntil is called instead of vTaskDelay
	TickType_t delay = pdMS_TO_TICKS(READ_FC_INTERVAL);
	while (1)
	{
		for (int i = 0; i < NUM_FUELCELLS; i++)
		{
			(fuelCell + i)->logData();
		}
		vTaskDelayUntil(&prevTick, delay); // more accurate compared to vTaskDelay, since the delay is shrunk according to execution time of this piece of code
	}
}
void TaskLogCurrentSensor(void* pvParameters)
{
	// Obtain current sensor object references from parameter passed in
	AttopilotCurrentSensor* sensor = (AttopilotCurrentSensor*)pvParameters;

	TickType_t prevTick = xTaskGetTickCount(); // only needed when vTaskDelayUntil is called instead of vTaskDelay
	TickType_t delay = pdMS_TO_TICKS(READ_MT_INTERVAL);
	while (1)
	{
		for (int i = 0; i < NUM_CURRENTSENSORS; i++)
		{
			(sensor + i)->logData();
		}
		vTaskDelayUntil(&prevTick, delay);
	}
}
void QueueOutputData(void *pvParameters)
{
	const uint16_t fuelcell_logsend = FUELCELL_LOGSEND_INTERVAL / QUEUE_DATA_INTERVAL;
	const uint16_t motor_logsend = MOTOR_LOGSEND_INTERVAL / QUEUE_DATA_INTERVAL;
	const uint16_t speedo_refresh_interval = SPEEDOMETER_REFRESH_INTERVAL / QUEUE_DATA_INTERVAL;

	Packet outgoing;
	uint8_t syncCounter = 0;
	auto delay = pdMS_TO_TICKS(QUEUE_DATA_INTERVAL);

	while (1) // A Task shall never return or exit.
	{
		if (++syncCounter >= 128)
			syncCounter = 0;
		// Send one outgoing fuel cell (FC) frame every x ms
		if (syncCounter % fuelcell_logsend == 0)
		{
			// Arrange for outgoing fuel cell payload
			outgoing.ID = FC;
			// todo: move dumptimestamp func into packet class
			HESFuelCell::dumpTimestampInto(&outgoing.timeStamp);
			for (auto i = 0; i < NUM_FUELCELLS; i++)
			{
				if (hydroCells[i].hasUpdated())
				{
					hydroCells[i].dumpDataInto(outgoing.data);
				}
			}
			/* ------------------DATA FORMAT------------------
			FM								FS
			millis		V	A	W	Wh	T	P	V_c	St	V	A	W	Wh	T	P	V_c	St
			--------------------------------------------------*/
			xQueueSend(queueForLogSend, &outgoing, 100);
			if (CAN_avail)
				xQueueSend(queueForCAN, &outgoing, 100);
		}


		/* ------------------DATA FORMAT------------------
		Lwheel(L) -> Rwheel(R) -> Capacitor(c)

		millis		V_L		V_R		V_c		A_L		A_R		A_c		Ap_L*	Ap_R*	Ap_c*	Wh_L*	Wh_R*	Wh_c*
		*: only for display, not for logsend
		--------------------------------------------------*/
		// send one outgoing current sensor (CS) every x ms
		if (syncCounter % motor_logsend == 0)
		{
			// Arrange for outgoing current sensor payload
			outgoing.ID = CS;
			AttopilotCurrentSensor::dumpTimestampInto(&outgoing.timeStamp);
			for (int i = 0; i < NUM_CURRENTSENSORS; i++)
			{
				motors[i].dumpDataInto(outgoing.data);//len 5
			}
			xQueueSend(queueForLogSend, &outgoing, 100);
			if (CAN_avail)
				xQueueSend(queueForCAN, &outgoing, 100);
			//if (syncCounter % (back_lcd_refresh) == 0)
			//{
			//	for (int i = 0; i < NUM_CURRENTSENSORS; i++)
			//	{
			//		strcat(outgoing.payload, "\t");
			//		motors[i].dumpAmpPeakInto(outgoing.payload);//len 5
			//	}
			//	for (int i = 0; i < NUM_CURRENTSENSORS; i++)
			//	{
			//		strcat(outgoing.payload, "\t");
			//		motors[i].dumpTotalEnergyInto(outgoing.payload);//len 7
			//	}
			//}
		}
		// send one outgoing speedometer (SM) every x ms
		if (syncCounter % speedo_refresh_interval == 0)
		{
			outgoing.ID = SM;
			Speedometer::dumpTimestampInto(&outgoing.timeStamp);
			speedo.dumpDataInto(outgoing.data);
			xQueueSend(queueForLogSend, &outgoing, 100);
			if (CAN_avail)
				xQueueSend(queueForCAN, &outgoing, 100);
		}
		vTaskDelay(delay);
	}
}

/// <summary>
/// Output task to log and send data
/// </summary>
/// <param name="pvParameters"></param>
void LogSendData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	Packet received;
	char data[MAX_STRING_LEN];
	TickType_t delay = pdMS_TO_TICKS(LOGSEND_INTERVAL); // delay 300 ms, shorter than reading/queueing tasks since this task has lower priority

	while (1)
	{
		BaseType_t success = xQueueReceive(queueForLogSend, &received, 100);
		if (success == pdPASS)
		{
			received.toString(data);

			// -------------- Store into SD -------------
			// log
			if (SD_avail)
			{
				// Set path char array to the document we want to save to, determined by a const array
				strcpy(path + FILENAME_HEADER_LENGTH, frameType_shortNames[received.ID]);
				strcat(path, ".txt");
				// DO NOT SWITCH OUT THIS TASK IN THE MIDST OF WRITING A FILE ON SD CARD
				vTaskSuspendAll();
				File writtenFile = SD.open(path, FILE_WRITE);
				writtenFile.println(data);
				writtenFile.close();
				xTaskResumeAll();
				// *path should only remain as /LOG_****/, clean up after use
				strcpy(path + FILENAME_HEADER_LENGTH, "");
			}
			// finally print out the payload to be transmitted by XBee
			Serial.println(data);
		}

		vTaskDelay(delay);
	}
}

void TaskBlink(void* pvParameters)
{
	bool lsigOn = false, rsigOn = false;
	// initialize light strips
	lightstrip.begin();
	lightstrip.setBrightness(50);
	for (int i = 0; i < 7; i++)
		lightstrip.setPixelColor(i, 255, 255, 255);
	lightstrip.show();

	lstrip.begin();
	rstrip.begin();
	lstrip.setBrightness(50);
	rstrip.setBrightness(50); pinMode(LED_BUILTIN, OUTPUT);
	while (1)
	{
		//if (peripheralStates[Headlights] == STATE_EN)debug(F("Headlights ON"));
		//else debug(F("Headlights OFF"));
		//if (peripheralStates[Rsig] == STATE_EN)debug(F("Rsig ON"));
		//else debug(F("Rsig OFF"));
		//debug(F("--------"));
		if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Lsig] == STATE_EN)
		{
			if (lsigOn)
			{
				lsigOn = false;
				for (int i = 0; i < 7; i++)
				{
					lstrip.setPixelColor(i, 0, 0, 0);
					lstrip.show(); 
					digitalWrite(LED_BUILTIN, LOW);
				}
			}
			else
			{
				lsigOn = true;
				for (int i = 0; i < 7; i++)
				{
					lstrip.setPixelColor(i, 255, 165, 0);
					lstrip.show();
					digitalWrite(LED_BUILTIN, HIGH);
				}
			}
		}
		else
		{
			lsigOn = false;
			for (int i = 0; i < 7; i++)
			{
				lstrip.setPixelColor(i, 0, 0, 0);
				lstrip.show();
				digitalWrite(LED_BUILTIN, LOW);
			}
		}

		if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Rsig] == STATE_EN)
		{
			if (rsigOn)
			{
				rsigOn = false;
				for (int i = 0; i < 7; i++)
				{
					rstrip.setPixelColor(i, 0, 0, 0);
					rstrip.show();
				}
			}
			else
			{
				rsigOn = true;
				for (int i = 0; i < 7; i++)
				{
					rstrip.setPixelColor(i, 255, 165, 0);
					rstrip.show();
				}
			}
		}
		else
		{
			rsigOn = false;
			for (int i = 0; i < 7; i++)
			{
				rstrip.setPixelColor(i, 0, 0, 0);
				rstrip.show();
			}
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
void doReceiveAction(Packet* q)
{
	// ************************* Code for signal lights ****************************
	// kick up the blinking task and tell it to start or stop rightaway
	if (q->ID == BT)
	{
		for (int i = 0; i < NUMSTATES; i++)
			peripheralStates[i] = q->data[0][i];
		xTaskAbortDelay(taskBlink);
	}
}