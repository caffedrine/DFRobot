#include <Arduino.h>
#include "StepperPWM.h"

StepperPWM::StepperPWM(int DIR1, int DIR2, int STEP, int ENBL) :
		dir1Pin(DIR1), dir2Pin(DIR2), stepPin(STEP), enablePin(ENBL)
{
	this->initPins();
}

void StepperPWM::initPins()
{
	pinMode(dir1Pin, OUTPUT);
	pinMode(dir2Pin, OUTPUT);
	pinMode(stepPin, OUTPUT);
	pinMode(enablePin, OUTPUT);
}

bool StepperPWM::init()
{
	//initialize all timers except for 0, to save time keeping functions
	InitTimersSafe();

	//Setting up a default frequency
	bool success = SetPinFrequencySafe(stepPin, 2000); //9, 10 tested and works on NANO

	//And brake motor
	brake();

	return success;
}

void StepperPWM::brake()
{
	this->disable();
}

void StepperPWM::enable()
{
	digitalWrite(enablePin, LOW);
}

void StepperPWM::disable()
{
	digitalWrite(enablePin, HIGH);
}

bool StepperPWM::setFrequency(unsigned long freq)
{
	return SetPinFrequencySafe(stepPin, freq);
}

void StepperPWM::run()
{
	pwmWrite(stepPin, 1);
}

bool StepperPWM::setSpeed(int speed)
{
	// Convert speed to freq or send direct - we receive from 0 to 255
	int freq = map(speed, 0, 255, 0, 4096);
	bool result = this->setFrequency(freq);

	if (speed == 0)
		this->brake();
	else
		this->enable();

	return result;
}

void StepperPWM::set1Direction(int direction)
{
	if (direction == this->DIRECTIONS::FORWARD)
		digitalWrite(this->dir1Pin, HIGH);
	else if (direction == this->DIRECTIONS::BACKWARD)
		digitalWrite(this->dir1Pin, LOW);
}

void StepperPWM::set2Direction(int direction)
{
	if (direction == this->DIRECTIONS::FORWARD)
		digitalWrite(this->dir2Pin, HIGH);
	else if (direction == this->DIRECTIONS::BACKWARD)
		digitalWrite(this->dir2Pin, LOW);
}
