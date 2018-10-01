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
			//xQueueSend(queueForSendCAN, &outgoing, 0);
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
			xQueueSend(queueForSendCAN, &outgoing, 0);
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
			xQueueSend(queueForSendCAN, &outgoing, 100);
		}
		vTaskDelay(delay);
	}
}

void LogSendData(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	QueueItem received;
	TickType_t delay = pdMS_TO_TICKS(LOGSEND_INTERVAL); // delay 300 ms, shorter than reading/queueing tasks since this task has lower priority

	while (1)
	{
		BaseType_t success = xQueueReceive(queueForLogSend, &received, 100);
		if (success == pdPASS)
		{
			// Set path char array to the document we want to save to, determined by a const array
			strcpy(path + FILENAME_HEADER_LENGTH, dataPoint_shortNames[received.ID]);
			strcat(path, ".txt");

			//strncpy(shortFileName, CURRENTSENSOR_FILENAME, 2);
			//strcpy(path + FILENAME_HEADER_LENGTH, CURRENTSENSOR_FILENAME);

			// Make container for string representation of queueItem payload
			// 3 for short name, 9 for timestamp, remaining for all the floats and delimiters like \t " "
			char data[MAX_STRING_LEN];
			received.toString(data);
			// -------------- Store into SD -------------
			if (SD_avail)
			{
				// DO NOT SWITCH OUT THIS TASK IN THE MIDST OF WRITING A FILE ON SD CARD
				// (consequence: some serial payload may be missed. Hang or miss payload? Tough choice...)
				// ^^^^ this is probably fixed, if verified delete this line ^^^^
				vTaskSuspendAll();
				File writtenFile = SD.open(path, FILE_WRITE);
				writtenFile.println(data);
				writtenFile.close();
				xTaskResumeAll();
			}
			// *path should only remain as /LOG_****/
			// clean up after use
			strcpy(path + FILENAME_HEADER_LENGTH, "");

			// finally print out the payload to be transmitted by XBee
			//Serial.println(data);
		}

		vTaskDelay(delay);
	}
}
//void DisplayData(void *pvParameters)
//{
//	QueueItem received;
//	LiquidCrystal_I2C lcdScreen = LiquidCrystal_I2C(LCD1_I2C_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
//	DisplayLCD lcdManager = DisplayLCD(lcdScreen);
//
//	TickType_t delay = pdMS_TO_TICKS(DISPLAY_INTERVAL);
//
//	while (1)
//	{
//		BaseType_t success;
//		while (xQueueReceive(queueForDisplay, &received, 0) == pdPASS)
//		{
//			lcdManager.printData(received);
//		}
//		vTaskDelay(delay);
//	}
//}
void SendCANFrame(void *pvParameters __attribute__((unused)))  // This is a Task.
{
	QueueItem received;
	TickType_t delay = pdMS_TO_TICKS(CAN_FRAME_INTERVAL); // delay 150 ms, shorter than reading/queueing tasks since this task has lower priority
	
	while (1)
	{
		BaseType_t success = xQueueReceive(queueForSendCAN, &received, 0);
		//// -- test payload --
		//randomSeed(analogRead(A6));
		//received.ID = (DataSource)random(0, 3);
		//received.timeStamp = random();
		//for (int i = 0; i < 8; i++)
		//{
		//	received.data[0][i] = (i + 11) / 10.0;
		//}
		//for (int i = 0; i < 8; i++)
		//{
		//	received.data[1][i] = (i + 21) / 10.0;
		//}
		//for (int i = 0; i < 8; i++)
		//{
		//	received.data[2][i] = (i + 31) / 10.0;
		//}
		//// -- test payload --
		// we are going to encode 2 floats into 1 frame (2*4bytes = 8bytes)
		NV_CanFrames framesCollection;
		if (success == pdPASS)
		{
			// ------------------------  convert to frames --------------------
			received.toFrames(&framesCollection);

			for (uint8_t i = 0; i < framesCollection.getNumFrames(); i++)
			{
				NV_CanFrame& thisFrame = framesCollection.frames[i];
				byte status = CANObj.sendMsgBuf(thisFrame.id, 0, thisFrame.length, thisFrame.payload);

				if (status != CAN_OK)
				{
					// handle sending error
					Serial.println("FAIL SEND");
				}
				vTaskDelay(pdMS_TO_TICKS(5));
			}
			//// print raw frames
			//int i = 0;
			//do
			//{
			//	Serial.print("Frame ");
			//	Serial.print(i);
			//	Serial.print(": ");
			//	Serial.print(framesCollection.frames[i].id);
			//	Serial.print(" ");
			//	Serial.print(framesCollection.frames[i].length);
			//	Serial.print("\n");
			//	for (int j = 0;j<framesCollection.frames[i].length;j++)
			//		Serial.println(framesCollection.frames[i].payload[j], 16);
			//	i++;
			//} while ((framesCollection.frames[i-1].id & B11) != B11);

		}
		vTaskDelay(delay);
		
		//// ------------------------------ covert back -----------------------------------
		//QueueItem received2;
		//bool convertSucess = framesCollection.toQueueItem(&received);
		//// print converted results
		//if (!convertSucess)
		//{
		//	Serial.println("CONVERSION ERROR...");
		//	while (1);
		//}
		//char payload[3 + 9 + (FLOAT_TO_STRING_LEN + 1)*(QUEUEITEM_DATAPOINTS*QUEUEITEM_READVALUES) + QUEUEITEM_DATAPOINTS];
		//received.toString(payload);
		//Serial.println(payload);
			

			
			

	}
}