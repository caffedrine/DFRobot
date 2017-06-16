/*
 * This class must be written in Standard C++ as it will be used on arduino board as well.
 * This way we make sure we gonna use exactly the same data structure in all project
*/

#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include <string>

//#define __ARD__		//Use this when compile for Arduino
//#define __WIN32__		//Use this when compile for WINDOWS
//#define __UNIX__		//UNIX RULLZ

#ifdef __ARD__    //Make sure code is compatible with Arduino
    #include <Arduino.h>            //Including arduino libs
    #define std::string String      //Make conversion between String and std::string
    #define .substr( .substring(    //Also Arduino is using substring() instead of substr()
#else
	#include <stdio.h>
	#include <stdlib.h>
#endif

class DataStructure
{
public:
    //Convention for direction backward/forward values
    typedef enum DIRECTION
    {
        FORWARD  = 1,
        BACKWARD = 0

    }DIRECTION;

    //Delimiters and tokens used to generate and parse recv/send data
    struct GLOBALS
    {
        //Global tokens.
        //By default, data structure will look in the following way:
        //  >>[paramIdentifier,{value1-value2}]|[paraIdentifier2,{value1-value2...}<<

        static const char startToken            = '>'; //Data start token
        static const char blocksDelimiter       = '|';  //Data blocks delimiter
        static const char endToken              = '<'; //Data end token

        static const char blockLeftToken        = '[';  //Data block start token
        static const char blocksParamsDelimiter = ',';  //Parameters name delimiter
        static const char blocksRightToken      = ']';  //Data block end token

        static const char valuesLeftToken       = '{';  //Values right token
        static const char valuesDelimiter       = ';';  //Multiple values have this delimiter
        static const char valuesRightToken      = '}';  //Values left token

        //Param names
        static const char motorIdentifier       = 'm';  //Motors identifier token
        static const char speedIdentifier       = 's';  //Speed identifier token
        static const char steeringIdentifier    = 'd';  //Direction/Steering identifier token

        //Other values
        static const int MAX_MOTORS = 8;                //maximum numbers of motors
        static const int MAX_PARAMS_NUMBER = 3;          //Maximum number of values assigned to a param
        static const int MAX_BLOCKS_NUMBER = 8;        //Maximum number of blocks
    };

    //We keep last values in order to avoid sending them again if values does not require an update.
    //This way we can avoid congestion of data send/recv;
    //Motor data structure
    typedef struct
    {
        int id;                                 //Unique identifier for every motor
        int speed, lastSpeed;                   //It is used as offset if packet also contain speed/direction packet
        DIRECTION direction, lastDirection;     //Also used as offset if motor contain speed/direction packet

        std::string* to_array()
        {
            std::string *values = new std::string[GLOBALS::MAX_PARAMS_NUMBER];
            values[0] = to_string(id);          //ID
            values[1] = to_string(direction);   //DIRECTION
            values[2] = to_string(speed);       //SPEED
            return values;
        }

        void parse(std::string *data)
        {
            lastSpeed = speed;
            lastDirection = direction;

            direction = (DIRECTION)to_int(data[1]);
            speed = to_int(data[2]);
        }

    }Motor;

    //Speed data structure
    typedef struct
    {
        int lastVal;        //Last speed
        int currentVal;     //Current motor speed

        std::string *to_array()
        {
            std::string *values = new std::string[GLOBALS::MAX_PARAMS_NUMBER];
            values[0] = to_string(currentVal);
            return values;
        }

        void parse(std::string *data)
        {
            lastVal = currentVal;
            currentVal = to_int(data[0]);
        }

    }Speed;

    //Direction data structure
    typedef struct
    {
        int lastVal;      //Last motor direction
        int currentVal;   //Current motor direction

        std::string *to_array()
        {
            std::string *values = new std::string[GLOBALS::MAX_PARAMS_NUMBER];
            values[0] = to_string(currentVal);
            return values;
        }

        void parse(std::string *data)
        {
            lastVal = currentVal;
            currentVal = to_int(data[0]);
        }

    }Steering;

