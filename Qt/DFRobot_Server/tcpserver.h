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
	~TcpServer();

    void setPort(int port);
    bool startServer();
    void stopServer();
    qint64 write(QString data);
    bool getServerStatus();

signals:
	void tcpProcessRecvData(QString data);
	void tcpClientConnectionChanged(bool isConnected);

private slots:
	void tcpReadyRead();
    void newConnection();
    void clientConnected();
    void clientDisconnected();

private:
    QTcpServer *server = Q_NULLPTR; //Used to handle the server actions
    QTcpSocket *socket = Q_NULLPTR;  //Used to handle connection with clients
    int port;
};

#endif // TCPSERVER_H
