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
//StepperPWM right(2, 3, 9, 8);	//PINS: direction1, direction2, step, enable
//StepperPWM left(4, 12, 10, 8);

void setup()
{
	Serial.begin(115200);

	/**
	 * Because a modified version of Arduino UNO CNC Shield v3 customized, unused digital pins must be set as inputs!!!
	 * Nod doing this may broke the board ^_^
	 */

	pinMode(5, INPUT);
	pinMode(6, INPUT);
	pinMode(7, INPUT);
	pinMode(11, INPUT);
	pinMode(13, INPUT);

	//Don't forget to initialize motors
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);

	leftData.setTargetSpeed(700);
	rightData.setTargetSpeed(700);

	digitalWrite(8, HIGH);
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
		rightData.currSpeed = rightData.targetSpeed;
		leftData.currSpeed = leftData.targetSpeed;

		// Update left side speed
		static unsigned long lastMicros1 = micros();
		static bool lastState1 = false;

		if(micros() - lastMicros1 > (unsigned long)rightData.currSpeed)
		{
			lastMicros1 = micros();
			lastState1 = !lastState1;

			// write digital signal
			digitalWrite(9, lastState1);

			//Also update directions from right side
			digitalWrite(2, rightData.currM1Dir);
			digitalWrite(3, rightData.currM2Dir);
		}

		// update right side speed
		static unsigned long lastMicros2 = micros();
		static bool lastState2 = false;
		if(micros() - lastMicros2 > (unsigned long)leftData.currSpeed)
		{
			lastMicros2 = micros();
			lastState2 = !lastState2;

			// write digital signal
			digitalWrite(10, lastState2);

			//Also update directions from left side
			digitalWrite(4, leftData.currM1Dir);
			digitalWrite(12, leftData.currM2Dir);
		}

		// brake if necessary
		if((leftData.targetSpeed == 700 && rightData.targetSpeed == 700) ||
				(leftData.targetSpeed == 0 && rightData.targetSpeed == 0))
			digitalWrite(8, HIGH);
		else
			digitalWrite(8, LOW);
	}

	void brake()
	{
		digitalWrite(8, HIGH);
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
			printStr += "[L," + to_string(leftData.currM1Dir) + ","
			        + to_string(leftData.currM2Dir) + ","
			        + to_string(leftData.currSpeed) + "] ";
			printStr += "[R," + to_string(rightData.currM1Dir) + ","
			        + to_string(rightData.currM2Dir) + ","
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

			spd = to_string(map(to_int(spd), 0, 255, 70, 700));
			spd = to_string( map(to_int(spd), 70, 700, 700, 70) );

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
		/**
		 * Attempting to read serial without any loop delay in order
		 * to receive smooth speed changing.
		 * So reading char by char is the best option in this case
		 */
		if (Serial.available())
		{
			static String serialString;
			char c = Serial.read();

			if(c == '>')
			{
				// parse received string
				data::parseMessage(serialString);

				// reset serial string
				serialString = "";
			}
			else
			{
				serialString += c;
			}
		}
	}
}
