#include <Arduino.h>

#include "StepperPWM.h"
#include "my_util.h"

namespace utils
{
	void brake();
	void displayData();
	void checkSerial();
	void updateMotors();
	void parseMessage(String msg);
	void brakeIfNotHeardWithin(int interval);
}

////////////////////////////////////////////////////////
//Defining motors
//        |<-------Arduino connector
//  M1 O----O M2
//     |    |
//     |    |
//  M4 O----O M3
//
//All motors are updated automatically using the following params:
//Update these values via SDA/SCL;

//Motor1
int left_direction = StepperPWM::FORWARD;
int left_speed = 0;     // 0 - 255

//Motor2
int right_direction = StepperPWM::FORWARD;
int right_speed = 0;     // 0 - 255

//////////////////////////////////////////////////////

StepperPWM left(8, 9, 7);		//PINS: direction, step, enable
StepperPWM right(11, 10, 12);

void setup()
{
	Serial.begin(115200);

	//Don't forget to init motors
	if(!right.init() || !left.init())
	{
		Serial.println("Can't initialize motors frequency!");
		while(1);
	}

	//Stop all motors
	utils::brake();
}

long lastTimeHeardFromHost = 0;
void loop()
{
	//Read and parse serial message if available`
	utils::checkSerial();

	//Display motors
	utils::displayData();

	//Update motors speeds and the stuff
	utils::updateMotors();

	//Brake if not hear from remote controller
	//utils::brakeIfNotHeardWithin(10000);
	//*/
}

namespace utils
{
	void brakeIfNotHeardWithin(int interval)
	{
		if (millis() - lastTimeHeardFromHost > interval)
		{
			lastTimeHeardFromHost = millis();
			utils::brake();
		}
	}

	void updateMotors()
	{
		//Setting up direction of each side of car
		right.setSpeed(right_speed);
		right.setDirection(right_direction);
		right.run();

		left.setSpeed(left_speed);
		left.setDirection(left_direction);
		left.run();
	}

	void brake()
	{
		left.brake();
		right.brake();
	}

	void displayData()
	{
		static int interval = 1000;
		static long previousMillis = 0;
		if (millis() - previousMillis > interval)
		{
			previousMillis = millis();

			String printStr = "";
			printStr += "[L," + to_string(left_direction) + ","
			        + to_string(left_speed) + "] ";
			printStr += "[R," + to_string(right_direction) + ","
			        + to_string(right_speed) + "] ";

			Serial.println(printStr);
		}
	}

	void parseMessage(String msg)
	{
		//Recv message format:
		//[L,0,120]-[R,1,120]
		if (msg.length() <= 7)
			return;

		int packetsNr = getNumberOfChars(msg, '-');

		for (int i = 0; i <= packetsNr; i++)
		{
			String currPacket = getStringPartByNr(msg, '-', i);
			String motor, dir, spd; //motor,direction,speed

			currPacket = currPacket.substring(1);     // Remove "["
			currPacket = currPacket.substring(0, currPacket.length() - 1); // Remove "]"

			motor = getStringPartByNr(currPacket, ',', 0);
			dir = getStringPartByNr(currPacket, ',', 1);
			spd = getStringPartByNr(currPacket, ',', 2);

			if (motor == "L")
			{
				left_direction = to_int(dir);
				left_speed = to_int(spd);
			}
			else if (motor == "R")
			{
				right_direction = to_int(dir);
				right_speed = to_int(spd);
			}

		}

		lastTimeHeardFromHost = millis();
	}

	void checkSerial()
	{
		static String serialString = "";

		while (Serial.available())
		{
			char c = Serial.read();  //gets one byte from serial buffer
			serialString += c; //makes the String readString
			//for (int i = 0; i <= 100; i++);
			delay(1); //slow looping to allow buffer to fill with next character
		}

		if (serialString.length() > 0)
		{
			utils::parseMessage(serialString);
			serialString = "";
		}
	}
}
