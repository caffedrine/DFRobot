# DataStructure Class

This class is used to encode/decode data which is send/received from/to car! Also all data is stored by this class!

Example:

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