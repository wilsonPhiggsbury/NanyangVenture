/*
 Name:		NV10_ElectroClutchSync.ino
 Created:	6/24/2019 7:25:53 PM
 Author:	MX
*/
#include <Speedometer.h>
#include <RelayModule.h>

#define SPEEDO_WHEEL_INT 2
#define SPEEDO_GEAR_INT 3
#define TOGGLE_INPUT 8
#define CLUTCH_OUTPUT 9
Speedometer spWheel = Speedometer(SPEEDO_WHEEL_INT, 545, 2);
Speedometer spGear = Speedometer(SPEEDO_GEAR_INT, 545, 2);
RelayModule clutch = RelayModule(CLUTCH_OUTPUT, RelayModule::NO, RelayModule::activeHigh);
const float threshold = 5;
// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(SPEEDO_WHEEL_INT, INPUT_PULLUP);
	pinMode(SPEEDO_GEAR_INT, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(SPEEDO_WHEEL_INT), spWheel_ISR, FALLING);
	attachInterrupt(digitalPinToInterrupt(SPEEDO_GEAR_INT), spGear_ISR, FALLING);

	pinMode(TOGGLE_INPUT, INPUT_PULLUP);
	clutch.init();
}

// the loop function runs over and over again until power down or reset
void loop() {
	delay(2);
	float angleVelocityDiff = spWheel.getSpeedKmh() - spGear.getSpeedKmh();
	if (digitalRead(TOGGLE_INPUT) == LOW && (
		angleVelocityDiff > 0 && angleVelocityDiff < threshold ||
		angleVelocityDiff < 0 && angleVelocityDiff > -threshold
		)
	)
	{
		clutch.activate();
	}
	else if (digitalRead(TOGGLE_INPUT) == HIGH && clutch.activated())
	{
		clutch.deactivate();
	}
}
void spWheel_ISR()
{
	spWheel.trip();
}
void spGear_ISR()
{
	spGear.trip();
}