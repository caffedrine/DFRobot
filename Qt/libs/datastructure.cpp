#include "datastructure.h"

DataStructure::DataStructure(int motors_number, int valMin, int valMax)
{
    //Setting up and validating number of motors
    if(motors_number > this->GLOBALS::MAX_MOTORS)
        this->motorsNumber = this->GLOBALS::MAX_MOTORS;
    else if(motors_number < 1)
        this->motorsNumber = 1;
    this->motorsNumber = motors_number;



}
