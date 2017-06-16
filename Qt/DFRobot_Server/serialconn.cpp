#include "serialconn.h"

SerialConn::SerialConn()
{

}

SerialConn::~SerialConn()
{

}


//QString SerialConn::getLastError()
//{
//	return this->lastError;
//}

//void SerialConn::setLastError(QString error)
//{
//	this->lastError = error;
//}

//bool SerialConn::connect(QString portName, BoudRate boudRate)
//{
//	//Make sure we are not already connected
//	if(pSerialPort)
//	{
//		if(pSerialPort->isOpen())
//			pSerialPort->close();

//		delete pSerialPort;
//	}

//	//Initialize port
//	pSerialPort = 0;

//	//Connecting signals to slots
//	//QObject::connect(pSerialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SIGNAL(connectionStatusChanged(QSerialPort::SerialPortError)));
//	//QObject::connect(pSerialPort, SIGNAL(readyRead()), this, SIGNAL(readyRead()));

//	//Setting connection params
//	pSerialPort->setPortName(portName);
//	pSerialPort->setBaudRate(boudRate);

//	//Open RW Sessions
//	pSerialPort->open(QIODevice::ReadWrite);

//	//Check if connection succeed
//	if(pSerialPort->isOpen())
//	{
//		return true;
//	}
//	else
//	{
//		this->setLastError(pSerialPort->errorString());
//		return true;
//	}
//}

//bool SerialConn::disconnect()
//{
//	if(!pSerialPort)
//		return false;

//	return false;
//}
