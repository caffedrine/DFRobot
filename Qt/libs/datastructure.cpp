#include "datastructure.h"

DataStructure::DataStructure(int motors_number, int valMin, int valMax)
{
    //Setting up and validating number of motors
    if(motors_number > GLOBALS::MAX_MOTORS)
        this->motorsNumber = GLOBALS::MAX_MOTORS;
    else if(motors_number < 1)
        this->motorsNumber = 1;
    this->motorsNumber = motors_number;

    this->val_min = valMin;
    this->val_max = valMax;

    //Initializing motors values for the first time
    for(int i=1; i<=this->motorsNumber; i++)
    {
        motors[i].id = i;
        motors[i].direction = 0;
        motors[i].lastDirection = 0;
        motors[i].speed = 0;
        motors[i].lastSpeed = 0;
    }

    //Initializing speed for the first time
    this->speed.lastVal = 0;
    this->speed.lastVal = 0;

    //Initializing direction for the first time
    this->direction.lastVal = DIRECTION::FORWARD;
    this->direction.currentVal = DIRECTION::FORWARD;
}

std::string DataStructure::getDataString()
{
    std::string data = "";

    //Adding first token
    data += GLOBALS::startToken;

    //Proceed next to add elements blocks
    //Starting with motors blocks
    for(int i=1; i<=this->motorsNumber; i++)
    {
        //Update only motors with values changed since last time
        if(this->motors[i].speed != this->motors[i].lastSpeed || this->motors[i].direction != this->motors[i].lastDirection)
        {
            data += GLOBALS::blockLeftToken;
            {
                data += GLOBALS::motorIdentifier; data += this->to_string(i);
                data += GLOBALS::blocksParamsDelimiter;
                data += GLOBALS::valuesLeftToken;
                {
                    data += this->to_string(this->motors[i].direction);
                    data += GLOBALS::valuesDelimiter;
                    data += this->to_string(this->motors[i].speed);
                }
                data+=GLOBALS::valuesRightToken;
            }
            data += GLOBALS::blocksRightToken;
            data += GLOBALS::blocksParamsDelimiter;
        }
    }

    //Because we can't be sure motors values changed since last time
    if(data.length() >= 5)
        data = data.substr(0, data.length() - 1);   //we may just want to delete latest delimiter;


    //Adding direction only if it has changed since last time
    if(this->direction.currentVal != this->direction.lastVal)
    {
        //Add blocks delimiter only if we already have a block before
       if(data.length() >=5)
           data += GLOBALS::blocksParamsDelimiter;

        data += GLOBALS::blockLeftToken;
        {
            data += GLOBALS::directionIdentifier;
            data += GLOBALS::blocksDelimiter;
            data += GLOBALS::valuesLeftToken;
            {
                data += this->direction.currentVal;
            }
            data+=GLOBALS::valuesRightToken;

        }
        data += GLOBALS::blocksRightToken;
    }

    //Adding speed only if it has been changed
    if(this->speed.currentVal != this->speed.lastVal)
    {
        if(data.length() >=5)
            data += GLOBALS::blocksParamsDelimiter;

        data += GLOBALS::blockLeftToken;
        {
            data += GLOBALS::directionIdentifier;
            data += GLOBALS::blocksDelimiter;
            data += GLOBALS::valuesLeftToken;
            {
                data += this->speed.currentVal;
            }
            data+=GLOBALS::valuesRightToken;

        }
        data += GLOBALS::blocksRightToken;
    }

    //That's all for now! Feel free to add auxiliar params in the same way ^_^
    /*
    if(val_changed_since_last_time)
    {
        if(data.length() >=5)
            data += GLOBALS::blocksParamsDelimiter;

        data += GLOBALS::blockLeftToken;
        {
            data += GLOBALS::yout_value_identifier;
            data += GLOBALS::blocksDelimiter;
            data += GLOBALS::valuesLeftToken;
            {
                data += your_value
            }
            data+=GLOBALS::valuesRightToken;

        }
        data += GLOBALS::blocksRightToken;
    }
    */
}

//   _   _   _____   ___   _       ____
//  | | | | |_   _| |_ _| | |     / ___|
//  | | | |   | |    | |  | |     \___ \
//  | |_| |   | |    | |  | |___   ___) |
//   \___/    |_|   |___| |_____| |____/

int DataStructure::getNumberOfChars(const std::string &source, char character)
{
    int count = 0;

    for (int i = 0; i < source.length(); i++)
    {
        if (source[i] == character)
        {
            ++count;
        }
    }
    return count;
}

int DataStructure::getIndexOfNth(const std::string &source, char character, int index)
{
    return 0;
}

std::string DataStructure::getStringPartByNr(const std::string &data, char separator, int index)
{
    // spliting a string and return the part nr index
    // split by separator

    int stringData = 0;        //variable to count data part nr
    std::string dataPart = "";      //variable to hole the return text

    for (int i = 0; i <= data.length() - 1; i++)
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
            dataPart += data[i];

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

std::string DataStructure::to_string(int val)
{
    char snum[16];
    itoa(val, snum, 10);
    return snum;
}

int DataStructure::to_int(std::string str)
{
    return atoi(str.c_str());
}

//      ____      _                          _    ____       _
//     / ___| ___| |_ ___     __ _ _ __   __| |  / ___|  ___| |_ ___
//    | |  _ / _ \ __/ __|   / _` | '_ \ / _` |  \___ \ / _ \ __/ __|
//    | |_| |  __/ |_\__ \  | (_| | | | | (_| |   ___) |  __/ |_\__ \
//     \____|\___|\__|___/   \__,_|_| |_|\__,_|  |____/ \___|\__|___/

Motor DataStructure::getMotorInfo(int motorId)
{

}
