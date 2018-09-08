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
	const uint16_t back_lcd_refresh = BACK_LCD_REFRESH_INTERVAL / QUEUE_DATA_INTERVAL;

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
			// Arrange for outgoing fuel cell data
			outgoing.ID = FC;
			outgoing.data[0] = '\0';
			HESFuelCell::dumpTimestampInto(outgoing.data);
			for (int i = 0; i < NUM_FUELCELLS; i++)
			{
				strcat(outgoing.data, "\t");
				if (hydroCells[i].hasUpdated())
				{
					hydroCells[i].dumpDataInto(outgoing.data);
				}
				else
				{
					strcat(outgoing.data, "-\t-\t-\t-\t-\t-\t-\t-");
				}
			}
			xQueueSend(queueForLogSend, &outgoing, 100);
			xQueueSend(queueForDisplay, &outgoing, 100);
		}


		/* ------------------DATA FORMAT------------------
		Lwheel(L) -> Rwheel(R) -> Capacitor(c)

		millis		V_L		V_R		V_c		A_L		A_R		A_c		Ap_L*	Ap_R*	Ap_c*	Wh_L*	Wh_R*	Wh_c*
		*: only for display, not for logsend
		--------------------------------------------------*/
		if (syncCounter % motor_logsend == 0)
		{
			// Arrange for outgoing current sensor data
			outgoing.ID = CS;
			outgoing.data[0] = '\0';
			motors[0].dumpTimestampInto(outgoing.data);
			for (int i = 0; i < NUM_CURRENTSENSORS; i++)
			{
				motors[i].dumpDataInto(outgoing.data);//len 5
			}
			xQueueSend(queueForLogSend, &outgoing, 100);
			xQueueSend(queueForDisplay, &outgoing, 100);
			xQueueSend(queueForSendCAN, &outgoing, 100);
			//if (syncCounter % (back_lcd_refresh) == 0)
			//{
			//	for (int i = 0; i < NUM_CURRENTSENSORS; i++)
			//	{
			//		strcat(outgoing.data, "\t");
			//		motors[i].dumpAmpPeakInto(outgoing.data);//len 5
			//	}
			//	for (int i = 0; i < NUM_CURRENTSENSORS; i++)
			//	{
			//		strcat(outgoing.data, "\t");
			//		motors[i].dumpTotalEnergyInto(outgoing.data);//len 7
			//	}
			//}
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
		BaseType_t success = xQueueReceive(queueForLogSend, &received, 0);
		char shortFileName[3] = "";
		if (success == pdPASS)
		{
			switch (received.ID)
			{
			case FC:
				strncpy(shortFileName, FUELCELL_FILENAME, 2);
				strcpy(path + FILENAME_HEADER_LENGTH, FUELCELL_FILENAME);
				break;
			case CS:
				strncpy(shortFileName, MOTOR_FILENAME, 2);
				strcpy(path + FILENAME_HEADER_LENGTH, MOTOR_FILENAME);
				break;
			}
			Serial.print(shortFileName);
			Serial.print('\t');
			Serial.println(received.data);
			// -------------- Store into SD -------------
			if (SD_avail)
			{
				File writtenFile = SD.open(path, FILE_WRITE);
				writtenFile.println(received.data);
				writtenFile.close();
			}
			// *path should only remain as /LOG_****/. Clean up after use
			strcpy(path + FILENAME_HEADER_LENGTH, "");
		}

		vTaskDelay(delay);  // poll more frequently since more data comes in at a time
	}
}
void DisplayData(void *pvParameters)
{
	QueueItem received;
	LiquidCrystal_I2C lcdScreen = LiquidCrystal_I2C(LCD1_I2C_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
	DisplayLCD lcdManager = DisplayLCD(lcdScreen);

	TickType_t delay = pdMS_TO_TICKS(DISPLAY_INTERVAL);

	while (1)
	{
		BaseType_t success;
		while (xQueueReceive(queueForDisplay, &received, 0) == pdPASS)
		{
			lcdManager.printData(received);
		}
		vTaskDelay(delay);
	}
}
//void SendCANFrame(void *pvParameters __attribute__((unused)))  // This is a Task.
//{
//	QueueItem received;
//	TickType_t delay = pdMS_TO_TICKS(CAN_FRAME_INTERVAL); // delay 300 ms, shorter than reading/queueing tasks since this task has lower priority
//	if (CANObj.begin(MCP_ANY, CAN_1000KBPS, MCP_16MHZ) != CAN_OK)
//	{
//		Serial.println(F("NV10_back CAN init fail!"));
//		vTaskSuspend(NULL);
//	}
//	else
//	{
//		Serial.println(F("NV10_back CAN init success!"));
//	}
//
//	while (1)
//	{
//		BaseType_t success = xQueueReceive(queueForSendCAN, &received, 0);
//		if (success == pdPASS)
//		{
//			unsigned long id = NV_CAN_BACK;
//			switch (received.ID)
//			{
//			case FC:
//				id &= 0xFFD; // clear 2nd bit
//				break;
//			case CS:
//				id |= 0x002; // set 2nd bit
//				break;
//			}
//			char* curSeg = received.data;
//			byte curSegBuffer[8];
//			byte curSegLen = strcspn(received.data, "\t"); // skip the timestamp
//			curSeg += curSegLen + 1; // advance to next segment
//			////////////char* partial = strtok(received.data, "\t");
//			while (curSegLen != 0)
//			{
//				////////////partial = strtok(NULL, "\t");
//				curSegLen = strcspn(received.data, "\t");
//				////////////byte len = strlen(partial);
//				// copy contents from curSeg into curSegBuffer, only for type conversion compability with CANObj.setMsgBuf()
//				memcpy(curSegBuffer, curSeg, curSegLen); // does not copy '\0'
//				byte status = CANObj.sendMsgBuf(id, curSegLen, curSegBuffer);
//				// advance to next segment
//				Serial.print("Sent ID:");
//				Serial.println(id);
//				Serial.print("Content:\n");
//				for (int i = 0; i < curSegLen; i++)
//				{
//					Serial.print((int)curSegBuffer[i]);
//					Serial.print(" ");
//				}
//				Serial.println();
//				curSeg += curSegLen + 1;
//				if (status != CAN_OK)
//					break;
//
//
//			}
//			// set last bit for final frame
//			CANObj.sendMsgBuf(id | 0x001, 0, curSegBuffer); // WARNING, content is random
//			Serial.print("Sent last frame");
//		}
//		vTaskDelay(pdMS_TO_TICKS(150));
//	}
//}