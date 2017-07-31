#include <Arduino.h>
#include "StepperPWM.h"

StepperPWM::StepperPWM(int DIR, int STEP, int ENBL):
			dirPin(DIR), stepPin(STEP), enablePin(ENBL)
{
	this->initPins();
}

StepperPWM::StepperPWM(int DIR, int STEP, int ENBL, int MS1):
			dirPin(DIR), stepPin(STEP), enablePin(ENBL)
{
	this->initPins();
	pinMode(MS1, OUTPUT);
	digitalWrite(MS1, HIGH);
}

StepperPWM::StepperPWM(int DIR, int STEP, int ENBL, int MS1, int MS2):
			dirPin(DIR), stepPin(STEP), enablePin(ENBL)
{
	this->initPins();
	pinMode(MS1, OUTPUT);
	pinMode(MS2, OUTPUT);
	digitalWrite(MS1, HIGH);
	digitalWrite(MS2, HIGH);
}

StepperPWM::StepperPWM(int DIR, int STEP, int ENBL, int MS1, int MS2, int MS3):
			dirPin(DIR), stepPin(STEP), enablePin(ENBL)
{
	this->initPins();
	pinMode(MS1, OUTPUT);
	pinMode(MS2, OUTPUT);
	pinMode(MS3, OUTPUT);
	digitalWrite(MS1, HIGH);
	digitalWrite(MS2, HIGH);
	digitalWrite(MS3, HIGH);
}

void StepperPWM::initPins()
{
	pinMode(dirPin, OUTPUT);
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
