#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>

#include "mainwindow.h"

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);

    void setPort(int port);
    bool startServer();
    void stopServer();
    qint64 write(QString data);
    bool getServerStatus();

signals:

public slots:
    void newConnection();
    void readyRead();
    void clientConnected();
    void clientDisconnected();


private:
    QTcpServer *server = Q_NULLPTR; //Used to handle the server actions
    QTcpSocket *socket = Q_NULLPTR;  //Used to handle connection with clients
    int port;
};

#endif // TCPSERVER_H