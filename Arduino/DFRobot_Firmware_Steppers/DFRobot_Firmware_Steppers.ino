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
//  L1 O----O R1
//     |    |
//     |    |
//  L2 O----O R2
//
//All motors are updated automatically using the following params:
//Update these values via SDA/SCL;

//Motor1
int left_direction1 =  StepperPWM::FORWARD, left_direction2 =  StepperPWM::FORWARD;
int left_speed = 0;     // 0 - 255

//Motor2
int right_direction1 = StepperPWM::FORWARD, right_direction2 = StepperPWM::FORWARD;
int right_speed = 0;     // 0 - 255

//////////////////////////////////////////////////////

StepperPWM left(12, 11, 9, 13);	//PINS: direction1, direction2, step, enable
StepperPWM right(A5, 2, 10, A4);

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
		right.set1Direction(right_direction1);
		right.set2Direction(right_direction2);
		right.run();

		left.setSpeed(left_speed);
		left.set1Direction(left_direction1);
		left.set2Direction(left_direction2);
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
			printStr += "[L," + to_string(left_direction1) + "," + to_string(left_direction2) + ","
			        + to_string(left_speed) + "] ";
			printStr += "[R," + to_string(right_direction1) + "," + to_string(right_direction2) + ","
			        + to_string(right_speed) + "] ";

			Serial.println(printStr);
		}
	}

	void parseMessage(String msg)
	{
		//Recv message format:
		//[L,0,0,120]-[R,1,1,120]
		if (msg.length() <= 7)
			return;

		int packetsNr = getNumberOfChars(msg, '-');

		for (int i = 0; i <= packetsNr; i++)
		{
			String currPacket = getStringPartByNr(msg, '-', i);
			String motor, dir1, dir2, spd; //motor,direction,speed

			currPacket = currPacket.substring(1);     // Remove "["
			currPacket = currPacket.substring(0, currPacket.length() - 1); // Remove "]"

			motor = getStringPartByNr(currPacket, ',', 0);
			dir1 = getStringPartByNr(currPacket, ',', 1);
			dir2 = getStringPartByNr(currPacket, ',', 2);
			spd = getStringPartByNr(currPacket, ',', 3);

			if (motor == "L")
			{
				left_direction1 = to_int(dir1);
				left_direction2 = to_int(dir2);
				left_speed = to_int(spd);
			}
			else if (motor == "R")
			{
				right_direction1 = to_int(dir1);
				right_direction2 = to_int(dir2);
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
