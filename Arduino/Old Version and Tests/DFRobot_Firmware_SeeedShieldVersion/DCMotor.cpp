/*
 * DCMotor.cpp
 *
 *  Created on: Mar 17, 2017
 *      Author: Alex
 */

#include "DCMotor.h"
/*
  ____   ____ __  __       _                 ____ _
 |  _ \ / ___|  \/  | ___ | |_ ___  _ __    / ___| | __ _ ___ ___
 | | | | |   | |\/| |/ _ \| __/ _ \| '__|  | |   | |/ _` / __/ __|
 | |_| | |___| |  | | (_) | || (_) | |     | |___| | (_| \__ \__ \
 |____/ \____|_|  |_|\___/ \__\___/|_|      \____|_|\__,_|___/___/
*/
// Constructor and deconstructor functions.
//
DCMotor::DCMotor(const int motorSpeedPin, const int motorDirectionPin, bool fwdOn)
{
	DCMotor::speedPin     = motorSpeedPin;
	DCMotor::directionPin = motorDirectionPin;
	DCMotor::forwardOn    = fwdOn;

	//Setting up pins mode
	pinMode(motorSpeedPin, OUTPUT);
	pinMode(motorDirectionPin, OUTPUT);
}

DCMotor::~DCMotor()
{

}

//
// Implementing functions
//

//Updating speed variable
void DCMotor::setSpeed(int spd)
{
	DCMotor::speed = spd;
}

void DCMotor::setDirection(direction dir)
{
	DCMotor::motorDirection = dir;
}

void DCMotor::brake()
{
	setSpeed(0);
	analogWrite(speedPin, 0);
}

void DCMotor::run()
{
	//In order to run motor we have to:
	// 1. Send a digital signal indicating direction;
	// 2. Send analogic signal indicating speed;

	//Writing direction to direction PIN
	digitalWrite(directionPin, (forwardOn ? DCMotor::motorDirection : !DCMotor::motorDirection));

	//Writing speed to speed PIN;
	analogWrite(speedPin, DCMotor::speed);
}

/*
  ____   ____ ____  _     _      _     _ __  __       _                 ____ _
 |  _ \ / ___/ ___|| |__ (_) ___| | __| |  \/  | ___ | |_ ___  _ __    / ___| | __ _ ___ ___
 | | | | |   \___ \| '_ \| |/ _ \ |/ _` | |\/| |/ _ \| __/ _ \| '__|  | |   | |/ _` / __/ __|
 | |_| | |___ ___) | | | | |  __/ | (_| | |  | | (_) | || (_) | |     | |___| | (_| \__ \__ \
 |____/ \____|____/|_| |_|_|\___|_|\__,_|_|  |_|\___/ \__\___/|_|      \____|_|\__,_|___/___/


//
//Constructors and destructors
//

// !!!

DCShieldMotor::DCShieldMotor(int motorid)
{
	DCShieldMotor::motorID = motorid;
	shieldMotor.begin();
}

DCShieldMotor::~DCShieldMotor()
{

}

void DCShieldMotor::setDirection(direction dir)
{
	DCShieldMotor::motorDirection = dir;
}

void DCShieldMotor::setSpeed(int spd)
{
	DCShieldMotor::speed = map(spd, 0, 255, 70, 100);
}

void DCShieldMotor::brake()
{
	shieldMotor.brake(motorID);

	shieldMotor.stop(0);
	shieldMotor.stop(1);

	digitalWrite(9, LOW);
	digitalWrite(10, LOW);

}

void DCShieldMotor::run()
{
	//if(DCShieldMotor::motorDirection == DIRECTION_BACKWARD)
		//DCShieldMotor::speed *= -1;

	if(motorDirection == DIRECTION_FORWARD)
		shield::motor_forward(DCShieldMotor::motorID, DCShieldMotor::speed);
	else
		shield::motor_backward(DCShieldMotor::motorID, DCShieldMotor::speed);
}

namespace shield
{
	void motor_backward(int motorID, int speed)
	{
		speed = map(speed, 0, 255, 0, 100) *-1;

		if( speed == 0 )
		{
			shieldMotor.brake(motorID);

			shieldMotor.stop(motorID);

			digitalWrite(9 + motorID, LOW);
		}
		else
		{
			digitalWrite(9 + motorID, HIGH);

			shieldMotor.speed(motorID, speed);
		}
	}

	void motor_forward(int motorID, int speed)
	{
		speed = map(speed, 0, 255, 0, 100);

		if( speed == 0 )
		{
			shieldMotor.brake(motorID);

			shieldMotor.stop(motorID);

			digitalWrite(9 + motorID, LOW);
		}
		else
		{
			digitalWrite(9 + motorID, HIGH);

			shieldMotor.speed(motorID, speed);
		}
	}
}
*/
