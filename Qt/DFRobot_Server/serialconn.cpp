#include "serialconn.h"

SerialConn::SerialConn()
{

}

SerialConn::~SerialConn()
{
	if(!pSerialPort)
		delete pSerialPort;
}

QString SerialConn::getLastError()
{
	return this->lastError;
}

void SerialConn::setLastError(QString error)
{
	this->lastError = error;
}

bool SerialConn::connect(QString portName, BoudRate boudRate)
{
	//Make sure we are not already connected
	if(pSerialPort)
	{
		if(pSerialPort->isOpen())
			pSerialPort->close();

		delete pSerialPort;
	}

	//Initialize port
	pSerialPort = 0;

	//Create a new instance
	pSerialPort = new QSerialPort();

	//Connecting signals to slots
	QObject::connect(pSerialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(connectionStatusChanged(QSerialPort::SerialPortError)));
	QObject::connect(pSerialPort, SIGNAL(readyRead()), this, SIGNAL(readyRead()));

	//Setting connection params
	pSerialPort->setPortName(portName);
	pSerialPort->setBaudRate(boudRate);

	try
	{
		//Open RW Sessions
		pSerialPort->open(QIODevice::ReadWrite);

		//Check if connection succeed
		if(pSerialPort->isOpen())
		{
			return true;
		}
		else
		{
			this->setLastError(pSerialPort->errorString());
			return false;
		}
	}
	catch(...)
	{
		this->setLastError("Fatal error when try to call serialPort.open() function");
		return false;
	}
}

bool SerialConn::disconnect()
{
	if(!pSerialPort)
		return true;

	pSerialPort->close();

	if(!pSerialPort->isOpen())
		return true;

	return false;
}

void SerialConn::connectionStatusChanged(QSerialPort::SerialPortError errNo)
{
	if(errNo == 0)
	{
		emit connectionStatusChanged(true);
	}
	else
	{
		emit connectionStatusChanged(false);
	}
}

qint64 SerialConn::write(QString str)
{
	if(pSerialPort == Q_NULLPTR)
	{
		this->setLastError("Not connected to serial port!");
		return -1;
	}

	if( pSerialPort->isOpen() )
	{
		QByteArray writeData = str.toUtf8();
		return pSerialPort->write( writeData );
	}
	else
	{
		this->setLastError(pSerialPort->errorString());
		return -1;
	}
}
