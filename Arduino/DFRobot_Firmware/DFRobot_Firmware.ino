#include "DCMotor.h"
#include "my_util.h"

namespace utils
{
	void brake();
	void displayData();
	void checkSerial();
	void updateMotors();
	void parseMessage(String msg);
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
int motor1_direction = 0;       // 1 - Forward, 0 - Backward
int motor1_speed = 0;     // 0 - 255

//Motor2
int motor2_direction = 0;       // 1 - Forward, 0 - Backward
int motor2_speed = 0;     // 0 - 255

//Motor3
int motor3_direction = 0;       // 1 - Forward, 0 - Backward
int motor3_speed = 0;     // 0 - 255

//Motor4
int motor4_direction = 0;       // 1 - Forward, 0 - Backward
int motor4_speed = 0;     // 0 - 255
//////////////////////////////////////////////////////

//DCMotor <NAME>(const int speedPin, const int directionPin, bool forwardOn);
//I have to specify whether the motor moves forward on HIGH or on LOW;
//
//DCMotor M4(5,   4, 0);
//DCMotor M3(6,   7, 1);
//DCMotor M2(3,  12, 0);
//DCMotor M1(11, 13, 0);
DCMotor M2(5,   4, 0);
DCMotor M1(6,   7, 1);
DCMotor M3(3,  12, 0);
DCMotor M4(11, 13, 0);

void setup()
{
	Serial.begin(19200);

	//Stop all motors
	utils::brake();
}

void loop()
{
	//Read and parse serial message if available`
	utils::checkSerial();

	//Display motors
	utils::displayData();

	//Update motors speeds and the stuff
	utils::updateMotors();
}

namespace utils
{
	void updateMotors()
	{
		//////////////////////////////////////////////////////////////////////////
		//Motors speed and direction update
		M1.setSpeed(motor1_speed);
		M1.setDirection((direction) motor1_direction);
		M1.run();       //Motor 1

		M2.setSpeed(motor2_speed);
		M2.setDirection((direction) motor2_direction);
		M2.run();       //Motor 2

		M3.setSpeed(motor3_speed);
		M3.setDirection((direction) motor3_direction);
		M3.run();       //Motor 3

		M4.setSpeed(motor4_speed);
		M4.setDirection((direction) motor4_direction);
		M4.run();       //Motor 4
		/////////////////////////////////////////////////////////////////////////
	}

	void brake()
	{
		motor1_speed = 0;
		motor2_speed = 0;
		motor3_speed = 0;
		motor4_speed = 0;
	}

	void displayData()
	{
		static int interval = 1000;
		static long previousMillis = 0;
		if (millis() - previousMillis > interval)
		{
			previousMillis = millis();

			String printStr = "";
			printStr += "[M1," + to_string(motor1_direction) + ","
			        + to_string(motor1_speed) + "] ";
			printStr += "[M2," + to_string(motor2_direction) + ","
			        + to_string(motor2_speed) + "] ";
			printStr += "[M3," + to_string(motor3_direction) + ","
			        + to_string(motor3_speed) + "] ";
			printStr += "[M4," + to_string(motor4_direction) + ","
			        + to_string(motor4_speed) + "]";

			Serial.println(printStr);
		}
	}

	void parseMessage(String msg)
	{
		//Recv message format:
		//[M1,0,123]-[M2,0,12]
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

			if (motor == "M1")
			{
				motor1_direction = to_int(dir);
				motor1_speed = to_int(spd);
			}
			else if (motor == "M2")
			{
				motor2_direction = to_int(dir);
				motor2_speed = to_int(spd);
			}
			else if (motor == "M3")
			{
				motor3_direction = to_int(dir);
				motor3_speed = to_int(spd);
			}
			else if (motor == "M4")
			{
				motor4_direction = to_int(dir);
				motor4_speed = to_int(spd);
			}
		}
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

