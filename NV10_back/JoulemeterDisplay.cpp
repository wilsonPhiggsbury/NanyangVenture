// 
// 
// 

#include "JoulemeterDisplay.h"
#include "Behaviour.h"
// Color definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF

DisplayLCD::DisplayLCD(LiquidCrystal_I2C& screenPtr):screen(screenPtr)
{
	screen.begin(20, 4);
	screen.backlight();
	screen.print("       V    A     Wh");
	screen.setCursor(0, 1);
	screen.print("L");
	screen.setCursor(0, 2);
	screen.print("R");
	screen.setCursor(0, 3);
	screen.print("c");
}
//void DisplayLCD::printData(QueueItem& received)
//{
//	char localStrCopy[max(FUELCELL_DATALEN, MOTOR_DATALEN)];
//	strcpy(localStrCopy, received.data);
//	// LCD data comes in as:
//	//	FC	0	-	-
//	//	FC	3af	V	A	W	Wh	Vcap	State	V	A	W	Wh	Vcap	State
//	//	MT	3af	V_L	V_R	V_c	A_L	A_R	A_c	Ap_L	Ap_R	Ap_c
//	// (V_L: voltage of left motor, , A_R: current of right motor, Ap_c: peak current of capacitor)
//	/* show on LCD only data with MT header:
//				V	Ap	Wh
//			L
//			R
//			c
//	*/
//
//	// navigate pointer to the start of actual data (skip the millis)
//	char* strPointer;
//	switch (received.ID)
//	{
//	case FC:
//
//		break;
//	case CS:
//		if (NUM_CURRENTSENSORS > 3)
//			debug(F("Not enough place to print motor data!"));
//		// print V
//		strPointer = strtok(localStrCopy, "\t");
//		for (int i = 0; i < NUM_CURRENTSENSORS; i++)
//		{
//			strPointer = strtok(NULL, "\t");
//			screen.setCursor(4, i + 1);
//			screen.print(strPointer);
//		}
//		// skip A
//		for (int i = 0; i < NUM_CURRENTSENSORS; i++)
//		{
//			strPointer = strtok(NULL, "\t");
//		}
//		// print Apeak
//		for (int i = 0; i < NUM_CURRENTSENSORS; i++)
//		{
//			strPointer = strtok(NULL, "\t");
//			screen.setCursor(9, i + 1);
//			screen.print(strPointer);
//		}
//		// print Wh
//		for (int i = 0; i < NUM_CURRENTSENSORS; i++)
//		{
//			strPointer = strtok(NULL, "\t");
//			screen.setCursor(14, i + 1);
//			screen.print(strPointer);
//		}
//		break;
//	}
//}