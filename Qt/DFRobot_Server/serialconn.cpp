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
			//Clean buffers to throw away garbage from the wires - but it is now working :(
			pSerialPort->flush();
			pSerialPort->clear();
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
		this->setLastError("Fatal error when called serialPort.open() function");
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

QList<QString> SerialConn::getSerialPorts()
{
	QList<QString> ports;

	//Get serial ports info
	QSerialPortInfo *portsInfo = new QSerialPortInfo;

	//Fetching available ports into a list
	QList<QSerialPortInfo> serialPorts = portsInfo->availablePorts();

	//Filling combo list
	foreach (QSerialPortInfo serialPort, serialPorts)
	{
		ports.push_back(serialPort.portName());
	}

	//clearing pointer we just created
	delete portsInfo;

	return ports;
}

QString SerialConn::readString()
{
	// Read data
	static QByteArray byteArray;
	byteArray += pSerialPort->readAll();

	//we want to read all message not only chunks
	if(!QString(byteArray).contains("\n"))
		return "^_^";

	//sanitize data
	QString data = QString( byteArray ).remove("\r").remove("\n");
	byteArray.clear();

	//Send it to visual console too
	return data;
}
