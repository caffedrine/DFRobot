#ifndef I2C_H
#define I2C_H

/// This may help: https://github.com/jrowberg/i2cdevlib

/** I will implement this on Linux as it is nearly impossible to have a portable code.
  * This is the class which manage I2C connection. It implements ICommands interface.
  */

class I2C : public ICommands
{
public:
    void I2C();
    void ~I2C();

    // Implemented methods from ICommunication
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

};

#endif  // I2C_H
