/*
 * DataStructure.cpp
 *
 *  Created on: Aug 1, 2017
 *      Author: curiosul
 */

#include "DataStructure.h"

DataStructure::DataStructure()
{
	// TODO Auto-generated constructor stub

}

DataStructure::~DataStructure()
{
	// TODO Auto-generated destructor stub
}

void DataStructure::setTargetSpeed(int speed)
{
	this->lastSpeed = this->targetSpeed;
	this->targetSpeed = speed;
}

void DataStructure::setCurrM1Dir(int dir)
{
	this->lastM1Dir = this->currM1Dir;
	this->currM1Dir = dir;
}

void DataStructure::setCurrM2Dir(int dir)
{
	this->lastM2Dir = this->currM2Dir;
	this->currM2Dir = dir;
}
