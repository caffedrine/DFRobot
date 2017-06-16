#include "datastructure.h"

#include <QDebug>
#include <QString>


DataStructure::DataStructure(int motors_number)
{
    //Setting up and validating number of motors
    if(motors_number > GLOBALS::MAX_MOTORS)
        this->motorsNumber = GLOBALS::MAX_MOTORS;
    else if(motors_number < 1)
        this->motorsNumber = 1;
    this->motorsNumber = motors_number;

    //Initializing motors values for the first time
    // !!! Motors ID starts from 1 !!!
    for(int i=1; i<=this->motorsNumber; i++)
    {
        motors[i].id = i;
        motors[i].direction = this->FORWARD;
        motors[i].lastDirection = this->FORWARD;
        motors[i].speed = 0;
        motors[i].lastSpeed = 0;
    }

    //Initializing speed for the first time
    this->speed.lastVal = 0;
    this->speed.lastVal = 0;

    //Initializing direction for the first time
    this->steering.lastVal = 0;
    this->steering.currentVal = 0;
}

std::string DataStructure::buildDataString()
{
    //We have to return a string
    std::string buildedString = "";

    //Blocks array we have to pass
    BLOCK_STRUCT blocks[GLOBALS::MAX_BLOCKS_NUMBER];

    //Populating array with blocks

    //Adding motors blocks
    for(int i=1; i<=this->motorsNumber; i++)
    {
        //Update only motors with values changed since last time
        if(this->motors[i].speed != this->motors[i].lastSpeed || this->motors[i].direction != this->motors[i].lastDirection)
        {
            std::string *values = motors[i].to_array();
            blocks[i-1].build( (const char)GLOBALS::motorIdentifier, values);
            delete[] values;
        }
    }

    //Speed
    if(speed.currentVal != speed.lastVal)
    {
        //Use this to prevent memory leak - we can delete this obj
        std::string *values = speed.to_array();
        blocks[4].build( (const char)GLOBALS::speedIdentifier, values );
        delete[] values;
    }

    //Steering
    if(steering.currentVal != steering.lastVal)
    {
        std::string *values = steering.to_array();
        blocks[5].build( (const char)GLOBALS::steeringIdentifier, steering.to_array() );
        delete[] values; //Don;t judge me! Adruino does not support smart pointers!!!
    }

    //Add custom values following the above pattern

    //Add your custom data here
    // blocks[6].build() or blocks[6].parse()
    //At the end, we get something like this
    // ">[m,{1;1;100}]|[m,{2;1;200}]|[m,{3;0;300}]|[m,{4;0;400}]|[s,{255}]|[d,{123}]<"


    //Procees with bulding the string we want to send
    buildedString += GLOBALS::startToken;
    for(int i=0; i < GLOBALS::MAX_BLOCKS_NUMBER; i++)
    {
        if( !(blocks[i].isEmpty()) )
        {
            buildedString += blocks[i].to_string();
            if(!blocks[i+1].isEmpty())
                buildedString += GLOBALS::blocksDelimiter;
        }
        else
            break;
    }
    buildedString += GLOBALS::endToken;
    return buildedString;
}

bool DataStructure::parseDataString(const std::string &data)
{
    //this function receive data and have to parse it
    if( this->checkDataIntegrity(data) == false )
        return false;

    //EXAMPLE: >[m,{1;1;100}]|[m,{2;1;200}]|[m,{3;0;300}]|[m,{4;0;400}]|[s,{255}]|[d,{123}]<
    //Regex like a boss? Naa, not on arduino :(

    //We convert the received string to an array of blocks in order to process it easily
    BLOCK_STRUCT blocks[GLOBALS::MAX_BLOCKS_NUMBER];

    //Getting number of blocks
    int blocks_number = getNumberOfChars(data, GLOBALS::blocksDelimiter) + 1;

    //process every block
    for(int i=0; i < blocks_number; i++)
    {
        std::string currentBlock = this->getStringPartByNr(data, GLOBALS::blocksDelimiter, i);

        //remove first and last token
        if(i == 0) currentBlock = currentBlock.substr(1);
        if(i == (blocks_number - 1)) currentBlock = currentBlock.substr(0, currentBlock.length() - 1);

        blocks[i].parse(currentBlock);
    }

    //We got an array of blocks. Now we may want to fill our variables

    //Fill up the speed
    this->setSpeed( to_int( getParamsByName(blocks, (const char)GLOBALS::speedIdentifier).param_values[0] ) );

    //Fill up steering
    this->setSteering( to_int( getParamsByName(blocks, (const char)GLOBALS::steeringIdentifier).param_values[0] ) );

    //Filling up motors values
    for(int i=1; i <= motorsNumber; i++)
    {
        BLOCK_STRUCT motorBlock = getParamsByName(blocks, (const char)GLOBALS::motorIdentifier, i);
        setMotorInfo(i, to_int(motorBlock.param_values[2]), (DIRECTION)to_int(motorBlock.param_values[1]));
    }

    return true;
}

