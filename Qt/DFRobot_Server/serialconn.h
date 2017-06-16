#ifndef SERIALCONN_H
#define SERIALCONN_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>

class SerialConn: public QObject
{
	Q_OBJECT

public:
	SerialConn();
	~SerialConn();

	typedef QSerialPort::BaudRate BoudRate;	//let user choose his own boudrate

private:
	//Serial port handler
	QSerialPort *pSerialPort = Q_NULLPTR;

	//Period of time after trying to recdonnect in case of connectrion lost
	long reconnectAfterMillis = 100;

	//Set this if you want to automatically attempt to reconnect
	bool attemptReconnect = true;

	//Store last error
	QString lastError;


public:
	bool connect(QString portName, BoudRate boudRate);
	bool disconnect();

	qint64 write(QString str);
	qint64 read();

	//Other methods
	QString getLastError();

private:
	void setLastError(QString error);

private slots:
	void connectionStatusChanged(QSerialPort::SerialPortError errNo);

signals:
	void readyRead();
	void connectionStatusChanged(bool connected);
};

#endif // SERIALCONN_H
