/**
 * @file   Serial.h
 * @author Alex (616c6578@protonmail.com)
 * @date   July, 2017
 * @brief  Serial communication class
 */
#ifndef SERIAL_H
#define SERIAL_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QList>
#include <QByteArray>

/**
 * @name Class Serial
 * @brief The Serial class is used to establish communication to device via
 * serial protocol. It implements interface ICommunication
 */
class Serial : public ICommunication
{
public:
    //Public variables
    typedef QSerialPort::BaudRate BR;

    //Implemented methods
    Serial(QString portName, BR baudRate);
    ~Serial();
    bool connect();
    bool disconnect();
    bool isConnected();
    QString readAll();
    qint64 write(QString);
    QString getLastError();

signals:
    void connectionStatusChanged(bool newStatus);
    void readyRead();

private:
    //Store the serial port name
    QString serialPortName;

    //Store serial port baud rate
    BR baud_rate;

    //Serial port handler
    QSerialPort *pSerialPort = Q_NULLPTR;

    //Store last error
    QString lastError;

    //Method to set last error
    void setLastError(QString error);

private slots:
    void serialPortError(QSerialPort::SerialPortError errNo);
};

#endif  // SERIAL_H
