#ifndef STEPPERPWM_H_
#define STEPPERPWM_H_

#include "PWM.h"

class StepperPWM
{
public:
	enum DIRECTIONS
	{
		BACKWARD,
		FORWARD
	};

public:
	StepperPWM(int DIR, int STEP, int ENBL);	//AS PWM: only 9, 10 tested and works on NANO
	StepperPWM(int DIR, int STEP, int ENBL, int MS1);
	StepperPWM(int DIR, int STEP, int ENBL, int MS1, int MS2);
	StepperPWM(int DIR, int STEP, int ENBL, int MS1, int MS2, int MS3);
	virtual ~StepperPWM(){}

	bool init();
	bool setFrequency(unsigned long freq);

	void run();
	void brake();
	void enable();	/// Use PIN 13 to have led ON when brakes are enabled
	void disable();

	//Custom classes
	void setSpeed(int speed);
	void setDirection(int DIR);

private:
	void initPins();
	int dirPin, stepPin, enablePin;
};

#endif /* STEPPERPWM_H_ */
