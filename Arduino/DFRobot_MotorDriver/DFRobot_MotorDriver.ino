/*

 Mimics the fade example but with an extra parameter for frequency. It should dim but with a flicker
 because the frequency has been set low enough for the human eye to detect. This flicker is easiest to see when
 the LED is moving with respect to the eye and when it is between about 20% - 60% brighness. The library
 allows for a frequency range from 1Hz - 2MHz on 16 bit timers and 31Hz - 2 MHz on 8 bit timers. When
 SetPinFrequency()/SetPinFrequencySafe() is called, a bool is returned which can be tested to verify the
 frequency was actually changed.

 This example runs on mega and uno.
 */

#include <Arduino.h>
#include "DCMotor.h"
#include "PWM.h"
#include "my_util.h"

//use pin 11 on the Mega instead, otherwise there is a frequency cap at 31 Hz
int motorPin = 3;
int speed = 127;
int32_t frequency = 5000; 		//frequency (in Hz) - 5000

void setup()
{
	//Set pin as output
	pinMode(motorPin, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(12, OUTPUT);

	//initialize all timers except for 0, to save time keeping functions
	InitTimersSafe();
	Serial.begin(115200);

	//sets the frequency for the specified pin
	bool success = SetPinFrequencySafe(motorPin, frequency);

	//if the pin frequency was set successfully
	if (success)
	{
		Serial.println("SUCCCESS");
	}
	else
	{
		Serial.println("FAILED");
	}
}

void loop()
{
	//use this functions instead of analogWrite on 'initialized' pins
	pwmWrite(motorPin, speed);

	//Update motors speeds from serial
	if(Serial.available())
		speed = to_int(Serial.readString());

	//Print speed every 1 seconds
	printPeriodicData("Speed: " + to_string(speed), 1000);
}
