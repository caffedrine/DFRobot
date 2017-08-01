/*
 * DataStructure.h
 *
 *  Created on: Aug 1, 2017
 *      Author: curiosul
 */

#ifndef DATASTRUCTURE_H_
#define DATASTRUCTURE_H_

class DataStructure
{
public:
	enum DIRECTION
	{
		BACKWARD,
		FORWARD
	};

	typedef struct MOTOR
	{
		int currSpeed, lastSpeed;
		int currDirection, lastDirection;

	}Motor;


	DataStructure();
	virtual ~DataStructure();
};

#endif /* DATASTRUCTURE_H_ */
