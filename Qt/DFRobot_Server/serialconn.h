#ifndef SERIALCONN_H
#define SERIALCONN_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QDebug>

class SerialConn: public QObject
{
	Q_OBJECT

public:
	typedef QSerialPort::BaudRate BaudRate;	//let user choose his own boudrate

	SerialConn();
	~SerialConn();

private:
	//Serial port handler
	QSerialPort *pSerialPort = Q_NULLPTR;

	//Store last error
	QString lastError;

public:
	bool connect(QString portName, BaudRate baudRate);
	bool disconnect();

	qint64 write(QString str);
	qint64 read();	//not implemented yet
	QString readString();
	static QList<QString> getSerialPorts();
	bool isOpen();

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
