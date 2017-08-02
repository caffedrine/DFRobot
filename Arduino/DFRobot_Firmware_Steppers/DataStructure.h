/*
 * DataStructure.h
 *
 *  Created on: Aug 1, 2017
 *      Author: curiosul
 */

#ifndef DATASTRUCTURE_H_
#define DATASTRUCTURE_H_

/**
 * Class used just to store data
 */
class DataStructure
{
public:

	enum DIRECTION
	{
		BACKWARD, FORWARD
	};

	/**
	 * Because motors speeds are controlled in pairs, we define a structure for one pair.
	 * Also a pai means half of car (two motors from right or left side)
	 */
	/*
	typedef struct PAIR
	{
		int currSpeed, lastSpeed;
		int currM1Dir, lastM1Dir;
		int currM2Dir, lastM2Dir;
	} Pair;
	*/

	// We use directly variables as this class is  only used to store data
	int lastSpeed, currSpeed = 0, targetSpeed = 0;
	int currM1Dir = DIRECTION::FORWARD, lastM1Dir;
	int currM2Dir = DIRECTION::FORWARD, lastM2Dir;

	DataStructure();
	virtual ~DataStructure();

	void setTargetSpeed(int speed);
	void setCurrM1Dir(int direction);
	void setCurrM2Dir(int direction);
};

#endif /* DATASTRUCTURE_H_ */
