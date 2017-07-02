/*
 * DCMotor.h
 *
 *  Created on: Mar 17, 2017
 *      Author: Alex
 */

#ifndef DCMOTOR_H_
#define DCMOTOR_H_

#include <Arduino.h>

//A typedef to set direction easily
typedef enum
{
	DIRECTION_BACKWARD,
	DIRECTION_FORWARD
}direction;

class DCMotor
{
public:
	//Class contructor
	DCMotor(const int motorSpeedPin, const int motorDirectionPin, bool forwardOn);

	//Class destructor
	~DCMotor();

	//Class methods
	void setSpeed(int); //Speed interval: 0-255
	void setDirection(direction);

	void brake();
	void run();

	direction motorDirection = direction::DIRECTION_FORWARD; // Initializing direction to forward
	int speed = 0;

private:
	//Some private class variables
	int speedPin, directionPin;

	//Initialize direction and speed. Also a default value is set.

	//Example:
	//Motor1: To move FORWARD, apply HIGH to direction pin;
	//Motor2: To move FORWARD, apply LOW to direction pin;
	//We have to make all the stuff the same. So, all motors should move forward
	//on MOTOR_FORWARD, or 1 logic.
	bool forwardOn = 1;
};

/*
class DCShieldMotor
{
public:
	//Class contructor
	DCShieldMotor(int motorID);

	//Class destructor
	~DCShieldMotor();

	//Class methods
	void setSpeed(int); //Speed interval: -100 to 100. 0 meanr brake;
	void setDirection(direction);

	void brake();
	void setup();
	void run();

	direction motorDirection = direction::DIRECTION_FORWARD; // Initializing direction to forward

	MotorDriver shieldMotor;

private:
	//Instance of ShieldMotorDriver

	//Initialize direction and speed. Also a default value is set.
	int speed = 0;
	int motorID = 0;

};

namespace shield
{
	void motor_forward(int motorID, int speed);
	void motor_backward(int motorID, int speed);
}

*/

#endif /* DCMOTOR_H_ */
