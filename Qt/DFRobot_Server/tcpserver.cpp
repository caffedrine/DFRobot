#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{

}

bool TcpServer::startServer()
{
    server = new QTcpServer(this);

    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if(!server->listen(QHostAddress::Any, this->port))
    {
        qDebug() << "FAILED: Server could not start!";
        return true;
    }
    else
    {
        qDebug() << "SUCCESS: Server started!";
        return false;
    }
}

void TcpServer::stopServer()
{

    if(this->socket != Q_NULLPTR && this->socket)
    {
        this->socket->close();
        qDebug() << "SUCCESS: Socked closed!";
    }

    if(this->server != Q_NULLPTR && this->server)
    {
        this->server->close();
        qDebug() << "SUCCESS: Server stopped!";
    }

    delete socket;
    socket = Q_NULLPTR;

    delete server;
    server = Q_NULLPTR;
}

void TcpServer::newConnection()
{
    clientConnected();
    qDebug() << "A NEW CONECTION: ESTABLISHED";

    // need to grab the socket
    this->socket = server->nextPendingConnection();
    this->socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    connect(this->socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(this->socket, SIGNAL(connected()), this, SLOT(clientConnected()));
    connect(this->socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    //connect(this->socket, SIGNAL());


    //((MainWindow*)(parent()))->clientConnected();

    //((MainWindow*)(parent()))->

    socket->write("Hello! I'am the robot you want to control!\r\n");
    socket->flush();

    socket->waitForBytesWritten(10000);

    //socket->close();
}

void TcpServer::readyRead()
{
    QString recvData = this->socket->readAll();

    // read the data from the socket
    qDebug() << recvData;

    ((MainWindow*)(parent()))->processRecvData(recvData);
}

qint64 TcpServer::write(QString msg)
{

    if(this->server == Q_NULLPTR || this->server == 0)
    {
        qDebug() << "FAILED: Server is not started!";
        return -1;
    }

    if(this->socket == Q_NULLPTR || !this->socket->isOpen() || !this->socket->isWritable())
    {
        qDebug() << "FAILED: Can't write to socket. Is this open and writable?";
        return -1;
    }

    const QByteArray bytesToSend = QByteArray::fromStdString( msg.toStdString() );
    return this->socket->write(bytesToSend);
}

void TcpServer::setPort(int prt)
{
    this->port = prt;
}

bool TcpServer::getServerStatus()
{
    return this->server->isListening();
}

void TcpServer::clientConnected()
{
    qDebug() << "NEW CONNECTION ESTABLISHED";
    ((MainWindow*)(parent()))->clientConnectionChanged(1);
}

void TcpServer::clientDisconnected()
{
    qDebug() << "A CONNECTION WAS CLOSED!";
    ((MainWindow*)(parent()))->clientConnectionChanged(0);
}
