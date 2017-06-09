/*
 * This class must be written in Standard C++ as it will be used on arduino board as well.
 * This way we make sure we gonna use exactly the same data structure in all project
*/

#include <string>

#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

class DataStructure
{

    //Delimiters and tokens used to generate and parse recv/send data
    struct GLOBALS
    {
        //Global tokens.
        //By default, data structure will look in the following way:
        //  >>[paramIdentifier,{value1-value2}]|[paraIdentifier2,{value1-value2...}<<

        const char startToken         = '>'; //Data start token
        const char blocksDelimiter    = '|';  //Data blocks delimiter
        const char endToken           = '<'; //Data end token

        const char leftToken          = '[';  //Data block start token
        const char paramsDelimiter    = ',';  //Parameters values delimiter
        const char rightToken         = ']';  //Data block end token

        const char valuesLeftToken    = '{';  //Values right token
        const char valuesDelimiter    = '-';  //Multiple values have this delimiter
        const char valuesRightToken   = '}';  //Values left token

        const char motorIdentifier    = 'm';  //Motors identifier token
        const char speedIdentifier    = 's';  //Speed identifier token
        const char directionIdentifier= 'd';  //Direction identifier token

        //Other values
        static const int MAX_MOTORS = 8; //maximum numbers of motors
    };

    //We keep last values in order to avoid sending them again if values does not require an update.
    //This way we can avoid congestion of data send/recv;
    //Motor data structure
    typedef struct motor
    {
        int id;
        int speed, lastSpeed;
        int direction, lastDirection;

    }Motor;

    //Speed data structure
    typedef struct
    {
        int lastVal;
        int currentVal;

    }Speed;

    //Direction data structure
    typedef struct
    {
        int lastVal;
        int currentVal;

    }Direction;

public:
    DataStructure(int motors_number = 4, int valMin = 0, int valMax = 255);
    bool parseDataString(std::string *data);
    std::string getDataString();

private:
    //Number of motors we actually have;
    int motorsNumber;

    //Array of motors. Max size is defined in GLOBALS structure;
    Motor motors[GLOBALS::MAX_MOTORS];

    //Integrity data handler - basic checks in order to make sure that received data is valid
    bool checkDataIntegrity(std::string *data);
    bool repairCorruptedData(std::string *data);

    //Used functions to make life easier
    static int getNumarDeAparitii(char caracter);
    static int getIndexOfNth(char caracter, int index);

};

#endif // DATASTRUCTURE_H
