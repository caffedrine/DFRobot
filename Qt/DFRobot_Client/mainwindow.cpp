#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	//Load speedmetter stuff
	viewGauge = new QQuickView();
	containerGauge = QWidget::createWindowContainer(viewGauge, this);
	containerGauge->setMinimumSize(160, 160);
	containerGauge->setMaximumSize(160, 160);
	containerGauge->setFocusPolicy(Qt::TabFocus);
	viewGauge->setSource(QUrl("../CircularGauge.qml"));
	ui->verticalLayout->addWidget(containerGauge);

	//lOAD JOYSTICK
	QQuickView *view = new QQuickView();
	view->setSource(QUrl("../virtual_joystick.qml"));
#ifndef _WIN32
	view->setClearBeforeRendering(true);
	view->setColor(QColor(Qt::transparent));
#endif
	QQuickItem *root = view->rootObject();
	connect(root, SIGNAL(joystick_moved(double, double)), this, SLOT(joystick_moved(double, double)) );

	// Create a container widget for the QQuickView
	QWidget *container = QWidget::createWindowContainer(view, this);
	container->setMinimumSize(160, 160);
	container->setMaximumSize(160, 160);
	container->setFocusPolicy(Qt::TabFocus);
	ui->joystickLayout->addWidget(container);


    // Autofill ip address field when working on localhost
    QString ipAddress;
    QList<QHostAddress> ips = QNetworkInterface::allAddresses();

    foreach(QHostAddress ip, ips)
    {
        //Get a valid local IP address; localhost is excluded;
        if(ip != QHostAddress::LocalHost && ip.toIPv4Address())
        {
            ipAddress = ip.toString();
        }
    }

    if(!ipAddress.isEmpty())
        ui->ipTextBox->setText(ipAddress);
    else
        ui->ipTextBox->setText("localhost");
    //*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateSpeedGauge(int val)
{
	if(val < 0)
		val *=-1;

	if(val > 255)
		val = 255;

	//maping val to interval 0 - 100
	val = (val * 100)/255;

	//Update value
    QObject *rectangle      = viewGauge->rootObject(); //warning: only works when #include <QtQuick/QtQuick>
    QObject *circularGauge  = rectangle->findChild<QObject *>("circular_gauge");

	if(circularGauge)
	{
		circularGauge->setProperty("value", val);
	}
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_m1slider_valueChanged(int value)
{
    ui->m1speedLabel->setText( QString::number(value) );
    updateServer();
}

void MainWindow::on_m2slider_valueChanged(int value)
{
    ui->m2speedLabel->setText( QString::number(value) );
    updateServer();
}

void MainWindow::on_m3slider_valueChanged(int value)
{
    ui->m3speedLabel->setText( QString::number(value) );
    updateServer();
}

void MainWindow::on_m4slider_valueChanged(int value)
{
    ui->m4speedLabel->setText( QString::number(value) );
    updateServer();
}

void MainWindow::on_connectButton_clicked()
{
	if(hSocket)
		delete hSocket;

	hSocket = new TcpSocket(this);

	hSocket->setHostname(ui->ipTextBox->text());
	hSocket->setPort(ui->portTextBox->text().toInt());

	hSocket->doConnect();
}

void MainWindow::updateServer()
{
	if(!hSocket)
	{
		qDebug() << "Connect first...";
		return;
	}

    //Build the string we want to send via network
    //I will use JSON

    //For now, we only send motors parameters
    QString m1s, m1d, m2s, m2d, m3s, m3d, m4s, m4d; //Motors, directions and speeds

	QString carDir, carSpeed;

    m1s = ui->m1speedLabel->text();
    m2s = ui->m2speedLabel->text();
    m3s = ui->m3speedLabel->text();
    m4s = ui->m4speedLabel->text();

    m1d = ui->m1DirButton->isChecked() ? "1" : "0";
    m2d = ui->m2DirButton->isChecked() ? "1" : "0";
    m3d = ui->m3DirButton->isChecked() ? "1" : "0";
    m4d = ui->m4DirButton->isChecked() ? "1" : "0";

    carDir      = ui->directionLabel->text();
    carSpeed    = ui->speedLabel->text();

    QString data;
    data += "[" + m1s + "|" + m1d + "],";
    data += "[" + m2s + "|" + m2d + "],";
    data += "[" + m3s + "|" + m3d + "],";
	data += "[" + m4s + "|" + m4d + "],";
	data += "[s|" + carSpeed + "],";
	data += "[d|" + carDir + "]";

    qDebug() << data;

	if(this->hSocket->isAvailable())
		this->hSocket->write(data);
	else
		qDebug() << "FAILED: Cant write to socket. Is it opened and readable?";
}

void MainWindow::on_speedSlider_valueChanged(int value)
{
	this->ui->speedLabel->setText(QString::number(value));
	updateSpeedGauge(value);

	updateServer();
}

void MainWindow::on_directionSlider_valueChanged(int value)
{
	this->ui->directionLabel->setText(QString::number(value));
	updateSpeedGauge(value);

	updateServer();
}

void MainWindow::on_emergencyButton_clicked()
{
	this->ui->speedSlider->setValue(0);
	this->ui->directionSlider->setValue(0);
	delay(200);
	updateServer();
}

void MainWindow::delay(long milis)
{
	QTime dieTime= QTime::currentTime().addMSecs(milis);
	while (QTime::currentTime() < dieTime)
		QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
}

int map(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void MainWindow::joystick_moved(double x, double y)
{
	//qDebug() << "Speed: " << map(x, -1.00000, +1.00000, -255, +255 ) << " " << "Direction: " << map(y, -1.00000, +1.00000, -255, +255 ) << "\n";
	this->ui->speedSlider->setValue( map(y, -1.00000, 1.00000, -255, +255) );
	this->ui->directionSlider->setValue( map(x, -1.00000, 1.00000, -255, +255) );
}

void MainWindow::serverConnectionChanged(bool status)
{
	this->ui->connStatusLabel->setText( status ? "CONNECTED" : "NOT CONNECTED" );
}

void MainWindow::on_disconnectButton_clicked()
{
	if(!hSocket)
			return;

    this->hSocket->doDisconnect();
}

void MainWindow::on_testButton_clicked()
{
    DataStructure data(4);
    //DataStructure *data = new DataStructure(4); //It may be problematic on Arduino

    data.setMotorInfo(1, 100, DataStructure::DIRECTION::FORWARD);
    data.setMotorInfo(2, 200, DataStructure::DIRECTION::FORWARD);
    data.setMotorInfo(3, 300, DataStructure::DIRECTION::BACKWARD);
    data.setMotorInfo(4, 400, DataStructure::DIRECTION::BACKWARD);

    qDebug() << "Motors INFO:";
    for(int i=1; i <= 4; i++)
    {
        qDebug() << "Motor ID: " << data.getMotorInfo(i).id << " Speed: " << data.getMotorInfo(i).speed << " Direction: " << (data.getMotorInfo(i).direction);// == DataStructure::DIRECTION::FORWARD)?"FORWARD":"BACKWARD";
    }

    data.setSpeed(255);
    qDebug() << "Car speed: " << data.getSpeed().currentVal;

    data.setSteering(123);
    qDebug() << "Steering: " << data.getSteering().currentVal;

    qDebug() << "BUILD: " << QString::fromStdString( data.buildDataString() );


    //std::string str = ">[m1,{1;100}]|[m2,{1;200}]|[m3,{0;300}]|[m4,{0;400}]|[s,{255}]|[d,{123}]<";
    //qDebug() << "PARSING: " << ((data.parseDataString(str))?"SUCCESS":"FAILED");

}