    //Public methods and constructor
    DataStructure(int motors_number = 4);
    bool parseDataString(const std::string &data);
    std::string buildDataString();

    //Gets and sets

    //Motors params
    Motor getMotorInfo(int motorId);
    void setMotorInfo(int motorID, int speed, DataStructure::DIRECTION direction);

    //Car speed
    Speed getSpeed();
    void setSpeed(int speed);

    //Car direction
    Steering getSteering();
    void setSteering(int steering);

    //Util functions used functions to make life easier
    static int getNumberOfChars(const std::string &str, char checkCharacter);
    static std::string getStringPartByNr(const std::string &data, char separator, int index);
    static std::string to_string(int i);
    static int to_int(std::string);
	static void itoa_custom(int n, char s[]);

private:
    //Final builded message will be an array of blocks - let's define structure of a block
    typedef struct
    {
        //Data stored on struct
        char param_name;
        std::string param_values[GLOBALS::MAX_PARAMS_NUMBER];

        //Usefull methods
        std::string to_string()
        {
            std::string str;
            str += GLOBALS::blockLeftToken;
            str += param_name;
            str += GLOBALS::blocksParamsDelimiter;
            str += GLOBALS::valuesLeftToken;
            for(int i=0; i < GLOBALS::MAX_PARAMS_NUMBER; i++)
            {
                if(!param_values[i].empty())
                {
                    str += param_values[i];
                    if(i+1 < GLOBALS::MAX_PARAMS_NUMBER && !param_values[i+1].empty())
                    {
                        str += GLOBALS::valuesDelimiter;
                    }
                    else
                        break;
                }
                else
                    break;
            }
            str += GLOBALS::valuesRightToken;
            str += GLOBALS::blocksRightToken;
            return str; // Format: [m,{4;0;400}]
        }

        void parse(const std::string &data)
        {
            //decode received data - formet used is the one used in to_string() function
            //Removing block delimiters
            std::string tmp = data;

            tmp = tmp.substr( 1 );
            tmp = tmp.substr(0, tmp.length() - 1);

            //Getting param name and values
            std::string paramName = getStringPartByNr(tmp, GLOBALS::blocksParamsDelimiter, 0);
            std::string paramValues = getStringPartByNr(tmp, GLOBALS::blocksParamsDelimiter, 1);

            //remove values right and left tokens
            paramValues = paramValues.substr(1);
            paramValues = paramValues.substr(0, paramValues.length() - 1);

            //Get number of values we received
            int values_number = getNumberOfChars(paramValues, GLOBALS::valuesDelimiter) + 1;

            //fill variables with received values
            param_name = paramName[0];


            for(int i=0; i <values_number; i++)
            {
                param_values[i] = getStringPartByNr(paramValues, GLOBALS::valuesDelimiter, i);
            }
        }

        void build(const char &param_nm, std::string *param_vals)
        {
            param_name = param_nm;

            for(int i=0; i < GLOBALS::MAX_PARAMS_NUMBER; i++)
            {
                if(!param_vals[i].empty())
                    param_values[i] = param_vals[i];
                else
                    break;
            }
        }

        bool isEmpty()
        {
            if(param_name == '\0' || param_values[0].empty())
                return true;
            return false;
        }

    }BLOCK_STRUCT;

    //Number of motors we actually have;
    int motorsNumber;

    //Array of motors. Max size is defined in GLOBALS structure;
    Motor motors[GLOBALS::MAX_MOTORS];

    //A variable to store speed
    Speed speed;

    //A variable to store direction
    Steering steering;

    //Integrity data handler - basic checks in order to make sure that received data is valid
    bool checkDataIntegrity(const std::string &data);
    bool repairCorruptedData(const std::string &data);
    BLOCK_STRUCT getParamsByName(BLOCK_STRUCT *blocks, const char &name, int id = -1);
};

#endif // DATASTRUCTURE_H
