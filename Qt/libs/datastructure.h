/*
 * This class must be written in Standard C++ as it will be used on arduino board as well.
 * This way we make sure we gonna use exactly the same data structure in all project
*/

#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include <string>

#ifdef _ARD_    //Make sure code is compatible with Arduino
    #include <Arduino.h>        //Including arduino libs
    #define std::string String  //Make conversion between String and std::string
#endif

class DataStructure
{
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
        static const char blocksParamsDelimiter = ',';  //Parameters values delimiter
        static const char blocksRightToken      = ']';  //Data block end token

        static const char valuesLeftToken       = '{';  //Values right token
        static const char valuesDelimiter       = '-';  //Multiple values have this delimiter
        static const char valuesRightToken      = '}';  //Values left token

        static const char motorIdentifier       = 'm';  //Motors identifier token
        static const char speedIdentifier       = 's';  //Speed identifier token
        static const char directionIdentifier   = 'd';  //Direction identifier token

        //Other values
        static const int MAX_MOTORS = 8; //maximum numbers of motors
    };

    //We keep last values in order to avoid sending them again if values does not require an update.
    //This way we can avoid congestion of data send/recv;
    //Motor data structure
    typedef struct
    {
        int id;                         //Unique identifier for every motor
        int speed, lastSpeed;           //It is used as offset if packet also contain speed/direction packet
        DIRECTION direction, lastDirection;   //Also used as offset if motor contain speed/direction packet

        int getId() { return id; }
        int getCurrentSpeed() { return speed; }
        int getLastSpeed() { return lastSpeed; }
        DIRECTION getCurrentDirection() { return direction; }
        DIRECTION getLastDirection() { return lastDirection; }

    }Motor;

    //Speed data structure
    typedef struct
    {
        int lastVal;        //Last speed
        int currentVal;     //Current motor speed

        //And some usefull methods in case we want just a value
        int getCurrentVal() { return currentVal; }
        int getLastVal() { return lastVal; }

    }Speed;

    //Direction data structure
    typedef struct
    {
        DIRECTION lastVal;      //Last motor direction
        DIRECTION currentVal;   //Current motor direction

        //Methods
        DIRECTION getLastVal() { return lastVal; }
        DIRECTION getCurrentVal() { return currentVal; }

    }Direction;

public:
    DataStructure(int motors_number = 4, int valMin = 0, int valMax = 255);
    bool parseDataString(std::string *data);
    std::string getDataString();

    //Gets and sets

    //Motors params
    Motor getMotorInfo(int motorId);
    void setMotorInfo(int motorID, int speed, int direction);

    //Car speed
    Speed getSpeed();
    void setSpeed(int speed);

    //Car direction
    Direction getDirection();
    void setDirection(Direction, direction);

    //Util functions used functions to make life easier
    static int getNumberOfChars(const std::string &str, char checkCharacter);
    static int getIndexOfNth(const std::string &str, char caracter, int index = 0);
    static std::string getStringPartByNr(const std::string &data, char separator, int index);
    static std::string to_string(int i);
    static int to_int(std::string);

private:
    //Final builded message will be an array of blocks - let's define structure of a block
//    typedef struct BLOCK
//    {
//        std::string param_name;
//        int values_number;
//        std::string values[values_number?];
//    };


    //Number of motors we actually have;
    int motorsNumber;

    //Maximum and minimum speed/offset for motors. Basically, all values have to fit in this interval
    int val_min, val_max;

    //Array of motors. Max size is defined in GLOBALS structure;
    Motor motors[GLOBALS::MAX_MOTORS];

    //A variable to store speed
    Speed speed;

    //A variable to store direction
    Direction direction;

    //Define store data string builded wight here
    std::string sendData = "";  //output string. This is what should be send to other terminals

    //Last string parsed is stored here
    std::string recvData = "";  //This string is received and parsed

    //Integrity data handler - basic checks in order to make sure that received data is valid
    bool checkDataIntegrity(std::string *data);
    bool repairCorruptedData(std::string *data);
};

#endif // DATASTRUCTURE_H
