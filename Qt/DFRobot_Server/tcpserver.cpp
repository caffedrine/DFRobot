#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent) : QObject(parent)
{

}

TcpServer::~TcpServer()
{
	if(this->socket != Q_NULLPTR)
	{
		if(socket)
			socket->disconnectFromHost();

		delete socket;
		socket = Q_NULLPTR;
	}

	if(this->server != Q_NULLPTR)
	{
		if(server)
			server->disconnect();
		delete server;
		server  = Q_NULLPTR;
	}
}

bool TcpServer::startServer()
{
    server = new QTcpServer(this);

    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if(!server->listen(QHostAddress::Any, this->port))
    {
		qDebug() << "TCP FAILED: Server could not start!";
        return false;
    }
    else
    {
		qDebug() << "TCP SUCCESS: Server started!";
        return true;
    }
}

void TcpServer::stopServer()
{

    if(this->socket != Q_NULLPTR && this->socket)
    {
        this->socket->close();
		qDebug() << "TCP SUCCESS: Socket closed!";
    }

    if(this->server != Q_NULLPTR && this->server)
    {
        this->server->close();
		qDebug() << "TCP SUCCESS: Server stopped!";
    }

    delete socket;
    socket = Q_NULLPTR;

    delete server;
    server = Q_NULLPTR;
}

void TcpServer::newConnection()
{
    clientConnected();
	qDebug() << "TCP: A NEW CONECTION WAS ESTABLISHED";

    // need to grab the socket
    this->socket = server->nextPendingConnection();
    this->socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

	connect(this->socket, SIGNAL(readyRead()), this, SLOT(tcpReadyRead()));
    connect(this->socket, SIGNAL(connected()), this, SLOT(clientConnected()));
    connect(this->socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
    //connect(this->socket, SIGNAL());


    //((MainWindow*)(parent()))->clientConnected();
    socket->write("Hello! I'am the robot you want to control!\r\n");
    socket->flush();

    socket->waitForBytesWritten(10000);

    //socket->close();
}

qint64 TcpServer::write(QString msg)
{
    if(this->server == Q_NULLPTR || this->server == 0)
    {
		qDebug() << "TCP FAILED: Server is not started!";
        return -1;
    }

    if(this->socket == Q_NULLPTR || !this->socket->isOpen() || !this->socket->isWritable())
    {
		qDebug() << "TCP FAILED: Can't write to socket. Is this open and writable?";
        return -1;
    }

    //converion function requires Qt >= 5.4 or something...
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
	emit tcpClientConnectionChanged(true);
}

void TcpServer::clientDisconnected()
{
	emit tcpClientConnectionChanged(false);
}

void TcpServer::tcpReadyRead()
{
	QString recvData = this->socket->readAll();
	emit tcpProcessRecvData(recvData);
}
