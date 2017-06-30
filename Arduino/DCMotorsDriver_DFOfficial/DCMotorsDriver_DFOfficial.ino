#include <Arduino.h>

//Standard PWM DC control
int E1 = 5;     //M1 Speed Control
int E2 = 6;     //M2 Speed Control
int M1 = 4;    //M1 Direction Control
int M2 = 7;    //M1 Direction Control

//Shield
int E3 = 3;
int E4 = 11;
int M3 = 12;
int M4 = 13;

///For previous Romeo, please use these pins.
//int E1 = 6;     //M1 Speed Control
//int E2 = 9;     //M2 Speed Control
//int M1 = 7;    //M1 Direction Control
//int M2 = 8;    //M1 Direction Control

void stop(void)                    //Stop
{
	digitalWrite(E1, LOW);
	digitalWrite(E2, LOW);
	digitalWrite(E3, LOW);
	digitalWrite(E4, LOW);
}
void advance(char a, char b)          	//Move forward
{
	analogWrite(E1, a);      			//PWM Speed Control
	digitalWrite(M1, HIGH);

	analogWrite(E2, b);
	digitalWrite(M2, HIGH);

	analogWrite(E3, b);
	digitalWrite(M3, HIGH);

	analogWrite(E4, b);
	digitalWrite(M4, HIGH);
}
void back_off(char a, char b)          //Move backward
{
	analogWrite(E1, a);
	digitalWrite(M1, LOW);

	analogWrite(E2, b);
	digitalWrite(M2, LOW);

	analogWrite(E3, b);
	digitalWrite(M2, LOW);

	analogWrite(E4, b);
	digitalWrite(M2, LOW);
}
void turn_L(char a, char b)             //Turn Left
{
	analogWrite(E1, a);
	digitalWrite(M1, LOW);
	analogWrite(E4, a);
	digitalWrite(M4, LOW);

	analogWrite(E2, b);
	digitalWrite(M2, HIGH);
	analogWrite(E3, b);
	digitalWrite(M3, HIGH);
}
void turn_R(char a, char b)             //Turn Right
{
	analogWrite(E1, a);
	digitalWrite(M1, HIGH);
	analogWrite(E4, a);
	digitalWrite(M4, HIGH);

	analogWrite(E2, b);
	digitalWrite(M2, LOW);
	analogWrite(E3, b);
	digitalWrite(M3, LOW);
}
void setup(void)
{
	int i;
	for (i = 3; i <= 13; i++)
		pinMode(i, OUTPUT);


	Serial.begin(115200);      //Set Baud Rate
	Serial.println("Run keyboard control");
}
void loop(void)
{
	if (Serial.available())
	{
		char val = Serial.read();
		if (val != -1)
		{
			switch (val)
			{
				case 'w':      //Move Forward
					advance(255, 255);   //move forward in max speed
					break;
				case 's':   //Move Backward
					back_off(255, 255);   //move back in max speed
					break;
				case 'a':   //Turn Left
					turn_L(100, 100);
					break;
				case 'd':   //Turn Right
					turn_R(100, 100);
					break;
				case 'z':
					Serial.println("Hello");
					break;
				case 'x':
					stop();
					break;
			}
		}
		else
			stop();
	}
}
