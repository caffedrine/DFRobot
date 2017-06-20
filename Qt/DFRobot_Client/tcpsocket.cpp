#include "tcpsocket.h"

TcpSocket::TcpSocket(QObject *parent) : QObject(parent)
{

}

void TcpSocket::setHostname(QString host)
{
	this->hostname = host;
}

void TcpSocket::setPort(int prt)
{
	this->port = prt;
}

void TcpSocket::doConnect()
{
	this->socket = new QTcpSocket(this);
	this->socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

	connect(socket, SIGNAL(connected()),this, SLOT(connected()));
	connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
	//connect(socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
	connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));

	qDebug() << "Connecting...";

	// this is not blocking call
	socket->connectToHost(this->hostname, this->port);

	// we need to wait...
	if(!socket->waitForConnected(10000))
	{
		qDebug() << "Error: " << socket->errorString();
	}
}

void TcpSocket::connected()
{
	((MainWindow*)(parent()))->serverConnectionChanged(true);
	qDebug() << "connected...";

	// Hey server, tell me about you.
	socket->write("RECV: RC Connected!");
}

void TcpSocket::disconnected()
{
	((MainWindow*)(parent()))->serverConnectionChanged(false);
	qDebug() << "disconnected...";
}

//void TcpSocket::bytesWritten(qint64 bytes)
//{
//    //qDebug() << bytes << " bytes written...";
//}

void TcpSocket::readyRead()
{
	qDebug() << "reading...";

	// read the data from the socket
	qDebug() << socket->readAll();
}

void TcpSocket::write(QString msg)
{
	const QByteArray bytesToSend = QByteArray::fromStdString( msg.toStdString() );
	this->socket->write(bytesToSend);
}

bool TcpSocket::isAvailable()
{
	if(this->socket == Q_NULLPTR || this->socket == 0)
		return false;

	if(this->socket->isOpen() && this->socket->isWritable())
		return true;
	else
		return false;
}

void TcpSocket::doDisconnect()
{
	this->socket->disconnectFromHost();
}
