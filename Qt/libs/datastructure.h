/*
 * This class must be written in Standard C++ as it will be used on arduino board as well.
 * This way we make sure we gonna use exactly the same data structure in all project
*/

#include <string>

#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

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

        static const char startToken         = '>'; //Data start token
        static const char blocksDelimiter    = '|';  //Data blocks delimiter
        static const char endToken           = '<'; //Data end token

        static const char leftToken          = '[';  //Data block start token
        static const char paramsDelimiter    = ',';  //Parameters values delimiter
        static const char rightToken         = ']';  //Data block end token

        static const char valuesLeftToken    = '{';  //Values right token
        static const char valuesDelimiter    = '-';  //Multiple values have this delimiter
        static const char valuesRightToken   = '}';  //Values left token

        static const char motorIdentifier    = 'm';  //Motors identifier token
        static const char speedIdentifier    = 's';  //Speed identifier token
        static const char directionIdentifier= 'd';  //Direction identifier token

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
        int direction, lastDirection;   //Also used as offset if motor contain speed/direction packet

    }Motor;

    //Speed data structure
    typedef struct
    {
        int lastVal;        //Last speed
        int currentVal;     //Current motor speed

    }Speed;

    //Direction data structure
    typedef struct
    {
        DIRECTION lastVal;      //Last motor direction
        DIRECTION currentVal;   //Current motor direction

    }Direction;

public:
    DataStructure(int motors_number = 4, int valMin = 0, int valMax = 255);
    bool parseDataString(std::string *data);
    std::string getDataString();

private:
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

    //Integrity data handler - basic checks in order to make sure that received data is valid
    bool checkDataIntegrity(std::string *data);
    bool repairCorruptedData(std::string *data);

    //Used functions to make life easier
    static int getNumarDeAparitii(char caracter);
    static int getIndexOfNth(char caracter, int index);

};

#endif // DATASTRUCTURE_H
