#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QBitArray>
#include <QDebug>

#include "mainwindow.h"

class TcpSocket : public QObject
{
    Q_OBJECT
public:
    explicit TcpSocket(QObject *parent = 0);

    void setHostname(QString hostname);
    void setPort(int port);

    void doConnect();
    void write(QString data);
    bool isAvailable();

signals:

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();

private:
    QString hostname;
    int port;

    QTcpSocket *socket;
};

#endif // TCPSOCKET_H
