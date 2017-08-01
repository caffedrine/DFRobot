#ifndef my_util_h
#define my_util_h

#include <Arduino.h>

//Prototypes
String to_string(int intNumber);
int to_int(String str);
int getNumberOfChars(String str, char checkCharacter);
String getStringPartByNr(String data, char separator, int index);
void printPeriodicData(String data, int interval = 1000);

//Functions defined
String to_string(int val)
{
	char snum[16];
	itoa(val, snum, 10);
	return snum;
}

int to_int(String str)
{
	return atoi(str.c_str());
}

int getNumberOfChars(String str, char checkCharacter)
{
	int count = 0;

	for (unsigned int i = 0; i < str.length(); i++)
	{
		if (str[i] == checkCharacter)
		{
			++count;
		}
	}
	return count;
}

String getStringPartByNr(String data, char separator, int index)
{
	// spliting a string and return the part nr index
	// split by separator

	int stringData = 0;        //variable to count data part nr
	String dataPart = "";      //variable to hole the return text

	for (unsigned int i = 0; i <= data.length() - 1; i++)
	{
		//Walk through the text one letter at a time
		if (data[i] == separator)
		{
			//Count the number of times separator character appears in the text
			stringData++;
		}
		else if (stringData == index)
		{
			//get the text when separator is the rignt one
			dataPart.concat(data[i]);

		}
		else if (stringData > index)
		{
			//return text and stop if the next separator appears - to save CPU-time
			return dataPart;
			break;
		}

	}
	//return text if this is the last part
	return dataPart;
}

void printPeriodicData(String data, unsigned int interval)
{
	static long previousMillis = 0;
	if (millis() - previousMillis > interval)
	{
		previousMillis = millis();
		Serial.println(data);
	}
}

#endif
