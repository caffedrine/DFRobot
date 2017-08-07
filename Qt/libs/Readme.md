# DataStructure Class

This class is used to encode/decode data which is send/received from/to car! Also all data is stored by this class!

Plain C++ written!

This class can contain the following data:
  1. 4 motors with ID, SPEED and DIRECTION;
  2. Car speed (global speed)
  3. Car direction
Notes: If you define motors speed then you don't have to define speed and direction anymore as it is already given by motors speed.

### bUILDING EXAMPLE:
```

// Defining 4 motors with it's speed and direction

DataStructure data(4); 4 = number of motors
data.setMotorInfo(1, 100, DataStructure::DIRECTION::FORWARD);
data.setMotorInfo(2, 200, DataStructure::DIRECTION::FORWARD);
data.setMotorInfo(3, 300, DataStructure::DIRECTION::BACKWARD);
data.setMotorInfo(4, 400, DataStructure::DIRECTION::BACKWARD);

// Setting up a variable called speed
data.setSpeed(255);

// Also setting a variable which mean steering
data.setSteering(123);

//Building string which will be send to the other side
std::string finalStr = data.buildDataString();				// the string which can be parsed
```

### Parsing example

```
// Assuming that this string was received:
std::string = yourFunctionToGetStr();

DataStructure data(4);
if(data.parseDataString( str ))
	// We have a valid string parsed and stored inside *data*
else
	// Invalid data to be parsed

//Now just get data out from structure

//First motor
data.getMotorInfo(1).id
data.getMotorInfo(1).speed
data.getMotorInfo(1).direction

//Get speed variable
data.getSpeed().currentVal;

// Get steering
data.getSteering().currentVal;

```


### Fullxample:

```
void test()
{
    DataStructure data(4);
    //DataStructure *data = new DataStructure(4); //It may be problematic on Arduino

    data.setMotorInfo(1, 100, DataStructure::DIRECTION::FORWARD);
    data.setMotorInfo(2, 200, DataStructure::DIRECTION::FORWARD);
    data.setMotorInfo(3, 300, DataStructure::DIRECTION::BACKWARD);
    data.setMotorInfo(4, 400, DataStructure::DIRECTION::BACKWARD);

    qDebug() << "Motors INFO:";
    for(int i=1; i <= 4; i++)
    {
        qDebug() << "Motor ID: " << data.getMotorInfo(i).id << " Speed: " << data.getMotorInfo(i).speed << " Direction: " << (data.getMotorInfo(i).direction);// == DataStructure::DIRECTION::FORWARD)?"FORWARD":"BACKWARD";
    }

    data.setSpeed(255);
    qDebug() << "Car speed: " << data.getSpeed().currentVal;

    data.setSteering(123);
    qDebug() << "Steering: " << data.getSteering().currentVal;

    const std::string str = data.buildDataString();
    qDebug() << "BUILD: " << QString::fromStdString( str );


    //Proceed with parsing a recv string
    qDebug() << "PARSING: " << ((data.parseDataString( str ))?"SUCCESS":"FAILED");

    qDebug() << "NEW VALUES:";

    qDebug() << "Motors INFO:";
    for(int i=1; i <= 4; i++)
    {
        qDebug() << "Motor ID: " << data.getMotorInfo(i).id << " Speed: " << data.getMotorInfo(i).speed << " Direction: " << (data.getMotorInfo(i).direction);// == DataStructure::DIRECTION::FORWARD)?"FORWARD":"BACKWARD";
    }

    data.setSpeed(255);
    qDebug() << "Car speed: " << data.getSpeed().currentVal;

    data.setSteering(123);
    qDebug() << "Steering: " << data.getSteering().currentVal;
}
```
