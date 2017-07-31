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
	StepperPWM(int DIR1, int DIR2, int STEP, int ENBL);	//AS PWM: only 9, 10 tested and works on NANO
	virtual ~StepperPWM(){}

	bool init();
	bool setFrequency(unsigned long freq);

	void run();
	void brake();
	void enable();	/// Use PIN 13 to have led ON when brakes are enabled
	void disable();

	//Custom classes
	void setSpeed(int speed);
	void set1Direction(int DIR);	//1 = front wheel
	void set2Direction(int DIR);	//2 = back wheel
private:
	void initPins();
	int dir1Pin, dir2Pin, stepPin, enablePin;
};

#endif /* STEPPERPWM_H_ */
