/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
void ReadFuelCell(void *pvParameters)  // This is a Task.
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
void ReadMotorPower(void* pvParameters)
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
	const uint16_t CAN_frame_interval = CAN_FRAME_INTERVAL / QUEUE_DATA_INTERVAL;
	const uint16_t speedo_refresh_interval = SPEEDOMETER_REFRESH_INTERVAL / QUEUE_DATA_INTERVAL;

	QueueItem outgoing;
	uint8_t syncCounter = 0;
	//HESFuelCell* masterCell = (HESFuelCell*)pvParameters;
	//HESFuelCell* slaveCell = ((HESFuelCell*)pvParameters) + 1;
	//AttopilotCurrentSensor* motor1 = ((Loggers*)pvParameters)->motors;
	//AttopilotCurrentSensor* motor2 = ((Loggers*)pvParameters)->motors + 1;
	//AttopilotCurrentSensor* motor3 = ((Loggers*)pvParameters)->motors + 2;
	TickType_t delay = pdMS_TO_TICKS(QUEUE_DATA_INTERVAL);
	BaseType_t success;

	while (1) // A Task shall never return or exit.
	{
		success = pdPASS;
		syncCounter++;
		if (syncCounter >= 128)
			syncCounter = 0;
		/* ------------------DATA FORMAT------------------
		FM								FS
		millis		V	A	W	Wh	T	P	V_c	St	V	A	W	Wh	T	P	V_c	St
		--------------------------------------------------*/

		if (syncCounter % fuelcell_logsend == 0)
		{
			// Arrange for outgoing fuel cell payload
			outgoing.ID = FC;
			HESFuelCell::dumpTimestampInto(&outgoing.timeStamp);
			for (int i = 0; i < NUM_FUELCELLS; i++)
			{
				if (hydroCells[i].hasUpdated())
				{
					hydroCells[i].dumpDataInto(outgoing.data);
				}
				/*else
				{
					strcat(outgoing.payload, "-\t-\t-\t-\t-\t-\t-\t-");
				}*/
			}
			xQueueSend(queueForLogSend, &outgoing, 100);
			if(CAN_avail)
				xQueueSend(queueForCAN, &outgoing, 100);
		}


		/* ------------------DATA FORMAT------------------
		Lwheel(L) -> Rwheel(R) -> Capacitor(c)

		millis		V_L		V_R		V_c		A_L		A_R		A_c		Ap_L*	Ap_R*	Ap_c*	Wh_L*	Wh_R*	Wh_c*
		*: only for display, not for logsend
		--------------------------------------------------*/
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
			if(CAN_avail)
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
		if (syncCounter % speedo_refresh_interval == 0)
		{
			outgoing.ID = SM;
			Speedometer::dumpTimestampInto(&outgoing.timeStamp);
			speedo.dumpDataInto(outgoing.data);
			xQueueSend(queueForLogSend, &outgoing, 100);
			if(CAN_avail)
				xQueueSend(queueForCAN, &outgoing, 100);
		}
		vTaskDelay(delay);
	}
}

void LogSendData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	QueueItem received;
	char data[MAX_STRING_LEN];
	TickType_t delay = pdMS_TO_TICKS(LOGSEND_INTERVAL); // delay 300 ms, shorter than reading/queueing tasks since this task has lower priority

	while (1)
	{
		BaseType_t success = xQueueReceive(queueForLogSend, &received, 100);
		if (success == pdPASS)
		{
			received.toString(data);

			// -------------- Store into SD -------------
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
	//pinMode(LSIG_PIN, OUTPUT);
	//pinMode(RSIG_PIN, OUTPUT);
	lstrip.begin();
	rstrip.begin();
	lstrip.setBrightness(64);
	rstrip.setBrightness(64);
	while (1)
	{
		for (int i = 0; i < 7; i++)
		{
			if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Lsig] == STATE_EN)
			{
				if (lsigOn)
				{
					lsigOn = false;
					lstrip.setPixelColor(i, 0, 0, 0);
					lstrip.show();
				}
				else
				{
					lsigOn = true;
					lstrip.setPixelColor(i, 255, 165, 0);
					lstrip.show();
				}
			}
			else
			{
				lsigOn = false;
				lstrip.setPixelColor(i, 0, 0, 0);
				lstrip.show();
			}

			if (peripheralStates[Hazard] == STATE_EN || peripheralStates[Rsig] == STATE_EN)
			{
				//Serial.println("RSIG ON");
				if (rsigOn)
				{
					rsigOn = false;
					rstrip.setPixelColor(i, 0, 0, 0);
					rstrip.show();
				}
				else
				{
					rsigOn = true;
					rstrip.setPixelColor(i, 255, 165, 0);
					rstrip.show();
				}
			}
			else
			{
				//Serial.println("RSIG OFF");
				rsigOn = false;
				rstrip.setPixelColor(i, 0, 0, 0);
				rstrip.show();
			}
		}
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
void doReceiveAction(QueueItem* q)
{
	// ************************* Code for signal lights ****************************
	// similar to TaskRespond in NV10_back_MEGA.ino, without really opening a new task
	// (since only the only thing to control is signal lights)
	if (q->ID == BT)
	{
		for (int i = 0; i < NUMSTATES; i++)
		{
			peripheralStates[i] = q->data[0][i];
		}
		xTaskAbortDelay(taskBlink);
	}
}