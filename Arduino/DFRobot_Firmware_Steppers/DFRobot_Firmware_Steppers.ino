#include <Arduino.h>

#include "StepperPWM.h"
#include "my_util.h"
#include "DataStructure.h"

namespace utils
{
	void brake();
	void updateMotors();
	void brakeIfNotHeardWithin(int interval);
}

namespace data
{
	void displayData();
	void parseMessage();
	void checkSerial();
}

////////////////////////////////////////////////////////
//Defining motors
//  L1 O----O R1
//     |    |
//     |    |
//  L2 O----O R2

// Structures to store data about motors
DataStructure leftData;
DataStructure rightData;

// Motors controllers
//StepperPWM right(12, 11, 9, 13);	//PINS: direction1, direction2, step, enable
//StepperPWM left(A5, 2, 10, A4);

// Configured to be compatible with Arduino CNC Shield v3
StepperPWM right(4, 5, 9, 13);	//PINS: direction1, direction2, step, enable
StepperPWM left(6, 7, 10, A4);

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
	data::checkSerial();

	//Display motors
	data::displayData();

	//Update motors speeds and the stuff
	utils::updateMotors();

	//Brake if not hear from remote controller
	//utils::brakeIfNotHeardWithin(10000);
	//*/
}

/**
 * Namespace with util functions
 */
namespace utils
{
	void brakeIfNotHeardWithin(unsigned int interval)
	{
		if (millis() - lastTimeHeardFromHost > interval)
		{
			lastTimeHeardFromHost = millis();
			utils::brake();
		}
	}

	void updateMotors()
	{
		// Update motors this way to avoid current peaks
//		static unsigned long prevMicros = 0;
//		if(micros() - prevMicros > 200)	// increment speed until target every 200 microseconds
//		{
//			if(rightData.currSpeed != rightData.targetSpeed)	// if target data not reached
//			{
//				if(rightData.currSpeed < rightData.targetSpeed)	//we need to accelerate
//				{
//					rightData.currSpeed++;
//				}
//				else	// we need to slow down
//				{
//					rightData.currSpeed--;
//				}
//			}
//		}

		rightData.currSpeed = rightData.targetSpeed;
		leftData.currSpeed = leftData.targetSpeed;


		//Setting up direction of each side of car
		right.setSpeed(rightData.currSpeed);
		right.set1Direction(rightData.currM1Dir);
		right.set2Direction(rightData.currM2Dir);
		right.run();

		left.setSpeed(leftData.currSpeed);
		left.set1Direction(leftData.currM1Dir);
		left.set2Direction(leftData.currM2Dir);
		left.run();
	}

	void brake()
	{
		left.brake();
		right.brake();
	}
}

/**
 * Namespace used to get and parse data to serial
 */
namespace data
{
	void displayData()
	{
		static unsigned int interval = 1000;
		static long previousMillis = 0;
		if (millis() - previousMillis > interval)
		{
			previousMillis = millis();

			String printStr = "";
			printStr += "[L," + to_string(leftData.currM1Dir) + "," + to_string(leftData.currM2Dir) + ","
			        + to_string(leftData.currSpeed) + "] ";
			printStr += "[R," + to_string(rightData.currM1Dir) + "," + to_string(rightData.currM2Dir) + ","
			        + to_string(rightData.currSpeed) + "] ";

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
				leftData.setCurrM1Dir(to_int(dir1));
				leftData.setCurrM2Dir(to_int(dir2));
				leftData.setTargetSpeed(to_int(spd));
			}
			else if (motor == "R")
			{
				rightData.setCurrM1Dir(to_int(dir1));
				rightData.setCurrM2Dir(to_int(dir2));
				rightData.setTargetSpeed(to_int(spd));
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
			data::parseMessage(serialString);
			serialString = "";
		}
	}
}