bool DataStructure::checkDataIntegrity(const std::string &data)
{
    //function to perform basic data integrity check in order to make sure we didn't receive garbage

    //Make sure we have first and last token
    if(this->getNumberOfChars(data, GLOBALS::startToken) < 1 ||  this->getNumberOfChars(data, GLOBALS::endToken) < 1)
        return false;

    //Make sure we have blocks
    if(this->getNumberOfChars(data, GLOBALS::blockLeftToken) != this->getNumberOfChars(data, GLOBALS::blocksRightToken))
        return false;

    //Checking values paranthesis validity
    if(this->getNumberOfChars(data, GLOBALS::valuesLeftToken) != this->getNumberOfChars(data, GLOBALS::valuesRightToken))
        return false;

    //Make sure block contain parameter name and values
    if(this->getNumberOfChars(data, GLOBALS::valuesLeftToken) != this->getNumberOfChars(data, GLOBALS::blocksParamsDelimiter))
        return false;

    if(this->getNumberOfChars(data, GLOBALS::valuesLeftToken)-1 != this->getNumberOfChars(data, GLOBALS::blocksDelimiter) )
        return false;

    //Matemathical poly functions can be used here if you wanna be a GURU :))

    return true;
}

DataStructure::BLOCK_STRUCT DataStructure::getParamsByName(BLOCK_STRUCT *blocks, const char &name, int id)
{
    DataStructure::BLOCK_STRUCT block;
    for(int i=0; i < GLOBALS::MAX_BLOCKS_NUMBER; i++)
    {
        if(!blocks[i].isEmpty())
        {
            if(blocks[i].param_name == name)
            {
                if(id != -1)
                {
                    if(blocks[i].param_values[0] == to_string(id))
                        return blocks[i];
                }
                else
                {
                    return blocks[i];
                }
            }
        }
        else
            return block;
    }
    return block;
}
/*
//   _   _   _____   ___   _       ____
//  | | | | |_   _| |_ _| | |     / ___|
//  | | | |   | |    | |  | |     \___ \
//  | |_| |   | |    | |  | |___   ___) |
//   \___/    |_|   |___| |_____| |____/
*/
int DataStructure::getNumberOfChars(const std::string &source, char character)
{
    int count = 0;

    for (size_t i = 0; i < source.length(); i++)
    {
        if (source[i] == character)
        {
            ++count;
        }
    }
    return count;
}

std::string DataStructure::getStringPartByNr(const std::string &data, char separator, int index)
{
    // spliting a string and return the part nr index
    // split by separator

    int stringData = 0;        //variable to count data part nr
    std::string dataPart = "";      //variable to hole the return text

    for (size_t i = 0; i <= data.length() - 1; i++)
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
#ifdef __ARD__ || __WIN32__
	itoa(val, snum, 10);	//it is onbly supported by windows and arduino - no Linux :(
#else
	itoa_custom(val, snum);
#endif
    return snum;
}

void DataStructure::itoa_custom(int n, char s[])
{
	int i, sign;

	if ((sign = n) < 0)  /* record sign */
		n = -n;          /* make n positive */
	i = 0;
	do {       /* generate digits in reverse order */
		s[i++] = n % 10 + '0';   /* get next digit */
	} while ((n /= 10) > 0);     /* delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';

	//reversing
	char c;
	for (int i = 0, j = strlen(s)-1; i<j; i++, j--)
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

int DataStructure::to_int(std::string str)
{
    return atoi(str.c_str());
}

/*
//      ____      _                          _    ____       _
//     / ___| ___| |_ ___     __ _ _ __   __| |  / ___|  ___| |_ ___
//    | |  _ / _ \ __/ __|   / _` | '_ \ / _` |  \___ \ / _ \ __/ __|
//    | |_| |  __/ |_\__ \  | (_| | | | | (_| |   ___) |  __/ |_\__ \
//     \____|\___|\__|___/   \__,_|_| |_|\__,_|  |____/ \___|\__|___/
*/
DataStructure::Motor DataStructure::getMotorInfo(int motorId)
{
    //In case validation is required
    //    for(int i=0; i < this->motorsNumber; i++)
    //    {
    //        if(this->motors[i].id == motorId)
    //            return this->motors[i];
    //    }
    //return;
    return motors[motorId];
}

void DataStructure::setMotorInfo(int motorID, int speed, DIRECTION direction)
{
    //Updating last values
    this->motors[motorID].lastSpeed = this->motors[motorID].speed;
    this->motors[motorID].lastDirection = this->motors[motorID].direction;

    //Updating current values
    this->motors[motorID].speed = speed;
    this->motors[motorID].direction = direction;
}

DataStructure::Speed DataStructure::getSpeed()
{
   return this->speed;
}

void DataStructure::setSpeed(int speed)
{
    this->speed.lastVal = this->speed.currentVal;
    this->speed.currentVal = speed;
}

DataStructure::Steering DataStructure::getSteering()
{
    return this->steering;
}

void DataStructure::setSteering(int steer)
{
    this->steering.lastVal = this->steering.currentVal;
    this->steering.currentVal = steer;
}
